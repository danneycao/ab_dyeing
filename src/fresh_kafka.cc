#include "fresh_kafka.h"
#include "util/inv_config.h"
namespace inv{
    
    
class KafkaThread : public INV_Thread {
  public:
    explicit KafkaThread(DyeingCurrentFresh* owner, const std::string& conf_path)
        : _owner(owner), _conf_path(conf_path){
      assert(owner != NULL);
    }
    virtual ~KafkaThread() {
      stop();
    }

    void stop() {
      _running = false;
      getThreadControl().join();
    }
    
	void loadbase(std::vector<std::pair<std::string, uint16_t> >& brokers,
                  std::string& conf_path,
                  std::string& topic,
                  std::string& group){
    	INV_Config conf;
    	conf.parseFile(_conf_path);
    
    	std::vector<std::string> kafkaServers = conf.getDomainLine("/main/fresh/brokers");
    	for(size_t i = 0; i < kafkaServers.size(); i++)
        {
        	vector<string> kv = inv::INV_Util::sepstr<string>(kafkaServers[i], "=");

        	vector<string> servers = inv::INV_Util::sepstr<string>(kv[1], ":");
        	string ip = servers[0];
        	int port = inv::INV_Util::strto<int>(servers[1]);
        	brokers.push_back(std::make_pair(ip, port));
        }
    	if(!brokers.size())
        {
        	TLOGINFO("/main/fresh/brokers empty");
        	exit(-1);
        }
    	topic = conf.get("/main/fresh/[topic]");
    	if (topic.empty())
        {
        	TLOGINFO("/main/fresh/[topic] empty");
        	exit(-1);
        }
    	group = conf.get("/main/fresh/[group]");
    	if (group.empty())
        {
        	TLOGINFO("/main/fresh/[group] empty");
        	exit(-1);
        }
    }
    
	void processkafka(std::string& msg) {
    	rapidjson::Document d;
    	d.Parse(msg.c_str());
    	if (d.HasParseError()) {
            TLOGINFO(__FILE__<<"-"<<__LINE__<<"| wrong record : " << msg <<endl );
        	return;
        }
    	if (!d.IsObject()) {
            TLOGINFO(__FILE__<<"-"<<__LINE__<< "|wrong record " <<endl );
        	return;
        }
    	if (d.FindMember("uid") == d.MemberEnd()) {
        	TLOGINFO(__FILE__<<"-"<<__LINE__<<" wrong record, no uid: " <<endl);
        	return;
        }
    	std::string uid = d["uid"].GetString();
    	FDLOG("recv_kafka") << __FILE__<<"-"<<__LINE__ << "|" <<uid <<"|" << d["event_time"].GetString() << std::endl;
        //TLOGINFO("kafka get uid: " << uid <<endl );
    	_owner->set(uid);
    }
    
    virtual void run() {
    	std::vector<std::pair<std::string, uint16_t> > brokers;
    	std::string topic;
    	std::string group;
    	loadbase(brokers, _conf_path, topic, group);
    	KafkaConsumer consumer(brokers, group, topic);
        TLOGINFO("kafka client init group: " << group << " | topic: " <<topic<<endl );
    	if (consumer.Init() != 0) {
        	TLOGINFO("kafka client init error : " << consumer.GetLastErrMsg().c_str());
        	exit(1);
        	return ;
        }
        while (_running) {
        	std::string msg;
        	if (consumer.FetchMsg(msg)) {
            	if(consumer.GetLastErrMsg() != "No message")TLOGINFO(__FILE__<<"-"<<__LINE__<<" FetchMsg error: "<<consumer.GetLastErrMsg()<<endl);
            	continue;
            }
        	processkafka(msg);
        }
    }

  private:
    DyeingCurrentFresh* _owner;
	std::string _conf_path;
};

class ClearThread : public INV_Thread {
  public:
    explicit ClearThread(DyeingCurrentFresh* owner, const std::string& fresh_hour)
        : _owner(owner), _fresh_hour(fresh_hour), _fresh_tags(true){
      assert(owner != NULL);
    }
    virtual ~ClearThread() {
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
        	sleep(10 * 60);
        	if(_fresh_hour == getAbTimeOfHour(true) && _fresh_tags)
            {
            	_owner->clear();
            	_fresh_tags = false;
            }
        	if(_fresh_hour == getAbTimeOfHour(false))
            	_fresh_tags = true;
        }
    }

  private:
    DyeingCurrentFresh* _owner;
	std::string _fresh_hour;
	bool _fresh_tags;
};

DyeingCurrentFresh::DyeingCurrentFresh(const std::string& conf_path, kafkaFreshQueue* kq)
    :_kmq(kq){

}

bool DyeingCurrentFresh::init(const std::string& conf_path)
{
	INV_Config conf;
    conf.parseFile(conf_path);
	_kafka_rec_thread.reset(new KafkaThread(this, conf_path));
	_kafka_rec_thread->start();
	std::string fresh_hour = conf.get("/main/base/[fresh_hour]");
    if(!fresh_hour.length())
        fresh_hour = "0";
    TLOGINFO( 
        __FILE__<<"-"<<__LINE__ << "| DyeingCurrentFresh hour: " << fresh_hour << std::endl);
	_kafka_clear_thread.reset(new ClearThread(this,fresh_hour));
	_kafka_clear_thread->start();
	return true;
}
    
void DyeingCurrentFresh::set(std::string& uid)
{
	FDLOG("kafka") << __FILE__<<"-"<<__LINE__ << "| set |" << uid << std::endl;
	do{
    	INV_ThreadWLock wl(_kmsg_mutex);
    	_time_kmsg[TNOWUS] = uid;
    }while(0);
	INV_ThreadWLock wlock(_kmq->lock);
	_kmq->kq[uid]=TNOWUS;
    FDLOG("kafka") << __FILE__<<"-"<<__LINE__ << "| set |" << uid << " | " << _kmq->kq.size() << std::endl;
}

void DyeingCurrentFresh::clear() {
    //删除一天前的uid
	std::vector<std::string> rubsh;
	uint64_t cnow = TNOWUS - 24*3600*1000*1000;
	do{
		std::vector<uint64_t> time_rubsh;
    	INV_ThreadWLock wl(_kmsg_mutex);
    	for(auto itrr = _time_kmsg.begin();itrr != _time_kmsg.end(); ++itrr) {
	        FDLOG("kafka") << __FILE__<<"-"<<__LINE__ << "|clear |cnow " 
                << cnow <<"|itrr->first:" << itrr->first 
                << "|itrr->second: " << itrr->second << std::endl;
        	if(cnow > itrr->first) {
            	rubsh.push_back(itrr->second);
				time_rubsh.push_back(itrr->first);
			} else {
                break;
            }
        }
		for(auto titr= time_rubsh.begin();titr != time_rubsh.end(); ++titr)
			_time_kmsg.erase(*titr);
    }while(0);
	for(auto itr = rubsh.begin(); itr!= rubsh.end(); ++itr)
    {
    	INV_ThreadWLock wlock(_kmq->lock);
    	_kmq->kq.erase(*itr);
    }
	FDLOG("kafka") << __FILE__<<"-"<<__LINE__ << "|clear | " << _kmq->kq.size() <<"|" << _time_kmsg.size()<< std::endl;
}
};
