#ifndef __INV_LOGGER_H
#define __INV_LOGGER_H

#include <streambuf>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
#include <sys/syscall.h>
#include <iomanip>

#ifdef __GNUC__
#   include <features.h>
#   if __GNUC_PREREQ(4,8)
//      If  gcc_version >= 4.8
#       include <unordered_map>
#       define logger_map_type std::unordered_map
#   else
//       Else
#       include <ext/hash_map>
#       define logger_map_type hash_map
#   endif
#else
#   include <ext/hash_map>
#   define logger_map_type hash_map
#endif

#include "util/inv_ex.h"
#include "util/inv_autoptr.h"
#include "util/inv_util.h"
#include "util/inv_file.h"
#include "util/inv_thread.h"
#include "util/inv_monitor.h"
#include "util/inv_thread_pool.h"
#include "util/inv_timeprovider.h"
#include "util/inv_singleton.h"

using namespace std;
using namespace __gnu_cxx;

namespace inv
{

	struct INV_Logger_Exception : public INV_Exception
	{
		INV_Logger_Exception(const string &buffer) : INV_Exception(buffer){};
		INV_Logger_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
		~INV_Logger_Exception() throw(){};
	};

/**
 * @brief 写操作
 */
	class INV_DefaultWriteT
	{
	public:
		void operator()(ostream &of, const deque<pair<int, string> > &ds)
		{
			deque<pair<int, string> >::const_iterator it = ds.begin();
			while (it != ds.end())
			{
				of << it->second;
				++it;
			}
			of.flush();
		}
	};

	class INV_LoggerThreadGroup;

//////////////////////////////////////////////////////////////////////////////
/**
 * @brief 具体写日志基类
 */
	class INV_LoggerRoll : public INV_HandleBase
	{
	public:
		/**
		 * @brief 构造函数
		 */
		INV_LoggerRoll() : _pThreadGroup(NULL)
		{
		}

		/**
		 * @brief 实时记日志, 并且滚动.
		 *
		 * 不同的策略日志类,实现不同的逻辑
		 * @param buffer
		 */
		virtual void roll(const deque<pair<int, string> > &ds) = 0;

		/**
		 * @brief 安装线程.
		 *
		 * @param ltg
		 */
		void setupThread(INV_LoggerThreadGroup *pThreadGroup);

		/**
		 * @brief 取消线程.
		 *
		 */
		void unSetupThread();

		/**
		 * @brief 写到日志.
		 *
		 * @param 日志内容
		 */
		void write(const pair<int, string> &buffer);

		/**
		 * @brief 刷新缓存到文件
		 */
		void flush();

		/**
		 * @brief 设置染色是否生效.
		 *
		 * @param bEnable
		 */
		void enableDyeing(bool bEnable, const string& sKey)
		{
			INV_LockT<INV_ThreadMutex> lock(_mutexDyeing);

			if (bEnable)
			{
				_mapThreadID.insert(std::make_pair(pthread_self(), sKey));
				//_setThreadID.insert(pthread_self());
			}
			else
			{
                _mapThreadID.erase(pthread_self());
				//_setThreadID.erase(pthread_self());
			}

			_bDyeingFlag = (_mapThreadID.size() > 0);
			//_bDyeingFlag = (_setThreadID.size() > 0);
		}

	protected:

		/**
		 * buffer
		 */
		INV_ThreadQueue<pair<int, string> >  _buffer;

		/**
		 * 锁
		 */
		INV_ThreadMutex          _mutex;

		/**
		 * 线程组
		 */
		INV_LoggerThreadGroup    *_pThreadGroup;

		
		static bool 			_bDyeingFlag;

		
		static INV_ThreadMutex	_mutexDyeing;

		
		// static hash_map<pthread_t, string> _mapThreadID;
		static logger_map_type<pthread_t, string> _mapThreadID;
	};

	typedef INV_AutoPtr<INV_LoggerRoll> INV_LoggerRollPtr;

//////////////////////////////////////////////////////////////////////////////
//
/**
 * @brief 写日志线程组.
 *
 * 关键点:注册日志后,会保存职能指针,保证日志对象一直存在
 */
	class INV_LoggerThreadGroup : public INV_ThreadLock
	{
	public:
		/**
		 * @brief 构造函数
		 */
		INV_LoggerThreadGroup();

		/**
		 * @brief 析够函数
		 */
		~INV_LoggerThreadGroup();

		/**
		 * @brief 启动线程.
		 *
		 * @param iThreadNum线程数
		 */
		void start(size_t iThreadNum);

		/**
		 * @brief 注册logger对象.
		 *
		 * @param l INV_LoggerRollPtr对象
		 */
		void registerLogger(INV_LoggerRollPtr &l);

		/**
		 * @brief 卸载logger对象.
		 *
		 * @param l INV_LoggerRollPtr对象
		 */
		void unRegisterLogger(INV_LoggerRollPtr &l);

		/**
		 * @brief 刷新所有的数据
		 */
		void flush();

	protected:

		/**
		 * @brief 写日志
		 */
		void run();

		/**
		 * @brief 指针比较
		 */
		struct KeyComp
		{
			bool operator()(const INV_LoggerRollPtr& p1, const INV_LoggerRollPtr& p2) const
			{
				return p1.get() <  p2.get();
			}
		};

		typedef set<INV_LoggerRollPtr, KeyComp>  logger_set;

	protected:

		/**
		 * 结束
		 */
		bool            _bTerminate;

		/**
		 * 写线程
		 */
		INV_ThreadPool   _tpool;

		/**
		 * logger对象
		 */
		logger_set      _logger;

	};

/**
 * @brief 自定义logger buffer
 */
	class LoggerBuffer : public std::basic_streambuf<char>
	{
	public:
		/**
		 * @brief 定义最大的buffer大小(10M)
		 */
		enum
		{
			MAX_BUFFER_LENGTH = 1024*1024*10,
		};

		/**
		 * @brief 构造函数
		 */
		LoggerBuffer();

		/**
		 * @brief 构造函数.
		 *
		 * @param roll        INV_LoggerRollPtr对象
		 * @param buffer_len  buffer大小
		 */
		LoggerBuffer(INV_LoggerRollPtr roll, size_t buffer_len);

		/**
		 * @brief 析构函数
		 */
		~LoggerBuffer();

	protected:

		/**
		 * @brief 分配空间.
		 *
		 * @param n
		 */
		void reserve(std::streamsize n);

		/**
		 * @brief 放数据.
		 *
		 * @param s
		 * @param n
		 *
		 * @return streamsize
		 */
		virtual streamsize xsputn(const char_type* s, streamsize n);

		/**
		 * @brief buffer满了, 具体写数据.
		 *
		 * @param c
		 * @return int_type
		 */
		virtual int_type overflow(int_type c);

		/**
		 * @brief 读空间empty了(不实现).
		 *
		 * @return int_type
		 */
		virtual int_type underflow()    { return std::char_traits<char_type>::eof();}

		/**
		 * @brief 具体写逻辑.
		 *
		 * @return int
		 */
		virtual int sync();

	protected:
		LoggerBuffer(const LoggerBuffer&);
		LoggerBuffer& operator=(const LoggerBuffer&);

	protected:
		/**
		 * @brief 写日志
		 */
		INV_LoggerRollPtr    _roll;

		/**
		 * 缓冲区
		 */
		char*                       _buffer;

		/**
		 * 缓冲区大小
		 */
		std::streamsize     _buffer_len;
	};

/**
 * @brief 临时类, 析够的时候写日志
 */
	class LoggerStream
	{
	public:
		/**
		 * @brief 构造.
		 *
		 * @param stream
		 * @param mutex
		 */
		LoggerStream(ostream *stream, ostream *estream, INV_ThreadMutex &mutex) : _stream(stream), _estream(estream), _mutex(mutex)
		{
		}

		/**
		 * @brief 析构
		 */
		~LoggerStream()
		{
			if (_stream)
			{
				_stream->flush();
				_mutex.unlock();
			}
		}

		/**
		* @brief 重载<<
		*/
		template <typename P>
		LoggerStream& operator << (const P &t)  { if (_stream) *_stream << t;return *this;}

		/**
		 * @brief endl,flush等函数
		 */
		typedef ostream& (*F)(ostream& os);
		LoggerStream& operator << (F f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * @brief  hex等系列函数
		 */
		typedef ios_base& (*I)(ios_base& os);
		LoggerStream& operator << (I f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * @brief 字段转换成ostream类型.
		 *
		 * @return ostream&
		 */
		operator ostream&()
		{
			if (_stream)
			{
				return *_stream;
			}

			return *_estream;
		}

		//不实现
		LoggerStream(const LoggerStream& lt);
		LoggerStream& operator=(const LoggerStream& lt);

	protected:

		/**
		 * 输出流
		 */
		std::ostream    *_stream;

		/**
		 *
		 */
		std::ostream    *_estream;

		/**
		 * 锁
		 */
		INV_ThreadMutex  &_mutex;
	};

/**
 * @brief 日志基类
 */
	template<typename WriteT, template<class> class RollPolicy>
	class INV_Logger : public RollPolicy<WriteT>::RollWrapperI
	{
	public:

		/**
		 * @brief 设置显示标题
		 */
		enum
		{
			HAS_TIME   = 0x01,	/**秒级别时间*/
			HAS_PID    = 0x02,	/**进程ID*/
			HAS_LEVEL  = 0x04,	/**日志等级*/
			HAS_MTIME  = 0x08	/**毫秒级别时间(不建议使用, 会影响性能)*/
		};

		/**
		* @brief 枚举类型,定义日志的四种等级 .
		*/
		enum
		{
			NONE_LOG    = 1,
			ERROR_LOG   = 2,
			WARN_LOG    = 3,
			INFO_LOG    = 4,
            DEBUG_LOG   = 5
		};

		/**
		 * @brief 日志级别名称
		 */
		static const string LN[6];

		/**
		 * @brief 构造函数
		 */
		INV_Logger()
		: _flag(HAS_TIME)
		, _level(DEBUG_LOG)
		, _buffer(INV_LoggerRollPtr::dynamicCast(this->_roll), 1024)
		, _stream(&_buffer)
		, _ebuffer(NULL, 0)
		, _estream(&_ebuffer)
		,_sSepar("|")
		,_bHasSquareBracket(false)
		{
		}

		/**
		 * @brief 析够函数
		 */
		~INV_Logger()
		{
		}

		/**
		 * @brief 修改标题.
		 *
		 * @param flag
		 * @param add
		 */
		void modFlag(int flag, bool add = true)
		{
			if (add)
			{
				_flag |= flag;
			}
			else
			{
				_flag &= ~flag;
			}
		}

		/**
		 * @brief 是否有某标示.
		 *
		 * @return bool
		 */
		bool hasFlag(int flag) const    { return _flag & flag;}

		/**
		 * @brief 获取flag
		 * @return flag的值
		 */
		int getFlag() const             { return _flag;}

		/**
		* @brief 获取日志等级.
		*
		* @return int 等级
		*/
		int getLogLevel() const         { return _level;}

		/**
		* @brief 设置日志等级.
		*
		* @param level 等级
		* @return      成功设置返回0，否则返回-1
		*/
		int setLogLevel(int level)
		{
			if (!isLogLevelValid(level))
			{
				return -1;
			}

			_level = level;
			return 0;
		}

		/**
		 * @brief 设置等级.
		 *
		 * @param level
		 * @param int
		 */
		int setLogLevel(const string &level)
		{
			if (level == "ERROR")
			{
				return setLogLevel(ERROR_LOG);
			}
			else if (level == "WARN")
			{
				return setLogLevel(WARN_LOG);
			}
			else if (level == "DEBUG")
			{
				return setLogLevel(DEBUG_LOG);
			}
			else if (level == "NONE")
			{
				return setLogLevel(NONE_LOG);
			}
			else if (level == "INFO")
			{
				return setLogLevel(INFO_LOG);
			}
			return -1;
		}

		bool IsNeedLog(const string &level)
		{
			if (level == "ERROR")
			{
				return IsNeedLog(ERROR_LOG);
			}
			else if (level == "WARN")
			{
				return IsNeedLog(WARN_LOG);
			}
			else if (level == "DEBUG")
			{
				return IsNeedLog(DEBUG_LOG);
			}
			else if (level == "NONE")
			{
				return IsNeedLog(NONE_LOG);
			}
			else if (level == "INFO")
			{
				return IsNeedLog(INFO_LOG);
			}
			else
			{
				return true;
			}
		}
		bool IsNeedLog(int level)
		{
			return level <= _level;
		}
		/**
		 * @brief 框架中增加的日志内容之间的分割符，默认是"|"
		 * @param str
		 */
		void setSeparator(const string& str) {_sSepar = str;}

		/**
		 * @brief 框架中日期和时间之间是否需要加中括号[],有些统计有特殊需求；默认不加
		 * @param bEnable
		 */
		void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}

		/**
		* @brief DEBUG记日志
		*/
		LoggerStream info()     { return stream(INFO_LOG);}

		/**
		* @brief DEBUG记日志
		*/
		LoggerStream debug()    { return stream(DEBUG_LOG);}

		/**
		* @brief WARNING记日志
		*/
		LoggerStream warn()     { return stream(WARN_LOG);}

		/**
		* @brief ERROR记日志
		*/
		LoggerStream error()    { return stream(ERROR_LOG);}

		/**
		* @brief 记所有日志, 与等级无关
		*/
		LoggerStream any()      { return stream(0);}

	    LoggerStream log(int level) { return stream(level);}
	protected:

		/**
		 * @brief 获取头部信息.
		 *
		 * @param c
		 * @param len
		 * @param level
		 */
		void head(char *c, int len, int level)
		{
			size_t n = 0;

			if (hasFlag(INV_Logger::HAS_MTIME))
			{
				struct timeval t;
				INV_TimeProvider::getInstance()->getNow(&t);
				//gettimeofday(&t, NULL);

				tm tt;
				localtime_r(&t.tv_sec, &tt);
				const char *szFormat = (_bHasSquareBracket)?("[%04d-%02d-%02d %02d:%02d:%02d.%03ld]%s"):("%04d-%02d-%02d %02d:%02d:%02d.%03ld%s");
				n += snprintf(c + n, len-n, szFormat,
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, t.tv_usec/1000,_sSepar.c_str());
			}
			else if (hasFlag(INV_Logger::HAS_TIME))
			{
				time_t t = TNOW;
				tm tt;
				localtime_r(&t, &tt);
				const char *szFormat = (_bHasSquareBracket)?("[%04d-%02d-%02d %02d:%02d:%02d]%s"):("%04d-%02d-%02d %02d:%02d:%02d%s");
				n += snprintf(c + n, len-n, szFormat,
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec,_sSepar.c_str());
			}

			if (hasFlag(INV_Logger::HAS_PID))
			{
				n += snprintf(c + n, len - n, "%ld%s", syscall(SYS_gettid), _sSepar.c_str());
			}

			if (hasFlag(INV_Logger::HAS_LEVEL))
			{
				n += snprintf(c + n, len - n, "%s%s", INV_Logger::LN[level].c_str(), _sSepar.c_str());
			}
		}

		/**
		 * @brief 流.
		 *
		 * @param level
		 * @return LoggerStream
		 */
		LoggerStream stream(int level)
		{
			ostream *ost = NULL;

			if (level <= _level)
			{
				char c[128] = "\0";
				head(c, sizeof(c) - 1, level);

				_mutex.lock();
				ost = &_stream;
				_stream.clear();
				_stream << c;
			}

			return LoggerStream(ost, &_estream, _mutex);
		}

		/**
		* @brief 进程等级是否有效.
		*
		* @param level : 进程等级
		* @return bool,等级是否有效
		*/
		bool isLogLevelValid(int level)
		{
			switch (level)
			{
			case NONE_LOG:
			case ERROR_LOG:
			case WARN_LOG:
			case DEBUG_LOG:
			case INFO_LOG:
				return true;
				break;
			}

			return false;
		}

	protected:
		/**
		 * 显示头
		 */
		int             _flag;

		/**
		* 日志最高等级
		*/
		int             _level;

		/**
		 * buffer
		 */
		LoggerBuffer    _buffer;

		/**
		 * logger临时流
		 */
		std::ostream    _stream;

		/**
		 * 空buffer
		 */
		LoggerBuffer   _ebuffer;

		/**
		 * 空流
		 */
		std::ostream   _estream;

		/**
		 * 锁
		 */
		INV_ThreadMutex  _mutex;
		/**
		 * 分隔符
		 */
		 string        _sSepar;
		/**
		 * 日期部分是否加上[]
		 */
		 bool 		  _bHasSquareBracket;

	};

	template<typename WriteT, template<class> class RollPolicy>
	const string INV_Logger<WriteT, RollPolicy>::LN[6] = {"ANY", "NONE_LOG", "ERROR", "WARN", "INFO", "DEBUG"};

////////////////////////////////////////////////////////////////////////////////

	class RollWrapperInterface
	{
	public:

		virtual ~RollWrapperInterface() {}

		/**
		* @brief DEBUG记日志
		*/
		virtual LoggerStream info() = 0;

		/**
		* @brief DEBUG记日志
		*/
		virtual LoggerStream debug() = 0;

		/**
		* @brief WARNING记日志
		*/
		virtual LoggerStream warn() = 0;

		/**
		* @brief ERROR记日志
		*/
		virtual LoggerStream error() = 0;

		/**
		* @brief 记所有日志, 与等级无关
		*/
		virtual LoggerStream any() = 0;

		/**
		 *@brief 按照等级来输出日志
		 */
		virtual LoggerStream log(int level)=0;
		/**
		 * @brief 如果是异步调用，则马上进行刷新
		 */
		virtual void flush() = 0;
	};


	template<typename RollPolicyWriteT>
	class RollWrapperBase : public RollWrapperInterface
	{
	public:

		typedef INV_AutoPtr<RollPolicyWriteT>     RollPolicyWriteTPtr;

		/**
		 * @brief 构造
		 */
		RollWrapperBase()
		{
			_roll = new RollPolicyWriteT;
		}

		/**
		 * @brief 安装线程.
		 *
		 * @param ltg
		 */
		void setupThread(INV_LoggerThreadGroup *ltg) { _roll->setupThread(ltg);}

		/**
		 * @brief 取消线程
		 */
		void unSetupThread()                        { _roll->unSetupThread();}

		/**
		 * @brief 获取写对象.
		 *
		 * @return WriteT&
		 */
		typename RollPolicyWriteT::T &getWriteT()   { return _roll->getWriteT();}

		/**
		 * @brief 写日志.
		 *
		 * @param buffer
		 */
		void roll(const pair<int, string> &buffer)  { _roll->write(buffer);}

		/**
		* @brief 获取roll实例.
		 *
		 * @return RollPolicyWriteTPtr&
		 */
		RollPolicyWriteTPtr & getRoll()          	{return _roll;}

		/**
		 * @brief 异步刷新
		 */
		void flush()								{ _roll->flush(); }

	protected:

		/**
		 * @brief 具体写日志操作类
		 */
		RollPolicyWriteTPtr      _roll;

	};

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 日志滚动方法, 根据日志大小滚动
 */
	template<typename WriteT>
	class INV_RollBySize : public INV_LoggerRoll, public INV_ThreadMutex
	{
	public:
		typedef WriteT T;

		/**
		 * @brief 封装类(提供接口)
		 */
		class RollWrapperI : public RollWrapperBase<INV_RollBySize<WriteT> >
		{
		public:
			/**
			 * @brief 初始化.
			 *
			 * @param path
			 * @param iMaxSize, 字节
			 * @param iMaxNum
			 */
			void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
			{
				this->_roll->init(path, iMaxSize, iMaxNum);
			}

			/**
			 * @brief 获取日志路径.
			 *
			 * @return string
			 */
			string getPath()                    { return this->_roll->getPath();}

			/**
			 * @brief 设置文件路径
			 */
			void setPath(const string &path)    { this->_roll->setPath(path);}

			/**
			 * @brief 获取最大大小.
			 *
			 * @return int
			 */
			int getMaxSize()                    { return this->_roll->getMaxSize();}

			/**
			 * @brief 设置最大大小.
			 *
			 * @param maxSize
			 */
			void setMaxSize(int maxSize)        { this->_roll->setMaxSize(maxSize);}

			/**
			 * @brief 获取最大个数.
			 *
			 * @return int
			 */
			int getMaxNum()                     { return this->_roll->getMaxNum();}

			/**
			 * @brief 设置文件个数.
			 *
			 * @param maxNum
			 */
			void setMaxNum(int maxNum)          { this->_roll->setMaxNum(maxNum);}
		};

		/**
		 * @brief 构造函数
		 */
		INV_RollBySize() : _maxSize(5000000),_maxNum(10),_iUpdateCount(0), _lt(time(NULL))
		{
		}

		/**
		 * @brief 析够
		 */
		~INV_RollBySize()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * @brief 初始化.
		 *
		 * @param path, 文件名
		 * @param iMaxSize, 最大大小
		 * @param iMaxNum, 最大个数
		 */
		void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
		{
			INV_LockT<INV_ThreadMutex> lock(*this);

			_path   = path;
			_maxSize= iMaxSize;
			_maxNum = iMaxNum;
		}

		/**
		 * @brief 获取日志路径.
		 *
		 * @return string
		 */
		string getPath()                    { INV_LockT<INV_ThreadMutex> lock(*this); return _path;}

		/**
		 * @brief 设置路径.
		 */
		void setPath(const string &path)    { INV_LockT<INV_ThreadMutex> lock(*this); _path = path;}

		/**
		 * @brief 获取最大大小.
		 *
		 * @return int
		 */
		int getMaxSize()                    { INV_LockT<INV_ThreadMutex> lock(*this); return _maxSize;}

		/**
		 * @brief 设置最大大小.
		 *
		 * @param maxSize
		 * @return void
		 */
		void setMaxSize(int maxSize)        { INV_LockT<INV_ThreadMutex> lock(*this); return _maxSize = maxSize;}

		/**
		 * @brief 获取最大个数.
		 *
		 * @return int
		 */
		int getMaxNum()                     { INV_LockT<INV_ThreadMutex> lock(*this); return _maxNum;}

		/**
		 * @brief 设置最大个数.
		 *
		 * @param maxNum
		 */
		void setMaxNum(int maxNum)          { INV_LockT<INV_ThreadMutex> lock(*this); return _maxNum = maxNum;}

		/**
		 * @brief 获取写示例.
		 *
		 * @return T&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * 函数对象
		 *
		 * @param string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			INV_LockT<INV_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}

			time_t t = TNOW;
			time_t tt= t - _lt;
			//每隔5, 重新打开一次文件, 避免文件被删除后句柄不释放
			if (tt > 5 || tt < 0)
			{
				_lt = t;
				_of.close();
			}

			//检查日志文件是否打开
			if (!_of.is_open())
			{
				string sLogFileName = _path + ".log";
				_of.open(sLogFileName.c_str(), ios::app);

				string sLogFilePath = INV_File::extractFilePath(_path);

				if (!INV_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					INV_File::makeDirRecursive(sLogFilePath);
				}

				if (!_of)
				{
					//抛异常前继续进入_t 以便打远程日志
					_t(_of, buffer);

					throw INV_Logger_Exception("[INV_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
				}

			}

			_t(_of, buffer);

			if (tt <= 5)
			{
				return;
			}

			//文件大小小于限制, 直接返回
			if (_of.tellp() < _maxSize)
			{
				return;
			}

			//文件大小超出限制,删除最后一个文件
			string sLogFileName = _path + INV_Util::tostr(_maxNum - 1) + ".log";
			if (access(sLogFileName.c_str(), F_OK) == 0)
			{
				if (remove(sLogFileName.c_str()) < 0 )
				{
					return;
				}
			}

			//将log文件命名shift, xxx1.log=>xxx2.log,第一文件还是为xxx.log
			for (int i = _maxNum-2; i >= 0; i--)
			{
				if (i == 0)
				{
					sLogFileName = _path + ".log";
				}
				else
				{
					sLogFileName = _path + INV_Util::tostr(i) + ".log";
				}

				if (access(sLogFileName.c_str(), F_OK) == 0)
				{
					string sNewLogFileName = _path + INV_Util::tostr(i + 1) + ".log";
					rename(sLogFileName.c_str(), sNewLogFileName.c_str());
				}
			}

			_of.close();
			_of.open(sLogFileName.c_str(), ios::app);
			if (!_of)
			{
				throw INV_Logger_Exception("[INV_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
			}
		}

	protected:

		/**
		 * 文件路径
		 */
		string      _path;

		/**
		* 日志文件的最大大小
		*/
		int         _maxSize;

		/**
		* log文件最大数log文件命名为:xxx.log xxx1.log ...
		* xxx[_maxNum-1].log,_maxNum<=1 ?xxx.log
		*/
		int         _maxNum;

		/**
		 * 日志文件
		 */
		ofstream    _of;

		/**
		 * 具体写操作
		 */
		WriteT      _t;

		/**
		 * 多长时间检查一次文件大小
		 */
		short       _iUpdateCount;

		/**
		 * 多长时间检查一次文件大小
		 */
		time_t      _lt;
	};

	typedef INV_Logger<INV_DefaultWriteT, INV_RollBySize> INV_RollLogger;

   /**
    * @brief 根据时间滚动日志分隔类型
    */
	class InvLogType:public INV_HandleBase
    {
	public:
		InvLogType():_next_time_t(TNOW),_format("%Y%m%d"),_next_cut_time(INV_Util::tm2str(TNOW, "%Y%m%d")),_frequency(1),_des("day"){}
		virtual ~InvLogType(){}
		//频率值
		virtual size_t frequence()=0;

		virtual std::string get_init_time()
		{
			time_t tmp = _next_time_t - frequence();
			return INV_Util::tm2str(tmp, _format);
		}

		//返回空表示没有到下一个记录点
        virtual std::string get_new_time(time_t t)
		{
			if(t >= _next_time_t && INV_Util::tm2str(t,_format) >= _next_cut_time)
			{
			    //时间规整
				time_t new_nt = _next_time_t + ((t - _next_time_t) / frequence()) * frequence();
				_next_time_t = new_nt + frequence();
				_next_cut_time = INV_Util::tm2str(_next_time_t,_format);
	            return  INV_Util::tm2str(new_nt,_format);
			}
			else
			{
				return "";
			}
		}

		//转换成相应的字符串形式:1day,1hour,2minute
        inline std::string& toString()
        {
			return _des;
		}
	protected:
		//计算下一个时间点
        void init(const string &format = "%Y%m%d",size_t frequency=1)
		{
			_frequency = (frequency == 0?1:frequency);
			_format = format;

			_next_time_t = TNOW / frequence() + 1;
			_next_time_t *= frequence();

			_next_cut_time = INV_Util::tm2str(_next_time_t, _format);
		}
	protected:
		time_t _next_time_t;
		string _format;
        string _next_cut_time;
        size_t _frequency;
        string _des;
    };
    typedef INV_AutoPtr<InvLogType> InvLogTypePtr;

	class InvLogByDay:public InvLogType
	{
	public:
		static const string FORMAT;
	public:
		explicit InvLogByDay(const string &format = "%Y%m%d",size_t frequency=1)
        {
            init(format,frequency);
            _des = INV_Util::tostr(_frequency) + "day";
        }
	 	size_t frequence()
	 	{
			const size_t DAY_IN_SECOND = 60 * 60 * 24;
			return _frequency * DAY_IN_SECOND;
	 	}
	};

	class InvLogByHour:public InvLogType
	{
	public:
		static const string FORMAT;
	public:
		explicit InvLogByHour(const string &format = "%Y%m%d%H",size_t frequency=1)
        {
            init(format,frequency);
            _des = INV_Util::tostr(_frequency) + "hour";
        }

	 	size_t frequence()
	 	{
			const size_t HOUR_IN_SECOND = 60 * 60;
			return _frequency * HOUR_IN_SECOND;
	 	}
	};


	class InvLogByMinute:public InvLogType
	{
	public:
		static const string FORMAT;
 	public:
		explicit InvLogByMinute(const string &format = "%Y%m%d%H%M",size_t frequency=1)
        {
            init(format,frequency);
            _des = INV_Util::tostr(_frequency) + "minute";
        }
	 	size_t frequence()
	 	{
			const size_t MINUTE_IN_SECOND = 60;
			return _frequency * MINUTE_IN_SECOND;
	 	}
	};


   /**
    * @brief 根据时间滚动日志
    */
	template<typename WriteT>
	class INV_RollByTime : public INV_LoggerRoll, public INV_ThreadMutex
	{
	public:

		typedef WriteT T;

		/**
		 * @brief 封装类(接口类)
		 */
		class RollWrapperI : public RollWrapperBase<INV_RollByTime<WriteT> >
		{
		public:

			/**
			 * @brief 初始化.
			 *
			 * @param path,日志路径
			 * @param format，日志文件记录格式，按天，小时，分钟
			 * @param bHasSufix,日志文件是否添加".log"后缀
			 * @param sConcatstr,日志路径和时间字串之间的连接符,例如：app_log/test_20121210.log
			 * @param logTypePtr,日志记录类型，详见InvLogType
			 * @param bIsRemote,是否是远程日志实例
			 */
			void init(const string &path, const string &format = "%Y%m%d",bool bHasSufix=true,const string&sConcatstr="_",const InvLogTypePtr& logTypePtr=NULL,bool bIsRemote=false)
			{
				this->_roll->init(path, format,bHasSufix,sConcatstr,logTypePtr,bIsRemote);
			}

			/**
			 * @brief 获取日志路径.
			 *
			 * @return string
			 */
			string getPath()                        { return this->_roll->getPath();}

			/**
			 * @brief 设置文件路径
			 */
			void setPath(const string &path)        { this->_roll->setPath(path);}

			/**
			 * @brief 获取格式.
			 *
			 * @return string
			 */
			string getFormat()                      { return this->_roll->getFormat();}

			/**
			 * @brief 设置格式.
			 *
		 	 * @param format,支持按天，按小时，按分钟格式
			 */
			void setFormat(const string &format)    { this->_roll->setFormat(format);}
		};

		/**
		 * @brief 构造
		 */
		INV_RollByTime() : _lt(TNOW),_logTypePtr(NULL),_bRemoteType(false)
		{
			_bHasSufix = true;
			_sConcatStr = "_";
		}

		/**
		 * @brief 析够
		 */
		~INV_RollByTime()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * @brief 初始化.
		 *
		 * @param path, 文件路径
		 * @param format, 格式
		 * @param bHasSufix
		 * @param sConcatstr
		 * @param logTypePtr
		 * @param bIsRemote
		 */

		void init(const string &path, const string &format = "%Y%m%d",bool bHasSufix=true,const string &sConcatstr="_",const InvLogTypePtr& logTypePtr=NULL,bool bIsRemote=false)
		{
			INV_LockT<INV_ThreadMutex> lock(*this);

			_logTypePtr = logTypePtr;
			_path       = path;
			_format     = format;
			_bHasSufix = bHasSufix;
			_sConcatStr = sConcatstr;
			_bRemoteType = bIsRemote;

			if(_logTypePtr)
			{
				_currentTime = logTypePtr->get_init_time();
			}
			else
			{
				_currentTime = INV_Util::tm2str(TNOW, _format);
			}
		}

		/**
		 * @brief 获取日志路径.
		 *
		 * @return string
		 */
		string getPath()                    { INV_LockT<INV_ThreadMutex> lock(*this); return _path;}

		/**
		 * @brief 设置文件路径
		 */
		void setPath(const string &path)    { INV_LockT<INV_ThreadMutex> lock(*this); _path = path;}

		/**
		 * @brief 获取格式.
		 *
		 * @return string
		 */
		string getFormat()                  { INV_LockT<INV_ThreadMutex> lock(*this); return _format;}

		/**
		 * @brief 设置格式.
		 *
		 * @param format,支持按天，按小时，按分钟格式
		 */
		void setFormat(const string &format){ INV_LockT<INV_ThreadMutex> lock(*this); _format = format;}
		/**
		 * @brief 获取写示例.
		 *
		 * @return WriteT&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * @brief 函数对象.
		 *
		 * @param path
		 * @param of
		 * @return string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			INV_LockT<INV_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}
			//远程日志在本地不用打开文件
			if(_bRemoteType)
			{
				_t(_of, buffer);
				return;
			}

			time_t t = TNOW;

			string nowTime = "";
			//如果设置了记录类型,则使用记录类型来计算时间
			if(_logTypePtr)
			{
				nowTime = _logTypePtr->get_new_time(t);
				//时间跨越了
				if(nowTime != "")
				{
					_currentTime = nowTime;
					_of.close();
				}
				else
				{
					nowTime = _currentTime;
				}
			}
			else
			{
				nowTime = INV_Util::tm2str(t, _format);
				//检查时间是否跨时间了, 跨时间后关闭文件
				if (_currentTime != nowTime)
				{
					_currentTime = nowTime;
					_of.close();
				}
			}

			//每隔10s, 重新打开一次文件, 避免文件被删除后句柄不释放
			if (t - _lt > 10 || t - _lt < 0)
			{
				_lt = t;
				_of.close();
			}

			if (!_of.is_open())
			{
				string sLogFileName = _path + _sConcatStr + nowTime + (_bHasSufix?".log":"");
				string sLogFilePath = INV_File::extractFilePath(_path);
				if (!INV_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					INV_File::makeDirRecursive(sLogFilePath);
				}

				_of.open(sLogFileName.c_str(), ios::app);

				if (!_of)
				{
					//抛异常前继续进入_t 以便打远程日志
					_t(_of, buffer);
					throw INV_Logger_Exception("[INV_RollByTime::roll]:fopen fail: " + sLogFileName, errno);
				}
			}

			//写
			_t(_of, buffer);
		}

	protected:
		/**
		 * 文件路径
		 */
		string      _path;

		/**
		 * 时间格式
		 */
		string      _format;

		/**
		 * 上次roll时的时间
		 */
		string      _currentTime;

		/**
		 * 日志文件
		 */
		ofstream    _of;

		/**
		 * 具体写操作
		 */
		WriteT      _t;

		/**
		 * 多长时间检查一次文件大小
		 */
		time_t      _lt;
		/**
		 * 日志文件名是否带.log后缀
		 */
		bool					_bHasSufix;
		/**
		 * 日志文件名中用户自定义字符与日期字符间的连接符，默认是"_"
		 */
		string					_sConcatStr;


		/**
		 * 按天/小时/分钟输出日志时的记录类型
		 */

		InvLogTypePtr     _logTypePtr;
		/**
		 * 是否是远程日志实例
		 */
        bool              _bRemoteType;
	};

	typedef INV_Logger<INV_DefaultWriteT, INV_RollByTime> INV_DayLogger;
    
   ///////////////////////////////////////////////////////////////////////////////////////
   //
   /**
    * 写日志线程
    * 将写本地日志和远程分开到不同的线程
    * 作为单件存在, 且是永生不死的单件
    */
   class InvLoggerThread : public INV_Singleton<InvLoggerThread, CreateUsingNew, PhoneixLifetime>
   {
   public:
       /**
        * 构造函数
        */
       InvLoggerThread();

       /**
        * 析够函数
        */
       ~InvLoggerThread();

       /**
        * 本地写日志线程
        */
       INV_LoggerThreadGroup* local();

       /**
        * 远程写日志线程
        *
        * @return INV_LoggerThreadGroup*
        */
       INV_LoggerThreadGroup* remote();

   protected:

       /**
        * 本地线程组
        */
       INV_LoggerThreadGroup    _local;

       /**
        * 远程写线程组
        */
       INV_LoggerThreadGroup    _remote;
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   class TimeWriteT;

   /**
    * 远程的Log写操作类
    */
   class RemoteTimeWriteT
   {
   public:
   	RemoteTimeWriteT();
       ~RemoteTimeWriteT();

       /**
        * 构造函数
        */
       void setTimeWriteT(TimeWriteT *pTimeWrite);

       /**
        * 具体调用
        * @param of
        * @param buffer
        */
       void operator()(ostream &of, const deque<pair<int, string> > &buffer);

   protected:
       /**
        * 同步到远程
        */
       void sync2remote(const vector<string> &buffer);

       /**
        * 染色日志同步到远程
        */
       void sync2remoteDyeing(const vector<string> &buffer);

   protected:
       /**
        * 指针
        */
       TimeWriteT          *_pTimeWrite;

   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * 写Logger
    */
   class TimeWriteT
   {
   public:
       typedef INV_Logger<RemoteTimeWriteT, INV_RollByTime> RemoteTimeLogger;

       typedef INV_Logger<INV_DefaultWriteT, INV_RollByTime> DyeingTimeLogger;

       /**
        * 构造
        */
       TimeWriteT();

       /**
        * 析够
        */
       ~TimeWriteT();

       /**
        * 设置基本信息
        * @param app, 应用名称
        * @param server, 服务名称
        * @param file, 日志文件名
        * @param sFormat, 格式
        * @param setdivision,set名称
        * @param sLogType,日志记录类型
        */
       void setLogInfo(const string &sApp, const string &sServer, const string &sFile, const string &sLogpath, const string &sFormat,const string& setdivision="",const string& sLogType="");

       /**
        * 远程日志功能打开或关闭
        * @param bEnable
        */
       void enableRemote(bool bEnable)         { _bRemote = bEnable; }

       /**
        * 本地日志功能功能打开或关闭
        * @param bEnable
        */
       void enableLocal(bool bEnable);

       /**
        * 染色日志功能打开或关闭
        * @param bEnable
        */
       void enableDyeing (bool bEnable, const string& sDyeingKey = "");


   	/**
   	 * @brief 日志文件名是否带.log后缀
   	 * @param bEnable
   	 */
   	void enableSufix(bool bEnable=true){_bHasSufix = bEnable;}
   	/**
   	 * @brief 是否允许框架在日志文件名上增加业务相关的标识
   	 * @param bEnable
   	 */
   	void enablePrefix(bool bEnable=true){_bHasAppNamePrefix = bEnable;}
   	/**
   	 * @brief 日志文件名中用户自定义字符与日期字符间的连接符，默认是"_"
   	 * @param str
   	 */
   	void setFileNameConcatStr(const string& str) {_sConcatStr = str;}

   	/**
   	 * @brief 框架中增加的日志内容之间的分割符，默认是"|"
   	 * @param str
   	 */
   	void setSeparator(const string& str) {_sSepar = str;}

   	/**
   	 * @brief 框架中日期和时间之间是否需要加中括号[],有些统计由特殊需求；默认不加
   	 * @param bEnable
   	 */
   	void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}


       /**
        * 设置时间格式("%Y%m%d")
        * @param sFormat
        */
       void setFormat(const string &sFormat)   { _sFormat = sFormat;}

       /**
        * 具体调用
        * @param of
        * @param buffer
        */
       void operator()(ostream &of, const deque<pair<int, string> > &buffer);

   protected:

       /**
        * 友元
        */
       friend class RemoteTimeWriteT;

       /**
        * 记录错误文件
        * @param buffer
        */
       void writeError(const vector<string> &buffer);

       /**
        * 记录错误文件
        * @param buffer
        */
       void writeError(const deque<pair<int, string> > &buffer);

       /**
        * 初始化logger
        */
       void initError();

       /**
        * 初始化染色日志
        */
       void initDyeingLog();

   protected:

       /**
        * 远程时间日志
        */
       RemoteTimeLogger    *_pRemoteTimeLogger;

       /**
        * 本地功能
        */
       bool                _bLocal;

       /**
        * 远程功能
        */
       bool                _bRemote;

       /**
        * 远程服务句柄
        */

       /**
        * app名称
        */
       string              _sApp;

       /**
        * 服务名称
        */
       string              _sServer;

       /**
        * 日志文件名称
        */
       string              _sFile;

       /**
        * 时间格式
        */
       string              _sFormat;

       /**
        * 具体文件
        */
       string              _sFilePath;

       /**
        * 错误文件
        */
       INV_DayLogger        _elogger;

       /**
        * 缺省写模式
        */
       INV_DefaultWriteT    _wt;

       /**
        * 染色日志
        */
       static int                _bDyeing;

       /**
        * 染色日志目录路径
        */
       string              _sDyeingFilePath;

       /**
        * 远程时间日志
        */
       DyeingTimeLogger    *_pDyeingTimeLogger;

       /**
        * set分组信息
        */
       string                  _sSetDivision;

   	/**
   	 * 日志文件名是否带.log后缀
   	 */
   	bool					_bHasSufix;
   	/**
   	 * 是否允许框架在日志文件名上增加业务相关的标识
   	 */
   	bool 					_bHasAppNamePrefix;

   	/**
   	 * 日志文件名中用户自定义字符与日期字符间的连接符，默认是"_"
   	 */
   	string 					_sConcatStr;
   	/**
   	 * 分隔符
   	 */
   	 string        _sSepar;
   	/**
   	 * 日期部分是否加上[]
   	 */
   	 bool 		  _bHasSquareBracket;

   	 /*
   	  * 本地日志的记录类型,格式为InvLogType.toString()返回值,如果不采用InvLogType，则该值为""
   	  */
   	 string       _sLogType;
   };

   ////////////////////////////////////////////////////////////////////////////
   /**
    * 远程日志帮助类, 单件
    */
   class InvTimeLogger : public INV_HandleBase
                       , public INV_ThreadLock
                       , public INV_Singleton<InvTimeLogger, CreateUsingNew, DefaultLifetime>
   {
   public:

       //定义按时间滚动的日志
       typedef INV_Logger<TimeWriteT, INV_RollByTime> TimeLogger;

       /**
        * 构造
        */
       InvTimeLogger();

       /**
        * 析够
        */
       ~InvTimeLogger();

       /**
        * 设置本地信息
        * @param comm, 通信器
        * @param obj, 日志对象名称
        * @param app, 业务名称
        * @param server, 服务名称
        * @param logpath, 日志路径
        */
       void setLogInfo(const string &obj, const string &sApp, const string &sServer, const string &sLogpath,const string& setdivision="");

   	/**
        * 初始化设置时间格式("%Y%m%d")
        * 不要动态修改, 线程不安全
        * 如果有需要, 初始化后直接修改
        * @param sFormat, 文件名称, 为空表示缺省的时间日志
        */
   	void initFormat(const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr=NULL);
   	void initFormat(const string &sApp, const string &sServer,const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr=NULL);
       /**
        * 初始化设置时间格式("%Y%m%d")
        * 不要动态修改, 线程不安全
        * 如果有需要, 初始化后直接修改
        * @param sFormat, 文件名称, 为空表示缺省的时间日志
        * @param frequency 支持每多少天/小时/分钟，详见INV_logger.h中关于InvLogByDay,InvLogByHour,InvLogByMinute的描述
        *
        * 用法: 按两个小时记录日志
        * initFormat<InvLogByHour>("logfile",InvLogByHour::FORMAT,2);
        */
       template<class InvLogType>
       void initFormatWithType(const string &sFile, const string &sFormat,size_t frequency)
       {
           InvLogTypePtr logTypePtr = new InvLogType(sFormat,frequency);
           initFormat(sFile,sFormat,logTypePtr);
       }

       template<class InvLogType>
       void initFormatWithType(const string &sApp, const string &sServer,const string &sFile, const string &sFormat,size_t frequency)
       {
           InvLogTypePtr logTypePtr = new InvLogType(sFormat,frequency);
           initFormat(sApp,sServer,sFile,sFormat,logTypePtr);
       }
       /**
        * 获取时间日志
        * @param file
        */
       TimeLogger *logger(const string &sFile = "");

       /**
        * 获取时间日志
        * @param app, 业务名称
        * @param server, 服务名称
        * @param file
        */
       TimeLogger *logger(const string &sApp, const string &sServer,const string &sFile = "");

       /**
        * 同步写本地时间日志(远程日志一定是异步写的, 无法调整)
        * @param bSync
        */
       void sync(const string &sFile, bool bSync);

       /**
        * 远程时间日志
        * @param sFile, 文件名称, 为空表示缺省的时间日志
        * @param bEnable
        */
       void enableRemote(const string &sFile, bool bEnable);

       /**
        * 远程时间日志
        * @param sApp,应用名称
        * @param sServer,服务名称
        * @param sFile, 文件名称, 为空表示缺省的时间日志
        * @param bEnable
        */
   	void enableRemoteEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable);
       /**
        * 本地时间日志
        * @param sFile,文件名称, 为空表示缺省的时间日志
        * @param bEnable
        */
       void enableLocal(const string &sFile, bool bEnable);
       /**
        * 本地时间日志
        * @param sApp,应用名称
        * @param sServer,服务名称
        * @param sFile, 文件名称, 为空表示缺省的时间日志
        * @param bEnable
        */
   	void enableLocalEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable);

   	/**
   	 * @brief 日志文件名是否带.log后缀,影响全部日志文件
   	 * @param bEnable
   	 */
   	void enableSufix(bool bEnable=true){_bHasSufix = bEnable;}
   	/**
   	 * @brief 是否允许框架在日志文件名上增加业务相关的标识,影响全部日志文件
   	 * @param bEnable
   	 */
   	void enablePrefix(bool bEnable=true){_bHasAppNamePrefix = bEnable;}
   	/**
   	 * @brief 日志文件名中用户自定义字符与日期字符间的连接符，默认是"_",影响全部日志文件
   	 * @param str
   	 */
   	void setFileNameConcatStr(const string& str) {_sConcatStr = str;}

   	/**
   	 * @brief 框架中增加的日志内容之间的分割符，默认是"|",影响全部日志文件
   	 * @param str
   	 */
   	void setSeparator(const string& str) {_sSepar = str;}

   	/**
   	 * @brief 框架中日期和时间之间是否需要加中括号[],有些统计由特殊需求；默认不加,影响全部日志文件
   	 * @param bEnable
   	 */
   	void enableSqareWrapper(bool bEnable) {_bHasSquareBracket = bEnable;}
       /**
   	 * @brief 是否输出本地日志文件，影响全部日志文件
   	 * @param bEnable
        */
       void enableLocalLog(bool bEnable) {_bLocal = bEnable;}
   	/**
   	 * @brief 是否输出远程日志文件,影响全部日志文件
   	 * @param bEnable
   	 */
   	void enableRemoteLog(bool bEnable) {_bRemote = bEnable;}
   protected:

       /**
        * 初始化时间日志
        * @param pTimeLogger
        * @param sFile
        * @param sFormat
        * @param frequence, 每多少天/小时/分钟,单位是秒
        */
       void initTimeLogger(TimeLogger *pTimeLogger, const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr=NULL);

       /**
        * 初始化时间日志
        * @param pTimeLogger
        * @param sApp
        * @param sServer
        * @param sFile
        * @param sFormat
        * @param frequence, 每多少天/小时/分钟,单位是秒
        */

       void initTimeLogger(TimeLogger *pTimeLogger,const string &sApp, const string &sServer, const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr=NULL);

   protected:

       /**
        * 通信器
        */

       /**
        * 远程服务句柄
        */

       /**
        * 应用
        */
       string                  _sApp;

       /**
        * 服务名称
        */
       string                  _sServer;

       /**
        * 日志路径
        */
       string                  _sLogpath;

       /**
        * 缺省按天日志
        */
       TimeLogger              *_pDefaultLogger;

       /**
        * 远程日志
        */
       map<string, TimeLogger*>    _loggers;
       /**
        * set分组信息
        */
       string                  _sSetDivision;

   	/**
   	 * 是否带.log后缀
   	 */
   	bool					_bHasSufix;
   	/**
   	 * 是否允许框架在日志文件名上增加业务相关的标识
   	 */
   	bool 					_bHasAppNamePrefix;

   	/**
   	 * 日志文件名中用户自定义字符与日期字符间的连接符，默认是"_"
   	 */
   	string 					_sConcatStr;
   	/**
   	 * 分隔符
   	 */
   	string        _sSepar;
   	/**
   	 * 日期部分是否加上[]
   	 */
   	bool 		  _bHasSquareBracket;
       /**
        * 是否输出本地日志
        */
   	bool         _bLocal;
       /**
        * 是否输出远程日志
        */
   	bool        _bRemote;
   };

   class InvRollLogger;
   
   /**
    * 染色开关类，析构时关闭
    */
   class InvDyeingSwitch
   {
   public:
       /**
        * 构造函数，默认不打开染色日志
        */
       InvDyeingSwitch()
       :_bNeedDyeing(false)
       {
       }

       /**
        * 析构函数，关闭已打开的染色日志
        */
       ~InvDyeingSwitch()
   	{
   		if(_bNeedDyeing)
   		{
   			//InvRollLogger::getInstance()->enableDyeing(false);
   		}
   	}
    
   	static bool getDyeingKey(string & sDyeingkey)
   	{
   		return false;
   	}

       /**
        * 启用染色日志
        */
       void enableDyeing(const string & sDyeingKey = "")
   	{
   		//InvRollLogger::getInstance()->enableDyeing(true);

   		//_bNeedDyeing = true;
          // _sDyeingKey = sDyeingKey;
   	}

   protected:
       bool _bNeedDyeing;
       string _sDyeingKey;
   };

}

#endif

