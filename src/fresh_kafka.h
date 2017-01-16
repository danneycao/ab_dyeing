#include "ab_def.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#include "kafka_consumer.h"

namespace inv{
class DyeingCurrentFresh
{
	public:
		explicit DyeingCurrentFresh(const std::string& conf_path, kafkaFreshQueue* kq);
		virtual ~DyeingCurrentFresh() = default;
		bool init(const std::string& conf_path);
		void set(std::string& uid);
		void clear();
	private:
		kafkaFreshQueue* _kmq;
		std::unique_ptr<INV_Thread> _kafka_rec_thread;
		std::unique_ptr<INV_Thread> _kafka_clear_thread;
		std::unordered_map<std::string, uint64_t> _kmsg;
		std::map<uint64_t, std::string> _time_kmsg;
		INV_ThreadRWLocker _kmsg_mutex;
};



};
