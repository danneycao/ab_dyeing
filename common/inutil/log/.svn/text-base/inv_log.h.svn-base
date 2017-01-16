#ifndef __INV_LOGGER_H__
#define __INV_LOGGER_H__

#include "util/inv_logger.h"

namespace inv
{

class RollWriteT
{
public:
    RollWriteT();
    ~RollWriteT();

    void operator()(ostream &of, const deque<pair<int, string> > &ds);

    void setDyeingLogInfo(const string &sApp, const string &sServer, const string & sLogPath,
            int iMaxSize, int iMaxNum);

protected:

    string _sApp;
    string _sServer;
    string _sLogPath;
    int _iMaxSize;
    int _iMaxNum;

};

class InvRollLogger : public INV_Singleton<InvRollLogger, CreateUsingNew, PhoneixLifetime>
{
public:
		enum
		{
			NONE_LOG    = 1,	
			ERROR_LOG   = 2,	
			WARN_LOG    = 3,	
            INFO_LOG    = 4,	
			DEBUG_LOG   = 5	
		};
public:
    typedef INV_Logger<RollWriteT, INV_RollBySize> RollLogger;

    void setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize = 1024*1024*50, int iMaxNum = 10);

    void sync(bool bSync = true);

    RollLogger *logger()          { return &_logger; }

protected:

    string                  _sApp;

    string                  _sServer;

    string                  _sLogpath;

    RollLogger           _logger;

    INV_LoggerThreadGroup    _local;

};


#define INVLOG             (InvRollLogger::getInstance()->logger())


#define LOGMSG(level,msg...) do{ if(INVLOG->IsNeedLog(level)) INVLOG->log(level)<<msg;}while(0)

#define TLOGINFO(msg...)    LOGMSG(InvRollLogger::INFO_LOG,msg)
#define TLOGDEBUG(msg...)   LOGMSG(InvRollLogger::DEBUG_LOG,msg)
#define TLOGWARN(msg...)    LOGMSG(InvRollLogger::WARN_LOG,msg)
#define TLOGERROR(msg...)   LOGMSG(InvRollLogger::ERROR_LOG,msg)

#define DLOG            (InvTimeLogger::getInstance()->logger()->any())
#define FDLOG(x)        (InvTimeLogger::getInstance()->logger(x)->any())
#define FFDLOG(x,y,z)   (InvTimeLogger::getInstance()->logger(x,y,z)->any())

}

#endif
