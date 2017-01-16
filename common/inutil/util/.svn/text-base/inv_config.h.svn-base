#ifndef __INV_CONFIG_H_
#define __INV_CONFIG_H_

#include <map>
#include <list>
#include <stack>
#include <vector>
#include "util/inv_ex.h"

namespace inv
{

const char INV_CONFIG_DOMAIN_SEP = '/';

const char INV_CONFIG_PARAM_BEGIN = '[';

const char INV_CONFIG_PARAM_END = ']';


struct INV_Config_Exception : public INV_Exception
{
	INV_Config_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_Config_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
	~INV_Config_Exception() throw(){};
};


struct INV_ConfigNoParam_Exception : public INV_Exception
{
	INV_ConfigNoParam_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_ConfigNoParam_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
	~INV_ConfigNoParam_Exception() throw(){};
};


class INV_ConfigDomain
{
public:
    friend class INV_Config;

    INV_ConfigDomain(const string &sLine);

    ~INV_ConfigDomain();

    INV_ConfigDomain(const INV_ConfigDomain &tcd);


    INV_ConfigDomain& operator=(const INV_ConfigDomain &tcd);


    struct DomainPath
    {
        vector<string>  _domains;
        string          _param;
    };

    static DomainPath parseDomainName(const string& path, bool bWithParam);

    INV_ConfigDomain* addSubDomain(const string& name);

    INV_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd);
    const INV_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const;

    string getParamValue(const string &sName) const;

    const map<string, string>& getParamMap() const { return _param; }

    const map<string, INV_ConfigDomain*>& getDomainMap() const { return _subdomain; }

    void setParamValue(const string &sLine);


    void insertParamValue(const map<string, string> &m);

    void destroy();

    string getName() const;

    void setName(const string& name);

    vector<string> getKey() const;

    vector<string> getLine() const;

    vector<string> getSubDomain() const;

    string tostr(int i) const;

    INV_ConfigDomain* clone() const 
    { 
        return new INV_ConfigDomain(*this); 
    }

protected:

    static string parse(const string& s);

    static string reverse_parse(const string &s);

    void setParamValue(const string &sName, const string &sValue);

protected:

    string                  _name;

    map<string, string>     _param;

    vector<string>          _key;

    map<string, INV_ConfigDomain*>	    _subdomain;

    vector<string>          _domain;

    vector<string>          _line;
};

class INV_Config
{
public:

    INV_Config();

    INV_Config(const INV_Config &tc);

    INV_Config& operator=(const INV_Config &tc);

    void parseFile(const string& sFileName);

    void parseString(const string& buffer);

    string operator[](const string &sName);

    string get(const string &sName, const string &sDefault="") const;

    bool getDomainMap(const string &path, map<string, string> &m) const;

    map<string, string> getDomainMap(const string &path) const;

    vector<string> getDomainKey(const string &path) const;

    vector<string> getDomainLine(const string &path) const;

    bool getDomainVector(const string &path, vector<string> &v) const;

    vector<string> getDomainVector(const string &path) const;

    bool hasDomainVector(const string &path) const;

    int insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate);

    int insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate);

    void joinConfig(const INV_Config &cf, bool bUpdate);

	string tostr() const;

protected:

    void parse(istream &is);

    INV_ConfigDomain *newTcConfigDomain(const string& sName);

	INV_ConfigDomain *searchTcConfigDomain(const vector<string>& domains);
	const INV_ConfigDomain *searchTcConfigDomain(const vector<string>& domains) const;

protected:


	INV_ConfigDomain _root;
};

}
#endif //_INV_CONFIG_H_
