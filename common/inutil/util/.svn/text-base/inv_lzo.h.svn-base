#ifndef _INV_LZO_H
#define _INV_LZO_H

#include <string>

#include "lzo/lzoconf.h"
#include "lzo/lzo1x.h"

#include "util/inv_monitor.h"
#include "util/inv_thread.h"
#include "util/inv_autoptr.h"

namespace inv
{

#define INV_LZO_MAX 10485760 //限制10MB

class INV_Lzo;

typedef INV_AutoPtr<INV_Lzo> INV_LzoPtr;

class INV_Lzo : public INV_HandleBase
{
public:
	static INV_Lzo* getInstance();

	INV_Lzo()
	{
		lzo_init();
	}

	~INV_Lzo()
	{
	}

    int lzo_compress(const std::string &in, std::string &out);
    int lzo_decompress(const std::string &in, std::string &out);

protected:
    static INV_ThreadLock        g_lzotl;

    static INV_LzoPtr   g_lzo;

};



}

#endif
