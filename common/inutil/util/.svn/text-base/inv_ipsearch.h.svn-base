#ifndef _INV_IPSEARCH_
#define _INV_IPSEARCH_

#include <string>
#include <vector>

#include "util/inv_monitor.h"
#include "util/inv_thread.h"
#include "util/inv_autoptr.h"

using namespace std;

namespace inv
{

typedef struct _INV_IPInfo
{
	int32_t from;
	int32_t to;
	int32_t find;
	string opera;		//运营商
	string city;		//城市
	string province;	//省
	string country;		//国家

}INV_IPInfo;

class INV_IP;

typedef INV_AutoPtr<INV_IP> INV_IPPtr;

class INV_IP : public INV_HandleBase
{
public:
	static INV_IP* getInstance();

	INV_IP(){};
	~INV_IP(){};

	int init(const string &filepath);
	int getIPInfo(const string& strIP,INV_IPInfo &info);
	int getIPInfo(uint32_t ip, INV_IPInfo &info);

	uint32_t s2ip(const std::string& sip);

protected:
	static INV_ThreadLock        g_iptl;

	static INV_IPPtr   g_ip;

private:
	uint32_t s2u(const string &s);
	

	vector<INV_IPInfo> _infos;
};

}

#endif
