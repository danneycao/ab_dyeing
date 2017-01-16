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
 * @brief  ��ģ�����������������ʹ�ã�
 * ����ʱ�������������ʱ�����
 */
template <typename T>
class INV_LockT
{
public:

    /**
	 * @brief  ���캯��������ʱ����
	 *  
     * @param mutex ������
     */
    INV_LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  ����������ʱ����
     */
    virtual ~INV_LockT()
    {
        if (_acquired)
        {
            _mutex.unlock();
        }
    }

    /**
     * @brief  ����, ����Ѿ�����,���׳��쳣
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
     * @brief  ��������.
     *
     * @return  �ɹ�����true�����򷵻�false
     */
    bool tryAcquire() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    /**
     * @brief  �ͷ���, ���û���Ϲ���, ���׳��쳣
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
     * @brief  �Ƿ��Ѿ�����.
     *
     * @return  ����true�Ѿ����������򷵻�false
     */
    bool acquired() const
    {
        return _acquired;
    }

protected:

    /**
	 * @brief ���캯��
	 * ���������Բ�������INV_LockT����
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
     * ������
     */
    const T&        _mutex;

    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;
};

/**
 * @brief  ��������
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
 * @brief  ����, �����κ�������
 */
class INV_EmptyMutex
{
public:
    /**
	* @brief  д��.
	*  
    * @return int, 0 ��ȷ
    */
    int lock()  const   {return 0;}

    /**
    * @brief  ��д��
    */
    int unlock() const  {return 0;}

    /**
	* @brief  ���Խ���. 
	*  
    * @return int, 0 ��ȷ
    */
    bool trylock() const {return true;}
};

/**
 * @brief  ��д������ģ����
 * ����ʱ�������������ʱ�����
 */

template <typename T>
class INV_RW_RLockT
{
public:
    /**
	 * @brief  ���캯��������ʱ����
	 *
     * @param lock ������
     */
	INV_RW_RLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.ReadLock();
		_acquired = true;
	}

    /**
	 * @brief ����ʱ����
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
	 *������
	 */
	const T& _rwLock;

    /**
     * �Ƿ��Ѿ�����
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
	 * @brief  ���캯��������ʱ����
	 *
     * @param lock ������
     */
	INV_RW_WLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.WriteLock();
		_acquired = true;
	}
    /**
	 * @brief ����ʱ����
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
	 *������
	 */
	const T& _rwLock;
    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;

	INV_RW_WLockT(const INV_RW_WLockT&);
	INV_RW_WLockT& operator=(const INV_RW_WLockT&);
};

};
#endif

