#ifndef _INV_XXTEA_H
#define _INV_XXTEA_H

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <utility>
#include <vector>

using namespace std;

#define XXTEA_MX ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + ((k[(p & 3) ^ e]) ^ z))
#define XXTEA_DELTA 0x9e3779b9

namespace inv
{

class INV_xxTea
{

public:
    /* key为16字节 */
	static string encrypt(const string &data, const string &key);
	static string decrypt(const string &data, const string &key);

	static int xxtea_encrypt(const unsigned char* str, int slen, const unsigned char* key, int keylen, unsigned char* enc_chr, int enclen);
	static int xxtea_decrypt(const unsigned char* str, int slen, const unsigned char* key, int keylen, unsigned char* dec_chr, int declen);
};

}

#endif
