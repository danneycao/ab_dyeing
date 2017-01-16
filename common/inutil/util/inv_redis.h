#ifndef _INV_REDIS_H
#define _INV_REDIS_H

#include <stdint.h>
#include <string>
#include <vector>
#include <hiredis.h>
#include <async.h>

#include "log/inv_log.h"

using namespace std;

namespace inv
{

/**
 * redis工具类，非线程安全
 */
class INV_Redis
{
public:
    // 异步命令的回调函数类型定义
    typedef void(*AsyncCmdCallback)(void *reply, void *privdata);

public:
    INV_Redis():_ip(""), _connected(false), _async(false), _context(NULL), 
    _async_context(NULL), _async_cmd_callback(NULL) {
    }

    ~INV_Redis();

    void init(const std::string& ip, const int port, const int timeout, bool async = false, bool ssdb = false);

    bool connect();

    void disconnect();

    void reconnect();

    bool connected() {return _connected;}

    /**
     * -6:没找到, 0:成功
     */
    int get(const std::string &key, std::string &value);
    /**
     * 异步接口
     */
    int getAsync(const std::string &key, AsyncCmdCallback cb, void *privdata);

    /**
     * 批量获取KEY的VALUE
     */
    int mget(const vector<string>& keys, vector<string>& values);

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
    int scan(int cursor, vector<string> &keys, const std::string& match="");

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
    int hashgetv2(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& value);
    
    /**
     * HASHMAP 子域删除操作
     * @param key : HASHMAP的大KEY
     * @param field: 要删除的子域
     * @return : 0 - 成功, -1 - 失败
     */
    int hashdel(const std::string &key, const std::vector<std::string>& field);

    /**
     * 遍历HASHMAP中key键下的匹配match表达式的所有子域
     * @param key : HASHMAP的一级KEY
     * @param cursor: 遍历的游标值，第一次调用时填0, 后面填函数返回的cursor值(当函数返回大于0时)
     * @param match: 匹配式
     * @return: 0 - 成功遍历key下面匹配match的子域并且已经遍历完成所有子域, >0 - 下次调用函数时传入的cursor值, -6 - 不存在的key,  -1 - 失败
     */
    int hashscan(const std::string& key, int cursor, map<string, string>& vals, const string& match);
    
    /**
     * ssdb遍历KEY
     */
    int keys(const std::string &start, const std::string &end, int num, std::vector<std::string> &keys);


    /**
     * redis遍历KEY, 效率低，慎用
     */
    int keys(const string& keymatch, vector<string>& keys);

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
    int zrem(const std::string &key, const string &member);
    int zrevrank(const std::string &key, const string &member);
    //ZREVRANGE key start stop [WITHSCORES]

    redisAsyncContext * async_context() { 
        return _async_context;
    }

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

private:
    static void onDisconnect(const struct redisAsyncContext*, int status);

    static void onAsyncCommandReply(redisAsyncContext *ac, void *reply, void *privdata);

    bool isNodeMoved(const std::string errStr, std::string& ip, int32_t& port);

private:
    std::string _ip;
    int _port;
    int _timeout; //second
    bool _connected;
    bool _async;
    bool _ssdb;
    redisContext *_context;
    redisAsyncContext *_async_context;
    AsyncCmdCallback _async_cmd_callback;
};

}

#endif
