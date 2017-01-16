#ifndef __INV_ENCODER_H_
#define __INV_ENCODER_H_

#include <vector>

#include "util/inv_ex.h"

namespace inv
{

struct INV_Encoder_Exception : public INV_Exception
{
	INV_Encoder_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_Encoder_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
	~INV_Encoder_Exception() throw(){};
};


class INV_Encoder
{
public:

    static void  gbk2utf8(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen);

    static string gbk2utf8(const string &sIn);

    static void gbk2utf8(const string &sIn, vector<string> &vtStr);

    static void utf82gbk(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen);

    static string utf82gbk(const string &sIn);

	static string transTo(const string& str, char f = '\n', char t = '\r', char u = '\0');

	static string transFrom(const string& str, char f = '\n', char t = '\r', char u = '\0');

    static string urlencode(const string& str);

    static string urldecode(const string& str);
};

}


#endif


