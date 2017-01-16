#ifndef __INV_SINGLETON_H__
#define __INV_SINGLETON_H__

#include "util/inv_monitor.h"
#include <cassert>
#include <cstdlib>

namespace inv
{

template<typename T>
class CreateUsingNew
{
public:
    /**
	 * @brief  创建.
	 *  
     * @return T*
     */
    static T* create() 
    { 
        return new T; 
    }

    /**
	 * @brief 释放. 
	 *  
     * @param t
     */
    static void destroy(T *t) 
    { 
        delete t; 
    }
};

template<typename T>
class CreateStatic
{
public:
	/**
	 * @brief   最大的空间
	 */ 
    union MaxAlign 
    { 
        char t_[sizeof(T)]; 
        long double longDouble_; 
    }; 

    /**
	 * @brief   创建. 
     * 
     * @return T*
     */
    static T* create() 
    { 
        static MaxAlign t; 
        return new(&t) T; 
    }

    /**
	 * @brief   释放. 
	 *  
     * @param t
     */
    static void destroy(T *t) 
    {
        t->~T();
    }
};

////////////////////////////////////////////////////////////////
/**
 * @brief 定义LifetimePolicy:定义对象的声明周期管理
 */
template<typename T>
class DefaultLifetime
{
public:
    static void deadReference()
    {
        throw std::logic_error("singleton object has dead.");
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        std::atexit(pFun);
    }
};

template<typename T>
class PhoneixLifetime
{
public:
    static void deadReference()
    {
        _bDestroyedOnce = true;
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        if(!_bDestroyedOnce)
            std::atexit(pFun);
    }
private:
    static bool _bDestroyedOnce; 
};
template <class T> 
bool PhoneixLifetime<T>::_bDestroyedOnce = false; 

template <typename T> 
struct NoDestroyLifetime 
{ 
    static void scheduleDestruction(T*, void (*)()) 
    {
    } 

    static void deadReference() 
    {
    } 
}; 

//////////////////////////////////////////////////////////////////////
// Singleton
template
<
    typename T,
    template<class> class CreatePolicy   = CreateUsingNew,
    template<class> class LifetimePolicy = DefaultLifetime
>
class INV_Singleton 
{
public:
    typedef T  instance_type;
    typedef volatile T volatile_type;

    /**
     * @brief 获取实例
     * 
     * @return T*
     */
    static T *getInstance()
    {
        //加锁, 双check机制, 保证正确和效率
        if(!_pInstance)
        {
            INV_ThreadLock::Lock lock(_tl);
            if(!_pInstance)
            {
                if(_destroyed)
                {
                    LifetimePolicy<T>::deadReference();
                    _destroyed = false;
                }
                _pInstance = CreatePolicy<T>::create();
                LifetimePolicy<T>::scheduleDestruction((T*)_pInstance, &destroySingleton);
            }
        }
        
        return (T*)_pInstance;
    }
    
    virtual ~INV_Singleton(){}; 

protected:

    static void destroySingleton()
    {
        assert(!_destroyed);
        CreatePolicy<T>::destroy((T*)_pInstance);
        _pInstance = NULL;
        _destroyed = true;
    }
protected:

    static INV_ThreadLock    _tl;
    static volatile T*      _pInstance;
    static bool             _destroyed;

protected:
    INV_Singleton(){}
    INV_Singleton (const INV_Singleton &); 
    INV_Singleton &operator=(const INV_Singleton &);
};

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
INV_ThreadLock INV_Singleton<T, CreatePolicy, LifetimePolicy>::_tl; 

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
bool INV_Singleton<T, CreatePolicy, LifetimePolicy>::_destroyed = false; 

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
volatile T* INV_Singleton<T, CreatePolicy, LifetimePolicy>::_pInstance = NULL; 

}

#endif
