#pragma once

#include "ab_def.h"
#include "util/inv_mysql.h"

namespace inv{

class DyeingFreshUserPlugin {
  public:
	explicit DyeingFreshUserPlugin(const std::string& prefix)
        :_prefix(prefix) {
    }
    virtual ~DyeingFreshUserPlugin() = default;

    bool flushFreshUser();
    bool get(const std::string& uid, const std::string& app, 
             const std::string& app_ver, const std::string& app_lan,
             const std::string& business_type, std::string* config_id);
    uint64_t getsize();

  private:
	bool SetMap(const std::string& uid);
    
	INV_Mysql _db;
    std::string _prefix;
	std::unordered_map<std::string, uint64_t> _map;
    
    DISALLOW_COPY_AND_ASSIGN(DyeingFreshUserPlugin);
  }; 
};

