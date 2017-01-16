#include "util/inv_thrift_service_monitor.h"
#include <iterator>
#include "util/inv_timeprovider.h"

using namespace std;

namespace inv
{

INV_ThriftServiceMonitor::MoniMap INV_ThriftServiceMonitor::moni_record_;

void INV_ThriftServiceMonitor::enterSrvFun(const string& srv_name, const string& fun_name)
{
    enter_time_us_ = TNOWUS;
    
    string key = srv_name + "::" + fun_name;
    moni_record_[key].call_cnt++;
    moni_record_[key].excpt_cnt++;
}

void INV_ThriftServiceMonitor::exitSrvFun(const string& srv_name, const string& fun_name) const
{
    int64_t exit_time_us = TNOWUS;

    string key = srv_name + "::" + fun_name;
    moni_record_[key].excpt_cnt--;
    int64_t call_time_us = exit_time_us - enter_time_us_;
    moni_record_[key].sum_time_us += call_time_us;
    if (call_time_us > moni_record_[key].max_time_us) moni_record_[key].max_time_us = call_time_us;
    if (call_time_us < moni_record_[key].min_time_us) moni_record_[key].min_time_us = call_time_us;
}

void INV_ThriftServiceMonitor::output(ostream& os)
{
    INV_ThriftServiceMonitor::MoniMap::iterator
    it = moni_record_.begin();
    os << "\n\n";
    os << "service::function\tcnt\te_cnt\tavg_t\tmax_t\tmin\t" << endl;
    for (; it != moni_record_.end(); ++it)
    {
        int succ_cnt = it->second.call_cnt - it->second.excpt_cnt;
        if (succ_cnt < 1) succ_cnt = 1; // 防止除0
        os << it->first << "\t"
            << it->second.call_cnt << "\t"
            << it->second.excpt_cnt << "\t"
            << it->second.sum_time_us/succ_cnt << "\t"
            << it->second.max_time_us << "\t"
            << it->second.min_time_us << "\t"
            << endl;
    }
    os << "\n";
}

const INV_ThriftServiceMonitor::MoniMap& INV_ThriftServiceMonitor::getMoniRecord()
{
    return moni_record_;
}

void INV_ThriftServiceMonitor::clearMoniRecord()
{
    moni_record_.clear();
    return;
}

} // namespace ivn end
