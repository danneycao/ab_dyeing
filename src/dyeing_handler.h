#pragma once

#include "push_user_type.h"
#include "fresh_user.h" 
#include "ab_def.h"
#include "fresh_kafka.h"
#include "proto/gen-cpp/ABDyeingService.h"

namespace inv {

class DyeingLogic;
class DyeingOnlinePlugin;
class DyeingWhiteListPlugin;

class INV_Thread;
class ABDyeingHandler : public inveno::ABDyeingServiceIf {
  public:
    ABDyeingHandler();
    virtual ~ABDyeingHandler();

    void flushPolicy();
    void flushFreshUserPolicy();
    void init(const std::string& conf_path);

    boost::shared_ptr<DyeingOnlinePlugin> getOnline() const {
      return _online_plugin;
    }
    boost::shared_ptr<DyeingWhiteListPlugin> getWhiteList() const {
      return _white_list;
    }
    boost::shared_ptr<DyeingFreshUserPlugin> getFreshUser() const {
      return _fresh_user;
    }
    boost::shared_ptr<DyeingPushUserTypePlugin> getPushUser() const {
      return _push_user;
    }

  private:
    virtual void dyeing(inveno::ABDyeingReply& _return, const std::string& uid,
                        const inveno::ABDyeingRequest& req);
    virtual void DyeingLan(inveno::ABDyeingReply& _return, const std::string& uid,
                        const inveno::ABDyeingLanRequest& req);
    virtual void BatchDyeing(inveno::ABDyeingBatchReply& _return, 
                             const inveno::ABDyeingBatchRequest& batch_reqs);
    virtual void BatchDyeingLan(inveno::ABDyeingBatchReply& _return,
                                const inveno::ABDyeingBatchLanRequest& batch_lan_reqs); 
    void SingleDyeing(inveno::ABDyeingUidRepInfo& uid_rep_info,
                      const inveno::ABDyeingUidReqInfo& uid_req_info); 
    void SingleLanDyeing(inveno::ABDyeingUidRepInfo& uid_rep_info,
                      const inveno::ABDyeingUidLanReqInfo& uid_lan_req_info); 
    std::vector<std::string> _types;
    std::string _white_prefix;
    std::string _online_prefix;
    std::string _fresh_prefix;
    std::string _fresh_hour;
    boost::shared_ptr<DyeingOnlinePlugin> _online_plugin;
    boost::shared_ptr<DyeingWhiteListPlugin> _white_list;
    boost::shared_ptr<DyeingFreshUserPlugin> _fresh_user;
    boost::shared_ptr<DyeingPushUserTypePlugin> _push_user;

    std::unique_ptr<INV_Thread> _flush_thread;
    std::unique_ptr<INV_Thread> _flush_fresh_user_thread;
    
    std::map<pthread_t, boost::shared_ptr<DyeingLogic> > _logics;
    boost::shared_ptr<DyeingLogic> getLogic() const {
      auto it = _logics.find(::pthread_self());
      return it != _logics.end() ? it->second : boost::shared_ptr<DyeingLogic>();
    }
  private:
	kafkaFreshQueue _kmq;
	std::unique_ptr<DyeingCurrentFresh> _fresh_current_list;

//    int64_t _count;
//    time_t  _time;
    DISALLOW_COPY_AND_ASSIGN(ABDyeingHandler);
};

}
