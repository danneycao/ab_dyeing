#ifndef _INV_LOCK_H
#define _INV_LOCK_H

#include <string>
#include <stdexcept>
#include <cerrno>
#include <pthread.h>
#include "util/inv_ex.h"

using namespace std;

namespace inv
{

struct INV_Lock_Exception : public INV_Exception
{
    INV_Lock_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_Lock_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_Lock_Exception() throw() {};
};

/**
 * @brief  锁模板类其他具体锁配合使用，
 * 构造时候加锁，析够的时候解锁
 */
template <typename T>
class INV_LockT
{
public:

    /**
	 * @brief  构造函数，构造时枷锁
	 *  
     * @param mutex 锁对象
     */
    INV_LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  析构，析构时解锁
     */
    virtual ~INV_LockT()
    {
        if (_acquired)
        {
            _mutex.unlock();
        }
    }

    /**
     * @brief  上锁, 如果已经上锁,则抛出异常
     */
    void acquire() const
    {
        if (_acquired)
        {
            throw INV_Lock_Exception("thread has locked!");
        }
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  尝试上锁.
     *
     * @return  成功返回true，否则返回false
     */
    bool tryAcquire() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    /**
     * @brief  释放锁, 如果没有上过锁, 则抛出异常
     */
    void release() const
    {
        if (!_acquired)
        {
            throw INV_Lock_Exception("thread hasn't been locked!");
        }
        _mutex.unlock();
        _acquired = false;
    }

    /**
     * @brief  是否已经上锁.
     *
     * @return  返回true已经上锁，否则返回false
     */
    bool acquired() const
    {
        return _acquired;
    }

protected:

    /**
	 * @brief 构造函数
	 * 用于锁尝试操作，与INV_LockT相似
	 *  
     */
    INV_LockT(const T& mutex, bool) : _mutex(mutex)
    {
        _acquired = _mutex.tryLock();
    }

private:

    // Not implemented; prevents accidental use.
    INV_LockT(const INV_LockT&);
    INV_LockT& operator=(const INV_LockT&);

protected:
    /**
     * 锁对象
     */
    const T&        _mutex;

    /**
     * 是否已经上锁
     */
    mutable bool _acquired;
};

/**
 * @brief  尝试上锁
 */
template <typename T>
class INV_TryLockT : public INV_LockT<T>
{
public:

    INV_TryLockT(const T& mutex) : INV_LockT<T>(mutex, true)
    {
    }
};

/**
 * @brief  空锁, 不做任何锁动作
 */
class INV_EmptyMutex
{
public:
    /**
	* @brief  写锁.
	*  
    * @return int, 0 正确
    */
    int lock()  const   {return 0;}

    /**
    * @brief  解写锁
    */
    int unlock() const  {return 0;}

    /**
	* @brief  尝试解锁. 
	*  
    * @return int, 0 正确
    */
    bool trylock() const {return true;}
};

/**
 * @brief  读写锁读锁模板类
 * 构造时候加锁，析够的时候解锁
 */

template <typename T>
class INV_RW_RLockT
{
public:
    /**
	 * @brief  构造函数，构造时枷锁
	 *
     * @param lock 锁对象
     */
	INV_RW_RLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.ReadLock();
		_acquired = true;
	}

    /**
	 * @brief 析构时解锁
     */
	~INV_RW_RLockT()
	{
		if (_acquired)
		{
			_rwLock.Unlock();
		}
	}
private:
	/**
	 *锁对象
	 */
	const T& _rwLock;

    /**
     * 是否已经上锁
     */
    mutable bool _acquired;

	INV_RW_RLockT(const INV_RW_RLockT&);
	INV_RW_RLockT& operator=(const INV_RW_RLockT&);
};

template <typename T>
class INV_RW_WLockT
{
public:
    /**
	 * @brief  构造函数，构造时枷锁
	 *
     * @param lock 锁对象
     */
	INV_RW_WLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.WriteLock();
		_acquired = true;
	}
    /**
	 * @brief 析构时解锁
     */
	~INV_RW_WLockT()
	{
		if(_acquired)
		{
			_rwLock.Unlock();
		}
	}
private:
	/**
	 *锁对象
	 */
	const T& _rwLock;
    /**
     * 是否已经上锁
     */
    mutable bool _acquired;

	INV_RW_WLockT(const INV_RW_WLockT&);
	INV_RW_WLockT& operator=(const INV_RW_WLockT&);
};

};
#endif

