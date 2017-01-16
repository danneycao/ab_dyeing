#ifndef __INV_THREAD_MUTEX_H
#define __INV_THREAD_MUTEX_H

#include "util/inv_lock.h"

namespace inv
{

class INV_ThreadCond;

/**
 * @brief �̻߳������
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
     * @brief ����
     */
    void lock() const;

    /**
     * @brief ������
     * 
     * @return bool
     */
    bool tryLock() const;

    /**
     * @brief ����
     */
    void unlock() const;

    bool willUnlock() const { return true;}

protected:

    // noncopyable
    INV_ThreadMutex(const INV_ThreadMutex&);
    void operator=(const INV_ThreadMutex&);

	/**
     * @brief ����
	 */
    int count() const;

    /**
     * @brief ����
	 */
    void count(int c) const;

    friend class INV_ThreadCond;

protected:
    mutable pthread_mutex_t _mutex;

};

/**
* @brief �߳�����. 
*  
* �����߳̿�ʵ��
**/
class INV_ThreadRecMutex
{
public:

    /**
    * @brief ���캯��
    */
    INV_ThreadRecMutex();

    /**
    * @brief ��������
    */
    virtual ~INV_ThreadRecMutex();

    /**
	* @brief ��, ����pthread_mutex_lock. 
	*  
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int lock() const;

    /**
	* @brief ����, pthread_mutex_unlock. 
	*  
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int unlock() const;

    /**
	* @brief ������, ʧ���׳��쳣. 
	*  
    * return : true, �ɹ���; false �����߳��Ѿ�����
    */
    bool tryLock() const;

    /**
     * @brief ���������unlock�Ƿ�����, ��INV_Monitorʹ�õ�
     * 
     * @return bool
     */
    bool willUnlock() const;
protected:

	/**
     * @brief ��Ԫ��
     */
    friend class INV_ThreadCond;

	/**
     * @brief ����
	 */
    int count() const;

    /**
     * @brief ����
	 */
    void count(int c) const;

private:
    /**
    ������
    */
    mutable pthread_mutex_t _mutex;
	mutable int _count;
};

}

#endif

