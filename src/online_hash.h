#pragma once

#include <string>
#include "dyeing_plugin.h"

namespace inv {

class DyeingOnlinePlugin : public DyeingPlugin {
  public:
    explicit DyeingOnlinePlugin(const std::string& prefix);
    virtual ~DyeingOnlinePlugin();

    virtual bool loadConfig(const std::vector<std::string>& types);
    virtual bool get(const std::string& uid, const std::string& app, 
                     const std::string& app_ver, const std::string& app_lan,
                     const std::string& type, std::string* config_id) const;

  private:
    const std::string _prefix;
    bool loadInternal(const std::string& type);
    bool ParseProductPolicy(const std::string& type,
                            const std::string& product_conf);

    struct ConfIdInfo{
        std::string conf_id;
        uint32 start_id;
        uint32 end_id;   
    };
    struct Policy {
        uint32 seed;
        std::string prefix;
        std::vector<ConfIdInfo> conf_id_infos;
    };

    std::unordered_map<std::string,
        std::unordered_map<std::string, std::shared_ptr<Policy>>>_policy_map;

    DISALLOW_COPY_AND_ASSIGN(DyeingOnlinePlugin);
  };

}
