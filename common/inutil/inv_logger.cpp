#include "util/inv_logger.h"
#include "util/inv_functor.h"
#include <iostream>
#include <string.h>

namespace inv
{
	bool INV_LoggerRoll::_bDyeingFlag = false;
	INV_ThreadMutex  INV_LoggerRoll::_mutexDyeing;
	// set<pthread_t>  INV_LoggerRoll::_setThreadID;
	// hash_map<pthread_t, string>  INV_LoggerRoll::_mapThreadID;
	logger_map_type<pthread_t, string>  INV_LoggerRoll::_mapThreadID;
    const string InvLogByDay::FORMAT = "%Y%m%d";
    const string InvLogByHour::FORMAT = "%Y%m%d%H";
    const string InvLogByMinute::FORMAT = "%Y%m%d%H%M";

	void INV_LoggerRoll::setupThread(INV_LoggerThreadGroup *pThreadGroup)
	{
		assert(pThreadGroup != NULL);

		unSetupThread();

		INV_LockT<INV_ThreadMutex> lock(_mutex);

		_pThreadGroup = pThreadGroup;

		INV_LoggerRollPtr self = this;

		_pThreadGroup->registerLogger(self);
	}

	void INV_LoggerRoll::unSetupThread()
	{
		INV_LockT<INV_ThreadMutex> lock(_mutex);

		if (_pThreadGroup != NULL)
		{
			_pThreadGroup->flush();

			INV_LoggerRollPtr self = this;

			_pThreadGroup->unRegisterLogger(self);

			_pThreadGroup = NULL;
		}

		flush();
	}

	void INV_LoggerRoll::write(const pair<int, string> &buffer)
	{
		pthread_t ThreadID = 0;
		if (_bDyeingFlag)
		{
			INV_LockT<INV_ThreadMutex> lock(_mutexDyeing);

			pthread_t tmp = pthread_self();
			//if (_setThreadID.count(tmp) == 1)
			if (_mapThreadID.count(tmp) == 1)
			{
				ThreadID = tmp;
			}
		}

		if (_pThreadGroup)
		{
			_buffer.push_back(make_pair(ThreadID, buffer.second));
		}
		else
		{
			//同步记录日志
			deque<pair<int, string> > ds;
			ds.push_back(make_pair(ThreadID, buffer.second));
			roll(ds);
		}
	}

	void INV_LoggerRoll::flush()
	{
		INV_ThreadQueue<pair<int, string> >::queue_type qt;
		_buffer.swap(qt);

		if (!qt.empty())
		{
			roll(qt);
		}
	}

//////////////////////////////////////////////////////////////////
//
	INV_LoggerThreadGroup::INV_LoggerThreadGroup() : _bTerminate(false)
	{
	}

	INV_LoggerThreadGroup::~INV_LoggerThreadGroup()
	{
		flush();

		_bTerminate = true;

		{
			Lock lock(*this);
			notifyAll();
		}

		_tpool.stop();
		_tpool.waitForAllDone();
	}

	void INV_LoggerThreadGroup::start(size_t iThreadNum)
	{
		_tpool.init(iThreadNum);
		_tpool.start();

		INV_Functor<void> cmd(this, &INV_LoggerThreadGroup::run);
		INV_Functor<void>::wrapper_type wrapper(cmd);
		for (size_t i = 0; i < _tpool.getThreadNum(); i++)
		{
			_tpool.exec(wrapper);
		}
	}

	void INV_LoggerThreadGroup::registerLogger(INV_LoggerRollPtr &l)
	{
		Lock lock(*this);

		_logger.insert(l);
	}

	void INV_LoggerThreadGroup::unRegisterLogger(INV_LoggerRollPtr &l)
	{
		Lock lock(*this);

		_logger.erase(l);
	}

	void INV_LoggerThreadGroup::flush()
	{
		logger_set logger;

		{
			Lock lock(*this);
			logger = _logger;
		}

		logger_set::iterator it = logger.begin();
		while (it != logger.end())
		{
			try
			{
				it->get()->flush();
			}
			catch (...)
			{
			}
			++it;
		}
	}

	void INV_LoggerThreadGroup::run()
	{
		while (!_bTerminate)
		{
			//100ms
			{
				Lock lock(*this);
				timedWait(100);
			}

			flush();
		}
	}

//////////////////////////////////////////////////////////////////////////////////

	LoggerBuffer::LoggerBuffer() : _buffer(NULL), _buffer_len(0)
	{
	}

	LoggerBuffer::LoggerBuffer(INV_LoggerRollPtr roll, size_t buffer_len) : _roll(roll), _buffer(NULL), _buffer_len(buffer_len)
	{
		//设置get buffer, 无效, 不适用
		setg(NULL, NULL, NULL);

		//设置put buffer
		if (_roll)
		{
			//分配空间
			_buffer = new char[_buffer_len];
			setp(_buffer, _buffer + _buffer_len);
		}
		else
		{
			setp(NULL, NULL);
			_buffer_len = 0;
		}
	}

	LoggerBuffer::~LoggerBuffer()
	{
		sync();
		if (_buffer)
		{
			delete[] _buffer;
		}
	}

	streamsize LoggerBuffer::xsputn(const char_type* s, streamsize n)
	{
		if (!_roll)
		{
			return n;
		}

		return std::basic_streambuf<char>::xsputn(s, n);
	}

	void LoggerBuffer::reserve(std::streamsize n)
	{
		if (n <= _buffer_len)
		{
			return;
		}

		//不超过最大大小
		if (n > MAX_BUFFER_LENGTH)
		{
			n = MAX_BUFFER_LENGTH;
		}

		int len = pptr() - pbase();
		char_type * p = new char_type[n];
		memcpy(p, _buffer, len);
		delete[] _buffer;
		_buffer     = p;
		_buffer_len = n;

		setp(_buffer, _buffer + _buffer_len);

		pbump(len);

		return;
	}

	std::basic_streambuf<char>::int_type LoggerBuffer::overflow(std::basic_streambuf<char>::int_type c)
	{
		if (!_roll)
		{
			return 0;
		}

		if (_buffer_len >= MAX_BUFFER_LENGTH)
		{
			sync();
		}
		else
		{
			reserve(_buffer_len * 2);
		}

		if (std::char_traits<char_type>::eq_int_type(c,std::char_traits<char_type>::eof()) )
		{
			return std::char_traits<char_type>::not_eof(c);
		}
		else
		{
			return sputc(c);
		}
		return 0;
	}

	int LoggerBuffer::sync()
	{
		//有数据
		if (pptr() > pbase())
		{
			std::streamsize len = pptr() - pbase();

			if (_roll)
			{
				//具体的写逻辑
				_roll->write(make_pair(0, string(pbase(), len)));
			}

			//重新设置put缓冲区, pptr()重置到pbase()处
			setp(pbase(), epptr());
		}
		return 0;
	}

////////////////////////////////////////////////////////////////////////////////////
//

}


