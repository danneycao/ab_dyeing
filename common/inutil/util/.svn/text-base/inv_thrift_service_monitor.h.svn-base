/**
 * @file inv_thrift_service_monitor.h
 * @brief 监控 thrift 服务接口调用情况的类
 * @author litang
 * @version 1.0
 * @date 2015-04-17
 */
#ifndef __INV_THRIFT_SERVICE_MONITOR_H
#define __INV_THRIFT_SERVICE_MONITOR_H

#include <string.h>
#include <map>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace inv
{

class INV_ThriftServiceMonitor
{
public:
    struct MoniData
    {
        int32_t call_cnt;   // 总调次数
        int32_t excpt_cnt;  // 调用异常次数
        int64_t sum_time_us;// 非异常调用总耗时(micro seconds)
        int64_t min_time_us;// 最短一次非异常调用耗时
        int64_t max_time_us;// 最长一次非异常调用耗时

        MoniData() 
        {
            bzero(this, sizeof(*this));
            min_time_us = 1000000; // 初始比较阈值
        }
    };

    typedef std::map<std::string, MoniData> MoniMap;

public:
    /**
     * @brief 进入服务函数时调用
     *
     * @param srv_name [IN]进入的服务名
     * @param fun_name [IN]进入的函数名
     */
    void enterSrvFun(const std::string& srv_name, const std::string& fun_name);

    /**
     * @brief 退出服务函数时调用
     *
     * @param srv_name [IN]进入的服务名
     * @param fun_name [IN]进入的函数名
     */
    void exitSrvFun(const std::string& srv_name, const std::string& fun_name) const;

    /**
     * @brief 获取进程内监控记录数据
     *
     * @return 
     */
    static const MoniMap& getMoniRecord();

    /**
     * @brief 清除进程内监控记录数据
     */
    static void clearMoniRecord();

    /**
     * @brief 格式化输出进程内监控记录数据
     *
     * @param os [OUT]目标输出流
     */
    static void output(std::ostream& os = std::cout);

private:
    int64_t enter_time_us_;// 进入函数时间
    static MoniMap moni_record_;

};

} // namespace ivn end
#endif
