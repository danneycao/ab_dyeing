#include "log/inv_log.h"
#include "util/inv_file.h"
#include "util/inv_singleton.h"

#define DYEING_DIR "inv_dyeing"
#define DYEING_FILE "dyeing"

namespace inv
{

int TimeWriteT::_bDyeing = 0;

/////////////////////////////////////////////////////////////////////////////////////

RollWriteT::RollWriteT():_iMaxSize(10000), _iMaxNum(1)
{
}

RollWriteT::~RollWriteT()
{
}

void RollWriteT::operator()(ostream &of, const deque<pair<int, string> > &ds)
{
    deque<pair<int, string> >::const_iterator it = ds.begin();
    while(it != ds.end())
    {
        of << it->second;

        ++it;
    }
    of.flush();

}

void RollWriteT::setDyeingLogInfo(const string &sApp, const string &sServer, const string & sLogPath, int iMaxSize, int iMaxNum)
{
    _sApp     = sApp;
    _sServer  = sServer;
    _sLogPath = sLogPath;
    _iMaxSize = iMaxSize;
    _iMaxNum  = iMaxNum;
}


/////////////////////////////////////////////////////////////////////////////////////

void InvRollLogger::setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize, int iMaxNum)
{
    _sApp       = sApp;
    _sServer    = sServer;
    _sLogpath   = sLogpath;

    INV_File::makeDirRecursive(_sLogpath + "/" + _sApp + "/" + _sServer);

    _local.start(1);

    _logger.init(_sLogpath + "/" + _sApp + "/" + _sServer + "/" + _sApp + "." + _sServer, iMaxSize, iMaxNum);
    _logger.modFlag(INV_DayLogger::HAS_TIME, false);
    _logger.modFlag(INV_DayLogger::HAS_TIME|INV_DayLogger::HAS_LEVEL|INV_DayLogger::HAS_PID, true);

    sync(false);

    _logger.getWriteT().setDyeingLogInfo(sApp, sServer, sLogpath, iMaxSize, iMaxNum);

}


void InvRollLogger::sync(bool bSync)
{
    if(bSync)
    {
        _logger.unSetupThread();
    }
    else
    {
        _logger.setupThread(&_local);
    }
}

/////////////////////////////////////////////////////////////////////////////////////

InvLoggerThread::InvLoggerThread()
{
    _local.start(1);
    _remote.start(1);
}

InvLoggerThread::~InvLoggerThread()
{
    _local.flush();

    _remote.flush();
}

INV_LoggerThreadGroup* InvLoggerThread::local()
{
    return &_local;
}

INV_LoggerThreadGroup* InvLoggerThread::remote()
{
    return &_remote;
}

/////////////////////////////////////////////////////////////////////////////////////
RemoteTimeWriteT::RemoteTimeWriteT():_pTimeWrite(NULL)
{
}

RemoteTimeWriteT::~RemoteTimeWriteT()
{
}

void RemoteTimeWriteT::setTimeWriteT(TimeWriteT *pTimeWrite)
{
    _pTimeWrite = pTimeWrite;
}

void RemoteTimeWriteT::operator()(ostream &of, const deque<pair<int, string> > &buffer)
{
}

void RemoteTimeWriteT::sync2remote(const vector<string> &v)
{
    try
    {
    }
    catch(exception &ex)
    {
        INVLOG->error() << "[INV] write to remote log server error:" << ex.what() << ": buffer size:" << v.size() << endl;
        _pTimeWrite->writeError(v);
    }
}

void RemoteTimeWriteT::sync2remoteDyeing(const vector<string> &v)
{
    try
    {
    }
    catch(exception &ex)
    {
        INVLOG->error() << "[INV] write dyeing log to remote log server error:" << ex.what() << ": buffer size:" << v.size() << endl;
        _pTimeWrite->writeError(v);
    }
}
/////////////////////////////////////////////////////////////////////////////////////
//
TimeWriteT::~TimeWriteT()
{
    if(_pRemoteTimeLogger)
    {
        delete _pRemoteTimeLogger;
    }
}

TimeWriteT::TimeWriteT() : _pRemoteTimeLogger(NULL), _bLocal(true), _bRemote(true), _pDyeingTimeLogger(NULL),_sSetDivision(""),
	_bHasSufix(true),_bHasAppNamePrefix(true),_sConcatStr("_"),_sSepar("|"),_bHasSquareBracket(false),_sLogType("")
{
}

void TimeWriteT::setLogInfo(const string &sApp, const string &sServer, const string &sFile, const string &sLogpath, const string &sFormat,const string& setdivision,const string& sLogType)
{
    _sApp       = sApp;
    _sServer    = sServer;
    _sFormat    = sFormat;
    _sFile      = sFile;
	_sSetDivision = setdivision;
    _sLogType = sLogType;

	string sAppSrvName = _bHasAppNamePrefix?(_sApp + "." + _sServer):"";

    _sFilePath = sLogpath + "/" + _sApp + "/" + _sServer + "/" + sAppSrvName;
    if(!_sFile.empty())
    {
        _sFilePath += (_bHasAppNamePrefix?_sConcatStr:"") + sFile;
    }

    _sDyeingFilePath = sLogpath + "/"DYEING_DIR"/";

    _pRemoteTimeLogger = new RemoteTimeLogger();
    _pRemoteTimeLogger->init(_sFilePath, _sFormat,_bHasSufix,_sConcatStr,NULL,true);
    _pRemoteTimeLogger->modFlag(0xffff, false);
	_pRemoteTimeLogger->setSeparator(_sSepar);
	_pRemoteTimeLogger->enableSqareWrapper(_bHasSquareBracket);
    _pRemoteTimeLogger->setupThread(InvLoggerThread::getInstance()->remote());
    _pRemoteTimeLogger->getWriteT().setTimeWriteT(this);

    if(!_bLocal)
    {
        initError();
    }
}

void TimeWriteT::initDyeingLog()
{
    INV_File::makeDirRecursive(_sDyeingFilePath);

    _pDyeingTimeLogger = new DyeingTimeLogger();
    _pDyeingTimeLogger->init(_sDyeingFilePath + "/"DYEING_FILE, _sFormat);
    _pDyeingTimeLogger->modFlag(0xffff, false);
}

void TimeWriteT::initError()
{
    _elogger.init(_sFilePath + ".remote.error", _sFormat);
    _elogger.modFlag(0xffff, false);
}

void TimeWriteT::enableLocal(bool bEnable)
{
    _bLocal = bEnable;
    if(!_bLocal)
    {
        initError();
    }
}

void TimeWriteT::operator()(ostream &of, const deque<pair<int, string> > &buffer)
{

    if(_bLocal && of && !buffer.empty())
    {
        try
        {
            _wt(of, buffer);
        }
        catch(...)
        {
        }
    }

    if(_bRemote && _pRemoteTimeLogger && !buffer.empty())
    {
        deque<pair<int, string> >::const_iterator it = buffer.begin();
        while(it != buffer.end())
        {
            _pRemoteTimeLogger->any() << it->second;
            ++it;
        }
    }

    vector<string> vDyeingLog;
    deque<pair<int, string> >::const_iterator it = buffer.begin();
    while(it != buffer.end())
    {
        if(it->first != 0)
        {
            if(!_pDyeingTimeLogger)
            {
                initDyeingLog();
            }
            _pDyeingTimeLogger->any() << _sApp << "." << _sServer << "|" << it->second;

            vDyeingLog.push_back(_sApp + "." + _sServer + "|" + it->second);
        }
        ++it;
    }
}

void TimeWriteT::writeError(const vector<string> &buffer)
{
    if(!_bLocal)
    {
        for(size_t i = 0; i < buffer.size(); i++)
        {
            _elogger.any() << buffer[i];
        }
    }

    string sInfo = _sApp + "." + _sServer + "|";
    FDLOG("inverror") << sInfo <<endl;
}

void TimeWriteT::writeError(const deque<pair<int, string> > &buffer)
{
    if(!_bLocal)
    {
        deque<pair<int, string> >::const_iterator it = buffer.begin();
        while(it != buffer.end())
        {
            _elogger.any() << it->second;
            ++it;
        }
    }

    string sInfo = _sApp + "." + _sServer + "|";
    FDLOG("inverror") << sInfo <<endl;

}

/////////////////////////////////////////////////////////////////////////////////////

InvTimeLogger::InvTimeLogger() : _pDefaultLogger(NULL),_bHasSufix(true),_bHasAppNamePrefix(true),_sConcatStr("_"),_sSepar("|"),_bHasSquareBracket(false),_bLocal(true),_bRemote(true)
{
}

InvTimeLogger::~InvTimeLogger()
{
    if(_pDefaultLogger != NULL)
    {
        delete _pDefaultLogger;
    }

    map<string, TimeLogger*>::iterator it = _loggers.begin();
    while(it != _loggers.end())
    {
        delete it->second;
        ++it;
    }
    _loggers.clear();
}

void InvTimeLogger::initTimeLogger(TimeLogger *pTimeLogger, const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr)
{
	string sAppSrvName = _bHasAppNamePrefix?(_sApp + "." + _sServer):"";
    string sFilePath = _sLogpath + "/" + _sApp + "/" + _sServer + "/" + sAppSrvName;

    if(!sFile.empty())
    {
        sFilePath += (_bHasAppNamePrefix?_sConcatStr:"") + sFile;
    }

    pTimeLogger->init(sFilePath, sFormat,_bHasSufix,_sConcatStr,logTypePtr);
	pTimeLogger->modFlag(0xffff, false);
    pTimeLogger->modFlag(INV_DayLogger::HAS_TIME, true);
	pTimeLogger->setSeparator(_sSepar);
	pTimeLogger->enableSqareWrapper(_bHasSquareBracket);
    pTimeLogger->setupThread(InvLoggerThread::getInstance()->local());

	pTimeLogger->getWriteT().enableSufix(_bHasSufix);
	pTimeLogger->getWriteT().enablePrefix(_bHasAppNamePrefix);
	pTimeLogger->getWriteT().setFileNameConcatStr(_sConcatStr);
	pTimeLogger->getWriteT().setSeparator(_sSepar);
	pTimeLogger->getWriteT().enableSqareWrapper(_bHasSquareBracket);
    pTimeLogger->getWriteT().enableLocal(_bLocal);
    pTimeLogger->getWriteT().enableRemote(_bRemote);

    string sLogType = "";
    if(logTypePtr)
    {
        sLogType = logTypePtr->toString();
    }

    pTimeLogger->getWriteT().setLogInfo(_sApp, _sServer, sFile, _sLogpath, sFormat,_sSetDivision,sLogType);
}

void InvTimeLogger::initTimeLogger(TimeLogger *pTimeLogger,const string &sApp, const string &sServer, const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr)
{
	string sAppSrvName = _bHasAppNamePrefix?(sApp + "." + sServer):"";
    string sFilePath = _sLogpath + "/" + sApp + "/" + sServer + "/" + sAppSrvName;

    if(!sFile.empty())
    {
		sFilePath += (_bHasAppNamePrefix?_sConcatStr:"") + sFile;

    }

    pTimeLogger->init(sFilePath,sFormat,_bHasSufix,_sConcatStr,logTypePtr);
    pTimeLogger->modFlag(0xffff, false);
    pTimeLogger->modFlag(INV_DayLogger::HAS_TIME, true);
	pTimeLogger->setSeparator(_sSepar);
	pTimeLogger->enableSqareWrapper(_bHasSquareBracket);
    pTimeLogger->setupThread(InvLoggerThread::getInstance()->local());

	pTimeLogger->getWriteT().enableSufix(_bHasSufix);
	pTimeLogger->getWriteT().enablePrefix(_bHasAppNamePrefix);
	pTimeLogger->getWriteT().setFileNameConcatStr(_sConcatStr);
	pTimeLogger->getWriteT().setSeparator(_sSepar);
	pTimeLogger->getWriteT().enableSqareWrapper(_bHasSquareBracket);
    pTimeLogger->getWriteT().enableLocal(_bLocal);
    pTimeLogger->getWriteT().enableRemote(_bRemote);
    string sLogType = "";
    if(logTypePtr)
    {
        sLogType = logTypePtr->toString();
    }

    pTimeLogger->getWriteT().setLogInfo(sApp, sServer, sFile, _sLogpath, sFormat,_sSetDivision,sLogType);
}

void InvTimeLogger::setLogInfo(const string &obj, const string &sApp, const string &sServer, const string &sLogpath,const string& setdivision)
{
    _sApp       = sApp;
    _sServer    = sServer;
    _sLogpath   = sLogpath;
	_sSetDivision = setdivision;

    INV_File::makeDirRecursive(_sLogpath + "/" + _sApp + "/" + _sServer);
}

void InvTimeLogger::initFormat(const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr)
{
    if(sFile.empty())
    {
        if(!_pDefaultLogger)
        {
            _pDefaultLogger = new TimeLogger();

        }
        initTimeLogger(_pDefaultLogger, "", sFormat,logTypePtr);
    }
    else
    {
        string s = _sApp + "/" + _sServer + "/"+ sFile;
        Lock lock(*this);
        map<string, TimeLogger*>::iterator it = _loggers.find(s);
        if( it == _loggers.end())
        {
            TimeLogger *p = new TimeLogger();
            initTimeLogger(p, sFile, sFormat,logTypePtr);
            _loggers[s] = p;
            return;
        }

        initTimeLogger(it->second, sFile, sFormat,logTypePtr);
    }
}
void InvTimeLogger::initFormat(const string &sApp, const string &sServer,const string &sFile, const string &sFormat,const InvLogTypePtr& logTypePtr)
{
    string s = sApp + "/" + sServer + "/"+ sFile;
    Lock lock(*this);
    map<string, TimeLogger*>::iterator it = _loggers.find(s);
    if( it == _loggers.end())
    {
        TimeLogger *p = new TimeLogger();
        initTimeLogger(p, sApp, sServer, sFile, sFormat,logTypePtr);
        _loggers[s] = p;
        return;
    }

    initTimeLogger(it->second, sApp, sServer, sFile, sFormat,logTypePtr);
}

InvTimeLogger::TimeLogger* InvTimeLogger::logger(const string &sFile)
{
    if(sFile.empty())
    {
        if(!_pDefaultLogger)
        {
            _pDefaultLogger = new TimeLogger();
            initTimeLogger(_pDefaultLogger, "", "%Y%m%d");
        }
        return _pDefaultLogger;
    }

    string s = _sApp + "/" + _sServer + "/"+ sFile;
    Lock lock(*this);
    map<string, TimeLogger*>::iterator it = _loggers.find(s);
    if( it == _loggers.end())
    {
        TimeLogger *p = new TimeLogger();
        initTimeLogger(p, sFile, "%Y%m%d");
        _loggers[s] = p;
        return p;
    }

    return it->second;
}

InvTimeLogger::TimeLogger* InvTimeLogger::logger(const string &sApp, const string &sServer,const string &sFile)
{
    string s = sApp + "/" + sServer + "/"+ sFile;

    Lock lock(*this);
    map<string, TimeLogger*>::iterator it = _loggers.find(s);
    if( it == _loggers.end())
    {
        TimeLogger *p = new TimeLogger();
        initTimeLogger(p, sApp, sServer, sFile, "%Y%m%d");
        _loggers[s] = p;
        return p;
    }

    return it->second;
}


void InvTimeLogger::sync(const string &sFile, bool bSync)
{
    if(bSync)
    {
        logger(sFile)->unSetupThread();
    }
    else
    {
        logger(sFile)->setupThread(InvLoggerThread::getInstance()->local());
    }
}

void InvTimeLogger::enableRemote(const string &sFile, bool bEnable)
{
    logger(sFile)->getWriteT().enableRemote(bEnable);
}

void InvTimeLogger::enableRemoteEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable)
{
	logger(sApp,sServer,sFile)->getWriteT().enableRemote(bEnable);
}
void InvTimeLogger::enableLocal(const string &sFile, bool bEnable)
{
    logger(sFile)->getWriteT().enableLocal(bEnable);
}

void InvTimeLogger::enableLocalEx(const string &sApp, const string &sServer,const string &sFile, bool bEnable)
{
	logger(sApp,sServer,sFile)->getWriteT().enableLocal(bEnable);
}

}
