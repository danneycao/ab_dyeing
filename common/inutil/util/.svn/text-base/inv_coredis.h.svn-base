#ifndef _INV_CoRedis_H
#define _INV_CoRedis_H

#include <stdint.h>
#include <string>
#include <vector>
#include <hiredis.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log/inv_log.h"
#include "inv_coclientsocket.h"

using namespace std;

#define INV_COREDIS_MAX_SLOT_SIZE 0x4000

namespace inv
{

/**
 * redis工具类，非线程安全
 */
class INV_CoRedis
{
public:
    
    typedef union NAddr {
        uint64_t n;
        struct {
            uint32_t ip;
            uint16_t port;
        } addr;
    } NAddr;

    typedef struct Addr {
        string _ip;
        uint16_t _port;

        NAddr _naddr;
        
        Addr(const string& ip, const uint16_t port) : _ip(ip), _port(port) 
        {  
            _naddr.addr.ip = inet_addr(ip.c_str());
            _naddr.addr.port = port;
        }

        Addr(const uint64_t n)
        {
            _naddr.n = n;

            struct in_addr na;
            memcpy(&na, &_naddr.addr.ip, sizeof(_naddr.addr.ip));
            _ip = inet_ntoa(na);
            _port = _naddr.addr.port;
        }

        bool operator < (const Addr& r) const
        {
            /*
            if (_ip != r._ip)
                return _ip < r._ip;
            if (_port != r._port)
                return _port < r._port;
            return false;
            */

            return _naddr.n < r._naddr.n;
        }
    } Addr;
    
    INV_CoRedis():_ip(""), _connected(false) {
    }

    ~INV_CoRedis();

    void init(const std::string& ip, const int port, const int timeout, bool ssdb = false, bool mutilconn = true);

    bool connect();

    void disconnect();

    void reconnect();

    bool connected() {return _connected;}

    /**
     * -6:没找到, 0:成功
     */
    int get(const std::string &key, std::string &value);

    /**
     * -1 失败
     */
    int set(const std::string &key, const std::string &value);

    /**
     * -1 失败
     */
    int remove(const std::string &key);

    /**
     * 将key中储存的数字值增一
     * @return 增加之后的值
     * <=0 失败
     */
    int64_t incr(const std::string &key);
    int64_t decr(const std::string &key);

    /**
     * 遍历key
     * @return 新的cursor, 0:本次遍历结束
     */
    int scan(int cursor, vector<string> &keys);

    /**
     * 列表操作-新增
     */
    int rpush(const std::string &key, const std::string &value);

    /**
     * 列表操作-设置
     */
    int lset(const std::string &key, int index, const std::string &value);

    /**
     * 列表操作-获取
     */
    int lindex(const std::string &key, int index, std::string &value);

    int lpop(const std::string &key,std::string &value);
    
    int blpop(const std::string& key, std::string& value);
   
     /**
     * 列表操作-获取
     */
    int lrange(const std::string &key, int start, int end, std::vector<string> &values);

    /**
     * 列表操作-长度
     */
    int llen(const std::string &key);

    /**
     * 列表操作-删除
     */
    int lrem(const std::string &key, const std::string &value);

    /**
     * Hash操作-设置
     */
    int hashset(const std::string &key, const std::vector<std::string>& field, const std::vector<std::string>& value);

    /**
     * 列表操作-获取
     */
    int hashget(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& value);
	
	/**
     * Hash操作-设置
     */
    int hashset(const std::string &key, const std::string& field, const std::string& value);

    /**
     * 列表操作-获取
     */
    int hashget(const std::string &key, const std::string& field, std::string& value);
    
    /**
     * ssdb遍历KEY
     */
    int keys(const std::string &start, const std::string &end, int num, std::vector<std::string> &keys);

    /*SortedSet*/
    int zcount(const std::string &key, int32_t score);
    int zadd(const std::string &key, int64_t score, const string &member);
    int zadd(const std::string &key, int32_t score, int32_t member);
    int zrevrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids);
    int zremrangebyscore(const std::string &key, int32_t score);
    int zrevrange(const std::string &key, int32_t start, int32_t stop, vector<int32_t> &ids);
    int zrevrange(const std::string &key, int32_t start, int32_t stop, vector<string> &ids);
    int zcard(const std::string &key);
    int zrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids);
    int zrank(const std::string &key, const string &member);
    int zrevrank(const std::string &key, const string &member);
    //ZREVRANGE key start stop [WITHSCORES]

    void* getcommand(const string& key, const vector<string>& vtKey);
    
    void freeReply(redisReply *reply) {
        if(reply != NULL) {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    static size_t redis_hash(const string &s) {
        const char *ptr= s.c_str();
        size_t key_length = s.length();
        uint32_t value= 0;
        
        while (key_length--) 
        {
            value += *ptr++;
            value += (value << 10);
            value ^= (value >> 6);
        }
        value += (value << 3);
        value ^= (value >> 11);
        value += (value << 15); 
        
        return value == 0 ? 1 : value;
    }

protected:
    redisReply* realdo(const string& method, char* cmdbuf, int cmdlen);
    redisReply* exec0p(const string& method, const char* format);
    redisReply* exec1p(const string& method, const char* format, const char* cmd1, int cmd1len);
    redisReply* exec2p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len);
    redisReply* exec3p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len, const char* cmd3, int cmd3len);
    redisReply* exec4p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len, const char* cmd3, int cmd3len, const char* cmd4, int cmd4len);
    redisReply* execmp(const string& method, int argc, const char** argv, const size_t* argvlen);
    
    void AdjustClient(const string& key);

public:
    /**
     * 获取集群信息
     * -1 失败   0 成功
     */
    int ClusterNodes();

    int ClusterSlots();

private:
    std::string _ip;
    int _port;
    int _timeout; //second

    INV_CoTCPClient* _client;
    map<Addr, INV_CoTCPClient*> _clients;
    volatile static uint64_t _slotCache[INV_COREDIS_MAX_SLOT_SIZE + 1];

    bool _connected;
    bool _ssdb;
};

}

#endif
