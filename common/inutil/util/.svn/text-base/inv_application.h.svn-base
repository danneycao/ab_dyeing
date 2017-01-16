#ifndef _INV_APPLICATION_H
#define _INV_APPLICATION_H

#include "log/inv_log.h"
#include "util/inv_argv.h"
#include "util/inv_config.h"

#ifdef MONITOR_REPORT_ENABLE
  #include "util/inv_thrift_service_monitor_report.h"
#endif

#include <ctime>
#include <vector>
#include <map>

#include <boost/type_traits/remove_const.hpp>
#include <protocol/TCompactProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/server/TNonblockingServer.h>

using namespace inv;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using boost::shared_ptr;

namespace inv
{

#define LEVEL_INV_LOGGER(LEVEL) do{\
	InvRollLogger::getInstance()->logger()->setLogLevel(LEVEL);\
}while(0);

#define INIT_INV_LOGGER(APP, SERVER) do{\
    InvRollLogger::getInstance()->setLogInfo(APP, SERVER, "./");\
    InvRollLogger::getInstance()->logger()->setLogLevel("DEBUG");\
    InvRollLogger::getInstance()->sync(false);\
    InvTimeLogger::getInstance()->setLogInfo("", APP, SERVER, "./");\
}while(0);

//TCompactProtocol 经过压缩
template<typename ThriftStruct>
std::string Thrift2String(const ThriftStruct& ts) {
	boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
	boost::shared_ptr<TTransport> trans(buffer);
	TCompactProtocol protocol(trans);
	ts.write(&protocol);
	uint8_t* buf;
	uint32_t size;
	buffer->getBuffer(&buf, &size);
	return std::string((char*)buf, (unsigned int)size);
}

//TCompactProtocol 经过压缩
template<typename ThriftStruct>
bool String2Thrift(const std::string& buff, ThriftStruct *ts) {

	if( !ts ) {
		return false;
	}

	try {
		boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
		buffer->write((const uint8_t*)buff.data(), buff.size());
		boost::shared_ptr<TTransport> trans(buffer);
		TCompactProtocol protocol(trans);
		ts->read(&protocol);
		return true;
	}
    catch(TTransportException &e) {
        FDLOG("error") << "String2Thrift:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;
        //cout << "String2Thrift1:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;

    }

    catch(TProtocolException &e ) {
        FDLOG("error") << "String2Thrift:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;

        //cout << "String2Thrift2:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;
    }

    return false;
}

//反序列化binary格式
template<typename ThriftStruct>
bool String2Thrift2(const std::string& buff, ThriftStruct *ts) {

	if (!ts) {
		return false;
	}

	try {
		boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
		buffer->write((const uint8_t*)buff.data(), buff.size());
		boost::shared_ptr<TTransport> trans(buffer);
		TBinaryProtocol protocol(trans);
		ts->read(&protocol);
		return true;
	}
	catch (TTransportException &e) {
		FDLOG("error") << "String2Thrift:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;
		//cout << "String2Thrift1:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;

	}

	catch (TProtocolException &e) {
		FDLOG("error") << "String2Thrift:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;

		//cout << "String2Thrift2:" << ts->ascii_fingerprint << " len:" << buff.length() << " err:" << e.what() << endl;
	}

	return false;
}

class INVThreadFactory : public PosixThreadFactory {

public:
	//记录thrift产生的线程id
	static std::vector<boost::shared_ptr<Thread> > thrift_threads;

	boost::shared_ptr<Thread> newThread(boost::shared_ptr<Runnable> runnable) const {

		boost::shared_ptr<Thread> th = PosixThreadFactory::newThread(runnable);
		INVThreadFactory::thrift_threads.push_back(th);
		
		return th;
	}

};

template<class INVHandler, class INVProcess>
class INVApplication {
public:

	void usage(char **argv) {
		cerr << "usage:" << argv[0] << " --config=config.conf --port=port --threadnum=num --Ice.Config=ice_config.conf" << endl;
	    exit(0);
	}

	void start(int argc, char **argv) {
		INV_Argv ag;
	    ag.decode(argc, argv);
	    string confpath = ag.getValue("config");
	    if(confpath == "")
	    {
	        usage(argv);
	    }

	    INV_Config config;
	    config.parseFile(confpath);

	    //监听端口
	    string strPort = config.get("/main/[port]");
	    if(strPort.length() == 0)
	    {
		    strPort = ag.getValue("port");
		    if(strPort == "")
		    {
		        usage(argv);
		    }
		}


		string strThreadNum = config.get("/main/[threadnum]");
		if(strThreadNum.length() == 0)
	    {
	    	strThreadNum = ag.getValue("threadnum");
		    if(strThreadNum == "")
		    {
		        usage(argv);
		    }
	    }

	    string strLoglevel = config.get("/main/[loglevel]");
	    if(strLoglevel == "")
	    {
	        strLoglevel = "DEBUG";
	    }
	    LEVEL_INV_LOGGER(strLoglevel);

#ifdef MONITOR_REPORT_ENABLE
        // 启动监控上报线程
        INV_TriftServiceMonitorReportClientApp app;
        INV_TriftServiceMonitorReportClientThread reportThread(app, argc, argv);
        reportThread.start();
#endif

	    std::srand(std::time(0));

	    int port = atoi(strPort.c_str());
	    int threadnum = atoi(strThreadNum.c_str());

	    cout << "threadnum:" << threadnum << endl;
	    
	    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());  
	    boost::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(threadnum);  
	    boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory > (new INVThreadFactory());
	    threadManager->threadFactory(threadFactory);  
	    threadManager->start();

	    boost::shared_ptr<INVHandler> handler(new INVHandler());
	    handler->init(confpath); 
	    boost::shared_ptr<TProcessor> processor(new INVProcess(handler));  

	    TNonblockingServer server(processor, protocolFactory, port, threadManager);  
	    server.serve(); 
	}
};

}

#endif
