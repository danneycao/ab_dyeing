#pragma once

#include "ab_def.h"

namespace inv {

class DyeingPlugin {
  public:
    virtual ~DyeingPlugin() = default;

    virtual bool loadConfig(const std::vector<std::string>& types) = 0;

    virtual bool get(const std::string& uid, const std::string& app,
                     const std::string& app_ver, const std::string& app_lan,
                     const std::string& type, std::string* config_id) const = 0;

  protected:
    const std::string _name;
    DyeingPlugin(const std::string& name)
        : _name(name) {
      assert(!name.empty());
    }
};
}
