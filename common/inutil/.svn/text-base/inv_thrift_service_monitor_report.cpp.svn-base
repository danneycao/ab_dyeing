#include "util/inv_thrift_service_monitor_report.h"
#include "log/inv_log.h"

namespace inv
{

void ThriftServiceMonitorI::reportMoniData(const MoniMap& m, const Ice::Current& curr)
{
    Ice::IPConnectionInfoPtr connectionPtr = 
        Ice::IPConnectionInfoPtr::dynamicCast(curr.con->getInfo()); 
    // 格式化打印上报的监控数据
    MoniMap::const_iterator it = m.begin();
    for (; it != m.end(); ++it)
    {
        int succCnt = it->second.callCnt - it->second.excptCnt;
        if (succCnt < 1) succCnt = 1; // 防止除0
        FDLOG("thrift_service") << it->first
            << "|" << connectionPtr->remoteAddress
            << "|" << it->second.callCnt 
            << "|" << it->second.excptCnt
            << "|" << it->second.sumTimeUS/succCnt
            << "|" << it->second.maxTimeUS
            << "|" << it->second.minTimeUS
            << endl;
    }
}

int INV_TriftServiceMonitorReportServerApp::run(int argc, char* argv[])
{
    int status = 0;
    // 初始化LOG配置
    const char* APP = "INV";
    const char* SERVER = "MonitorReportIceServer";
    InvRollLogger::getInstance()->setLogInfo(APP, SERVER, "./");
    InvRollLogger::getInstance()->logger()->setLogLevel("DEBUG");
    InvRollLogger::getInstance()->sync(false);
    InvTimeLogger::getInstance()->setLogInfo("", APP, SERVER, "./");
    Ice::CommunicatorPtr ic;
    try 
    {
        ic = communicator();
        Ice::PropertiesPtr props = ic->getProperties();
        string adapter_name = props->getProperty("Adapter.Name");
        string endpoints = props->getProperty("Endpoints.Server");
        string servant = props->getProperty("Servant.Name");
        cerr << adapter_name << endl
            << endpoints << endl
            << servant << endl
            << endl;
        Ice::ObjectAdapterPtr adapter =
            ic->createObjectAdapterWithEndpoints(adapter_name, endpoints);
        Ice::ObjectPtr object = new ThriftServiceMonitorI;
        adapter->add(object, ic->stringToIdentity(servant));
        adapter->activate();
        ic->waitForShutdown();
    } 
    catch (const Ice::Exception& e) 
    {
        cerr << e << endl;
        status = 1;
    } 
    catch (const char* msg) 
    {
        cerr << msg << endl;
        status = 2;
    }
    return status;
}

int INV_TriftServiceMonitorReportClientApp::run(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    MoniMap monimap;
    while (true)
    {
        status = 0;
        try 
        {
            ic = communicator();
            Ice::PropertiesPtr props = ic->getProperties();
            string endpoints = props->getProperty("Endpoints.Client");
            string servant = props->getProperty("Servant.Name");
            int interval = props->getPropertyAsIntWithDefault("Report.Interval", 60);
            cerr << "monitor report ice endpoints: "<< endpoints << endl
                << "servant name: "<< servant << endl
                << "report interval(secs): " << interval << endl
                << endl;
            Ice::ObjectPrx base = ic->stringToProxy(servant+ ":" + endpoints);
            ThriftServiceMonitorPrx monitor = ThriftServiceMonitorPrx::checkedCast(base);
            if (!monitor)
            {
                throw "Invalid proxy";
            }
            // 循环上报监控数据
            while (true)
            {
                // 获取监控数据并转换
                convertMoniData(INV_ThriftServiceMonitor::getMoniRecord(), &monimap);
                // 清理现存监控数据
                INV_ThriftServiceMonitor::clearMoniRecord();
                // 上报监控数据数据
                monitor->reportMoniData(monimap);
                sleep(interval);
            }
        } 
        catch (const Ice::Exception& ex) 
        {
            FDLOG("error") << "ice exception: " << ex << endl;
            status = 1;
        } 
        catch (const char* msg) 
        {
            FDLOG("error") << "ice other error msg" << msg << endl;
            status = 2;
        }
        if (status)
        {
            // 发生异常,10秒后尝试重新连接监控上报服务器
            cerr << "Connect to monitor report server failed, try to reconnect 10s later..." << endl;
            sleep(10);
            continue;
        }
        // 监控上报正常结束
        break;
    }

    return status;
}

void INV_TriftServiceMonitorReportClientApp::convertMoniData(const inv::INV_ThriftServiceMonitor::MoniMap& src, inv::MoniMap* dst)
{
    dst->clear();
    inv::INV_ThriftServiceMonitor::MoniMap::const_iterator it = src.begin();
    for (; it != src.end(); ++it)
    {
        (*dst)[it->first].callCnt = it->second.call_cnt;
        (*dst)[it->first].excptCnt = it->second.excpt_cnt;
        (*dst)[it->first].sumTimeUS = it->second.sum_time_us;
        (*dst)[it->first].minTimeUS = it->second.min_time_us;
        (*dst)[it->first].maxTimeUS = it->second.max_time_us;
    }
    return;
}

void INV_TriftServiceMonitorReportClientThread::run()
{
    _app.main(_argc, _argv);
    return;
}

}
