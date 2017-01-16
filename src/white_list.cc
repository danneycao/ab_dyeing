#include "white_list.h"
#include "util/inv_util.h"

#include "log/inv_log.h"

#include "util/inv_config.h"
#include "util/inv_qconfig.h"

namespace inv {

DyeingWhiteListPlugin::DyeingWhiteListPlugin(const std::string& prefix)
    : DyeingPlugin("while_list_plugin"), _prefix(prefix) {
}

bool DyeingWhiteListPlugin::loadConfig(const std::vector<std::string>& business_types) {
  
  INV_QConfig qconf;
  std::string types_path = _prefix + "/value";
  std::string qvalue = qconf.get(types_path);

  std::vector<std::string> tmp_business_types;
  if (!qvalue.empty()) {

      tmp_business_types = inv::INV_Util::sepstr<std::string>(qvalue, "|", true);
  } else {

      TLOGINFO(__FILE__ << "-" << __LINE__
              << ": in_DyeingWhiteListPlugin_loadConfig qvalue empty: "
              << ": types_path: " << types_path
              << endl);
  }
  if (tmp_business_types.empty()) {
      tmp_business_types = business_types;
  }

  bool rtval = false;
  for (auto types_iter = tmp_business_types.begin();
       types_iter != tmp_business_types.end();
       types_iter++) {

      rtval = loadInternal(*types_iter);
  }

  return rtval;
}

bool DyeingWhiteListPlugin::loadInternal(const std::string& business_type) {
  INV_QConfig qconf;

  std::string full_path = _prefix + "/" + business_type + "/value";
  std::string qvalue = qconf.get(full_path);
#if 0   // just for test
  qvalue = "123=2;456=3";
#endif
  auto keys = INV_Util::sepstr<std::string>(qvalue, ";");
  for (auto it = keys.begin(); it != keys.end(); ++it) {
    auto kv = INV_Util::sepstr<std::string>(*it, "=");
    if (kv.size() != 2) {
      // todo: log it.
      continue;
    }

    // todo: log it.
    pushConf(business_type, kv[0], kv[1]);
  }

  return true;
}

}
