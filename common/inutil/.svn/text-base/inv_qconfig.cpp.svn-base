#include "util/inv_qconfig.h"
#include "util/inv_ex.h"
#include "util/inv_file.h"
#include "util/inv_util.h"
#include <execinfo.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <cerrno>

namespace inv {

bool INV_QConfig::m_bInited = false;
INV_ThreadLock INV_QConfig::m_lock;

INV_QConfig::INV_QConfig()
{
    if (!m_bInited)
    {
        INV_ThreadLock::Lock lock(m_lock);
        if (!m_bInited)
        {
            int ret = qconf_init();
            if (ret != QCONF_OK)
            {
                throw INV_QConfig_Exception("qconf_init", ret);
            }

            m_bInited = true;
        }
    }
}

INV_QConfig::INV_QConfig(const INV_QConfig& qconf)
{

}

INV_QConfig& INV_QConfig::operator = (const INV_QConfig& qconf)
{
    return *this;
}

std::string INV_QConfig::get(const std::string& path, const string& defaultval)
{
    char* value = new char[QCONF_CONF_BUF_MAX_LEN];
    int ret = qconf_get_conf(path.c_str(), value, QCONF_CONF_BUF_MAX_LEN, NULL);
    if (ret == QCONF_OK)
    {
        string val = value;
        delete value;
        return val;
    }

    else if (ret == QCONF_ERR_NOT_FOUND)
    {
        printf("warning!config node is null:%s \n", path.c_str());
        delete value;
        return defaultval;
    }



    delete value;
    if (ret == QCONF_ERR_NULL_VALUE)
    {
        return defaultval;
    }

    throw INV_QConfig_Exception(string("get::qconf_get_conf path:") + path, ret);

    return defaultval;
}

std::string INV_QConfig::getConfPath(const std::string& path, const string& defaultval)
{
    const std::string strConfDir = "/dev/shm/";
    char* value = new char[QCONF_CONF_BUF_MAX_LEN];
    int ret = qconf_get_conf(path.c_str(), value, QCONF_CONF_BUF_MAX_LEN, NULL);
    if (ret == QCONF_OK)
    {
        string fpath = path;
        for (string::iterator it = fpath.begin(); it != fpath.end(); ++it)
            if (*it == '/') *it = '_';
        std::string strConfPath = strConfDir + fpath;
        ret = INV_File::save2file(strConfPath, value, strlen(value));
        delete value;
        if (ret != 0)
        {
            throw INV_QConfig_Exception(string("getConfPath save2file failed! path:") + path + ", confpath:" + strConfPath + ", err:" + strerror(errno), errno);
            return defaultval;
        }
        return strConfPath;
    }

    delete value;
    if (ret == QCONF_ERR_NULL_VALUE)
    {
        return defaultval;
    }

    throw INV_QConfig_Exception(string("qconf_get_conf path:") + path, ret);

    return defaultval;
}

const char* INV_QConfig_Exception::what() const throw()
{
    return m_buffer.c_str();
}

void INV_QConfig_Exception::getBacktrace()
{
    void * array[64];
    int nSize = backtrace(array, 64);
    char ** symbols = backtrace_symbols(array, nSize);

    for (int i = 0; i < nSize; i++)
    {
        m_buffer += symbols[i];
        m_buffer += "\n";
    }
    free(symbols);
}

}
