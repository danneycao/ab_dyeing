#ifndef __INV_BASE64_H
#define __INV_BASE64_H

#include <string>

using namespace std;

namespace inv
{

class INV_Base64
{
public:
    static string encode(const string &data, bool bChangeLine = false);
    
   
    static string decode(const string &data);


    static int encode(const unsigned char* pSrc, int nSrcLen, char* pDst, bool bChangeLine = false);
    
  
    static int decode(const char* pSrc, int nSrcLen, unsigned char* pDst);
    
protected:


    static const char EnBase64Tab[];

    static const char DeBase64Tab[];
};

}
#endif
