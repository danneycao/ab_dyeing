#include "util/inv_redis.h"

using namespace std;
using namespace inv;

ListonThread INV_Redis::_lthr;

INV_Redis::~INV_Redis() {
    disconnect();
}

void INV_Redis::init(const string& ip, const int port, const int timeout, bool async, bool ssdb) {
	_ip = ip;
	_port = port;
	_timeout = timeout;
    _async = async;
    _ssdb = ssdb;
}

void INV_Redis::init(const std::string &ip, const int port, const int timeout,
						 const int reloadtime, bool async, bool ssdb){
	init(ip, port, timeout, async, ssdb);
	_lthr.init(this,reloadtime);
}

void INV_Redis::onDisconnect(const struct redisAsyncContext* ac, int status)
{
    // 断开重连
    LOGMSG(InvRollLogger::ERROR_LOG, " async disconnected, try to reconnect, status:" << status << endl);
    INV_Redis *the_redis = (INV_Redis*)ac->data;
    the_redis->connect();
}

bool INV_Redis::connect() {
	struct timeval timeout = { _timeout, 0 };
    if (_async)
    {
        _async_context = redisAsyncConnect(_ip.c_str(), _port);
        if (_async_context && !_async_context->err) _connected = true;
        _async_context->data = this;
        // 注册异步断开回调
        redisAsyncSetDisconnectCallback(_async_context, onDisconnect);
    }
    else
    {
        _context = redisConnectWithTimeout(_ip.c_str(), _port, timeout);
        if (_context && !_context->err) _connected = true;
        //read write 超时控制
        redisSetTimeout(_context, timeout);
    }
    if (!_connected)
    {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis connect error:" 
                << (_context?_context->err:0)
                << (_async_context?_async_context->err:0)
                << (_async ? "[async]":"[sync]")
                << " timeout:" << _timeout
                << " ip:" << _ip
                << " port:" << _port 
                << endl);
        return false;
    }

    return true;
}

void INV_Redis::disconnect() {
	if(!_async && _context != NULL && _connected) {
		redisFree(_context);
	} else if(_async && _async_context != NULL && _connected) {
		redisAsyncFree(_async_context);
	}

	_connected = false;
}

void INV_Redis::reconnect() {
	disconnect();
	connect();
}

void INV_Redis::onAsyncCommandReply(redisAsyncContext *ac, void *reply, void *privdata)
{
    INV_Redis *the_redis = (INV_Redis*)ac->data;
    if (the_redis->_async_cmd_callback)
    {
        the_redis->_async_cmd_callback(reply, privdata);
    }
}

int INV_Redis::get(const string &key, string &value) {

#ifdef __INV_DEBUG__
	int64_t start = TNOWMS;
#endif
	_lthr.AddTotalNum();
	int ret = 0;
	if(!_connected && !_lthr.IsBadConnect()) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis get not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		_lthr.AddFailNum();
		return -1;
	}

    //redisReply *reply = (redisReply *)redisCommand(_context, ("GET "+key).c_str());
    redisReply *reply = (redisReply *)redisCommand(_context, "GET %b", key.c_str(), key.size());
    if(reply == NULL) {
	    ret = _context->err;
    	FDLOG("error") << "INV_Redis get:" << key  << ":" 
    		<< "ret:" << _context->err << ":" 
    		<< _context->errstr << ":" 
    		<< pthread_self() << ":" 
    		<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
    	if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO || ret == REDIS_ERR_PROTOCOL) {
    		//关闭连接，尝试重连
    		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
    		reconnect();
			_lthr.AddFailNum();
    	}

    	return -1;
    }

    if(reply->type == REDIS_REPLY_NIL) {
		//不存在的key
		ret = -6;
	}
	else if(reply->type == REDIS_REPLY_STRING && reply->str != NULL) {
        value = string(reply->str, reply->len);
    }
    else {
    	ret = -1;
    }

    freeReply(reply);
#ifdef __INV_DEBUG__
	int diff = TNOWMS - start;
    if(diff > 5)
    {
		FDLOG("time") << __FUNCTION__<<__LINE__<< " get redis time:" << diff << ":ip:"
        	<< _ip << ":port:" << _port  << ":key:"<< key << ":value.size:"<< value.size << endl;

    }
#endif
    return ret;
}

int INV_Redis::getAsync(const std::string &key, AsyncCmdCallback cb, void *privdata)
{
    _async_cmd_callback = cb;
    int i = redisAsyncCommand(_async_context, onAsyncCommandReply, privdata, "GET %b",
            key.c_str(), key.size()); 
    return i;
}

int INV_Redis::set(const string &key, const string &value) {

#ifdef __INV_DEBUG__
	int64_t start = TNOWMS;
#endif
	_lthr.AddTotalNum();
	int ret = 0;

	if(!_connected && !_lthr.IsBadConnect()) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis set not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		_lthr.AddFailNum();
		return -1;
	}

	redisReply *reply = (redisReply *)redisCommand(_context, "SET %b %b", key.c_str(), key.size(), value.c_str(), value.size());
	if(reply == NULL){
		FDLOG("error") << "INV_Redis set:" << key << ":" 
			<< "ret:" << _context->err << ":" 
			<< _context->errstr << ":" 
			<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
		return -1;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "set:" << key << " reply->type:" << reply->type << " status:" << status << endl);

    freeReply(reply);

#ifdef __INV_DEBUG__
	int diff = TNOWMS - start;
    if(diff > 5)
    {
        FDLOG("time") << __FUNCTION__<<__LINE__<< " set redis time:" << diff << ":ip:"
        << _ip << ":port:" << _port  << ":key:"<< key << ":value.size:"<< value.size << endl;
    }
#endif

    return ret;
}

int INV_Redis::remove(const string &key) {
	
	int ret = 0;

	if(!_connected) {
		reconnect();
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis remove not connected" << endl);
		return -1;
	}

	redisReply *reply = (redisReply *)redisCommand(_context, "DEL %b", key.c_str(), key.size());
	if(reply == NULL) {
		FDLOG("error") << "INV_Redis remove:" << key << " ret:" << _context->err << ":" << _context->errstr << endl;
    	ret = -1;
    }

    freeReply(reply);

    return ret;
}

int64_t INV_Redis::incr(const string &key) {
	
	if(!_connected) {
		reconnect();
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis incr not connected" << endl);
		return -1;
	}

	redisReply *reply = (redisReply *)redisCommand(_context, "INCR %b", key.c_str(), key.size());
	if(reply == NULL) {
		FDLOG("error") << "INV_Redis incr:" << key << " ret:" << _context->err << ":" << _context->errstr << endl;
    	return -1;
    }

    int ret = reply->integer;
    
    freeReply(reply);

    return ret;
}

int64_t INV_Redis::decr(const string &key) {
    
    if(!_connected) {
        reconnect();
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis decr not connected" << endl);
        return -1;
    }

    redisReply *reply = (redisReply *)redisCommand(_context, "DECR %b", key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_Redis decr:" << key << " ret:" << _context->err << ":" << _context->errstr << endl;
        return -1;
    }

    int ret = reply->integer;
    
    freeReply(reply);

    return ret;
}

int INV_Redis::scan(int cursor, vector<string> &keys) {
	int ret = 0;
	if(!_connected) {
		reconnect();
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis scan not connected:" << cursor << endl);
		return -1;
	}

    redisReply *reply = (redisReply *)redisCommand(_context, ("SCAN "+INV_Util::tostr(cursor)).c_str());
    if(reply == NULL) {
	    ret = _context->err;
    	FDLOG("error") << "INV_Redis scan:" << cursor << " ret:" << _context->err << ":" << _context->errstr << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
    		//关闭连接，尝试重连
    		reconnect();
    	}

    	return -1;
    }

    do {
    	if(reply->type == REDIS_REPLY_NIL) {
    		//不存在的key
    		ret = -6;
    		break;
    	}
    	else if(reply->type == REDIS_REPLY_ARRAY) {
	        for (unsigned int i = 0; i < reply->elements; i++) {
	        	if(reply->element[i]->type == REDIS_REPLY_STRING){
	        		cursor = atoi(reply->element[i]->str);
	        	}
	        	else if (reply->element[i]->type == REDIS_REPLY_ARRAY) {

	        		for (unsigned int j = 0; j < reply->element[i]->elements; j++) {
	        			if(reply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
	        				string value = string(reply->element[i]->element[j]->str, reply->element[i]->element[j]->len);
	        				keys.push_back(value);
	        			}
	        		}

	        	}
        	}
	    }
	    else {
	    	FDLOG("error") << "INV_Redis scan reply->type:" << reply->type
                << " ip:" << _ip
                << " port:" << _port
                << endl;
	    	ret = -1;
	    }

    }while(0);

    freeReply(reply);

    return cursor;
}

int INV_Redis::rpush(const std::string &key, const std::string &value)
{
	int ret = 0;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis rpush not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		return -1;
	}

	redisReply *reply = (redisReply *)redisCommand(_context, "RPUSH %b %b", key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
	    FDLOG("error") << "INV_Redis rpush:" << key << ":" 
			<< "ret:" << _context->err << ":" 
			<< _context->errstr << ":" 
			<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
		return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "rpush:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);

	return ret;
}

int INV_Redis::lset(const std::string &key, int index, const std::string &value)
{
	int ret = 0;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lset not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		return -1;
	}

	string command = "LSET %b "+INV_Util::tostr(index)+" %b";
	redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
	    FDLOG("error") << "INV_Redis lset:" << key << ":" 
			<< "ret:" << _context->err << ":" 
			<< _context->errstr << ":" 
			<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
		return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "lset:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);

	return ret;
}

int INV_Redis::llen(const std::string &key)
{
	int ret = -1;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis llen not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		return -1;
	}

    string command = "LLEN %b";
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
	    ret = _context->err;
    	FDLOG("error") << "INV_Redis llen:" << key  << ":" 
    		<< "ret:" << _context->err << ":" 
    		<< _context->errstr << ":" 
    		<< pthread_self() << ":" 
    		<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
    	if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
    		//关闭连接，尝试重连
    		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
    		reconnect();
    	}

    	return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

	return ret;
}

int INV_Redis::lpop(const std::string &key,std::string &value)
{
    int ret = 0;
    if(!_connected) {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lpop not connected:" << key
            << " timeout:" << _timeout
            << " ip:" << _ip
            << " port:" << _port
            << endl);
        reconnect();
        return -1;
    }

    redisReply *reply = (redisReply *)redisCommand(_context, "LPOP %b", key.c_str(), key.size());
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis lindex:" << key  << ":" << ":"
            << "ret:" << _context->err << ":"
            << _context->errstr << ":"
            << pthread_self() << ":"
            << "ip:" << _ip << ":"
            << "port:" << _port
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    if(reply->type == REDIS_REPLY_NIL) {
        //不存在的key
        ret = -6;
    }
    else if(reply->type == REDIS_REPLY_STRING && reply->str != NULL) {
        value = string(reply->str, reply->len);
    }
    else {
        ret = -1;
    }

    freeReply(reply);

    return ret;
}


int INV_Redis::lindex(const std::string &key, int index, std::string &value)
{
    int ret = 0;
    if(!_connected) {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lindex not connected:" << key 
            << " timeout:" << _timeout
            << " ip:" << _ip
            << " port:" << _port 
            << endl);
        reconnect();
        return -1;
    }

    string command = "LINDEX %b "+INV_Util::tostr(index);
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis lindex:" << key  << ":"  << index  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    if(reply->type == REDIS_REPLY_NIL) {
        //不存在的key
        ret = -6;
    }
    else if(reply->type == REDIS_REPLY_STRING && reply->str != NULL) {
        value = string(reply->str, reply->len);
    }
    else {
        ret = -1;
    }

    freeReply(reply);

    return ret;
}

int INV_Redis::lrange(const std::string &key, int start, int end, std::vector<string> &values)
{
	int ret = 0;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lrange not connected:" << key 
			<< " timeout:" << _timeout
        	<< " ip:" << _ip
        	<< " port:" << _port 
			<< endl);
		reconnect();
		return -1;
	}

	string range = INV_Util::tostr(start)+" "+INV_Util::tostr(end);
    string command = "LRANGE %b "+range;
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
	    ret = _context->err;
    	FDLOG("error") << "INV_Redis lrange:" << key  << ":"  << start  << ":"  << end  << ":" 
    		<< "ret:" << _context->err << ":" 
    		<< _context->errstr << ":" 
    		<< pthread_self() << ":" 
    		<< "ip:" << _ip << ":" 
        	<< "port:" << _port 
        	<< endl;
    	if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
    		//关闭连接，尝试重连
    		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
    		reconnect();
    	}

    	return -1;
    }

    //int count = 0;
    if(reply->type == REDIS_REPLY_ARRAY) {
		//便利结果

		for (unsigned int i = 0; i < reply->elements; i++) {
        	if(reply->element[i]->type == REDIS_REPLY_STRING){
        		//count = atoi(reply->element[i]->str);
                string value = string(reply->element[i]->str, reply->element[i]->len);
                values.push_back(value);
        	}
            /*
        	else if (reply->element[i]->type == REDIS_REPLY_ARRAY) {

        		for (unsigned int j = 0; j < reply->element[i]->elements; j++) {
        			if(reply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
        				string value = string(reply->element[i]->element[j]->str, reply->element[i]->element[j]->len);
        				values.push_back(value);
        			}
        		}

        	}*/
    	}
	}

    freeReply(reply);

	return ret;
}

int INV_Redis::lrem(const std::string &key, const std::string &value)
{
    int ret = 0;
    if(!_connected) {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lrem not connected:" << key 
            << " timeout:" << _timeout
            << " ip:" << _ip
            << " port:" << _port 
            << endl);
        reconnect();
        return -1;
    }

    string command = "LREM %b 1 %b";
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_Redis lrem:" << key << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "lrem:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);

    return ret;
}

void* setcommand( redisContext *c, const string& key, const vector<string>& vtKey, const vector<string>& vtVal)
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
	return (redisReply *)redisCommandArgv(c,argv.size(),&(argv[0]),&(argvlen[0]));
}

void* getcommand( redisContext *c, const string& key, const vector<string>& vtKey)
{
	string strCmd = "HMGET ";
	strCmd += key;
	for(size_t i = 0; i < vtKey.size(); i++)
	{
		strCmd += " "+vtKey[i];
	}
	return redisCommand(c, strCmd.c_str() );
}

int INV_Redis::hashset(const std::string &key, const std::vector<std::string>& field,
					   const std::vector<std::string>& value)
{
	int ret = 0;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis hashset not connected:" << key
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
		FDLOG("error") << "INV_Redis hashset:" << key << ":"
		<< "ret:" << _context->err << ":"
		<< _context->errstr << ":"
		<< "ip:" << _ip << ":"
		<< "port:" << _port
		<< endl;
		return -1;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "hashset:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

	freeReply(reply);

	return ret;
}

int INV_Redis::hashget(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& values)
{
	int ret = -1;
	if(!_connected) {
		LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis hashget not connected:" << key
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
		FDLOG("error") << "INV_Redis hashget:" << key << ":"
		<< "ret:" << _context->err << ":"
		<< _context->errstr << ":"
		<< "ip:" << _ip << ":"
		<< "port:" << _port
		<< endl;
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

#ifndef _RECONNECT_
#define _RECONNECT_ \
    if(!_connected) { \
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis lindex not connected:" << key \
            << " timeout:" << _timeout \
            << " ip:" << _ip \
            << " port:" << _port \
            << endl); \
        reconnect(); \
        return -1; \
    }
#endif

int INV_Redis::zadd(const std::string &key, int32_t score, int32_t member)
{
    int ret = 0;
    _RECONNECT_

    string sScore = INV_Util::tostr(score);
    string sMember = INV_Util::tostr(member);

    redisReply *reply = NULL;
    if(_ssdb)
    {
        reply = (redisReply *)redisCommand(_context, "zset %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        sMember.c_str(), sMember.size());
    }
    else
    {
        reply = (redisReply *)redisCommand(_context, "zadd %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        sMember.c_str(), sMember.size());
    }

    if(reply == NULL){
        ret = _context->err;
        FDLOG("error") << "INV_Redis zadd:" << key  << ":" << ":"
            << "ret:" << _context->err << ":"
            << _context->errstr << ":"
            << pthread_self() << ":"
            << "ip:" << _ip << ":"
            << "port:" << _port
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "zadd:" << key << " reply->type:" << reply->type << " status:" << status << endl);

    freeReply(reply);

    return ret;
}

int INV_Redis::zrevrange(const std::string &key, int32_t start, int32_t stop, vector<int32_t> &ids)
{
    int ret = 0;
    _RECONNECT_

    string sStart = INV_Util::tostr(start);
    string sStop = INV_Util::tostr(stop);


    //key max min limit
    //zRANGEBYSCORE k 8002 100000 limit 0 3 
    string command;
    if(_ssdb)
    {
        command = "zrrange %b %b %b";
    }
    else
    {
        command = "zrevrange %b %b %b";
    }

    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), 
        key.c_str(), key.size(), 
        sStart.c_str(), sStart.size(), 
        sStop.c_str(), sStop.size()
        );
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis zrevrange:" << key  << ":"  << sStart  << ":"  << sStop  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    //int count = 0;
    if(reply->type == REDIS_REPLY_ARRAY) {
        //便利结果

        for (unsigned int i = 0; i < reply->elements; i++) {
            if(reply->element[i]->type == REDIS_REPLY_STRING){
                //count = atoi(reply->element[i]->str);
                string value = string(reply->element[i]->str, reply->element[i]->len);
                ids.push_back(INV_Util::strto<int32_t>(value));
            }
            /*
            else if (reply->element[i]->type == REDIS_REPLY_ARRAY) {

                for (unsigned int j = 0; j < reply->element[i]->elements; j++) {
                    if(reply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
                        string value = string(reply->element[i]->element[j]->str, reply->element[i]->element[j]->len);
                        values.push_back(value);
                    }
                }

            }*/
        }
    }

    freeReply(reply);

    return ret;
}


int INV_Redis::zrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids)
{
    int ret = 0;
    _RECONNECT_

    string sMin = INV_Util::tostr(min);
    string sMax = INV_Util::tostr(max);
    string sLimit = INV_Util::tostr(limit);

    //key max min limit
    //zRANGEBYSCORE k 8002 100000 limit 0 3 
    string command;
    if(_ssdb)
    {
        command = "zscan %b 0 %b %b limit %b";
    }
    else
    {
        command = "zrangebyscore %b %b %b limit 0 %b";
    }

    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), 
        key.c_str(), key.size(), 
        sMin.c_str(), sMin.size(), 
        sMax.c_str(), sMax.size(), 
        sLimit.c_str(), sLimit.size()
        );
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis zrangebyscore:" << key  << ":"  << sMin  << ":"  << sMin  << ":" << sLimit  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    //int count = 0;
    if(reply->type == REDIS_REPLY_ARRAY) {
        //便利结果

        for (unsigned int i = 0; i < reply->elements; i++) {
            if(reply->element[i]->type == REDIS_REPLY_STRING){
                //count = atoi(reply->element[i]->str);
                string value = string(reply->element[i]->str, reply->element[i]->len);
                ids.push_back(INV_Util::strto<int32_t>(value));
            }
            /*
            else if (reply->element[i]->type == REDIS_REPLY_ARRAY) {

                for (unsigned int j = 0; j < reply->element[i]->elements; j++) {
                    if(reply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
                        string value = string(reply->element[i]->element[j]->str, reply->element[i]->element[j]->len);
                        values.push_back(value);
                    }
                }

            }*/
        }
    }

    freeReply(reply);

    return ret;
}


int INV_Redis::zrevrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids)
{
    int ret = 0;
    _RECONNECT_

    string sMin = INV_Util::tostr(min);
    string sMax = INV_Util::tostr(max);
    string sLimit = INV_Util::tostr(limit);

    //key max min limit
    //zRANGEBYSCORE k 8002 100000 limit 0 3 
    string command;
    if(_ssdb)
    {
        command = "zrscan %b 0 %b %b limit %b";
    }
    else
    {
        command = "zrevrangebyscore %b %b %b limit 0 %b";
    }
    
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), 
        key.c_str(), key.size(), 
        sMax.c_str(), sMax.size(), 
        sMin.c_str(), sMin.size(), 
        sLimit.c_str(), sLimit.size()
        );
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis zrevrangebyscore:" << key  << ":"  << sMin  << ":"  << sMin  << ":" << sLimit  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    //int count = 0;
    if(reply->type == REDIS_REPLY_ARRAY) {
        //便利结果

        for (unsigned int i = 0; i < reply->elements; i++) {
            if(reply->element[i]->type == REDIS_REPLY_STRING){
                //count = atoi(reply->element[i]->str);
                string value = string(reply->element[i]->str, reply->element[i]->len);
                ids.push_back(INV_Util::strto<int32_t>(value));
            }
            /*
            else if (reply->element[i]->type == REDIS_REPLY_ARRAY) {

                for (unsigned int j = 0; j < reply->element[i]->elements; j++) {
                    if(reply->element[i]->element[j]->type == REDIS_REPLY_STRING) {
                        string value = string(reply->element[i]->element[j]->str, reply->element[i]->element[j]->len);
                        values.push_back(value);
                    }
                }

            }*/
        }
    }

    freeReply(reply);

    return ret;
}

int INV_Redis::zcard(const std::string &key)
{
    int ret = -1;
    if(!_connected) {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis zcard not connected:" << key 
            << " timeout:" << _timeout
            << " ip:" << _ip
            << " port:" << _port 
            << endl);
        reconnect();
        return -1;
    }

    string command = "ZCARD %b";
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis zcard:" << key  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_Redis::zcount(const std::string &key, int32_t score)
{
    int ret = -1;
    if(!_connected) {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis zcount not connected:" << key 
            << " timeout:" << _timeout
            << " ip:" << _ip
            << " port:" << _port 
            << endl);
        reconnect();
        return -1;
    }

    string command = "ZCOUNT %b 0 "+INV_Util::tostr(score);
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), 
        key.c_str(), key.size());
    if(reply == NULL) {
        ret = _context->err;
        FDLOG("error") << "INV_Redis zcount:" << key  << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << pthread_self() << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        if(ret == REDIS_ERR_EOF || ret == REDIS_ERR_OOM || ret == REDIS_ERR_IO) {
            //关闭连接，尝试重连
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_Redis reconnect:" << key << ":" << pthread_self() << endl);
            reconnect();
        }

        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_Redis::zremrangebyscore(const std::string &key, int32_t score)
{
    int ret = 0;
    _RECONNECT_

    string min = INV_Util::tostr(score);
    string max = min;

    string command = "zremrangebyscore %b %b %b";
    redisReply *reply = (redisReply *)redisCommand(_context, command.c_str(), 
        key.c_str(), key.size(), 
        min.c_str(), min.size(), 
        max.c_str(), max.size());

    if(reply == NULL) {
        FDLOG("error") << "INV_Redis lrem:" << key << ":" 
            << "ret:" << _context->err << ":" 
            << _context->errstr << ":" 
            << "ip:" << _ip << ":" 
            << "port:" << _port 
            << endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "zremrangebyscore:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);


    return ret;
}

void ListonThread::ReSetCount()
{
	if( _reqTotalNum > 10 && (TNOW - _reSetTm) > _reload*1000)
	{
		_reqFailNum=0;
		_reqTotalNum=0;
		_reSetTm = TNOW;
		LOGMSG(InvRollLogger::DEBUG_LOG, "ListonThread::ReSetCount!" << pthread_self() << endl);
	}
}

void ListonThread::Recover()
{
	if(IsBadConnect()) {
		if(!_handle->connected()) {
			_handle->reconnect();
		}
		string value;
		_handle->set("_redis_liston", "isOK");
		_handle->get("_redis_liston",value);
	}
}
