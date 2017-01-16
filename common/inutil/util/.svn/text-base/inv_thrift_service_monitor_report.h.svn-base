/**
 * @file inv_thrift_service_monitor_report.h
 * @brief 使用ICE上报监控数据工具类
 * @author litang
 * @version 1.0
 * @date 2015-04-17
 */
#ifndef __INV_THRIFT_SERVICE_MONITOR_REPORT_H
#define __INV_THRIFT_SERVICE_MONITOR_REPORT_H

#include <Ice/Ice.h>
#include "util/inv_thrift_service_monitor.h"
#include "util/inv_thread.h"
#include "util/thrift_service_monitor.h"

namespace inv
{

/**
 * @brief 监控上报的ICE服务端接口实现
 */
class ThriftServiceMonitorI : public ThriftServiceMonitor 
{
public:
    virtual void reportMoniData(const MoniMap&, const Ice::Current&);
};

/**
 * @brief 监控上报的服务端APP类
 */
class INV_TriftServiceMonitorReportServerApp : virtual public Ice::Application 
{
public:
    /**
     * @brief 服务端App入口函数
     *
     * @param argc
     * @param argv[]
     *
     * @return 
     */
    virtual int run(int argc, char* argv[]);
};

/**
 * @brief 监控上报的客户端APP类
 */
class INV_TriftServiceMonitorReportClientApp : virtual public Ice::Application
{ 
public:
    /**
     * @brief 客户端App入口函数
     *
     * @param argc
     * @param argv[]
     *
     * @return 
     */
    virtual int run(int argc, char* argv[]);

private:
    /**
     * @brief 将监控到的数据类型转换成上报的数据类型,监控的数据类型直接使用c++原生类型定义,
     *        上报类型使用Ice的slice类型定义,再由slice2cpp映射成对应的C++类型
     *
     * @param src
     * @param dst
     */
    void convertMoniData(const inv::INV_ThriftServiceMonitor::MoniMap& src, inv::MoniMap* dst);

};

/**
 * @brief 上报监控数据的客户端线程
 */
class INV_TriftServiceMonitorReportClientThread : public  INV_Thread
{
public:
    INV_TriftServiceMonitorReportClientThread(Ice::Application &app, int argc, char **argv)
        : _app(app), _argc(argc), _argv(argv)
    {}

    virtual ~INV_TriftServiceMonitorReportClientThread(){}

protected:
    virtual void run();

private:
    Ice::Application &_app;
    int _argc;
    char **_argv;

};

}

#endif
