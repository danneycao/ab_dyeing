#pragma once

#include "ab_def.h"
#include "util/inv_mysql.h"

namespace inv{

class DyeingPushUserTypePlugin {
  public:
	explicit DyeingPushUserTypePlugin(const std::string& prefix)
        :_prefix(prefix) {
    }
    virtual ~DyeingPushUserTypePlugin() = default;

    bool flushPushUserType();
    bool get(const std::string& uid, const std::string& app, 
             const std::string& app_ver, const std::string& app_lan,
             const std::string& business_type, std::string* user_type);
    uint64_t getsize();

  private:
	INV_Mysql _db;
    std::string _prefix;
    
    struct UserType {
        uint64_t    timestamp;
        std::string user_type;
    };

    /* first:  product_id+uid
     * second: usertype*/
	std::unordered_map<std::string, std::shared_ptr<UserType>> _user_type_map;

    bool SetMap(const std::string& uid, 
                const std::string& product_id, 
                const uint64_t& timestamp,
                const std::string& user_type);
    
    DISALLOW_COPY_AND_ASSIGN(DyeingPushUserTypePlugin);
  }; 
};
