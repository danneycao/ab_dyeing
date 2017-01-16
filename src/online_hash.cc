#include "ab_def.h"
#include "online_hash.h"
#include "util/inv_util.h"
#include "log/inv_log.h"

#include "city.h"

#include "util/inv_config.h"
#include "util/inv_qconfig.h"

namespace inv {

DyeingOnlinePlugin::DyeingOnlinePlugin(const std::string& prefix)
    : DyeingPlugin("online_plugin"), _prefix(prefix) {
}

DyeingOnlinePlugin::~DyeingOnlinePlugin() {
}

bool DyeingOnlinePlugin::loadConfig(const std::vector<string>& business_types) {

  INV_QConfig qconf;
  std::string types_path = _prefix + "/value";
  std::string qvalue = qconf.get(types_path);

  TLOGINFO(__FILE__ << "-" << __LINE__
          << ": in_DyeingOnlinePlugin_loadConfig qvalue "
          << ": _prefix: " << _prefix
          << ": types_path: " << types_path
          << ": qvalue: " << qvalue
          << endl);
  
  std::vector<std::string> tmp_business_types;
  if (!qvalue.empty()) {
      
      tmp_business_types = inv::INV_Util::sepstr<std::string>(qvalue, "|", true);
  } else {
      
      TLOGINFO(__FILE__ << "-" << __LINE__
              << ": in_DyeingOnlinePlugin_loadConfig qvalue empty"
              << ": types_path: " << types_path
              << endl);
  }
  if (tmp_business_types.empty()) {
      tmp_business_types = business_types;
  }

  bool rtval = false;
  for (auto business_iter = tmp_business_types.begin();
       business_iter != tmp_business_types.end();
       business_iter++) {

      rtval = loadInternal(*business_iter);
  }
  return rtval;
}

bool DyeingOnlinePlugin::ParseProductPolicy(const std::string& business_type,
                                            const std::string& product_conf) {
  auto& product_policy_map = _policy_map[business_type];
  auto products = inv::INV_Util::sepstr<std::string>(product_conf, ";", true);
  for (uint32 i = 0; i < products.size(); ++i) {

    auto product_policy = inv::INV_Util::sepstr<std::string>(products[i], "|",
                                                             true);
    if (product_policy.size() != 2) {
      TLOGINFO(
          __FILE__<<"-"<<__LINE__ << "| error product policy: " << products[i] << endl);
      continue;
    }

    std::shared_ptr<Policy> ptr_policy(new Policy());
    std::string product_id = product_policy[0];

    auto fields = inv::INV_Util::sepstr<std::string>(product_policy[1], ":",
                                                     true);
    if (fields.size() < 3) {
      TLOGINFO(
          __FILE__<<"-"<<__LINE__ << "| error policy: " << product_policy[1] << endl);
      continue;
    }

    ptr_policy->prefix = fields[0];
    ptr_policy->seed = std::atoi(fields[1].c_str());
    for (size_t index=2; index < fields.size(); index++) {
        
        ConfIdInfo conf_id_info;
        auto config_ids = inv::INV_Util::sepstr<std::string>(fields[index], ",",
                                                             true);
        if (config_ids.size() != 3) {
            TLOGINFO(
                    __FILE__<<"-"<<__LINE__ << "| error policy: " << fields[index] << endl);
            continue;
        }

        conf_id_info.conf_id =  config_ids[0]; 
        conf_id_info.start_id = std::atoi(config_ids[1].c_str());
        conf_id_info.end_id = std::atoi(config_ids[2].c_str());
        
        ptr_policy->conf_id_infos.push_back(conf_id_info);
    }
  
    product_policy_map[product_id]= ptr_policy;
  }
  
  return true;
}

bool DyeingOnlinePlugin::loadInternal(const std::string& business_type) {
  std::string full_path = _prefix + "/" + business_type + "/value";

  INV_QConfig qconf;
  std::string product_conf = qconf.get(full_path);

  TLOGINFO(__FILE__<<"-"<<__LINE__ 
            << " : in loadTypeConfig business_type: " << business_type
            << " : _prefix: " << _prefix
            << " : full_path: " << full_path
            << " : product_conf: " << product_conf
            << endl);

#if 0 // just for test
  product_conf = "emui|:0:62,1,100:63,101,200:-1,201,1000;coolpad|b:1:64,901,1000";
#endif
  if (product_conf.empty()) {
    TLOGINFO(__FILE__<<"-"<<__LINE__ 
            << " : error in loadTypeConfig business_type: " << business_type 
            << " : qvalue is empty full_path: " << full_path
            << " : product_conf: " << product_conf
            << endl);
    return false;
  }

  // config_format : product_id|prefix:seed:conf_id,start_id,end_id:conf_id,start_id,end_id;
  //                 \product_id|prefix:seed:conf_id,start_id,end_id;conf_id,start_id,end_id
  if (!ParseProductPolicy(business_type, product_conf)) {
      TLOGINFO(__FILE__<<"-"<<__LINE__ 
              << ": error in ParseProductPolicy business_type: " << business_type 
              << ": product_conf: " << product_conf 
              << endl);
      return false;
  }

  return true;
}

bool DyeingOnlinePlugin::get(const std::string& uid, const std::string& app,
                             const std::string& app_ver, const std::string& app_lan,
                             const std::string& business_type, std::string* config_id) const {
  auto it = _policy_map.find(business_type);
  if (it == _policy_map.end()) {

      TLOGINFO(__FILE__<<"-"<<__LINE__ 
              << ": error in DyeingOnlinePlugin get() no support type: " << business_type 
              << ": app: " << app 
              << ": app_ver: " << app_ver
              << ": app_lan: " << app_lan
              << endl);
      return false;
  }

  std::string tmp_product_id = app+app_ver+app_lan;
  auto& product_policy_map = it->second;
  if (product_policy_map.find(tmp_product_id) == product_policy_map.end()) { // app + app_ver + app_lan
   
      tmp_product_id = app+app_lan;
      if (product_policy_map.find(tmp_product_id) == product_policy_map.end()) { // app + app_lan

          tmp_product_id = app+app_ver;
          if (product_policy_map.find(tmp_product_id) == product_policy_map.end()) { // app + app_ver

            tmp_product_id = app;
            if (product_policy_map.find(tmp_product_id) == product_policy_map.end()) { // app 
                tmp_product_id = "default";
            }
          }
      }
  }

  auto jt = product_policy_map.find(tmp_product_id);
  if (jt == product_policy_map.end()) {

    TLOGINFO(__FILE__<<"-"<<__LINE__ 
            << ": error in DyeingOnlinePlugin::get business_type: " << business_type
            << ": app: " << app 
            << ": app_ver: " << app_ver
            << ": app_lan: " << app_lan
            << endl);
    return false;
  }

  auto ptr_policy = jt->second;
  const static int max_config_id = 1000;
  uint64_t hv = CityHash64WithSeed(uid.c_str(), uid.size(), ptr_policy->seed) % max_config_id;

  std::string conf_id;
  for (size_t index =0; index < ptr_policy->conf_id_infos.size(); index++) {
   
      ConfIdInfo conf_id_info = ptr_policy->conf_id_infos[index];
      if (hv >= conf_id_info.start_id && hv <= conf_id_info.end_id) {
        
          conf_id = conf_id_info.conf_id;
          break;
      }
  }

  if (conf_id.empty()) { // no find set first

      conf_id = ptr_policy->conf_id_infos[0].conf_id;
  }

  TLOGINFO(__FILE__<<"-"<<__LINE__ 
           << ": in_DyeingOnlinePlugin::get business_type: " << business_type 
           << ": app: " << app
           << ": app_ver: " << app_ver
           << ": app_lan: " << app_lan
           << ": hv: "  << hv
           << ": conf_id: " << conf_id <<  endl);

  *config_id = ptr_policy->prefix + conf_id;

  return true;
}

}
