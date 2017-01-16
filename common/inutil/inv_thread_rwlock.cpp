#include "util/inv_thread_rwlock.h"
#include <string.h>
#include <iostream>
#include <cassert>

namespace inv
{

INV_ThreadRWLocker::INV_ThreadRWLocker()
{
	int ret = ::pthread_rwlock_init(&m_sect, NULL);
	assert(ret == 0);

	if(ret != 0)
	{
		throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::INV_ThreadRWLocker] pthread_rwlock_init error", ret);
	}
}

INV_ThreadRWLocker::~INV_ThreadRWLocker()
{
	int ret = ::pthread_rwlock_destroy(&m_sect);
	if(ret != 0)
	{
		cerr<<"[INV_ThreadRWLocker::~INV_ThreadRWLocker] pthread_rwlock_destroy error:"<<string(strerror(ret))<<endl;;
	}

}

void INV_ThreadRWLocker::ReadLock() const
{
	int ret = ::pthread_rwlock_rdlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::ReadLock] pthread_rwlock_rdlock dead lock error", ret);
		}
		else
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::ReadLock] pthread_rwlock_rdlock error", ret);
		}
	}
}


void INV_ThreadRWLocker::WriteLock() const
{
	int ret = ::pthread_rwlock_wrlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::WriteLock] pthread_rwlock_wrlock dead lock error", ret);
		}
		else
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::WriteLock] pthread_rwlock_wrlock error", ret);
		}
	}

}

void INV_ThreadRWLocker::TryReadLock() const
{
	int ret = ::pthread_rwlock_tryrdlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::TryReadLock] pthread_rwlock_tryrdlock dead lock error", ret);
		}
		else
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::TryReadLock] pthread_rwlock_tryrdlock error", ret);
		}
	}

}

void INV_ThreadRWLocker::TryWriteLock() const
{
	int ret = ::pthread_rwlock_trywrlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::TryWriteLock] pthread_rwlock_trywrlock dead lock error", ret);
		}
		else
		{
			throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::TryWriteLock] pthread_rwlock_trywrlock error", ret);
		}
	}

}

void INV_ThreadRWLocker::Unlock() const
{
	int ret = ::pthread_rwlock_unlock(&m_sect);
	if(ret != 0)
	{
		throw INV_ThreadRW_Exception("[INV_ThreadRWLocker::Unlock] pthread_rwlock_unlock error", ret);
	}

}

}


