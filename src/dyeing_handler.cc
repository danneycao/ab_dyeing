#include "dyeing_handler.h"
#include "online_hash.h"
#include "white_list.h"
#include "dyeing_logic.h"

#include "log/inv_log.h"
#include "util/inv_config.h"
#include "util/inv_qconfig.h"
#include "util/inv_thread.h"

#include "util/inv_application.h"

#include "proto/gen-cpp/ab_dyeing_types.h"
#include "proto/gen-cpp/ab_dyeing_constants.h"

namespace inv {

class FreshThread : public INV_Thread {
  public:
    explicit FreshThread(ABDyeingHandler* owner, const std::string& fresh_hour)
        : _owner(owner), _fresh_hour(fresh_hour), _fresh_tags(true),_init_mysql(false){
      assert(owner != NULL);
    }
    virtual ~FreshThread() {
      stop();
    }
    //void init()

    void stop() {
      _running = false;
      getThreadControl().join();
    }
	std::string getAbTimeOfHour(bool now) {
    	time_t timep;
    	struct tm *p_tm;
    	timep = time(NULL);
    	p_tm = localtime(&timep);
    	std::string ret;
		char buf[32];
    	if(now)
        	sprintf(buf,"%d",p_tm->tm_hour);
    	else
		{
			int clean_hour = p_tm->tm_hour + 1;
			if(p_tm->tm_hour == 23)
				clean_hour = 0;
        	sprintf(buf,"%d",clean_hour);
		}
		ret = buf;
    	return ret;
    }

    virtual void run() {
      while (_running) {
			if(!_init_mysql){
				_owner->flushFreshUserPolicy();
				_init_mysql = true;
			}
        	sleep(10 * 60);
        	if(_fresh_hour == getAbTimeOfHour(true) && _fresh_tags)
            {
                ProcessCostTimeMonitor process_monitor("fresh_user");
            	_owner->flushFreshUserPolicy();
            	_fresh_tags = false;
            }
        	if(_fresh_hour == getAbTimeOfHour(false))
            	_fresh_tags = true;
        }
    }

  private:
    ABDyeingHandler* _owner;
	std::string _fresh_hour;
	bool _fresh_tags;
	bool _init_mysql;
};

class Thread : public INV_Thread {
  public:
    explicit Thread(ABDyeingHandler* owner)
        : _owner(owner) {
      assert(owner != NULL);
    }
    virtual ~Thread() {
      stop();
    }

    void stop() {
      _running = false;
      getThreadControl().join();
    }

    virtual void run() {
      while (_running) {
        // todo :replace with SyncEvent;
        // todo: flags.
        sleep(1 * 60);

        _owner->flushPolicy();
      }
    }

  private:
    ABDyeingHandler* _owner;
};

ABDyeingHandler::ABDyeingHandler() {
}

ABDyeingHandler::~ABDyeingHandler() {
  if (_flush_thread != NULL) {
    _flush_thread.reset();
  }
}

std::vector<boost::shared_ptr<apache::thrift::concurrency::Thread> > INVThreadFactory::thrift_threads;
void ABDyeingHandler::init(const std::string& conf_path) {
  // bind logic per thread.
  
  for (size_t i = 0; i < INVThreadFactory::thrift_threads.size(); i++) {
    
    pthread_t tid = INVThreadFactory::thrift_threads[i]->getId();
    cout << "thread started:" << tid << endl;

    _logics[tid].reset(new DyeingLogic(this, &_kmq));
    _logics[tid]->init(conf_path);
  }

  INV_Config conf;
  conf.parseFile(conf_path);

  std::string config_types = conf.get("/main/base/[types]");
  assert(!config_types.empty());
  _types = inv::INV_Util::sepstr<std::string>(config_types, "|", true);
  assert(_types.size() >0);

  _white_prefix = conf.get("/main/base/[white_prefix]");
  TLOGINFO(
      __FILE__ << "-" << __LINE__ << "| white prefix: " << _white_prefix << std::endl);
  _white_list.reset(new DyeingWhiteListPlugin(_white_prefix));
  _white_list->loadConfig(_types);

  _online_prefix = conf.get("/main/base/[online_prefix]");
  TLOGINFO(
      __FILE__<<"-"<<__LINE__ << "| online prefix: " << _online_prefix << std::endl);
  _online_plugin.reset(new DyeingOnlinePlugin(_online_prefix));
  _online_plugin->loadConfig(_types);

  _fresh_prefix = conf.get("/main/base/[fresh_prefix]");
  _fresh_hour = conf.get("/main/base/[fresh_hour]");
  TLOGINFO(__FILE__<<"-"<<__LINE__ 
          << "| fresh prefix: " << _fresh_prefix 
          << "| fresh hour: "<< _fresh_hour << std::endl);
  _fresh_user.reset(new DyeingFreshUserPlugin(_fresh_prefix));
  _fresh_user->flushFreshUser(); //用于初始化读取mysql数据 可能会消耗一点启动时间
  
  _push_user.reset(new DyeingPushUserTypePlugin(_fresh_prefix));
  //_push_user->flushPushUserType();

  _flush_fresh_user_thread.reset(new FreshThread(this, _fresh_hour));
  _flush_fresh_user_thread->start();

  _flush_thread.reset(new Thread(this));
  _flush_thread->start();
  
  _fresh_current_list.reset(new DyeingCurrentFresh(conf_path, &_kmq));
  _fresh_current_list->init(conf_path);

//  _count = 0;
//  _time = time(NULL);
}

void ABDyeingHandler::flushPolicy() {
  boost::shared_ptr<DyeingWhiteListPlugin> version(
      new DyeingWhiteListPlugin(_white_prefix));
  if (version->loadConfig(this->_types)) {
    auto old = _white_list;
    _white_list = version;
  }

  boost::shared_ptr<DyeingOnlinePlugin> online_plugin(
      new DyeingOnlinePlugin(_online_prefix));
  if (online_plugin->loadConfig(this->_types)) {
    auto old = _online_plugin;
    _online_plugin = online_plugin;
  }
}

void ABDyeingHandler::flushFreshUserPolicy() {
  boost::shared_ptr<DyeingFreshUserPlugin> fresh_user_plugin(
      new DyeingFreshUserPlugin(_fresh_prefix));
  if (fresh_user_plugin->flushFreshUser()) {
    auto old = _fresh_user; // refcount +1
    _fresh_user = fresh_user_plugin;
  }

  boost::shared_ptr<DyeingPushUserTypePlugin> push_user_plugin(
      new DyeingPushUserTypePlugin(_fresh_prefix));
  if (push_user_plugin->flushPushUserType()) {
      auto old = _push_user; // refcount +1
      _push_user = push_user_plugin;
      TLOGINFO(
              __FILE__<<"-"<<__LINE__ << "|_user_type_map.size: " 
              << _push_user->getsize() << std::endl);
  }

  return;
}

void ABDyeingHandler::DyeingLan(inveno::ABDyeingReply& reply, const std::string& uid,
                                const inveno::ABDyeingLanRequest& req) {
  ProcessCostTimeMonitor process_monitor("ABDyeingHandler::DyeingLan");

  try {
    boost::shared_ptr<DyeingLogic> logic(getLogic());
    if (logic == NULL) {
      reply.status = inveno::ab_status::RC_SERVER_ERROR;
      // should not here forever.
      TLOGINFO(
          __FILE__<<"-"<<__LINE__ << "| in_DyeingLan dyeing logic is nullptr"<< std::endl);
      return;
    }

    std::string app = req.app;
    std::string app_ver = req.app_ver;
    std::string app_lan = req.app_lan; 
    auto types = req._types;
    if (types.empty()) {
        TLOGINFO(__FILE__<<"-"<<__LINE__ 
                << "| req _types is empty error" 
                << std::endl);
        reply.status = inveno::ab_status::RC_SERVER_ERROR;
        return;
    }

    for (auto types_iter = types.begin();
         types_iter != types.end();
         types_iter++) {

        if (!logic->onDyeing(uid, app, app_ver, app_lan, *types_iter, &reply)) {
            TLOGINFO(__FILE__<<"-"<<__LINE__ << "| onDyeing error"
                    << " :uid: " << uid
                    << " :app: " << app
                    << " :app_ver: " << app_ver
                    << " :app_lan: " << app_lan
                    << " :type: " << *types_iter
                    << std::endl);
            continue;
        }
    }

    if (reply._entries.empty()) {
      reply.status = inveno::ab_status::RC_SERVER_ERROR;
    } else {
      reply.status = inveno::ab_status::RC_STATUS_OK;
    }

  } catch (std::exception& e) {
    reply.status = inveno::ab_status::RC_SERVER_FAULT;
    TLOGINFO(
        __FILE__<<"-"<<__LINE__ << "| in_DyeingLan exception: " << e.what() << std::endl);
  }

  return;
}
 
void ABDyeingHandler::dyeing(inveno::ABDyeingReply& reply,
                             const std::string& uid,
                             const inveno::ABDyeingRequest& req) {
 
  ProcessCostTimeMonitor process_monitor("ABDyeingHandler::dyeing");
#if 0  //just for press_test
  ++_count;
  if (time(NULL) - _time >= 1) {
      TLOGINFO(__FILE__<<"-"<<__LINE__
              << "| the pthread_id: " << pthread_self()
              << "| 1 seconds counts: " << _count<< std::endl);
      _count = 0;
      _time = time(NULL);
  }
#endif
  try {
    boost::shared_ptr<DyeingLogic> logic(getLogic());
    if (logic == NULL) {
      reply.status = inveno::ab_status::RC_SERVER_ERROR;
      // should not here forever.
      TLOGINFO(
          __FILE__<<"-"<<__LINE__ << "| dyeing logic is nullptr"<< std::endl);
      return;
    }

    std::string app = req.app;
    std::string app_ver = req.app_ver;
    std::string app_lan = "";
    auto types = req._types;
    if (types.empty()) {
        TLOGINFO(__FILE__<<"-"<<__LINE__ 
                << "| req _types is empty error" 
                << std::endl);
        reply.status = inveno::ab_status::RC_SERVER_ERROR;
        return;
    }

    for (auto types_iter = types.begin();
         types_iter != types.end();
         types_iter++) {

        if (!logic->onDyeing(uid, app, app_ver, app_lan, *types_iter, &reply)) {
            TLOGINFO(__FILE__<<"-"<<__LINE__ << "| onDyeing error"
                    << " :uid: " << uid
                    << " :app: " << app
                    << " :type: " << *types_iter
                    << std::endl);
            continue;
        }
    }

    if (reply._entries.empty()) {
      reply.status = inveno::ab_status::RC_SERVER_ERROR;
    } else {
      reply.status = inveno::ab_status::RC_STATUS_OK;
    }

  } catch (std::exception& e) {
    reply.status = inveno::ab_status::RC_SERVER_FAULT;
    TLOGINFO(
        __FILE__<<"-"<<__LINE__ << "| dyeing exception: " << e.what() << std::endl);
  }

  return;
}

void ABDyeingHandler::SingleDyeing(inveno::ABDyeingUidRepInfo& uid_rep_info,
                                   const inveno::ABDyeingUidReqInfo& uid_req_info) {
    uid_rep_info.uid = uid_req_info.uid;

    dyeing(uid_rep_info._uid_rep, 
           uid_req_info.uid,
           uid_req_info._uid_info);

    return;
}

void ABDyeingHandler::SingleLanDyeing(inveno::ABDyeingUidRepInfo& uid_rep_info,
                                      const inveno::ABDyeingUidLanReqInfo& uid_lan_req_info) {
    uid_rep_info.uid = uid_lan_req_info.uid;

    DyeingLan(uid_rep_info._uid_rep, 
              uid_lan_req_info.uid,
              uid_lan_req_info._uid_lan_info);

    return;
}

void ABDyeingHandler::BatchDyeing(inveno::ABDyeingBatchReply& _return, 
                                  const inveno::ABDyeingBatchRequest& batch_reqs) {
    
    ProcessCostTimeMonitor process_monitor("ABDyeingHandler::BatchDyeing");

    auto uid_req_infos = batch_reqs._uid_req_infos;
    if (uid_req_infos.size() > 1000) {        

        _return.status = inveno::ab_status::RC_SERVER_ERROR;
        TLOGINFO(__FILE__ << "-" << __LINE__ << "| BatchDyeing error"
                 << ": req_uid_size: " << uid_req_infos.size()
                 << " > 1000" << std::endl);
        return;
    }

    _return.__isset._uid_rep_infos = true;
    
    for (auto uid_req_info_iter = uid_req_infos.begin();
        uid_req_info_iter != uid_req_infos.end();
        uid_req_info_iter++) {
        
        inveno::ABDyeingUidRepInfo uid_rep_info;
        SingleDyeing(uid_rep_info, *uid_req_info_iter);

        bool isset_entries = uid_rep_info._uid_rep.__isset._entries;
        if (!isset_entries) {

            TLOGINFO(__FILE__ << "-" << "__LINE__" << "| BatchDyeing "
                    << ":uid : " << uid_req_info_iter->uid
                    << ":status: " << uid_rep_info._uid_rep.status
                    << ":__isset._entries: " << isset_entries
                    << std::endl);
        }

        _return._uid_rep_infos.push_back(uid_rep_info);
    }

    return;
}

void ABDyeingHandler::BatchDyeingLan(inveno::ABDyeingBatchReply& _return,
                                     const inveno::ABDyeingBatchLanRequest& batch_lan_reqs) {

    ProcessCostTimeMonitor process_monitor("ABDyeingHandler::BatchDyeingLan");

    auto uid_lan_req_infos = batch_lan_reqs._uid_lan_req_infos;
    if (uid_lan_req_infos.size() > 1000) {        

        _return.status = inveno::ab_status::RC_SERVER_ERROR;
        TLOGINFO(__FILE__ << "-" << __LINE__ << "| BatchDyeingLan error"
                 << ": req_uid_size: " << uid_lan_req_infos.size()
                 << " > 1000" << std::endl);
        return;
    }

    _return.__isset._uid_rep_infos = true;
    
    for (auto uid_lan_req_info_iter = uid_lan_req_infos.begin();
        uid_lan_req_info_iter != uid_lan_req_infos.end();
        uid_lan_req_info_iter++) {
        
        inveno::ABDyeingUidRepInfo uid_rep_info;
        SingleLanDyeing(uid_rep_info, *uid_lan_req_info_iter);

        bool isset_entries = uid_rep_info._uid_rep.__isset._entries;
        if (!isset_entries) {

            TLOGINFO(__FILE__ << "-" << "__LINE__" << "| BatchDyeingLan "
                    << ":uid : " << uid_lan_req_info_iter->uid
                    << ":status: " << uid_rep_info._uid_rep.status
                    << ":__isset._entries: " << isset_entries
                    << std::endl);
        }

        _return._uid_rep_infos.push_back(uid_rep_info);
    }

    return;
}

}
