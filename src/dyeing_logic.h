#pragma once

#include "ab_def.h"
#include "fresh_user.h"

namespace inveno {
class ABDyeingReply;
}

namespace inv {

class INV_QConfig;
class ABDyeingHandler;

class DyeingLogic {
  public:
    explicit DyeingLogic(ABDyeingHandler* owner, kafkaFreshQueue* kq);
    ~DyeingLogic();

    bool init(const std::string& conf_path);

    bool onDyeing(const std::string& uid, const std::string& app,
                  const std::string& app_ver, const std::string& app_lan,
                  std::string type, inveno::ABDyeingReply* reply);

  private:
    ABDyeingHandler* _owner;

    bool handleWhiteList(const std::string& uid, const std::string& app,
                         const std::string& app_ver, const std::string& app_lan,
                         std::string type, std::string* config);
    bool handleOnlineHash(const std::string& uid, const std::string& app,
                          const std::string& app_ver, const std::string& app_lan,
                          std::string type, std::string* config);
	/*return 0: error 
     * 1: fresh_user
     * 2: new_user*/
    int handleFreshUser(const std::string& uid,const std::string& app,
                         const std::string& app_ver, const std::string& app_lan,
                         std::string& type, std::string* config);

    bool handlePushUser(const std::string& uid, const std::string& app,
                        const std::string& app_ver, const std::string& app_lan,
                        std::string& type, std::string* user_type);

  private:
  
    std::unique_ptr<INV_QConfig> _qconf;
	std::string _fresh_product_suffix;
    kafkaFreshQueue* _kmq;
    
    DISALLOW_COPY_AND_ASSIGN(DyeingLogic);
};

}
