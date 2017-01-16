#include "util/inv_thread_pool.h"
#include "util/inv_util.h"

#include <iostream>

namespace inv
{

INV_ThreadPool::ThreadWorker::ThreadWorker(INV_ThreadPool *tpool)
: _tpool(tpool)
, _bTerminate(false)
{
}

void INV_ThreadPool::ThreadWorker::terminate()
{
    _bTerminate = true;
    _tpool->notifyT();
}

void INV_ThreadPool::ThreadWorker::run()
{
    //调用初始化部分
    INV_FunctorWrapperInterface *pst = _tpool->get();
    if(pst)
    {
        try
        {
            (*pst)();
        }
        catch ( ... )
        {
        }
        delete pst;
        pst = NULL;
    }

    //调用处理部分
    while (!_bTerminate)
    {
        INV_FunctorWrapperInterface *pfw = _tpool->get(this);
        if(pfw != NULL)
        {
            auto_ptr<INV_FunctorWrapperInterface> apfw(pfw);

            try
            {
                (*pfw)();
            }
            catch ( ... )
            {
            }

            _tpool->idle(this);
        }
    }

    //结束
    _tpool->exit();
}

//////////////////////////////////////////////////////////////
//
//

INV_ThreadPool::KeyInitialize INV_ThreadPool::g_key_initialize;
pthread_key_t INV_ThreadPool::g_key;

void INV_ThreadPool::destructor(void *p)
{
    ThreadData *ttd = (ThreadData*)p;
    if(ttd)
    {
        delete ttd;
    }
}

void INV_ThreadPool::exit()
{
    INV_ThreadPool::ThreadData *p = getThreadData();
    if(p)
    {
        delete p;
        int ret = pthread_setspecific(g_key, NULL);
        if(ret != 0)
        {
            throw INV_ThreadPool_Exception("[INV_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    _jobqueue.clear();
}

void INV_ThreadPool::setThreadData(INV_ThreadPool::ThreadData *p)
{
    INV_ThreadPool::ThreadData *pOld = getThreadData();
    if(pOld != NULL && pOld != p)
    {
        delete pOld;
    }

    int ret = pthread_setspecific(g_key, (void *)p);
    if(ret != 0)
    {
        throw INV_ThreadPool_Exception("[INV_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

INV_ThreadPool::ThreadData* INV_ThreadPool::getThreadData()
{
    return (ThreadData *)pthread_getspecific(g_key);
}

void INV_ThreadPool::setThreadData(pthread_key_t pkey, ThreadData *p)
{
    INV_ThreadPool::ThreadData *pOld = getThreadData(pkey);
    if(pOld != NULL && pOld != p)
    {
        delete pOld;
    }

    int ret = pthread_setspecific(pkey, (void *)p);
    if(ret != 0)
    {
        throw INV_ThreadPool_Exception("[INV_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

INV_ThreadPool::ThreadData* INV_ThreadPool::getThreadData(pthread_key_t pkey)
{
    return (ThreadData *)pthread_getspecific(pkey);
}

INV_ThreadPool::INV_ThreadPool()
: _bAllDone(true)
{
}

INV_ThreadPool::~INV_ThreadPool()
{
    stop();
    clear();
}

void INV_ThreadPool::clear()
{
    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        delete (*it);
        ++it;
    }

    _jobthread.clear();
    _busthread.clear();
}

void INV_ThreadPool::init(size_t num)
{
    stop();

    Lock sync(*this);

    clear();

    for(size_t i = 0; i < num; i++)
    {
        _jobthread.push_back(new ThreadWorker(this));
    }
}

void INV_ThreadPool::stop()
{
    Lock sync(*this);

    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        if ((*it)->isAlive())
        {
            (*it)->terminate();
            (*it)->getThreadControl().join();
        }
        ++it;
    }
    _bAllDone = true;
}

void INV_ThreadPool::start()
{
    Lock sync(*this);

    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        (*it)->start();
        ++it;
    }
    _bAllDone = false;
}

bool INV_ThreadPool::finish()
{
    return _startqueue.empty() && _jobqueue.empty() && _busthread.empty() && _bAllDone;
}

bool INV_ThreadPool::waitForAllDone(int millsecond)
{
    Lock sync(_tmutex);

start1:
    //任务队列和繁忙线程都是空的
    if (finish())
    {
        return true;
    }

    //永远等待
    if(millsecond < 0)
    {
        _tmutex.timedWait(1000);
        goto start1;
    }

    int64_t iNow= INV_Util::now2ms();
    int m       = millsecond;
start2:

    bool b = _tmutex.timedWait(millsecond);
    //完成处理了
    if(finish())
    {
        return true;
    }

    if(!b)
    {
        return false;
    }

    millsecond = max((int64_t)0, m  - (INV_Util::now2ms() - iNow));
    goto start2;

    return false;
}

INV_FunctorWrapperInterface *INV_ThreadPool::get(ThreadWorker *ptw)
{
    INV_FunctorWrapperInterface *pFunctorWrapper = NULL;
    if(!_jobqueue.pop_front(pFunctorWrapper, 1000))
    {
        return NULL;
    }

    {
        Lock sync(_tmutex);
        _busthread.insert(ptw);
    }

    return pFunctorWrapper;
}

INV_FunctorWrapperInterface *INV_ThreadPool::get()
{
    INV_FunctorWrapperInterface *pFunctorWrapper = NULL;
    if(!_startqueue.pop_front(pFunctorWrapper))
    {
        return NULL;
    }

    return pFunctorWrapper;
}

void INV_ThreadPool::idle(ThreadWorker *ptw)
{
    Lock sync(_tmutex);
    _busthread.erase(ptw);

    //无繁忙线程, 通知等待在线程池结束的线程醒过来
    if(_busthread.empty())
    {
		_bAllDone = true;
        _tmutex.notifyAll();
    }
}

void INV_ThreadPool::notifyT()
{
    _jobqueue.notifyT();
}



}
