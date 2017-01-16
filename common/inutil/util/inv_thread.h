#ifndef	__INV_THREAD_H_
#define __INV_THREAD_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include "util/inv_ex.h"
#include "util/inv_monitor.h"

namespace inv
{

struct INV_ThreadThreadControl_Exception : public INV_Exception
{
    INV_ThreadThreadControl_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_ThreadThreadControl_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_ThreadThreadControl_Exception() throw() {};
};

/**
 * @brief  �߳̿�����
 */
class INV_ThreadControl
{
public:

    /**
	 * @brief  ����, ��ʾ��ǰ���е��̣߳�
	 *  		join��detach�ڲ����ڸö����ϵ���
	*/
	   
    INV_ThreadControl();

    /**
     * @return explicit
     */
    explicit INV_ThreadControl(pthread_t);

    /**
     * @brief  �ȴ���ǰ�߳̽���, �����ڵ�ǰ�߳��ϵ���
     */
    void join();

    /**
     * @brief  detach, �����ڵ�ǰ�߳��ϵ���
     */
    void detach();

    /**
     * @brief  ��ȡ��ǰ�߳�id.
     *
     * @return pthread_t��ǰ�߳�id
     */
    pthread_t id() const;

    /**
	 * @brief  ��Ϣmsʱ��. 
	 *  
     * @param millsecond ��Ϣ��ʱ�䣬����ms����
     */
    static void sleep(long millsecond);

    /**
     * @brief  ������ǰ�߳̿���Ȩ
     */
    static void yield();

private:

    pthread_t _thread;
};

/**
 *
 */
class INV_Runable
{
public:
    virtual ~INV_Runable(){};
    virtual void run() = 0;
};

/**
 * @brief �̻߳���. 
 * �̻߳��࣬�����Զ����̼̳߳��ڸ��࣬ͬʱʵ��run�ӿڼ���, 
 *  
 * ����ͨ��INV_ThreadContorl�����̡߳�
 */
class INV_Thread : public INV_Runable
{
public:

	/**
     * @brief  ���캯��
	 */
	INV_Thread();

	/**
     * @brief  ��������
	 */
	virtual ~INV_Thread(){};

	/**
     * @brief  �߳�����
	 */
	INV_ThreadControl start();

    /**
     * @brief  ��ȡ�߳̿�����.
     *
     * @return ThreadControl
     */
    INV_ThreadControl getThreadControl() const;

    /**
     * @brief  �߳��Ƿ���.
     *
     * @return bool ����true�����򷵻�false
     */
    bool isAlive() const;

	/**
     * @brief  ��ȡ�߳�id.
	 *
	 * @return pthread_t �߳�id
	 */
	pthread_t id() { return _tid; }

protected:

	/**
	 * @brief  ��̬����, �߳����. 
	 *  
	 * @param pThread �̶߳���
	 */
	static void threadEntry(INV_Thread *pThread);

	/**
     * @brief  ����
	 */
    virtual void run() = 0;

protected:
    /**
     * �Ƿ�������
     */
    bool            _running;

    /**
     * �߳�ID
     */
	pthread_t	    _tid;

    /**
     * �߳���
     */
    INV_ThreadLock   _lock;
};

}
#endif

