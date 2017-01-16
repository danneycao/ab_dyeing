#include "util/inv_util.h"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <cmath>

namespace inv
{
template <>
string INV_Util::tostr<bool>(const bool &t)
{
    char buf[2];
    buf[0] = t ? '1' : '0';
    buf[1] = '\0';
    return string(buf);
}


template <>
string INV_Util::tostr<char>(const char &t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string INV_Util::tostr<unsigned char>(const unsigned char &t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string INV_Util::tostr<short>(const short &t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string INV_Util::tostr<unsigned short>(const unsigned short &t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string INV_Util::tostr<int>(const int &t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string INV_Util::tostr<unsigned int>(const unsigned int &t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string INV_Util::tostr<long>(const long &t)
{
    char buf[32];
    snprintf(buf, 32, "%ld", t);
    return string(buf);
}

template <>
string INV_Util::tostr<long long>(const long long &t)
{
    char buf[32];
    snprintf(buf, 32, "%lld", t);
    return string(buf);
}


template <>
string INV_Util::tostr<unsigned long>(const unsigned long &t)
{
    char buf[32];
    snprintf(buf, 32, "%lu", t);
    return string(buf);
}

template <>
string INV_Util::tostr<float>(const float &t)
{
    char buf[32];
    snprintf(buf, 32, "%.5f", t);
    string s(buf);

    //È¥µôÎÞÐ§0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2ÎªÁËÈ¥µô"."ºÅ
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;
}

template <>
string INV_Util::tostr<double>(const double &t)
{
    char buf[32];
    snprintf(buf, 32, "%.5f", t);
    string s(buf);

    //È¥µôÎÞÐ§0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2ÎªÁËÈ¥µô"."ºÅ
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;

}

template <>
string INV_Util::tostr<long double>(const long double &t)
{
    char buf[32];
    snprintf(buf, 32, "%Lf", t);
    string s(buf);

    //È¥µôÎÞÐ§0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2ÎªÁËÈ¥µô"."ºÅ
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;

}

template <>
string INV_Util::tostr<std::string>(const std::string &t)
{
    return t;
}

int INV_Util::str2tm(const string &sString, const string &sFormat, struct tm &stTm)
{
    char *p = strptime(sString.c_str(), sFormat.c_str(), &stTm);
    return (p != NULL) ? 0 : -1;
}

int INV_Util::strgmt2tm(const string &sString, struct tm &stTm)
{
    return str2tm(sString, "%a, %d %b %Y %H:%M:%S GMT", stTm);
}

string INV_Util::tm2str(const struct tm &stTm, const string &sFormat)
{
    char sTimeString[255] = "\0";

    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), &stTm);

    return string(sTimeString);
}

string INV_Util::tm2str(const time_t &t, const string &sFormat)
{
    struct tm tt;
    localtime_r(&t, &tt);

    return tm2str(tt, sFormat);
}

int64_t INV_Util::format2sec(const string &sFormat)
{
    ptime p(from_iso_string(sFormat));
	ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type x = (p - epoch).total_seconds();
	return int64_t(x) - 8 * 3600;
}

int64_t INV_Util::perHour2Sec(const string &stime)
{
	int64_t time;
	if(stime.size() == 1)
	{
		time = strto<int32_t>(stime);
	}
	else if(stime.size() == 2)
	{
		if(stime[0] == '0')
		{
			string str;
			str.insert( str.begin(), stime[1] );
			time = strto<int32_t>(str);
		}
		else
		{
			time = strto<int32_t>(stime);
		}
	}
	else
	{
		time = 0;
	}

	ptime pt(second_clock::local_time());
	date dt = pt.date();
	ptime p(date(dt), time_duration(time, 0, 0));

	ptime epoch(boost::gregorian::date(1970,1,1));
	time_duration::sec_type x = (p - epoch).total_seconds();

	return int64_t(x) - 8*3600;
}


string INV_Util::now2str(const string &sFormat)
{
    time_t t = time(NULL);
    return tm2str(t, sFormat.c_str());
}

string INV_Util::now2GMTstr()
{
    time_t t = time(NULL);
    return tm2GMTstr(t);
}

string INV_Util::tm2GMTstr(const time_t &t)
{
    struct tm tt;
    gmtime_r(&t, &tt);
    return tm2str(tt, "%a, %d %b %Y %H:%M:%S GMT");
}

string INV_Util::tm2GMTstr(const struct tm &stTm)
{
    return tm2str(stTm, "%a, %d %b %Y %H:%M:%S GMT");
}

string INV_Util::nowdate2str()
{
    return now2str("%Y%m%d");
}

string INV_Util::nowtime2str()
{
    return now2str("%H%M%S");
}

int64_t INV_Util::now2ms()
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;
}

int64_t INV_Util::now2us()
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return tv.tv_sec * (int64_t)1000000 + tv.tv_usec;
}

string INV_Util::trim(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    if(!bChar)
    {
        return trimright(trimleft(sStr, s, false), s, false);
    }

    return trimright(trimleft(sStr, s, true), s, true);
}

string INV_Util::trimleft(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    if(!bChar)
    {
        if(sStr.length() < s.length())
        {
            return sStr;
        }

        if(sStr.compare(0, s.length(), s) == 0)
        {
            return sStr.substr(s.length());
        }

        return sStr;
    }

    string::size_type pos = 0;
    while(pos < sStr.length())
    {
        if(s.find_first_of(sStr[pos]) == string::npos)
        {
            break;
        }

        pos++;
    }

    if(pos == 0) return sStr;

    return sStr.substr(pos);
}

string INV_Util::trimright(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    if(!bChar)
    {
        if(sStr.length() < s.length())
        {
            return sStr;
        }

        if(sStr.compare(sStr.length() - s.length(), s.length(), s) == 0)
        {
            return sStr.substr(0, sStr.length() - s.length());
        }

        return sStr;
    }

    string::size_type pos = sStr.length();
    while(pos != 0)
    {
        if(s.find_first_of(sStr[pos-1]) == string::npos)
        {
            break;
        }

        pos--;
    }

    if(pos == sStr.length()) return sStr;

    return sStr.substr(0, pos);
}

static char c_b2s[256][4]={"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f","10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f","20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f","30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f","40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f","50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f","60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f","70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f","80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f","90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f","a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf","c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df","e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef","f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"};

char INV_Util::x2c(const string &sWhat)
{
    register char digit;

    if(sWhat.length() < 2)
    {
        return '\0';
    }

    digit = (sWhat[0] >= 'A' ? ((sWhat[0] & 0xdf) - 'A')+10 : (sWhat[0] - '0'));
    digit *= 16;
    digit += (sWhat[1] >= 'A' ? ((sWhat[1] & 0xdf) - 'A')+10 : (sWhat[1] - '0'));

    return(digit);
}

string INV_Util::bin2str(const void *buf, size_t len, const string &sSep, size_t lines)
{
    if(buf == NULL || len <=0 )
    {
        return "";
    }

    string sOut;
    const unsigned char *p = (const unsigned char *) buf;

    for (size_t i = 0; i < len; ++i, ++p)
    {
        sOut += c_b2s[*p][0];
        sOut += c_b2s[*p][1];
        sOut += sSep;

        if ((lines != 0) && ((i+1) % lines == 0))
        {
            sOut += "\n";
        }
    }

    return sOut;
}

string INV_Util::bin2str(const string &sBinData, const string &sSep, size_t lines)
{
    return bin2str((const void *)sBinData.data(), sBinData.length(), sSep, lines);
}

int INV_Util::str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize)
{
    int iAsciiLength = strlen(psAsciiData);

    int iRealLength = (iAsciiLength/2 > iBinSize)?iBinSize:(iAsciiLength/2);
    for (int i = 0 ; i < iRealLength ; i++)
    {
        sBinData[i] = x2c(psAsciiData + i*2);
    }
    return iRealLength;
}

string INV_Util::str2bin(const string &sString, const string &sSep, size_t lines)
{
    const char *psAsciiData = sString.c_str();

    int iAsciiLength = sString.length();
    string sBinData;
    for (int i = 0 ; i < iAsciiLength ; i++)
    {
        sBinData += x2c(psAsciiData + i);
        i++;
        i += sSep.length();

        if (lines != 0 && sBinData.length()%lines == 0)
        {
            i++;
        }
    }

    return sBinData;
}

size_t INV_Util::strlen_utf8(const std::string& str)
{
    size_t count = 0;
    int word_cnt = 0;
    size_t idx = 0;
    while(str.size() > idx)
    {
        if(0xc0 == uint8_t(str[idx] & 0xe0))
        {
            word_cnt = 2;
        }else if(0xe0 == uint8_t(str[idx] & 0xf0))
        {
            word_cnt = 3;
        }else if(0xf0 == uint8_t(str[idx] & 0xf8))
        {
            word_cnt = 4;
        }else if(0xf8 == uint8_t(str[idx] & 0xfc))
        {
            word_cnt = 5;
        }else if(0xfc == uint8_t(str[idx] & 0xfe))
        {
            word_cnt = 6;
        }else
        {
            word_cnt = 1;
        }
        count++;
        if(idx+word_cnt >= str.size()) idx = str.size();
        else idx += word_cnt;
    }
    return count;
}

const std::string INV_Util::substr_utf8(const std::string& str, const size_t pos, const size_t n)
{
    if(0 >= n) return "";

    bool got_start = false;
    size_t start = str.size();
    size_t count = 0;
    int word_cnt = 0;
    size_t idx = 0;
    while(str.size() > idx)
    {
        if(0xc0 == uint8_t(str[idx] & 0xe0))
        {
            word_cnt = 2;
        }else if(0xe0 == uint8_t(str[idx] & 0xf0))
        {
            word_cnt = 3;
        }else if(0xf0 == uint8_t(str[idx] & 0xf8))
        {
            word_cnt = 4;
        }else if(0xf8 == uint8_t(str[idx] & 0xfc))
        {
            word_cnt = 5;
        }else if(0xfc == uint8_t(str[idx] & 0xfe))
        {
            word_cnt = 6;
        }else
        {
            word_cnt = 1;
        }
        if(pos == count)
        {
            start = idx;
            got_start = true;
        }
        count++;
        if(idx+word_cnt >= str.size()) idx = str.size();
        else idx += word_cnt;
        if(got_start && n <= count - pos) break;
    }
    return str.substr(start, idx-start);
}

}
