#include "util/inv_redis_cluster.h"

using namespace std;
using namespace inv;

using namespace RedisCluster;

INV_RedisCluster::~INV_RedisCluster() {
    disconnect();
}

void INV_RedisCluster::init(const string& ip, const int port, const int timeout, bool async, bool ssdb) {
	_ip = ip;
	_port = port;
	_timeout = timeout;
    _async = async;
    _ssdb = ssdb;
}

// void INV_RedisCluster::onDisconnect(const struct redisAsyncContext* ac, int status)
// {
//     // 断开重连
//     LOGMSG(InvRollLogger::ERROR_LOG, " async disconnected, try to reconnect, status:" << status << endl);
//     INV_RedisCluster *the_redis = (INV_RedisCluster*)ac->data;
//     the_redis->connect();
// }
// 
bool INV_RedisCluster::connect() {
	// struct timeval timeout = { _timeout, 0 };
    // if (_async)
    // {
    //     _async_context = redisAsyncConnect(_ip.c_str(), _port);
    //     if (_async_context && !_async_context->err) _connected = true;
    //     _async_context->data = this;
    //     // 注册异步断开回调
    //     redisAsyncSetDisconnectCallback(_async_context, onDisconnect);
    // }
    // else
    // {
    //     _context = redisConnectWithTimeout(_ip.c_str(), _port, timeout);
    //     if (_context && !_context->err) _connected = true;
    //     //read write 超时控制
    //     redisSetTimeout(_context, timeout);
    // }
    // if (!_connected)
    // {
    //     LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster connect error:" 
    //             << (_context?_context->err:0)
    //             << (_async_context?_async_context->err:0)
    //             << (_async ? "[async]":"[sync]")
    //             << " timeout:" << _timeout
    //             << " ip:" << _ip
    //             << " port:" << _port 
    //             << endl);
    //     return false;
    // }

    try
    {
        this->_cluster_p = HiredisCommand<>::createCluster(_ip.c_str(), _port);
    }
    catch (ConnectionFailedException& ex)
    {
        return false;
    }
    return true;
}

void INV_RedisCluster::disconnect() {
	// if(!_async && _context != NULL && _connected) {
	// 	redisFree(_context);
	// } else if(_async && _async_context != NULL && _connected) {
	// 	redisAsyncFree(_async_context);
	// }

	// _connected = false;
    this->_cluster_p->disconnect();
}

void INV_RedisCluster::reconnect() {
	disconnect();
	connect();
}

// int INV_RedisCluster::get(const string &key, string &value) {
// 
// #ifdef __INV_DEBUG__
// 	int64_t start = TNOWMS;
// #endif
// 	int ret = 0;
// 	if(!_connected) {
// 		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster get not connected:" << key 
// 			<< " timeout:" << _timeout
//         	<< " ip:" << _ip
//         	<< " port:" << _port 
// 			<< endl);
// 		reconnect();
// 		return -1;
// 	}
// 
//     //redisReply *reply = (redisReply *)redisCommand(_context, ("GET "+key).c_str());
//     // redisReply *reply = (redisReply *)redisCommand(_context, "GET %b", key.c_str(), key.size());
//     // reply = static_cast<redisReply*>( HiredisCommand<>::Command( cluster_p, key, "SET %s %s", key.c_str(), "test" ) );
//     redisReply *reply = (redisReply *)redisCommand(_context, "GET %b", key.c_str(), key.size());
//     if(reply == NULL) {
// 	    ret = _context->err;
//     	FDLOG("error") << "INV_RedisCluster get:" << key  << ":" 
//     		<< "ret:" << _context->err << ":" 
//     		<< _context->errstr << ":" 
//     		<< pthread_self() << ":" 
//     		<< "ip:" << _ip << ":" 
//         	<< "port:" << _port 
//         	<< endl;
//     	if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
//     		//关闭连接，尝试重连
//             FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
//     		reconnect();
//     	}
// 
//     	return -1;
//     }
// 
//     if(reply->type == REDIS_REPLY_NIL) {
// 		//不存在的key
// 		ret = -6;
// 	}
// 	else if(reply->type == REDIS_REPLY_STRING && reply->str != NULL) {
//         value = string(reply->str, reply->len);
//     }
//     else {
//     	ret = -1;
//     }
// 
//     freeReply(reply);
// #ifdef __INV_DEBUG__
// 	int diff = TNOWMS - start;
//     if(diff > 5)
//     {
// 		FDLOG("time") << __FUNCTION__<<__LINE__<< " get redis time:" << diff << ":ip:"
//         	<< _ip << ":port:" << _port  << ":key:"<< key << ":value.size:"<< value.size << endl;
// 
//     }
// #endif
//     return ret;
// }
// 
// int INV_RedisCluster::set(const string &key, const string &value) {
// 
// #ifdef __INV_DEBUG__
// 	int64_t start = TNOWMS;
// #endif
// 	int ret = 0;
// 
// 	if(!_connected) {
// 		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster set not connected:" << key 
// 			<< " timeout:" << _timeout
//         	<< " ip:" << _ip
//         	<< " port:" << _port 
// 			<< endl);
// 		reconnect();
// 		return -1;
// 	}
// 
// 	redisReply *reply = (redisReply *)redisCommand(_context, "SET %b %b", key.c_str(), key.size(), value.c_str(), value.size());
// 	if(reply == NULL){
//         ret = _context->err;
// 		FDLOG("error") << "INV_RedisCluster set:" << key << ":" 
// 			<< "ret:" << _context->err << ":" 
// 			<< _context->errstr << ":" 
// 			<< "ip:" << _ip << ":" 
//         	<< "port:" << _port 
//         	<< endl;
//         if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
//             //关闭连接，尝试重连
//             FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
//             reconnect();
//     	}
// 
// 		return -1;
// 	}
// 
// 	string status = reply->str == NULL ? "" : reply->str;
// 	LOGMSG(InvRollLogger::DEBUG_LOG, "set:" << key << " reply->type:" << reply->type << " status:" << status << endl);
// 
//     freeReply(reply);
// 
// #ifdef __INV_DEBUG__
// 	int diff = TNOWMS - start;
//     if(diff > 5)
//     {
//         FDLOG("time") << __FUNCTION__<<__LINE__<< " set redis time:" << diff << ":ip:"
//         << _ip << ":port:" << _port  << ":key:"<< key << ":value.size:"<< value.size << endl;
//     }
// #endif
// 
//     return ret;
// }
// 
// int INV_RedisCluster::mget(const vector<string>& keys, vector<string>& values)
// {
// 	int ret = 0;
// 	if(!_connected) {
// 		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster mget not connected:" << keys.size()
// 			<< " timeout:" << _timeout
//         	<< " ip:" << _ip
//         	<< " port:" << _port 
// 			<< endl);
// 		reconnect();
// 		return -1;
// 	}
//     
// 	std::vector<const char*> argv;
// 	argv.reserve(keys.size()+1);
// 	std::vector<size_t> argvlen;
// 	argvlen.reserve(keys.size()+1);
// 
// 	static char mgetcmd[] = "MGET";
// 	argv.push_back(mgetcmd);
// 	argvlen.push_back(sizeof(mgetcmd) - 1);
// 
// 	for(size_t i = 0; i < keys.size(); i++) {
// 		argv.push_back(keys[i].c_str()); 
//         argvlen.push_back(keys[i].size());
// 	}
// 
//     redisReply *reply = (redisReply *)redisCommandArgv(_context, keys.size()+1, &(argv[0]), &(argvlen[0]));
// 
//     if (reply == NULL) {
// 	    ret = _context->err;
//     	FDLOG("error") << "INV_RedisCluster mget:" << keys.size()  << ":" 
//     		<< "ret:" << _context->err << ":" 
//     		<< _context->errstr << ":" 
//     		<< pthread_self() << ":" 
//     		<< "ip:" << _ip << ":" 
//         	<< "port:" << _port 
//         	<< endl;
//     	if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
//     		//关闭连接，尝试重连
//             FDLOG("error") << "INV_RedisCluster reconnect:" << keys.size() << ":" << pthread_self() << endl;
//     		reconnect();
//     	}
// 
//     	return -1;
//     }
// 
//     do 
//     {
//         if (reply->type != REDIS_REPLY_ARRAY)
//         {
//             FDLOG("error") << "INV_RedisCluster keys reply->type:" << reply->type
//                 << " ip:" << _ip
//                 << " port:" << _port
//                 << endl;
//             ret =  -1;
//             break;
//         }
// 
//         if (reply->elements != keys.size())
//         {
//             FDLOG("error") << "INV_RedisCluster keys reply->elements:" << reply->elements
//                 << " ip:" << _ip
//                 << " port:" << _port
//                 << endl;
//             ret =  -1;
//             break;
//         }
// 
//         for (size_t i = 0; i < reply->elements; i++) 
//         {
//             if(reply->element[i]->type == REDIS_REPLY_STRING)
//             {
//                 values.push_back(string(reply->element[i]->str, reply->element[i]->len));
//             }
//             else
//             {
//                 values.push_back("");
//             }
//         }
//     } while(0);
// 
//     freeReply(reply);
// 
//     return ret;
// }
// 
// 
// int INV_RedisCluster::remove(const string &key) {
// 	
// 	int ret = 0;
// 
// 	if(!_connected) {
// 		reconnect();
// 		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster remove not connected" << endl);
// 		return -1;
// 	}
// 
// 	redisReply *reply = (redisReply *)redisCommand(_context, "DEL %b", key.c_str(), key.size());
// 	if(reply == NULL) {
// 		FDLOG("error") << "INV_RedisCluster remove:" << key << " ret:" << _context->err << ":" << _context->errstr << endl;
//         ret = _context->err;
// 		FDLOG("error") << "INV_RedisCluster remove:" << key << ":" 
// 			<< "ret:" << _context->err << ":" 
// 			<< _context->errstr << ":" 
// 			<< "ip:" << _ip << ":" 
//         	<< "port:" << _port 
//         	<< endl;
//         if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
//             //关闭连接，尝试重连
//             FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
//             reconnect();
//     	}
//         return -1;
//     }
// 
//     freeReply(reply);
// 
//     return ret;
// }

void* INV_RedisCluster::setcommand( redisContext *c, const string& key, const vector<string>& vtKey, const vector<string>& vtVal)
{
	if (vtKey.size() == 0 || vtKey.size() != vtVal.size())
	{
		return 0;
	}

	std::vector<const char*> argv;
	argv.reserve(vtKey.size()+vtVal.size()+2);
	std::vector<size_t> argvlen;
	argvlen.reserve(vtKey.size()+vtVal.size()+2);

	static char mgetcmd[] = "HMSET";
	argv.push_back(mgetcmd);
	argvlen.push_back(sizeof(mgetcmd) - 1);
	argv.push_back(key.c_str());
	argvlen.push_back(key.size());

	for(size_t i = 0; i < vtKey.size(); i++) {
		argv.push_back(vtKey[i].c_str()), argvlen.push_back(vtKey[i].size());
		argv.push_back(vtVal[i].c_str()), argvlen.push_back(vtVal[i].size());
	}
    redisReply * reply;
	// return (redisReply *)redisCommandArgv(c,argv.size(),&(argv[0]),&(argvlen[0]));
    // static inline void* Command( typename Cluster::ptr_t cluster_p,
    //         string key,
    //         int argc,
    //         const char ** argv,
    //         const size_t *argvlen )
    // {
    //     return HiredisCommand( cluster_p, key, argc, argv, argvlen ).process();
    // }
    reply = (redisReply*)(HiredisCommand<>::Command(this->_cluster_p, key, argv.size(), &(argv[0]), &(argvlen[0])));
    return reply;
}

void* INV_RedisCluster::getcommand( redisContext *c, const string& key, const vector<string>& vtKey)
{
	if (vtKey.size() == 0)
	{
		return 0;
	}

	std::vector<const char*> argv;
	// argv.reserve(vtKey.size()+vtVal.size()+2);
	argv.reserve(vtKey.size()+2);
	std::vector<size_t> argvlen;
	// argvlen.reserve(vtKey.size()+vtVal.size()+2);
	argvlen.reserve(vtKey.size()+2);

	static char mgetcmd[] = "HMGET";
	argv.push_back(mgetcmd);
	argvlen.push_back(sizeof(mgetcmd) - 1);
	argv.push_back(key.c_str());
	argvlen.push_back(key.size());

	for(size_t i = 0; i < vtKey.size(); i++) {
		argv.push_back(vtKey[i].c_str()), argvlen.push_back(vtKey[i].size());
		// argv.push_back(vtVal[i].c_str()), argvlen.push_back(vtVal[i].size());
	}
    redisReply * reply;
	// return (redisReply *)redisCommandArgv(c,argv.size(),&(argv[0]),&(argvlen[0]));
    // static inline void* Command( typename Cluster::ptr_t cluster_p,
    //         string key,
    //         int argc,
    //         const char ** argv,
    //         const size_t *argvlen )
    // {
    //     return HiredisCommand( cluster_p, key, argc, argv, argvlen ).process();
    // }
    reply = (redisReply*)(HiredisCommand<>::Command(this->_cluster_p, key, argv.size(), &(argv[0]), &(argvlen[0])));
    return reply;
}

int INV_RedisCluster::hashset(const std::string &key, const std::vector<std::string>& field,
					   const std::vector<std::string>& value)
{
	int ret = 0;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster hashset not connected:" << key
										 << " timeout:" << _timeout
										 << " ip:" << _ip
										 << " port:" << _port
										 << endl);
		reconnect();
		return -1;
	}
	if(field.size() != value.size())
	{
		return -2;
	}

	redisReply *reply = (redisReply *)setcommand(_context, key, field, value);
	if(reply == NULL) {
        ret = _context->err;
		FDLOG("error") << "INV_RedisCluster hashset:" << key << ":"
		<< "ret:" << _context->err << ":"
		<< _context->errstr << ":"
		<< "ip:" << _ip << ":"
		<< "port:" << _port
		<< endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
            //关闭连接，尝试重连
            FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
            reconnect();
    	}
		return -1;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "hashset:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

	freeReply(reply);

	return ret;
}

int INV_RedisCluster::hashget(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& values)
{
	int ret = -1;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster hashget not connected:" << key
										 << " timeout:" << _timeout
										 << " ip:" << _ip
										 << " port:" << _port
										 << endl);
		reconnect();
		return -1;
	}

	redisReply *reply = (redisReply *)getcommand(_context, key, field);
	int32_t ret_type = 0;
	if(reply == NULL) {
        ret = _context->err;
		FDLOG("error") << "INV_RedisCluster hashget:" << key << ":"
		<< "ret:" << _context->err << ":"
		<< _context->errstr << ":"
		<< "ip:" << _ip << ":"
		<< "port:" << _port
		<< endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
            //关闭连接，尝试重连
            FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
            reconnect();
    	}
		return -1;
	}
	else if(reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i++) {
			ret_type = reply->element[i]->type;
			if(reply->element[i]->type == REDIS_REPLY_STRING){
				string value = string(reply->element[i]->str, reply->element[i]->len);
				values.push_back(value);
			}
		}
		ret = (ret_type ==  REDIS_REPLY_NIL) ? -6 : 0;
	}
	else if(reply->type == REDIS_REPLY_NIL ) {
		ret = -6;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "hashget:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

	freeReply(reply);

	return ret;
}

int INV_RedisCluster::hashgetv2(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& values)
{
	int ret = -1;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_RedisCluster hashgetv2 not connected:" << key
										 << " timeout:" << _timeout
										 << " ip:" << _ip
										 << " port:" << _port
										 << endl);
		reconnect();
		return -1;
	}

	redisReply *reply = (redisReply *)getcommand(_context, key, field);
	int32_t ret_type = 0;
	if(reply == NULL) {
        ret = _context->err;
		FDLOG("error") << "INV_RedisCluster hashgetv2:" << key << ":"
		<< "ret:" << _context->err << ":"
		<< _context->errstr << ":"
		<< "ip:" << _ip << ":"
		<< "port:" << _port
		<< endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
            //关闭连接，尝试重连
            FDLOG("error") << "INV_RedisCluster reconnect:" << key << ":" << pthread_self() << endl;
            reconnect();
    	}
		return -1;
	}
	else if(reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i++) {
			ret_type = reply->element[i]->type;
			if(reply->element[i]->type == REDIS_REPLY_STRING){
				string value = string(reply->element[i]->str, reply->element[i]->len);
				values.push_back(value);
			}else if(reply->element[i]->type == REDIS_REPLY_NIL){
                values.push_back("");
            }
		}
		ret = (ret_type ==  REDIS_REPLY_NIL) ? -6 : 0;
	}
	else if(reply->type == REDIS_REPLY_NIL ) {
		ret = -6;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "hashgetv2:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

	freeReply(reply);

	return ret;
}


