#ifndef __INV_THREAD_MUTEX_H
#define __INV_THREAD_MUTEX_H

#include "util/inv_lock.h"

namespace inv
{

class INV_ThreadCond;

/**
 * @brief 线程互斥对象
 */
struct INV_ThreadMutex_Exception : public INV_Lock_Exception
{
    INV_ThreadMutex_Exception(const string &buffer) : INV_Lock_Exception(buffer){};
    INV_ThreadMutex_Exception(const string &buffer, int err) : INV_Lock_Exception(buffer, err){};
    ~INV_ThreadMutex_Exception() throw() {};
};

class INV_ThreadMutex
{
public:

    INV_ThreadMutex();
    virtual ~INV_ThreadMutex();

    /**
     * @brief 加锁
     */
    void lock() const;

    /**
     * @brief 尝试锁
     * 
     * @return bool
     */
    bool tryLock() const;

    /**
     * @brief 解锁
     */
    void unlock() const;

    bool willUnlock() const { return true;}

protected:

    // noncopyable
    INV_ThreadMutex(const INV_ThreadMutex&);
    void operator=(const INV_ThreadMutex&);

	/**
     * @brief 计数
	 */
    int count() const;

    /**
     * @brief 计数
	 */
    void count(int c) const;

    friend class INV_ThreadCond;

protected:
    mutable pthread_mutex_t _mutex;

};

/**
* @brief 线程锁类. 
*  
* 采用线程库实现
**/
class INV_ThreadRecMutex
{
public:

    /**
    * @brief 构造函数
    */
    INV_ThreadRecMutex();

    /**
    * @brief 析够函数
    */
    virtual ~INV_ThreadRecMutex();

    /**
	* @brief 锁, 调用pthread_mutex_lock. 
	*  
    * return : 返回pthread_mutex_lock的返回值
    */
    int lock() const;

    /**
	* @brief 解锁, pthread_mutex_unlock. 
	*  
    * return : 返回pthread_mutex_lock的返回值
    */
    int unlock() const;

    /**
	* @brief 尝试锁, 失败抛出异常. 
	*  
    * return : true, 成功锁; false 其他线程已经锁了
    */
    bool tryLock() const;

    /**
     * @brief 加锁后调用unlock是否会解锁, 给INV_Monitor使用的
     * 
     * @return bool
     */
    bool willUnlock() const;
protected:

	/**
     * @brief 友元类
     */
    friend class INV_ThreadCond;

	/**
     * @brief 计数
	 */
    int count() const;

    /**
     * @brief 计数
	 */
    void count(int c) const;

private:
    /**
    锁对象
    */
    mutable pthread_mutex_t _mutex;
	mutable int _count;
};

}

#endif

