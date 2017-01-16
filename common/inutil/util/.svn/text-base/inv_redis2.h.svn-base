#ifndef _INV_REDIS_H
#define _INV_REDIS_H

#include <stdint.h>
#include <string>
#include <vector>
#include <hiredis.h>
#include <async.h>

#include "log/inv_log.h"

using namespace std;

namespace inv {

class INV_Redis;

class ListonThread : public INV_Thread, public INV_ThreadLock {
public:
    ListonThread()
    :bTerminate(true)
    ,_reload(0)
    ,_reCountTime(0)
    ,_reqTotalNum(0)
    ,_reqFailNum(0)
    ,_reSetTm(0)
    ,_handle(0)
    ,_failRate(0)
    {
    }

    int init(INV_Redis *handle, int32_t reload) {
        if (NULL == handle) {
            return -1;
        }
        if (_handle) {
            return  0;
        }

        _reload = reload;
        _handle = handle;
        bTerminate = false;
        return 0;
    }

    void terminate() {
        bTerminate = true;

        {
            INV_ThreadLock::Lock sync(*this);
            notifyAll();
        }
    }

    void process()
    {
        void ReSetCount();
    }

    void AddTotalNum() {++_reqTotalNum;}
    void AddFailNum()  {++_reqFailNum;}
    bool IsBadConnect()
    {
        if(_reqTotalNum > 10
           && double(_reqFailNum)/_reqTotalNum > _failRate)
        {
            return true;
        }
        return false;
    }

protected:
    virtual void run() {
        while (!bTerminate) {
            process();

            {
                INV_ThreadLock::Lock sync(*this);
                timedWait(_reload * 1000);
            }
        }
    }
    void ReSetCount();
    void Recover();

protected:
    bool bTerminate;
    int32_t _reload; //重载时间,单位秒
    int32_t _reCountTime;
    int64_t _reqTotalNum;
    int64_t _reqFailNum;
    int64_t _reSetTm;

    INV_Redis *_handle;

    double _failRate;
};


/**
* redis工具类，非线程安全
*/
class INV_Redis {
public:
    // 异步命令的回调函数类型定义
    typedef void(*AsyncCmdCallback)(void *reply, void *privdata);

public:
    INV_Redis() : _ip(""), _connected(false), _async(false), _context(NULL),
                  _async_context(NULL), _async_cmd_callback(NULL) {
    }

    ~INV_Redis();

    void init(const std::string &ip, const int port, const int timeout, bool async = false, bool ssdb = false);
    void init(const std::string &ip, const int port, const int timeout,
         const int reloadtime, bool async, bool ssdb);

    bool connect();

    void disconnect();

    void reconnect();

    bool connected() { return _connected; }

    /**
 * -6:没找到, 0:成功
 */
    int get(const std::string &key, std::string &value);

    /**
 * 异步接口
 */
    int getAsync(const std::string &key, AsyncCmdCallback cb, void *privdata);

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

    int lpop(const std::string &key, std::string &value);

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
    int hashset(const std::string &key, const std::vector<std::string> &field,
                const std::vector<std::string> &value);

    /**
 * 列表操作-获取
 */
    int hashget(const std::string &key, const std::vector<std::string> &field, std::vector<std::string> &value);

    /*SortedSet*/
    int zcount(const std::string &key, int32_t score);

    int zadd(const std::string &key, int32_t score, int32_t member);

    int zrevrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids);

    int zremrangebyscore(const std::string &key, int32_t score);

    int zrevrange(const std::string &key, int32_t start, int32_t stop, vector<int32_t> &ids);

    int zcard(const std::string &key);

    int zrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids);
    //ZREVRANGE key start stop [WITHSCORES]

    redisAsyncContext *async_context() {
        return _async_context;
    }

    void freeReply(redisReply *reply) {
        if (reply != NULL) {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    static size_t redis_hash(const string &s) {
        const char *ptr = s.c_str();
        size_t key_length = s.length();
        uint32_t value = 0;

        while (key_length--) {
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
    static void onDisconnect(const struct redisAsyncContext *, int status);

    static void onAsyncCommandReply(redisAsyncContext *ac, void *reply, void *privdata);

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

    static ListonThread _lthr;
};
}
#endif
