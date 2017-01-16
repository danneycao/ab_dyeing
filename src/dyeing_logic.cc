#include "push_user_type.h"
#include "dyeing_logic.h"
#include "dyeing_handler.h"
#include "white_list.h"
#include "online_hash.h"

#include "util/inv_config.h"
#include "util/inv_qconfig.h"

#include "proto/gen-cpp/ab_dyeing_types.h"
#include "proto/gen-cpp/ab_dyeing_constants.h"

#include <protocol/TCompactProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/server/TNonblockingServer.h>

namespace {

inveno::ABDyeingEntry toEntry(std::string type, const std::string& config_id) {
  inveno::ABDyeingEntry entry;
  
  entry.__set_type(type);
  entry.__set_config_id(config_id);
  return entry;
}
}

namespace inv {

std::chrono::time_point<std::chrono::high_resolution_clock> TimeStampMs() {
      return std::chrono::high_resolution_clock::now();
}

DyeingLogic::DyeingLogic(ABDyeingHandler* owner, kafkaFreshQueue* kq)
    : _owner(owner) , _kmq(kq){
  assert(owner != NULL);
}

DyeingLogic::~DyeingLogic() {
}

bool DyeingLogic::init(const std::string& conf_path) {
	_qconf.reset(new INV_QConfig);
    
	INV_Config conf;
	conf.parseFile(conf_path);
	
	_fresh_product_suffix = conf.get("/main/base/[fresh_product_suffix]");
    if(!_fresh_product_suffix.length())
        _fresh_product_suffix = "fresh";
    
  return true;
}

bool DyeingLogic::onDyeing(const std::string& uid, const std::string& app,
                           const std::string& app_ver, const std::string& app_lan, 
                           std::string type, inveno::ABDyeingReply* reply) {
  std::string config_id;
  if (handleWhiteList(uid, app, app_ver, app_lan, type, &config_id)) {
    reply->__isset._entries = true;
    reply->_entries.push_back(toEntry(type, config_id));
    return true;
  }

  std::string dyeing_app = app;
  if ("biz" == type && "noticias" == app) {

      std::string user_type;
      if (!handlePushUser(uid, dyeing_app, app_ver, app_lan, type, &user_type)) {

          if(2== handleFreshUser(uid, dyeing_app, app_ver, app_lan, type, &config_id)){
              
              dyeing_app = app + "_new";
          } else {

              dyeing_app = app + "_recurring";
          }
      } else {

          dyeing_app = app + "_" + user_type;
      } 
  } else{
  
      if(0 != handleFreshUser(uid, dyeing_app, app_ver, app_lan, type, &config_id)){
          dyeing_app = app + "_" + _fresh_product_suffix;
      }
  }

  TLOGINFO(__FILE__<<"-"<<__LINE__ << "| uid:" << uid <<"| dyeing_app: " << dyeing_app 
          << "|app: " << app << "|type:"<< type << std::endl);
  if (!handleOnlineHash(uid, dyeing_app, app_ver, app_lan, type, &config_id)) {
      return false;
  }

  reply->__isset._entries = true;
  reply->_entries.push_back(toEntry(type, config_id));
  return true;
}

bool DyeingLogic::handleWhiteList(const std::string& uid,
                                  const std::string& app,
                                  const std::string& app_ver,
                                  const std::string& app_lan,
                                  std::string type,
                                  std::string* config) {
  auto while_list = _owner->getWhiteList();
  if (while_list != NULL) {

    if (while_list->get(uid, app, app_ver, app_lan, type, config)) {
      // todo: log it
      return true;
    }
  }

  return false;
}

bool DyeingLogic::handleOnlineHash(const std::string& uid,
                                   const std::string& app,
                                   const std::string& app_ver,
                                   const std::string& app_lan,
                                   std::string type,
                                   std::string* config) {
  auto hash_plugin = _owner->getOnline();
  if (hash_plugin != NULL) {

    if (hash_plugin->get(uid, app, app_ver, app_lan, type, config)) {
      return true;
    }
  }

  return false;
}

int DyeingLogic::handleFreshUser(const std::string& uid,
                                  const std::string& app,
                                  const std::string& app_ver,
                                  const std::string& app_lan,
                                  std::string& type,
                                  std::string* config) {
    int is_new=0;
    auto fresh_user = _owner->getFreshUser();
    if (NULL != fresh_user && fresh_user.get()) {
        if(fresh_user->get(uid, app, app_ver, app_lan, type, config)) {
            is_new= 1;
        } 
    }
       
    if(!is_new) {
        INV_ThreadRLock rlock(_kmq->lock);
        if(_kmq->kq.find(uid) != _kmq->kq.end())
        {
            is_new =2;
        }
    }

    if (0 != is_new) {
        FDLOG("new_user") << __FILE__<<"-"<<__LINE__<<"| " << uid << "|" << is_new <<endl;
    }

    return is_new;
}

bool DyeingLogic::handlePushUser(const std::string& uid,
                                  const std::string& app,
                                  const std::string& app_ver,
                                  const std::string& app_lan,
                                  std::string& type,
                                  std::string* user_type) {
    auto push_user = _owner->getPushUser();
    if (NULL != push_user && 
        push_user.get()) {

        TLOGINFO(__FILE__<<"-"<<__LINE__ << "| handlePushUser uid:" << uid 
                << "|app: " << app << "|type:"<< type << "push_user map_size: " << push_user->getsize() << std::endl);

        if(push_user->get(uid, app, app_ver, app_lan, type, user_type)) {
            return true;
        } 
    }
   
    return false;
}

}
