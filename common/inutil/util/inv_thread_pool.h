#ifndef __INV_THREAD_POOL_H_
#define __INV_THREAD_POOL_H_

#include "util/inv_thread.h"
#include "util/inv_thread_queue.h"
#include "util/inv_monitor.h"
#include "util/inv_functor.h"

#include <vector>
#include <set>
#include <iostream>

using namespace std;

namespace inv
{

struct INV_ThreadPool_Exception : public INV_Exception
{
	INV_ThreadPool_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_ThreadPool_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_ThreadPool_Exception() throw(){};
};

class INV_ThreadPool : public INV_ThreadLock
{
public:

    /**
     * @brief 构造函数
     *
     */
    INV_ThreadPool();

    /**
     * @brief 析构, 会停止所有线程
     */
    ~INV_ThreadPool();

    /**
	 * @brief 初始化. 
	 *  
     * @param num 工作线程个数
     */
    void init(size_t num);

    /**
     * @brief 获取线程个数.
     *
     * @return size_t 线程个数
     */
    size_t getThreadNum()   { Lock sync(*this); return _jobthread.size(); }

    /**
     * @brief 获取线程池的任务数(exec添加进去的).
     *
     * @return size_t 线程池的任务数
     */
    size_t getJobNum()     { return _jobqueue.size(); }

    /**
     * @brief 停止所有线程
     */
    void stop();

    /**
     * @brief 启动所有线程
     */
    void start();

    /**
	 * @brief 启动所有线程并, 执行初始化对象. 
	 *  
     * @param ParentFunctor
     * @param tf
     */
    template<class ParentFunctor>
    void start(const INV_FunctorWrapper<ParentFunctor> &tf)
    {
        for(size_t i = 0; i < _jobthread.size(); i++)
        {
            _startqueue.push_back(new INV_FunctorWrapper<ParentFunctor>(tf));
        }

        start();
    }

    /**
	 * @brief 添加对象到线程池执行，该函数马上返回， 
	 *  	  线程池的线程执行对象
	 */
    template<class ParentFunctor>
	void exec(const INV_FunctorWrapper<ParentFunctor> &tf)
    {
        _jobqueue.push_back(new INV_FunctorWrapper<ParentFunctor>(tf));
    }

    /**
     * @brief 等待所有工作全部结束(队列无任务, 无空闲线程).
     *
     * @param millsecond 等待的时间(ms), -1:永远等待
	 * @return            true, 所有工作都处理完毕 
	 * 				     false,超时退出
     */
    bool waitForAllDone(int millsecond = -1);

public:

    /**
     * @brief 线程数据基类,所有线程的私有数据继承于该类
     */
    class ThreadData
    {
    public:
        /**
         * @brief 构造
         */
        ThreadData(){};
        /**
         * @brief 析够
         */
        virtual ~ThreadData(){};

        /**
		 * @brief 生成数据. 
		 *  
		 * @param T 
         * @return ThreadData*
         */
        template<typename T>
        static T* makeThreadData()
        {
            return new T;
        }
    };

    /**
	 * @brief 设置线程数据. 
	 *  
     * @param p 线程数据
     */
    static void setThreadData(ThreadData *p);

    /**
     * @brief 获取线程数据.
     *
     * @return ThreadData* 线程数据
     */
    static ThreadData* getThreadData();

    /**
	 * @brief 设置线程数据, key需要自己维护. 
	 *  
     * @param pkey 线程私有数据key
     * @param p    线程指针 
     */
    static void setThreadData(pthread_key_t pkey, ThreadData *p);

    /**
	 * @brief 获取线程数据, key需要自己维护. 
	 *  
     * @param pkey 线程私有数据key
     * @return     指向线程的ThreadData*指针
     */
    static ThreadData* getThreadData(pthread_key_t pkey);

protected:

    /**
	 * @brief 释放资源. 
	 *  
     * @param p
     */
    static void destructor(void *p);

    /**
     * @brief 初始化key
     */
    class KeyInitialize
    {
    public:
        /**
         * @brief 初始化key
         */
        KeyInitialize()
        {
            int ret = pthread_key_create(&INV_ThreadPool::g_key, INV_ThreadPool::destructor);
            if(ret != 0)
            {
                throw INV_ThreadPool_Exception("[INV_ThreadPool::KeyInitialize] pthread_key_create error", ret);
            }
        }

        /**
         * @brief 释放key
         */
        ~KeyInitialize()
        {
            pthread_key_delete(INV_ThreadPool::g_key);
        }
    };

    /**
     * @brief 初始化key的控制
     */
    static KeyInitialize g_key_initialize;

    /**
     * @brief 数据key
     */
    static pthread_key_t g_key;

protected:
    /**
     * @brief 线程池中的工作线程
     */
    class ThreadWorker : public INV_Thread
    {
    public:
        /**
		 * @brief 工作线程构造函数. 
		 *  
         * @param tpool
         */
        ThreadWorker(INV_ThreadPool *tpool);

        /**
         * @brief 通知工作线程结束
         */
        void terminate();

    protected:
        /**
         * @brief 运行
         */
        virtual void run();

    protected:
        /**
         * 线程池指针
         */
        INV_ThreadPool   *_tpool;

        /**
         * 是否结束线程
         */
        bool            _bTerminate;
    };

protected:

    /**
     * @brief 清除
     */
    void clear();

    /**
     * @brief 获取任务, 如果没有任务, 则为NULL.
     *
     * @return INV_FunctorWrapperInterface*
     */
    INV_FunctorWrapperInterface *get(ThreadWorker *ptw);

    /**
     * @brief 获取启动任务.
     *
     * @return INV_FunctorWrapperInterface*
     */
    INV_FunctorWrapperInterface *get();

    /**
	 * @brief 空闲了一个线程. 
	 *  
     * @param ptw
     */
    void idle(ThreadWorker *ptw);

    /**
     * @brief 通知等待在任务队列上的工作线程醒来
     */
    void notifyT();

    /**
     * @brief 是否处理结束.
     *
     * @return bool
     */
    bool finish();

    /**
     * @brief 线程退出时调用
     */
    void exit();

    friend class ThreadWorker;
protected:

    /**
     * 任务队列
     */
    INV_ThreadQueue<INV_FunctorWrapperInterface*> _jobqueue;

    /**
     * 启动任务
     */
    INV_ThreadQueue<INV_FunctorWrapperInterface*> _startqueue;

    /**
     * 工作线程
     */
    std::vector<ThreadWorker*>                  _jobthread;

    /**
     * 繁忙线程
     */
    std::set<ThreadWorker*>                     _busthread;

    /**
     * 任务队列的锁
     */
    INV_ThreadLock                               _tmutex;

	/**
	 * 是否所有任务都执行完毕
	 */
	bool                                        _bAllDone;
};

}
#endif

