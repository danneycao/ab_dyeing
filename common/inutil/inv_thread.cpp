#include "util/inv_thread.h"
#include <cerrno>

namespace inv
{

INV_ThreadControl::INV_ThreadControl(pthread_t thread) : _thread(thread)
{
}

INV_ThreadControl::INV_ThreadControl() : _thread(pthread_self())
{
}

void INV_ThreadControl::join()
{
    if(pthread_self() == _thread)
    {
        throw INV_ThreadThreadControl_Exception("[INV_ThreadControl::join] can't be called in the same thread");
    }

    void* ignore = 0;
    int rc = pthread_join(_thread, &ignore);
    if(rc != 0)
    {
        throw INV_ThreadThreadControl_Exception("[INV_ThreadControl::join] pthread_join error ", rc);
    }
}

void INV_ThreadControl::detach()
{
    if(pthread_self() == _thread)
    {
        throw INV_ThreadThreadControl_Exception("[INV_ThreadControl::join] can't be called in the same thread");
    }

    int rc = pthread_detach(_thread);
    if(rc != 0)
    {
        throw INV_ThreadThreadControl_Exception("[INV_ThreadControl::join] pthread_join error", rc);
    }
}

pthread_t INV_ThreadControl::id() const
{
    return _thread;
}

void INV_ThreadControl::sleep(long millsecond)
{
    struct timespec ts;
    ts.tv_sec = millsecond / 1000;
    ts.tv_nsec = millsecond % 1000;
    nanosleep(&ts, 0);
}

void INV_ThreadControl::yield()
{
    sched_yield();
}

INV_Thread::INV_Thread() : _running(false),_tid(-1)
{
}

void INV_Thread::threadEntry(INV_Thread *pThread)
{
    pThread->_running = true;

	//SEGV_THREAD_ENTRY;

    {
        INV_ThreadLock::Lock sync(pThread->_lock);
        pThread->_lock.notifyAll();
    }

    try
    {
        pThread->run();
    }
    catch(...)
    {
        pThread->_running = false;
        throw;
    }
    pThread->_running = false;
}

INV_ThreadControl INV_Thread::start()
{
    INV_ThreadLock::Lock sync(_lock);

    if(_running)
    {
        throw INV_ThreadThreadControl_Exception("[INV_Thread::start] thread has start");
    }

    int ret = pthread_create(&_tid,
                   0,
                   (void *(*)(void *))&threadEntry,
                   (void *)this);

    if(ret != 0)
    {
        throw INV_ThreadThreadControl_Exception("[INV_Thread::start] thread start error", ret);
    }

    _lock.wait();

    return INV_ThreadControl(_tid);
}

INV_ThreadControl INV_Thread::getThreadControl() const
{
    return INV_ThreadControl(_tid);
}

bool INV_Thread::isAlive() const
{
    return _running;
}

}

