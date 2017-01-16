#ifndef __INV_THREAD_RWLOCK_H
#define __INV_THREAD_RWLOCK_H

#include <pthread.h>
#include "util/inv_lock.h"

#if !defined(linux) && (defined(__linux) || defined(__linux__))
#define linux
#endif

using namespace std;

namespace inv
{

/**
 * @brief读写锁异常类
 */
struct INV_ThreadRW_Exception : public INV_Exception
{
    INV_ThreadRW_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_ThreadRW_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_ThreadRW_Exception() throw() {};
};


#if !defined(linux) || (defined __USE_UNIX98 || defined __USE_XOPEN2K)
class INV_ThreadRWLocker
{
public:
	/**
     * @brief 构造函数
     */
	INV_ThreadRWLocker();

    /**
     * @brief 析够函数
     */
	~INV_ThreadRWLocker();

	/**
	 *@brief 读锁定,调用pthread_rwlock_rdlock
	 *return : 失败则抛异常INV_ThreadRW_Exception
	 */
	void ReadLock() const;

	/**
	 *@brief 写锁定,调用pthread_rwlock_wrlock
	 *return : 失败则抛异常INV_ThreadRW_Exception
	 */
	void WriteLock() const;

	/**
	 *@brief 尝试读锁定,调用pthread_rwlock_tryrdlock
	 *return : 失败则抛异常INV_ThreadRW_Exception
	 */
	void TryReadLock() const;

	/**
	 *@brief 尝试写锁定,调用pthread_rwlock_trywrlock
	 *return : 失败则抛异常INV_ThreadRW_Exception
	 */
	void TryWriteLock() const ;

	/**
	 *@brief 解锁,调用pthread_rwlock_unlock
	 *return : 失败则抛异常INV_ThreadRW_Exception
	 */
	void Unlock() const;

private:

	mutable pthread_rwlock_t m_sect;

	// noncopyable
	INV_ThreadRWLocker(const INV_ThreadRWLocker&);
	INV_ThreadRWLocker& operator=(const INV_ThreadRWLocker&);

};

typedef INV_RW_RLockT<INV_ThreadRWLocker> INV_ThreadRLock;
typedef INV_RW_WLockT<INV_ThreadRWLocker> INV_ThreadWLock;

#endif

}
#endif
