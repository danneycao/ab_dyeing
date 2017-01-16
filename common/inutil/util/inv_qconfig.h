#ifndef _INV_QCONFIG_H_
#define _INV_QCONFIG_H_

#include "util/inv_util.h"
#include "util/inv_monitor.h"
#include "qconf.h"
#include <string>

namespace inv {

class INV_QConfig_Exception : public exception 
{
public:
    INV_QConfig_Exception(const std::string& buffer, int err) : m_errno(err) { m_buffer = buffer + " > (errno:" + INV_Util::tostr(err) + ")"; };
    ~INV_QConfig_Exception() throw(){};
    
    virtual const char* what() const throw();

private:
    void getBacktrace();

private:
    std::string m_buffer;
    int m_errno;
};

class INV_QConfig {
public:
    INV_QConfig();
    INV_QConfig(const INV_QConfig& qconf);

    INV_QConfig& operator = (const INV_QConfig& qconf);

public:
    std::string get(const std::string& path, const std::string& defaultval = "" ); 
    std::string getConfPath(const std::string& path, const string& defaultval = "");

private:
    static bool m_bInited;
    static INV_ThreadLock m_lock;
};

}

#endif
