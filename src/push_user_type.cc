#include "push_user_type.h"
#include "util/inv_util.h"

#include "log/inv_log.h"

#include "util/inv_config.h"
#include "util/inv_qconfig.h"

namespace inv {

bool DyeingPushUserTypePlugin::flushPushUserType() {
    ProcessCostTimeMonitor process_monitor("DyeingPushUserTypePlugin::flushPushUserType");

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
	sql = "select Timestamp, product_id, uid, push_user_type from t_daily_push_user_type where product_id=\"noticias\";";
	TLOGINFO(__FILE__ << "-" << __LINE__
              << ": DyeingPushUserTypePlugin db_fresh sql: " << sql 
              << endl);
    
    inv::INV_Mysql::MysqlData records;
    try {

        records = _db.queryRecord(sql);
        if (records.size() <= 0) {
            _db.disconnect();
            TLOGINFO(__FILE__ << "-" << __LINE__
                    << ": recoreds.size() <=0" << endl);
            return ret;
        }
    } catch (inv::INV_Mysql_Exception &e) {
        _db.disconnect();
        TLOGINFO(__FILE__ << "-" << __LINE__
                << ": catch a excep: " << e.what() 
                << endl);
        return ret;
    }

	if(records.size() != 0) {
    	TLOGINFO(__FILE__ << "-" << __LINE__
              << ": DyeingPushUserTypePlugin qvalue: records: " << records.size()
              << " flush before clear" << endl);
    	_user_type_map.clear();
    }
    for(size_t i=0; i< records.size(); i++) {

        std::string uid = records[i]["uid"];
        std::string product_id = records[i]["product_id"];
        std::string user_type = records[i]["push_user_type"];
        uint64_t timestamp = std::atol(records[i]["Timestamp"].c_str());

        ret = SetMap(uid, product_id, timestamp, user_type);
    }

    TLOGINFO(__FILE__ << "-" << __LINE__
            << ": DyeingPushUserTypePlugin map_size(): " << getsize() << endl);

	_db.disconnect();

    return ret;
}

bool DyeingPushUserTypePlugin::get(const std::string& uid, const std::string& app, 
                     const std::string& app_ver, const std::string& app_lan,
                     const std::string& business_type, std::string* user_type) {
    std::string key = app + uid;
    auto iter = _user_type_map.find(key);
	if(iter != _user_type_map.end()){

        *user_type = iter->second->user_type;
    	TLOGINFO(__FILE__ << "-" << __LINE__
              << ": uid " << uid
              << ": key " << key
              << ": find _user_type_map"
              << ": user_type " << *user_type 
              << ": size " << _user_type_map.size() << endl);

        return true;  
    }

    TLOGINFO(__FILE__ << "-" << __LINE__
            << ": uid " << uid
            << ": key " << key
            << ": no find _user_type_map"
            << ": size " << _user_type_map.size() << endl);

	return false;
}

uint64_t DyeingPushUserTypePlugin::getsize(){
	return (uint64_t)_user_type_map.size();
}

bool DyeingPushUserTypePlugin::SetMap(const std::string& uid,
                                      const std::string& product_id,
                                      const uint64_t& timestamp,
                                      const std::string& user_type) {

    std::string key = product_id + uid;
    auto iter = _user_type_map.find(key);
    if (iter == _user_type_map.end()) {

        std::shared_ptr<UserType> ptr_user_type(new UserType());
        ptr_user_type->timestamp = timestamp;
        ptr_user_type->user_type = user_type;

        _user_type_map[key] = ptr_user_type;
    } else {

        if (iter->second->timestamp < timestamp) {
        
            iter->second->timestamp = timestamp;
            iter->second->user_type = user_type;
        }
    }
    
    return true;
}

}
