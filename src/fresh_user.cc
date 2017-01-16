#include "fresh_user.h"
#include "util/inv_util.h"

#include "log/inv_log.h"

#include "util/inv_config.h"
#include "util/inv_qconfig.h"

namespace inv {

bool DyeingFreshUserPlugin::flushFreshUser() {
    
    ProcessCostTimeMonitor process_monitor("DyeingFreshUserPlugin::flushFreshUser");

	bool ret = false;
	INV_QConfig qconf;
	std::string ch_host = qconf.get(_prefix + "/ch_host");
	int32_t ch_port = std::atoi(qconf.get(_prefix + "/ch_port").c_str());
	std::string ch_user = qconf.get(_prefix + "/ch_user");
	std::string ch_pass = qconf.get(_prefix + "/ch_pass");
	std::string ch_db = qconf.get(_prefix + "/ch_db");
	std::string ch_charset = qconf.get(_prefix + "/ch_charset");
	_db.init(ch_host, ch_user, ch_pass, ch_db, ch_charset, ch_port);
	_db.connect();
	string sql;
	int day_now = time(NULL)/(24*3600);
	char daybuf[32];
	sprintf(daybuf,"%d",day_now);
	string epoch_day = daybuf;
	sql = "select uid from daily_fresh_user where epoch_day=" + epoch_day ;
	
	TLOGINFO(__FILE__ << "-" << __LINE__
              << ": DyeingFreshUserPlugin db_fresh sql: " << sql
              << endl);
    
    inv::INV_Mysql::MysqlData records;
    try {

        records = _db.queryRecord(sql);
        if (records.size() <= 0) {
        	_db.disconnect();
            return ret;
        }
    } catch (inv::INV_Mysql_Exception &e) {
    	_db.disconnect();
        return ret;
    }

	if(records.size() != 0) {
    	TLOGINFO(__FILE__ << "-" << __LINE__
              << ": DyeingFreshUserPlugin qvalue: records: " << records.size()
              << endl);
    	_map.clear();
    }
    for(size_t i=0;i<records.size();i++) {
        std::string uid = records[i]["uid"];
    	ret = SetMap(uid);
    }

	_db.disconnect();

    return ret;
}

bool DyeingFreshUserPlugin::SetMap(const std::string& uid) {
    _map[uid] = TNOWUS;
	return true;
}

bool DyeingFreshUserPlugin::get(const std::string& uid, const std::string& app, 
                     const std::string& app_ver, const std::string& app_lan,
                     const std::string& business_type, std::string* config_id){
	auto it = _map.find(uid);
	if(it != _map.end()){
    	return true;  
    }
	return false;
}

uint64_t DyeingFreshUserPlugin::getsize(){
	return (uint64_t)_map.size();
}

}
