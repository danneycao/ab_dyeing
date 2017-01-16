#ifndef __INV_SERVER_HELPER_H
#define __INV_SERVER_HELPER_H

#include "util/inv_thread.h"
#include "log/inv_log.h"
#include "city.h"

namespace inv 
{

//配置文件线程
template<typename HandlerT>
class ConfThread : public INV_Thread, public INV_ThreadLock
{
public:
    ConfThread() : _bTerminate(false)
    {
    }

    int init(const string &confpath, int reload)
    {
        _confpath = confpath;
        _reload = reload;
        string content = INV_File::load2str(confpath);
        _confhash = CityHash64(content.data(), content.size());

        return 0;
    }

    int init(HandlerT *handle) 
    {
        _handle = handle;
        return 0;
    }

    void terminate()
    {
        _bTerminate = true;
        {
            INV_ThreadLock::Lock sync(*this);
            notifyAll();
        }
    }

    // overload this function if you want to do sth of your own business
    virtual void process() {}

protected:
    virtual void run() 
    {
        LOGMSG(InvRollLogger::DEBUG_LOG, "ConfThread run!" << pthread_self() << endl);
        while(!_bTerminate)
        {
            process();

            {
                INV_ThreadLock::Lock sync(*this);
                //timedWait(60000);
                timedWait(_reload * 1000);
            }
        }
    }

protected:
    bool _bTerminate;
    int _reload; //重载时间,单位秒
    string _confpath;
    uint64_t _confhash;
    HandlerT *_handle;
};

template<typename HandlerT>
class ReloadThread : public INV_Thread, public INV_ThreadLock
{
public:
    ReloadThread() : _bTerminate(false)
    {
    }

    int init(int reload)
    {
        _reload = reload;
        return 0;
    }

    int init(HandlerT *handle) 
    {
        _handle = handle;
        return 0;
    }

    void terminate()
    {
        _bTerminate = true;

        {
            INV_ThreadLock::Lock sync(*this);
            notifyAll();
        }
    }

    // overload this function if you want to do sth of your own business
    virtual void process() {}

protected:
    virtual void run() 
    {
        LOGMSG(InvRollLogger::DEBUG_LOG, "ReloadThread run!" << pthread_self() << endl);
        while(!_bTerminate)
        {
            process();

            {
                INV_ThreadLock::Lock sync(*this);
                //timedWait(60000);
                timedWait(_reload * 1000);
            }
        }
    }

protected:
    bool _bTerminate;
    int _reload; //重载时间,单位秒
    HandlerT *_handle;
};

} // namespace inv

#endif // __INV_SERVER_HELPER_H
