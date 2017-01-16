#pragma once

#include "dyeing_plugin.h"

namespace inv {

class DyeingWhiteListPlugin : public DyeingPlugin {
  public:
    explicit DyeingWhiteListPlugin(const std::string& prefix);
    virtual ~DyeingWhiteListPlugin() = default;

    virtual bool loadConfig(const std::vector<std::string>& business_types);
    virtual bool get(const std::string& uid, const std::string& app, 
                     const std::string& app_ver, const std::string& app_lan,
                     const std::string& business_type, std::string* config_id) const {
      auto it = _map.find(business_type);
      if (it != _map.end()) {
        const auto& lmap = it->second;
        auto jt = lmap.find(uid);
        if (jt != lmap.end()) {
          *config_id = jt->second;
          return true;
        }
      }
      return false;
    }

  private:
    const std::string _prefix;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _map;
    void pushConf(const std::string& business_type, const std::string& k, const std::string& v) {
      auto& kv_map = _map[business_type];
      kv_map[k] = v;
    }

    bool loadInternal(const std::string&business_type);

    DISALLOW_COPY_AND_ASSIGN(DyeingWhiteListPlugin);
};

}
