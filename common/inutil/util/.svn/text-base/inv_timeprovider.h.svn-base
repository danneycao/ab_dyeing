#ifndef __INV_TIME_PROVIDER_H_
#define __INV_TIME_PROVIDER_H_

#include <string>
#include <string.h>
#include "util/inv_monitor.h"
#include "util/inv_thread.h"
#include "util/inv_autoptr.h"


#define TNOW     inv::INV_TimeProvider::getInstance()->getNow()
#define TNOWMS   inv::INV_TimeProvider::getInstance()->getNowMs()
#define TNOWUS   inv::INV_TimeProvider::getInstance()->getNowUs()

namespace inv
{

class INV_TimeProvider;

typedef INV_AutoPtr<INV_TimeProvider> INV_TimeProviderPtr;


class INV_TimeProvider : public INV_Thread, public INV_HandleBase
{
public:

    static INV_TimeProvider* getInstance();

    INV_TimeProvider() : _terminate(false),_use_tsc(true),_cpu_cycle(0),_buf_idx(0)
    {
        memset(_t,0,sizeof(_t));
        memset(_tsc,0,sizeof(_tsc));

        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        _t[0] = tv;
        _t[1] = tv;
    }


    ~INV_TimeProvider(); 


    time_t getNow()     {  return _t[_buf_idx].tv_sec; }


    void getNow(timeval * tv);


    int64_t getNowMs();

    int64_t getNowUs();

    float cpuMHz();

protected:

    virtual void run();

    static INV_ThreadLock        g_tl;

    static INV_TimeProviderPtr   g_tp;

private:
    void setTsc(timeval& tt);

    void addTimeOffset(timeval& tt,const int &idx);

protected:

    bool    _terminate;

    bool    _use_tsc;

private:
    float           _cpu_cycle; 

    volatile int    _buf_idx;

    timeval         _t[2];

    uint64_t        _tsc[2];  
};

}

#endif


