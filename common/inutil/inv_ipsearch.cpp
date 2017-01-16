#include <stdint.h>
#include <arpa/inet.h>
#include "util/inv_ipsearch.h"
#include "util/inv_util.h"
#include "util/inv_file.h"
#include "util/inv_xxtea.h"

#define INV_KEY "INV!@#$)(*!293sF"

using namespace std;

namespace inv
{

INV_ThreadLock INV_IP::g_iptl;
INV_IPPtr INV_IP::g_ip = NULL;

INV_IP* INV_IP::getInstance()
{
    if(!g_ip)
    {
        INV_ThreadLock::Lock lock(g_iptl);

        if(!g_ip)
        {
            g_ip = new INV_IP();
        }
    }
    return g_ip.get();
}

uint32_t INV_IP::s2u(const string &s)
{
	unsigned u = strtoul(s.c_str(),NULL,10);
	return u;
}

uint32_t INV_IP::s2ip(const std::string& sip)
{
	string ip1,ip2,ip3,ip4;
	unsigned dip, p1,p2,p3;
	dip=0;
	p1 = sip.find('.');
	p2 = sip.find('.', p1 + 1);
	p3 = sip.find('.', p2 + 1);
	p3 = sip.find('.', p2 + 1);
	ip1 = sip.substr(0, p1);
	ip2 = sip.substr(p1 + 1, p2 - p1 -1 );
	ip3 = sip.substr(p2 + 1, p3 - p2 -1 );
	ip4 = sip.substr(p3 + 1, sip.size() - p3 -1);
	(((unsigned char*)&dip)[0])  = s2u(ip1);
	(((unsigned char*)&dip)[1])  = s2u(ip2);
	(((unsigned char*)&dip)[2])  = s2u(ip3);
	(((unsigned char*)&dip)[3])  = s2u(ip4);
	return htonl(dip);
}

int INV_IP::init(const string &filepath)
{
	string strfile = INV_File::load2str(filepath);
	if (strfile.size() == 0)
	{
		return -1;
	}

	string tbs = INV_xxTea::decrypt(strfile, INV_KEY);

	vector<string> ips = INV_Util::sepstr<string>(tbs, "\n");
	if(ips.size() == 0)
	{
		return -2;
	}

	_infos.resize(ips.size());
	for(size_t i=0;i<ips.size();i++)
	{
		vector<string> vinfos = INV_Util::sepstr<string>(ips[i], "|");
		if(vinfos.size() < 5)
		{
			continue;
		}

		INV_IPInfo info;
		info.from = INV_Util::strto<int32_t>(vinfos[0]);
		info.to = INV_Util::strto<int32_t>(vinfos[1]);
		info.opera = vinfos[2];
		info.city = vinfos[3];
		info.province = vinfos[4];

        if(info.city == "unknown" &&
            (info.province == "北京"
            ||info.province == "上海"
            ||info.province == "天津"
            ||info.province == "重庆"))
        {
            info.city = info.province;
            //cout<<"Debug:province:"<<info.city<<endl;
        }

		//cout << info.city  << endl;

		_infos.push_back(info);
	}

	return 0;
}

int INV_IP::getIPInfo(uint32_t ip, INV_IPInfo &info)
{
	int find = 0;
	int low = 0;
	int high = _infos.size() - 1;
	while(low <= high)
	{
		int mid = (low + high) / 2;
		unsigned int from = _infos[mid].from;
		unsigned int to = _infos[mid].to;

		find++;

		if((ip >= from) && (ip<= to))
		{
			info.opera = _infos[mid].opera;
			info.city = _infos[mid].city;
			info.province = _infos[mid].province;
			info.country = _infos[mid].country;
			info.find = find;
			return 0;
		}
		
		if(ip < from)
		{
			high = mid -1 ;
		}
	
		if(ip > to)
		{
			low = mid + 1;
		}
	}	
	return -1;
}

int INV_IP::getIPInfo(const string& strIP, INV_IPInfo &info)
{
	return getIPInfo(inet_network(strIP.c_str()), info);
}

}
