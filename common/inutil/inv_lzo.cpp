#include <vector>
#include <string.h>
#include "util/inv_lzo.h"
#include "util/inv_util.h"
#include "log/inv_log.h"

using namespace std;
using namespace inv;

INV_ThreadLock INV_Lzo::g_lzotl;
INV_LzoPtr INV_Lzo::g_lzo = NULL;

INV_Lzo* INV_Lzo::getInstance()
{
    if(!g_lzo)
    {
        INV_ThreadLock::Lock lock(g_lzotl);

        if(!g_lzo)
        {
            g_lzo = new INV_Lzo();
        }
    }
    return g_lzo.get();
}

int INV_Lzo::lzo_compress(const string &in, string &out)
{
    int ret = 0;
    lzo_uint out_len;

    vector<char> outbuf;
    outbuf.resize((in.size() + in.size() / 16 + 64 + 3));

    vector<char> wrkmem;
    wrkmem.resize(LZO1X_1_MEM_COMPRESS);

    int r = lzo1x_1_compress((const unsigned char*)in.data(), in.size(),
        (unsigned char*)&outbuf[0], &out_len,
        (unsigned char*)&wrkmem[0]);
    if (r != LZO_E_OK)
    {
        ret = -1;
    }

    out.assign(&outbuf[0], out_len);

    //cout << out_len << ":" << INV_Util::bin2str(out) << endl;

    //增加4个字节的原始长度用于解压缩
    int rawlen = in.size();
    char lenflag[4];
    memset(lenflag, 0x0, sizeof(lenflag));
    memcpy(lenflag, &rawlen, sizeof(lenflag));

    out += string(lenflag, sizeof(lenflag));

    //cout << out.length() << ":" << INV_Util::bin2str(out) << endl;

    return ret;
}

int INV_Lzo::lzo_decompress(const string &in, string &out)
{
    int ret = 0;

    //最后4个字节用来表示原始内容的长度
    lzo_uint rawlen = 0;
    memcpy(&rawlen, in.data()+in.size()-4, 4);

    string s(in.data()+in.size()-4, 4);
    //cout << INV_Util::bin2str(s) << endl;

    if(rawlen > INV_LZO_MAX)
    {
        FDLOG("error") << "lzo_decompress rawlen|" << rawlen << "|" << in.size() << endl;
        return -2;
    }

    vector<char> outbuf;
    outbuf.resize(rawlen);

    int r = lzo1x_decompress_safe((const unsigned char*)in.data(), in.size()-4,
        (unsigned char*)&outbuf[0], &rawlen,
        NULL);
    if (r != LZO_E_OK)
    {
        ret = -1;
    }

    out.assign(&outbuf[0], rawlen);

    return ret;
}

