#ifndef _INV_THREAD_COND_H
#define _INV_THREAD_COND_H

#include <sys/time.h>
#include <cerrno>
#include <iostream>
#include "util/inv_ex.h"

namespace inv
{

class INV_ThreadMutex;

/**
 *  @brief 线程条件异常类
 */
struct INV_ThreadCond_Exception : public INV_Exception
{
    INV_ThreadCond_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_ThreadCond_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_ThreadCond_Exception() throw() {};
};

class INV_ThreadCond
{
public:

    /**
     *  @brief 构造函数
     */
    INV_ThreadCond();

    /**
     *  @brief 析构函数
     */
    ~INV_ThreadCond();

    /**
     *  @brief 发送信号, 等待在该条件上的一个线程会醒
     */
    void signal();

    /**
     *  @brief 等待在该条件的所有线程都会醒
     */
    void broadcast();

    /**
     *  @brief 获取绝对等待时间
     */
    timespec abstime(int millsecond) const;

    /**
	 *  @brief 无限制等待.
	 *  
     * @param M
     */
    template<typename Mutex>
    void wait(const Mutex& mutex) const
    {
        int c = mutex.count();
        int rc = pthread_cond_wait(&_cond, &mutex._mutex);
        mutex.count(c);
        if(rc != 0)
        {
            throw INV_ThreadCond_Exception("pthread_cond_wait error", errno);
        }
    }

    /**
	 * @brief 等待时间. 
	 *  
	 * @param M 
     * @return bool, false表示超时, true:表示有事件来了
     */
    template<typename Mutex>
    bool timedWait(const Mutex& mutex, int millsecond) const
    {
        int c = mutex.count();

        timespec ts = abstime(millsecond);

        int rc = pthread_cond_timedwait(&_cond, &mutex._mutex, &ts);

        mutex.count(c);

        if(rc != 0)
        {
            if(rc != ETIMEDOUT)
            {
                throw INV_ThreadCond_Exception("pthread_cond_timedwait error", errno);
            }

            return false;
        }
        return true;
    }

protected:
    
    INV_ThreadCond(const INV_ThreadCond&);
    INV_ThreadCond& operator=(const INV_ThreadCond&);

private:

    mutable pthread_cond_t _cond;

};

}

#endif

