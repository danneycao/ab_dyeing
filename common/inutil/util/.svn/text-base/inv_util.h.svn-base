#ifndef __INV_UTIL_H
#define __INV_UTIL_H

#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>
#include "util/inv_loki.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/greg_month.hpp>
#include <boost/date_time/gregorian/greg_year.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

using namespace std;

namespace p
{
    template<typename D>
    struct strto1
    {
        D operator()(const string &sStr)
        {
            string s = "0";

            if(!sStr.empty())
            {
                s = sStr;
            }

            istringstream sBuffer(s);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto1<char>
    {
        char operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return sStr[0];
            }
            return 0;
        }
    };

    template<>
    struct strto1<short>
    {
        short operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoi(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned short>
    {
        unsigned short operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<int>
    {
        int operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoi(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned int>
    {
        unsigned int operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<long>
    {
        long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atol(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<long long>
    {
        long long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoll(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned long>
    {
        unsigned long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<float>
    {
        float operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<double>
    {
        double operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<typename D>
    struct strto2
    {
        D operator()(const string &sStr)
        {
            istringstream sBuffer(sStr);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto2<string>
    {
        string operator()(const string &sStr)
        {
            return sStr;
        }
    };

}

namespace inv
{
inline bool IS_GBK_CHAR(char c1, char c2)
{
    return ((unsigned char)c1 >= 0x81 
            && (unsigned char)c1 <= 0xfe 
            && (unsigned char)c2 >= 0x40 
            && (unsigned char)c2 <= 0xfe 
            && (unsigned char)c2 != 0x7f);
}

inline int UTF8_LENGTH(char c)
{
    return ((unsigned char)c <= 0x7f ? 1 : 
            ((unsigned char)c & 0xe0) == 0xc0 ? 2: 
            ((unsigned char)c & 0xf0) == 0xe0 ? 3: 
            ((unsigned char)c & 0xf8) == 0xf0 ? 4: 
            ((unsigned char)c & 0xfc) == 0xf8 ? 5: 
            ((unsigned char)c & 0xfe) == 0xfc ? 6: 0 );
}

class INV_Util
{
public:

    static string trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    static string lower(const string &sString);

    static string upper(const string &sString);

    static bool isdigit(const string &sInput);

    static int str2tm(const string &sString, const string &sFormat, struct tm &stTm);

    static int strgmt2tm(const string &sString, struct tm &stTm);

    static string tm2str(const struct tm &stTm, const string &sFormat = "%Y%m%d%H%M%S");

    static string tm2str(const time_t &t, const string &sFormat = "%Y%m%d%H%M%S");

    static int64_t perHour2Sec(const string &stime);

    static int64_t format2sec(const string &sFormat);

    static string now2str(const string &sFormat = "%Y%m%d%H%M%S");

    static string tm2GMTstr(const struct tm &stTm);

    static string tm2GMTstr(const time_t &t);

    static string now2GMTstr();

    static string nowdate2str();

    static string nowtime2str();

    static int64_t now2ms();

    static int64_t now2us();

    template<typename T>
    static T strto(const string &sStr);

    template<typename T>
    static T strto(const string &sStr, const string &sDefault);

    template<typename T>
    static vector<T> sepstr(const string &sStr, const string &sSep, bool withEmpty = false);

    template<typename T>
    static string tostr(const T &t);

    template<typename T>
    static string tostr(const vector<T> &t);

    template<typename K, typename V, typename D, typename A>
    static string tostr(const map<K, V, D, A> &t);

    template<typename K, typename V, typename D, typename A>
    static string tostr(const multimap<K, V, D, A> &t);

    template<typename F, typename S>
    static string tostr(const pair<F, S> &itPair);

    template <typename InputIter>
    static string tostr(InputIter iFirst, InputIter iLast, const string &sSep = "|");

    static string bin2str(const void *buf, size_t len, const string &sSep = "", size_t lines = 0);

    static string bin2str(const string &sBinData, const string &sSep = "", size_t lines = 0);

    static int str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize);

    static string str2bin(const string &sBinData, const string &sSep = "", size_t lines = 0);

    static string replace(const string &sString, const string &sSrc, const string &sDest);

	static string replace(const string &sString, const map<string,string>& mSrcDest);

    static bool matchPeriod(const string& s, const string& pat);

    static bool matchPeriod(const string& s, const vector<string>& pat);

    static bool isPrimeNumber(size_t n);

    static void daemon();

    static void ignorePipe();

    static char x2c(const string &sWhat);

    static size_t toSize(const string &s, size_t iDefaultSize);
	
    // 输入为utf8字符串，输出为utf8字符数
    static size_t strlen_utf8(const std::string& str);

    // 输入为utf8字符串和字符位移, 输出utf8子字符串
    static const std::string substr_utf8(const std::string& str, const size_t pos, const size_t n);
};

template<typename T>
T INV_Util::strto(const string &sStr)
{
    typedef typename TL::TypeSelect<TL::TypeTraits<T>::isStdArith, p::strto1<T>, p::strto2<T> >::Result strto_type;

    return strto_type()(sStr);
}

template<typename T>
T INV_Util::strto(const string &sStr, const string &sDefault)
{
    string s;

    if(!sStr.empty())
    {
        s = sStr;
    }
    else
    {
        s = sDefault;
    }

    return strto<T>(s);
}


template<typename T>
vector<T> INV_Util::sepstr(const string &sStr, const string &sSep, bool withEmpty)
{
    vector<T> vt;

    string::size_type pos = 0;
    string::size_type pos1 = 0;

    while(true)
    {
        string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if(pos1 == string::npos)
        {
            if(pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if(pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }

        if(withEmpty)
        {
            vt.push_back(strto<T>(s));
        }
        else
        {
            if(!s.empty())
            {
                T tmp = strto<T>(s);
                vt.push_back(tmp);
            }
        }

        if(pos1 == string::npos)
        {
            break;
        }

        pos++;
    }

    return vt;
}
template<typename T>
string INV_Util::tostr(const T &t)
{
    ostringstream sBuffer;
    sBuffer << t;
    return sBuffer.str();
}

template<typename T>
string INV_Util::tostr(const vector<T> &t)
{
    string s;
    for(size_t i = 0; i < t.size(); i++)
    {
        s += tostr(t[i]);
        s += " ";
    }
    return s;
}

template<typename K, typename V, typename D, typename A>
string INV_Util::tostr(const map<K, V, D, A> &t)
{
    string sBuffer;
    typename map<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer += " [";
        sBuffer += tostr(it->first);
        sBuffer += "]=[";
        sBuffer += tostr(it->second);
        sBuffer += "] ";
        ++it;
    }
    return sBuffer;
}

template<typename K, typename V, typename D, typename A>
string INV_Util::tostr(const multimap<K, V, D, A> &t)
{
    string sBuffer;
    typename multimap<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer += " [";
        sBuffer += tostr(it->first);
        sBuffer += "]=[";
        sBuffer += tostr(it->second);
        sBuffer += "] ";
        ++it;
    }
    return sBuffer;
}

template<typename F, typename S>
string INV_Util::tostr(const pair<F, S> &itPair)
{
    string sBuffer;
    sBuffer += "[";
    sBuffer += tostr(itPair.first);
    sBuffer += "]=[";
    sBuffer += tostr(itPair.second);
    sBuffer += "]";
    return sBuffer;
}

template <typename InputIter>
string INV_Util::tostr(InputIter iFirst, InputIter iLast, const string &sSep)
{
    string sBuffer;
    InputIter it = iFirst;

    while(it != iLast)
    {
        sBuffer += tostr(*it);
        ++it;

        if(it != iLast)
        {
            sBuffer += sSep;
        }
        else
        {
            break;
        }
    }

    return sBuffer;
}

}
#endif

