#include <errno.h>
#include <fstream>
#include "util/inv_config.h"
#include "util/inv_util.h"

namespace inv
{

INV_ConfigDomain::INV_ConfigDomain(const string &sLine)
{
	_name = INV_Util::trim(sLine);
}

INV_ConfigDomain::~INV_ConfigDomain()
{
	destroy();
}

INV_ConfigDomain::INV_ConfigDomain(const INV_ConfigDomain &tcd)
{
    (*this) = tcd;
}

INV_ConfigDomain& INV_ConfigDomain::operator=(const INV_ConfigDomain &tcd)
{
    if(this != &tcd)
    {
        destroy();

        _name  = tcd._name;
        _param = tcd._param;
        _key   = tcd._key;
        _domain= tcd._domain;

        const map<string, INV_ConfigDomain*> & m = tcd.getDomainMap();
        map<string, INV_ConfigDomain*>::const_iterator it = m.begin();
        while(it != m.end())
        {
            _subdomain[it->first] = it->second->clone();
            ++it;
        }
    }
    return *this;
}

INV_ConfigDomain::DomainPath INV_ConfigDomain::parseDomainName(const string& path, bool bWithParam)
{
    INV_ConfigDomain::DomainPath dp;

    if(bWithParam)
    {
    	string::size_type pos1 = path.find_first_of(INV_CONFIG_PARAM_BEGIN);
    	if(pos1 == string::npos)
    	{
    		throw INV_Config_Exception("[INV_Config::parseDomainName] : param path '" + path + "' is invalid!" );
    	}

    	if(path[0] != INV_CONFIG_DOMAIN_SEP)
    	{
    		throw INV_Config_Exception("[INV_Config::parseDomainName] : param path '" + path + "' must start with '/'!" );
    	}

    	string::size_type pos2 = path.find_first_of(INV_CONFIG_PARAM_END);
    	if(pos2 == string::npos)
    	{
    		throw INV_Config_Exception("[INV_Config::parseDomainName] : param path '" + path + "' is invalid!" );
    	}

        dp._domains = INV_Util::sepstr<string>(path.substr(1, pos1-1), INV_Util::tostr(INV_CONFIG_DOMAIN_SEP));
        dp._param = path.substr(pos1+1, pos2 - pos1 - 1);
    }
    else
    {
//    	if(path.length() <= 1 || path[0] != INV_CONFIG_DOMAIN_SEP)
        if(path[0] != INV_CONFIG_DOMAIN_SEP)
    	{
    		throw INV_Config_Exception("[INV_Config::parseDomainName] : param path '" + path + "' must start with '/'!" );
    	}

        dp._domains = INV_Util::sepstr<string>(path.substr(1), INV_Util::tostr(INV_CONFIG_DOMAIN_SEP));
    }

    return dp;
}

INV_ConfigDomain* INV_ConfigDomain::addSubDomain(const string& name)
{
    if(_subdomain.find(name) == _subdomain.end())
    {
        _domain.push_back(name);

        _subdomain[name] = new INV_ConfigDomain(name);
    }
    return _subdomain[name];
}

string INV_ConfigDomain::getParamValue(const string &name) const
{
    map<string, string>::const_iterator it = _param.find(name);
	if( it == _param.end())
	{
		throw INV_ConfigNoParam_Exception("[INV_ConfigDomain::getParamValue] param '" + name + "' not exits!");
    }

	return it->second;
}

INV_ConfigDomain *INV_ConfigDomain::getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd)
{
    if(itBegin == itEnd)
    {
        return this;
    }

    map<string, INV_ConfigDomain*>::const_iterator it = _subdomain.find(*itBegin);

	if(it == _subdomain.end())
	{
		return NULL;
	}

	return it->second->getSubTcConfigDomain(itBegin + 1, itEnd);
}

const INV_ConfigDomain *INV_ConfigDomain::getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const
{
    if(itBegin == itEnd)
    {
        return this;
    }

    map<string, INV_ConfigDomain*>::const_iterator it = _subdomain.find(*itBegin);

	if(it == _subdomain.end())
	{
		return NULL;
	}

	return it->second->getSubTcConfigDomain(itBegin + 1, itEnd);
}

void INV_ConfigDomain::insertParamValue(const map<string, string> &m)
{
    _param.insert(m.begin(),  m.end());

    map<string, string>::const_iterator it = m.begin();
    while(it != m.end())
    {
        size_t i = 0;
        for(; i < _key.size(); i++)
        {
            if(_key[i] == it->first)
            {
                break;
            }
        }

        if(i == _key.size())
        {
            _key.push_back(it->first);
        }

        ++it;
    }
}

void INV_ConfigDomain::setParamValue(const string &name, const string &value)
{
    _param[name] = value;

    for(vector<string>::iterator it = _key.begin(); it != _key.end(); ++it)
    {
        if(*it == name)
        {
            _key.erase(it);
            break;
        }
    }

    _key.push_back(name);
}

void INV_ConfigDomain::setParamValue(const string &line)
{
    if(line.empty())
    {
        return;
    }

    _line.push_back(line);

    string::size_type pos = 0;
    for(; pos <= line.length() - 1; pos++)
    {
        if (line[pos] == '=')
        {
            if(pos > 0 && line[pos-1] == '\\')
            {
                continue;
            }

            string name  = parse(INV_Util::trim(line.substr(0, pos), " \r\n\t"));

            string value;
            if(pos < line.length() - 1)
            {
                value = parse(INV_Util::trim(line.substr(pos + 1), " \r\n\t"));
            }

            setParamValue(name, value);
            return;
        }
    }

    setParamValue(line, "");
}

string INV_ConfigDomain::parse(const string& s)
{
    if(s.empty())
    {
        return "";
    }

    string param;
    string::size_type pos = 0;
    for(; pos <= s.length() - 1; pos++)
    {
        char c;
        if(s[pos] == '\\' && pos < s.length() - 1)
        {
            switch (s[pos+1])
            {
            case '\\':
                c = '\\';
                pos++;
                break;
            case 'r':
                c = '\r';
                pos++;
                break;
            case 'n':
                c = '\n';
                pos++;
                break;
            case 't':
                c = '\t';
                pos++;
                break;
            case '=':
                c = '=';
                pos++;
                break;
            default:
                throw INV_Config_Exception("[INV_ConfigDomain::parse] '" + s + "' is invalid, '" + INV_Util::tostr(s[pos]) + INV_Util::tostr(s[pos+1]) + "' couldn't be parse!" );
            }

            param += c;
        }
        else if (s[pos] == '\\')
        {
            throw INV_Config_Exception("[INV_ConfigDomain::parse] '" + s + "' is invalid, '" + INV_Util::tostr(s[pos]) + "' couldn't be parse!" );
        }
        else
        {
            param += s[pos];
        }
    }

    return param;
}

string INV_ConfigDomain::reverse_parse(const string &s)
{
    if(s.empty())
    {
        return "";
    }

    string param;
    string::size_type pos = 0;
    for(; pos <= s.length() - 1; pos++)
    {
        string c;
        switch (s[pos])
        {
        case '\\':
            param += "\\\\";
            break;
        case '\r':
            param += "\\r";
            break;
        case '\n':
            param += "\\n";
            break;
        case '\t':
            param += "\\t";
            break;
            break;
        case '=':
            param += "\\=";
            break;
        case '<':
        case '>':
            throw INV_Config_Exception("[INV_ConfigDomain::reverse_parse] '" + s + "' is invalid, couldn't be parse!" );
        default:
            param += s[pos];
        }
    }

    return param;
}

string INV_ConfigDomain::getName() const
{
	return _name;
}

void INV_ConfigDomain::setName(const string& name)
{
    _name = name;
}

vector<string> INV_ConfigDomain::getKey() const
{
    return _key;
}

vector<string> INV_ConfigDomain::getLine() const
{
    return _line;
}

vector<string> INV_ConfigDomain::getSubDomain() const
{
    return _domain;
}

void INV_ConfigDomain::destroy()
{
    _param.clear();
    _key.clear();
    _line.clear();
    _domain.clear();

    map<string, INV_ConfigDomain*>::iterator it = _subdomain.begin();
    while(it != _subdomain.end())
    {
        delete it->second;
        ++it;
    }

    _subdomain.clear();
}

string INV_ConfigDomain::tostr(int i) const
{
    string sTab;
    for(int k = 0; k < i; ++k)
    {
        sTab += "\t";
    }

    ostringstream buf;

    buf << sTab << "<" << reverse_parse(_name) << ">" << endl;;

    for(size_t n = 0; n < _key.size(); n++)
    {
        map<string, string>::const_iterator it = _param.find(_key[n]);

        assert(it != _param.end());

        if(it->second.empty())
        {
            buf << "\t" << sTab << reverse_parse(_key[n]) << endl;
        }
        else
        {
            buf << "\t" << sTab << reverse_parse(_key[n]) << "=" << reverse_parse(it->second) << endl;
        }
    }

    ++i;

    for(size_t n = 0; n < _domain.size(); n++)
    {
        map<string, INV_ConfigDomain*>::const_iterator itm = _subdomain.find(_domain[n]);

        assert(itm != _subdomain.end());

        buf << itm->second->tostr(i);
    }


	buf << sTab << "</" << reverse_parse(_name) << ">" << endl;

	return buf.str();
}

/********************************************************************/
/*		INV_Config implement										    */
/********************************************************************/

INV_Config::INV_Config() : _root("")
{
}

INV_Config::INV_Config(const INV_Config &tc)
: _root(tc._root)
{

}

INV_Config& INV_Config::operator=(const INV_Config &tc)
{
    if(this != &tc)
    {
        _root = tc._root;
    }

    return *this;
}

void INV_Config::parse(istream &is)
{
    _root.destroy();

    stack<INV_ConfigDomain*> stkTcCnfDomain;
    stkTcCnfDomain.push(&_root);

    string line;
    while(getline(is, line))
	{
		line = INV_Util::trim(line, " \r\n\t");

		if(line.length() == 0)
		{
			continue;
		}

		if(line[0] == '#')
		{
			continue;
		}
		else if(line[0] == '[')
		{
			string::size_type posl = line.find_first_of(']');

			if(posl == string::npos)
			{
				throw INV_Config_Exception("[INV_Config::parse]:parse error! line : " + line);
			}

			if(line[1] == '/')
			{
				string sName(line.substr(2, (posl - 2)));

				if(stkTcCnfDomain.size() <= 0)
                {
					throw INV_Config_Exception("[INV_Config::parse]:parse error! <" + sName + "> hasn't matched domain.");
                }

                if(stkTcCnfDomain.top()->getName() != sName)
				{
					throw INV_Config_Exception("[INV_Config::parse]:parse error! <" + stkTcCnfDomain.top()->getName() + "> hasn't match <" + sName +">.");
				}

				stkTcCnfDomain.pop();
			}
			else
			{
				string name(line.substr(1, posl - 1));

                stkTcCnfDomain.push(stkTcCnfDomain.top()->addSubDomain(name));
			}
		}
		else
		{
            stkTcCnfDomain.top()->setParamValue(line);
		}
	}

	if(stkTcCnfDomain.size() != 1)
	{
		throw INV_Config_Exception("[INV_Config::parse]:parse error : hasn't match");
    }
}

void INV_Config::parseFile(const string &sFileName)
{
    if(sFileName.length() == 0)
    {
		throw INV_Config_Exception("[INV_Config::parseFile]:file name is empty");
    }

    ifstream ff;
    ff.open(sFileName.c_str());
	if (!ff)
	{
		throw INV_Config_Exception("[INV_Config::parseFile]:fopen fail: " + sFileName, errno);
    }

    parse(ff);
}

void INV_Config::parseString(const string& buffer)
{
    istringstream iss;
    iss.str(buffer);

    parse(iss);
}

string INV_Config::operator[](const string &path)
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, true);

	INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
		throw INV_ConfigNoParam_Exception("[INV_Config::operator[]] path '" + path + "' not exits!");
	}

	return pTcConfigDomain->getParamValue(dp._param);
}

string INV_Config::get(const string &sName, const string &sDefault) const
{
    try
    {
        INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(sName, true);

    	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    	if(pTcConfigDomain == NULL)
    	{
    		throw INV_ConfigNoParam_Exception("[INV_Config::get] path '" + sName + "' not exits!");
    	}

    	return pTcConfigDomain->getParamValue(dp._param);
    }
    catch ( INV_ConfigNoParam_Exception &ex )
    {
        return sDefault;
    }
}

bool INV_Config::getDomainMap(const string &path, map<string, string> &m) const
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
		return false;
	}

	m = pTcConfigDomain->getParamMap();

    return true;
}

map<string, string> INV_Config::getDomainMap(const string &path) const
{
    map<string, string> m;

    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain != NULL)
	{
        m = pTcConfigDomain->getParamMap();
    }

    return m;
}

vector<string> INV_Config::getDomainKey(const string &path) const
{
    vector<string> v;

    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain != NULL)
	{
        v = pTcConfigDomain->getKey();
    }

    return v;
}

vector<string> INV_Config::getDomainLine(const string &path) const
{
    vector<string> v;

    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

    const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

    if(pTcConfigDomain != NULL)
    {
        v = pTcConfigDomain->getLine();
    }

    return v;
}

bool INV_Config::getDomainVector(const string &path, vector<string> &vtDomains) const
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

    if(dp._domains.empty())
    {
        vtDomains = _root.getSubDomain();
        return !vtDomains.empty();
    }

	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
        return false;
	}

    vtDomains = pTcConfigDomain->getSubDomain();

	return true;
}

vector<string> INV_Config::getDomainVector(const string &path) const
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(path, false);

    if(dp._domains.empty())
    {
        return _root.getSubDomain();
    }

	const INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
        return vector<string>();
	}

    return pTcConfigDomain->getSubDomain();
}


INV_ConfigDomain *INV_Config::newTcConfigDomain(const string &sName)
{
	return new INV_ConfigDomain(sName);
}

INV_ConfigDomain *INV_Config::searchTcConfigDomain(const vector<string>& domains)
{
	return _root.getSubTcConfigDomain(domains.begin(), domains.end());
}

const INV_ConfigDomain *INV_Config::searchTcConfigDomain(const vector<string>& domains) const
{
	return _root.getSubTcConfigDomain(domains.begin(), domains.end());
}

int INV_Config::insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate)
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(sCurDomain, false);

	INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
        if(bCreate)
        {
            pTcConfigDomain = &_root;

            for(size_t i = 0; i < dp._domains.size(); i++)
            {
                pTcConfigDomain = pTcConfigDomain->addSubDomain(dp._domains[i]);
            }
        }
        else
        {
            return -1;
        }
	}

    pTcConfigDomain->addSubDomain(sAddDomain);

    return 0;
}

int INV_Config::insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate)
{
    INV_ConfigDomain::DomainPath dp = INV_ConfigDomain::parseDomainName(sCurDomain, false);

	INV_ConfigDomain *pTcConfigDomain = searchTcConfigDomain(dp._domains);

	if(pTcConfigDomain == NULL)
	{
        if(bCreate)
        {
            pTcConfigDomain = &_root;

            for(size_t i = 0; i < dp._domains.size(); i++)
            {
                pTcConfigDomain = pTcConfigDomain->addSubDomain(dp._domains[i]);
            }
        }
        else
        {
            return -1;
        }
	}

    pTcConfigDomain->insertParamValue(m);

    return 0;
}

string INV_Config::tostr() const
{
    string buffer;

    map<string, INV_ConfigDomain*> msd = _root.getDomainMap();
    map<string, INV_ConfigDomain*>::const_iterator it = msd.begin();
    while (it != msd.end())
    {
        buffer += it->second->tostr(0);
        ++it;
    }

    return buffer;
}

void INV_Config::joinConfig(const INV_Config &cf, bool bUpdate)
{
    string buffer;
    if(bUpdate)
    {
        buffer = tostr() + cf.tostr();
    }
    else
    {
        buffer = cf.tostr() + tostr();
    }
    parseString(buffer);
}

}

