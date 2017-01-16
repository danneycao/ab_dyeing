#ifndef __INV_MD5_H
#define __INV_MD5_H

#include <string>
#include "util/inv_util.h"

using namespace std;

namespace inv
{

#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ]       )        \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 2] << 16 )        \
        | ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif

class INV_MD5
{
    typedef unsigned char *POINTER;
    typedef unsigned short int UINT2;
    //typedef unsigned long int UINT4;
    typedef uint32_t UINT4;

    typedef struct 
    {
        /**
        * state (ABCD)
        */
        //unsigned long state[4];        
        UINT4 state[4];        

        /**
        * number of bits, modulo 2^64 (lsb first)
        */
        //unsigned long count[2];        
        UINT4 count[2];        

        /**
        * input buffer
        */
        unsigned char buffer[64];      
    }MD5_CTX;

public:

    static string md5bin(const string &sString);

    static string md5str (const string &sString);


    static string md5file(const string& fileName);

protected:

	static string bin2str(const void *buf, size_t len, const string &sSep);

    static void md5init(MD5_CTX *context);
	
  
    static void md5update (MD5_CTX *context, unsigned char *input,unsigned int inputLen);    

    static void md5final (unsigned char digest[16], MD5_CTX *context);
    
    static void md5_process( MD5_CTX *ctx, const unsigned char data[64]);
	
    static void encode (unsigned char *output,UINT4 *input,unsigned int len);

    static void decode (UINT4 *output,unsigned char *input,unsigned int len);
  
    static void md5_memcpy (POINTER output,POINTER input,unsigned int len);

    static void md5_memset (POINTER output,int value,unsigned int len);
    
    static unsigned char PADDING[64];
};

}
#endif
