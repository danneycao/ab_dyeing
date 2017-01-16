#include <iconv.h>
#include <errno.h>
#include <string.h>
#include "util/inv_encoder.h"

namespace inv
{

#define INV_CODE_GB "GB18030"

void INV_Encoder::gbk2utf8(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen)
{
    char * pIn = (char*)sIn;
    char * pEnd = pIn+iInLen;
    char * pOut = sOut;
    int iLeftLen;
    int iGbkLen;
    iconv_t cd;

    if (iInLen > iMaxOutLen)
    {
        throw INV_Encoder_Exception("[INV_Encoder::gbk2utf8] iInLen > iMaxOutLen error : ", errno);
    }

    cd = iconv_open("UTF-8", INV_CODE_GB);
    if (cd == (iconv_t)-1)
    {
        throw INV_Encoder_Exception("[INV_Encoder::gbk2utf8] iconv_open error : ", errno);
    }

    iLeftLen = iMaxOutLen;
    while(pIn < pEnd)
    {
        if((unsigned char)(*pIn)==0x80)
        {
            *pOut = 0xe2; pOut++; iLeftLen--;
            *pOut = 0x82; pOut++; iLeftLen--;
            *pOut = 0xac; pOut++; iLeftLen--;
            pIn++;
        }
        else if((unsigned char)(*pIn)<0x80)
        {
            *pOut = *pIn;
            pIn++;pOut++;iLeftLen--;
        }
        else
        {
            iGbkLen=2;
            int iRet=iconv(cd, &pIn, (size_t *)&iGbkLen, (char **)&pOut, (size_t *)&iLeftLen);
            if(iRet < 0)
            {
                *pOut = ' ';
                pIn+=2; pOut++; iLeftLen--;
            }
        }
    }

    iconv_close(cd);
    sOut[iMaxOutLen - iLeftLen] = '\0';
    iMaxOutLen = iMaxOutLen - iLeftLen;
}

string INV_Encoder::gbk2utf8(const string &sIn)
{
    iconv_t cd;

    cd = iconv_open("UTF-8", INV_CODE_GB);
    if (cd == (iconv_t)-1)
    {
        throw INV_Encoder_Exception("[INV_Encoder::gbk2utf8] iconv_open error", errno);
    }

    string sOut;

    for(string::size_type pos = 0; pos < sIn.length(); ++pos)
    {
        if((unsigned char)sIn[pos] == 0x80)
        {
            sOut += 0xe2;
            sOut += 0x82;
            sOut += 0xac;
        }
        else if((unsigned char)sIn[pos] < 0x80)
        {
            sOut += sIn[pos];
        }
        else
        {
            size_t sizeGbkLen = 2;
            char pIn[128] = "\0";

            strncpy(pIn, sIn.c_str() + pos, sizeGbkLen);
            char *p = pIn;

            size_t sizeLeftLen = 128;
            char pOut[128] = "\0";
            char *o = pOut;
            int iRet = iconv(cd, &p, &sizeGbkLen, (char **)&o, &sizeLeftLen);
            if(iRet < 0)
            {
                sOut += ' ';
            }
            else
            {
                sOut += pOut;
            }

            ++pos;

        }
    }

    iconv_close(cd);
    return sOut;
}

void INV_Encoder::gbk2utf8(const string &sIn, vector<string> &vtStr)
{
    iconv_t cd;

    cd = iconv_open("UTF-8", INV_CODE_GB);
    if (cd == (iconv_t)-1)
    {
        throw INV_Encoder_Exception("[INV_Encoder::gbk2utf8] iconv_open error", errno);
    }

    vtStr.clear();

    for(string::size_type pos = 0; pos < sIn.length(); ++pos)
    {
        string sOut;

        if((unsigned char)sIn[pos] == 0x80)
        {
            sOut += 0xe2;
            sOut += 0x82;
            sOut += 0xac;
        }
        else if((unsigned char)sIn[pos] < 0x80)
        {
            sOut += sIn[pos];
        }
        else
        {
            int iGbkLen = 2;
            char pIn[128] = "\0";

            strncpy(pIn, sIn.c_str() + pos, iGbkLen);
            char *p = pIn;

            int iLeftLen = 128;
            char pOut[128] = "\0";
            char *o = pOut;
            int iRet = iconv(cd, &p, (size_t *)&iGbkLen, (char **)&o, (size_t *)&iLeftLen);
            if(iRet < 0)
            {
                sOut += ' ';
            }
            else
            {
                sOut += pOut;
            }

            ++pos;

        }

        vtStr.push_back(sOut);
    }

    iconv_close(cd);
}

void INV_Encoder::utf82gbk(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen)
{
    iconv_t cd;

    cd = iconv_open(INV_CODE_GB, "UTF-8");
    if (cd == (iconv_t)-1)
    {
        throw INV_Encoder_Exception("[INV_Encoder::utf82gbk] iconv_open error", errno);
    }

    char * pIn = (char*)sIn;
    size_t sizeLeftLen  = iMaxOutLen;
    size_t sizeInLen    = iInLen;
    char* pOut = sOut;

    size_t ret = iconv(cd, &pIn, &sizeInLen, (char **)&sOut, &sizeLeftLen);
    if (ret == (size_t) - 1)
    {
        iMaxOutLen = 0;
        iconv_close(cd);
        throw INV_Encoder_Exception("[INV_Encoder::utf82gbk] iconv error", errno);
        return;
    }

    iconv_close(cd);

    pOut[iMaxOutLen - (int)sizeLeftLen] = '\0';

    iMaxOutLen = iMaxOutLen - (int)sizeLeftLen;
}

string INV_Encoder::utf82gbk(const string &sIn)
{
    if(sIn.length() == 0)
    {
        return "";
    }

    string sOut;

    int iLen = sIn.length() * 2 + 1;
    char *pOut = new char[iLen];

    try
    {
        utf82gbk(pOut, iLen, sIn.c_str(), sIn.length());
    }
    catch (INV_Encoder_Exception& e)
    {
        delete[] pOut;

        throw e;
    }

    sOut.assign(pOut, iLen);

    delete[] pOut;

    return sOut;
}

string INV_Encoder::transTo(const string& str, char f /*='\n'*/, char t /*= '\r'*/, char u /*= '\0'*/)
{
    string ret = str;

    for (size_t i = 0; i < ret.length(); ++i)
    {
        if (ret[i] == f)
        {
            ret[i] = t;

            ret.insert(++i, 1, u);
        }
        else if (ret[i] == t)
        {
            ret.insert(++i, 1, t);
        }
    }
    return ret;
}

string INV_Encoder::transFrom(const string& str, char f /*= '\n'*/, char t /*= '\r'*/, char u /*= '\0'*/)
{
    string ret = "";

    for (string::const_iterator it = str.begin()
        ; it != str.end()
        ; ++it)
    {
        ret.append(1, *it);

        if (*it == t)
        {
            if (*(++it) == u)
            {
                *ret.rbegin() = f;
            }
        }
    }
    return ret;
}

static inline unsigned char tohex(const unsigned char& x)
{
    return x > 9 ? x -10 + 'A': x + '0';
}

static inline unsigned char fromhex(const unsigned char& x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

static inline bool ishexchar(const char x)
{
    return ((x >= '0' && x <= '9') || 
            (x >= 'a' && x <= 'f') || 
            (x >= 'A' && x <= 'F'));
}

string INV_Encoder::urlencode(const string& str)
{
    string out;
    for(size_t ix = 0; ix < str.size(); ix++)
    {      
        if(isalnum((unsigned char)str[ix]) || str[ix] == '_' || str[ix] == '-' || str[ix] == '.')
        {      
            out += str[ix];
        }
        else
        {
            unsigned char buf[4] = {0};
            buf[0] = '%';
            buf[1] = tohex( (unsigned char)str[ix] >> 4 );
            buf[2] = tohex( (unsigned char)str[ix] % 16);
            out += (char*)buf;
        }
    }

    return out;
}

string INV_Encoder::urldecode(const string& str)
{
    string out;
    for(size_t ix = 0; ix < str.size(); ix++)
    {
        unsigned char ch = 0;
        if(str[ix]=='%' && ix + 2 < str.size() && ishexchar(str[ix + 1]) && ishexchar(str[ix + 2]))
        {
            ch = (fromhex(str[ix+1])<<4);
            ch |= fromhex(str[ix+2]);
            ix += 2;
        }
        else if(str[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = str[ix];
        }
        out += (char)ch;
    }

    return out;
}

}

