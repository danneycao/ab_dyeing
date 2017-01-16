#include "util/inv_coredis.h"
#include "util/inv_crc.h"
#include "co_routine.h"

using namespace std;
using namespace inv;

volatile uint64_t INV_CoRedis::_slotCache[INV_COREDIS_MAX_SLOT_SIZE + 1] = {0};

INV_CoRedis::~INV_CoRedis() {
    for (map<Addr, INV_CoTCPClient*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        it->second->close();
        delete it->second;
        it->second = NULL;       
    }

    _client = NULL;
}

void INV_CoRedis::init(const string& ip, const int port, const int timeout, bool ssdb, bool mutilconn) {
	_ip = ip;
	_port = port;
	_timeout = timeout;
    _ssdb = ssdb;

    _client = new INV_CoTCPClient;
    _client->init(ip, port, timeout);
    _clients[Addr(ip, port)] = _client;

    int ret = _client->connect();
    if (ret != 0)
    {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_CoRedis connect ret:"
                << ret 
                << ", error:" << strerror(errno)
                << ", timeout:" << _timeout
                << ", ip:" << _client->getIp()
                << ", port:" << _client->getPort() 
                << ", ssdb:" << _ssdb 
                << endl);
    }
    assert (ret == 0);

    if (_slotCache[INV_COREDIS_MAX_SLOT_SIZE] == 0)
    {
        _slotCache[INV_COREDIS_MAX_SLOT_SIZE] = 1;
        ret = ClusterSlots();
        if (ret < 0)
        {
            LOGMSG(InvRollLogger::ERROR_LOG, " INV_CoRedis ClusterSlots ret:"
                    << ret 
                    << ", error:" << strerror(errno)
                    << ", timeout:" << _timeout
                    << ", ip:" << _client->getIp()
                    << ", port:" << _client->getPort() 
                    << ", ssdb:" << _ssdb 
                    << endl);

            _slotCache[INV_COREDIS_MAX_SLOT_SIZE] = 2;
        }
        _slotCache[INV_COREDIS_MAX_SLOT_SIZE] = 3;
    }

    if (mutilconn)
    {
        for (int i = 0; i < INV_COREDIS_MAX_SLOT_SIZE; i += 1000)
        {
            uint64_t n = _slotCache[i];
            if (n)
            {
                Addr addr(n);
                if (_clients.find(addr) == _clients.end())
                {
                    INV_CoTCPClient* client = new INV_CoTCPClient;
                    client->init(addr._ip, addr._port, timeout);
                    _clients[addr] = client;
                    client->connect();
                }
            }       
        }
    }
}

bool INV_CoRedis::connect() {
    int ret = _client->connect();
    if (ret != 0)
    {
        LOGMSG(InvRollLogger::ERROR_LOG, " INV_CoRedis connect ret:"
                << ret 
                << ", error:" << strerror(errno)
                << ", timeout:" << _timeout
                << ", ip:" << _client->getIp()
                << ", port:" << _client->getPort() 
                << ", ssdb:" << _ssdb 
                << endl);
        return false;
    }

    return true;
}

void INV_CoRedis::disconnect() {
    _client->close();
}

void INV_CoRedis::reconnect() {
    disconnect();
    connect();
}

redisReply* INV_CoRedis::realdo(const string& method, char* cmdbuf, int cmdlen)
{
begin_real_do:
    int ret = _client->send(cmdbuf, cmdlen);
    if (ret < 0)
    {
        FDLOG("error") << "INV_CoRedis(" << method << ") send failed! ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", ret=" << ret << ", err:" << strerror(errno) << endl;
        if (ret == INV_CoClientSocket::EM_CONNECT || ret == INV_CoClientSocket::EM_TIMEOUT || ret == INV_CoClientSocket::EM_CLOSE || ret == INV_CoClientSocket::EM_SOCKET)
        {
            for (map<Addr, INV_CoTCPClient*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (_client != it->second)
                {
                    _client = it->second;
                    break;              
                }
            }
        }
        return NULL; 
    }

    redisReader *reader = NULL;
    reader = redisReaderCreate();
    if (reader == NULL) 
    {
        FDLOG("error") << "INV_CoRedis(" << method << ") create reader failed!" << endl;
        return NULL;
    }

    redisReply* reply = NULL;
    char recvbuf[1024] = {0};
    do {
        size_t len = sizeof(recvbuf);
        ret = _client->recv((char*)recvbuf, len);
        if (ret < 0)
        {
            redisReaderFree(reader);
            reader = NULL;
            FDLOG("error") << "INV_CoRedis(" << method << ") recv failed! ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", ret=" << ret << ", err:" << strerror(errno) << endl;
            if (ret == INV_CoClientSocket::EM_CONNECT || ret == INV_CoClientSocket::EM_TIMEOUT || ret == INV_CoClientSocket::EM_CLOSE || ret == INV_CoClientSocket::EM_SOCKET)
            {
                for (map<Addr, INV_CoTCPClient*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                {
                    if (_client != it->second)
                    {
                        _client = it->second;
                        break;              
                    }
                }
            }
            return NULL;
        }

        ret = redisReaderFeed(reader, recvbuf, len);
        if (REDIS_OK != ret)
        {
            redisReaderFree(reader);
            reader = NULL;
            _client->close();
            FDLOG("error") << "INV_CoRedis(" << method << ") redisReaderFeed failed! ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", ret=" << ret  << endl;
            return NULL;
        }

        ret = redisReaderGetReply(reader, (void**)&reply);
        if (ret != REDIS_OK)
        {
            redisReaderFree(reader);
            reader = NULL;
            _client->close();
            FDLOG("error") << "INV_CoRedis(" << method << ") redisGetReplyFromReader failed! ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", ret=" << ret  << endl;
            return NULL;
        }

    } while (reply == NULL);

    redisReaderFree(reader);
    reader = NULL;

    if (reply->type == REDIS_REPLY_ERROR)
    {
        int moveCmdLen = sizeof("MOVED ") - 1;
        if (strncmp(reply->str, "MOVED ", moveCmdLen) == 0)
        {
            FDLOG("error") << "INV_CoRedis(" << method << ") Need to moved ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", movedinfo:" << reply->str  << endl;
            string strRelocate(reply->str + moveCmdLen, reply->len - moveCmdLen);
            vector<string> relocateInfo = INV_Util::sepstr<string>(strRelocate, " ");
            if (relocateInfo.size() >= 2)
            {
                uint16_t slot = atoi(relocateInfo[0].c_str());
                strRelocate = relocateInfo[1];
                vector<string> vecAddr = INV_Util::sepstr<string>(strRelocate, ":");
                if (vecAddr.size() == 2)
                {
                    uint16_t port = atoi(vecAddr[1].c_str());
                    if (port != 0)
                    {
                        Addr addr(vecAddr[0], port);
                        map<Addr, INV_CoTCPClient*>::iterator it = _clients.find(addr);
                        if (it != _clients.end())
                        {
                            _client = it->second;
                            if (_client->isValid())
                                _client->connect();
                        }
                        else
                        {
                            INV_CoTCPClient* cli = new INV_CoTCPClient;
                            cli->init(addr._ip, addr._port, _timeout);
                            cli->connect();
                            
                            _clients[addr] = cli;
                            _client = cli;
                        }

                        if (slot < INV_COREDIS_MAX_SLOT_SIZE)
                            _slotCache[slot] = addr._naddr.n;

                        freeReply(reply);
                        reply = NULL;
                        goto begin_real_do;
                    }                   
                }
            }
        }
        else
        {
            FDLOG("error") << "INV_CoRedis(" << method << ") reply error! ip:" << _client->getIp() << ", port=" << _client->getPort() << ", timeout=" << _timeout << ", ssdb=" << _ssdb << ", err:" << reply->str  << endl;
        }
    }
    return reply;
}

redisReply* INV_CoRedis::exec0p(const string& method, const char* format)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommand(&cmdbuf, format);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << " " << format << ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << " " << format << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}
redisReply* INV_CoRedis::exec1p(const string& method, const char* format, const char* cmd1, int cmd1len)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommand(&cmdbuf, format, cmd1, cmd1len);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}

redisReply* INV_CoRedis::exec2p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommand(&cmdbuf, format, cmd1, cmd1len, cmd2, cmd2len);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}

redisReply* INV_CoRedis::exec3p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len, const char* cmd3, int cmd3len)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommand(&cmdbuf, format, cmd1, cmd1len, cmd2, cmd2len, cmd3, cmd3len);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << " " << cmd3 << ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << " " << cmd3 << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}

redisReply* INV_CoRedis::exec4p(const string& method, const char* format, const char* cmd1, int cmd1len, const char* cmd2, int cmd2len, const char* cmd3, int cmd3len, const char* cmd4, int cmd4len)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommand(&cmdbuf, format, cmd1, cmd1len, cmd2, cmd2len, cmd3, cmd3len, cmd4, cmd4len);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << " " << cmd3 << " " << cmd4 << ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << " " << cmd1 << " " << cmd2 << " " << cmd3 << " " << cmd4 << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}

redisReply* INV_CoRedis::execmp(const string& method, int argc, const char** argv, const size_t* argvlen)
{
    char* cmdbuf = NULL;
    int ret = redisFormatCommandArgv(&cmdbuf, argc, argv, argvlen);
    if (ret < 0)
    {
        if (cmdbuf != NULL)
        {
            free(cmdbuf);
            cmdbuf = NULL;
        }
        FDLOG("error") << "INV_CoRedis(" << method << "[pc:" << argc << "]"<< ") format cmd failed! ret=" << ret << endl;
        return NULL;  
    }

    redisReply* r = realdo(method, cmdbuf, ret);
    if (r == NULL)
    {
        FDLOG("error") << "INV_CoRedis(" << method << "[pc:" << argc << "]" << ") send or recv failed!" << endl;
    }

    if (cmdbuf != NULL)
    {
        free(cmdbuf);
        cmdbuf = NULL;
    }

    return r;
}

/* 集群信息获取命令 */
int INV_CoRedis::ClusterNodes()
{
    int ret = 0;
    redisReply* r = exec0p("cluster", "cluster nodes");
    cerr << r->type << endl;
    if (r == NULL)
    {
        ret = -1;
    }
    else if (r->type == REDIS_REPLY_STRING)
    {
        cerr << r->str << endl;
    }
    else 
    {
        ret = -1;
    }

    if (r)
    {
        freeReply(r);
    }

    return ret;
}

int INV_CoRedis::ClusterSlots()
{
    int ret = 0;
    redisReply* r = exec0p("ClusterSlots", "cluster slots");
    if (r == NULL)
    {
        ret = -1;
    }
    else if (r->type == REDIS_REPLY_ARRAY)
    {
        for (int i = 0; i < (int)r->elements; i++)
        {
            if (r->element[i]->type == REDIS_REPLY_ARRAY && r->element[i]->elements >= 3 
                    && r->element[i]->element[0]->type == REDIS_REPLY_INTEGER 
                    && r->element[i]->element[1]->type == REDIS_REPLY_INTEGER 
                    && r->element[i]->element[2]->type == REDIS_REPLY_ARRAY
                    && r->element[i]->element[2]->elements >= 2
                    && r->element[i]->element[2]->element[0]->type == REDIS_REPLY_STRING
                    && r->element[i]->element[2]->element[1]->type == REDIS_REPLY_INTEGER)
            {
                int startSlot = r->element[i]->element[0]->integer;
                int endSlot = r->element[i]->element[1]->integer;
                assert (endSlot < INV_COREDIS_MAX_SLOT_SIZE);

                string ip = r->element[i]->element[2]->element[0]->str;
                int port = r->element[i]->element[2]->element[1]->integer; 
                Addr addr(ip, port);
                cerr << "slots:" << startSlot << " -> " << endSlot << "| ip:" << ip << ", port:" << port << endl;
                for (int j = startSlot; j <= endSlot; j++)
                    _slotCache[j] = addr._naddr.n;
            }
        }
    }
    else 
    {
        ret = -1;
    }

    if (r)
    {
        freeReply(r);
    }

    return ret;
}

void INV_CoRedis::AdjustClient(const string& key)
{
    uint16_t slot = INV_Crc::crc16(key.c_str(), key.size()) % INV_COREDIS_MAX_SLOT_SIZE;
    if (_slotCache[slot])
    {
        Addr addr(_slotCache[slot]);
        map<Addr, INV_CoTCPClient*>::iterator it = _clients.find(addr);
        if (it != _clients.end() && it->second->isValid())
            _client = it->second;
    }
}

int INV_CoRedis::get(const string &key, string &value) {

#ifdef __INV_DEBUG__
	int64_t start = TNOWMS;
#endif
	int ret = 0;
    AdjustClient(key);
    redisReply* r = exec1p("GET", "GET %b", key.c_str(), key.size());
    if (r == NULL)
    {
        ret = -1;
    }
    else if(r->type == REDIS_REPLY_NIL) {
		//不存在的key
		ret = -6;
	}
	else if(r->type == REDIS_REPLY_STRING && r->str != NULL) {
        value = string(r->str, r->len);
    }
    else {
    	ret = -1;
    }

    if (r)
    {
        freeReply(r);
    }
#ifdef __INV_DEBUG__
	int diff = TNOWMS - start;
    if(diff > 5)
    {
		FDLOG("time") << __FUNCTION__<<__LINE__<< " get redis time:" << diff << ":ip:"
        	<< _client->getIp() << ":port:" << _client->getPort()  << ":key:"<< key << ":value.size:"<< value.size << endl;

    }
#endif
    return ret;
}

int INV_CoRedis::set(const string &key, const string &value) {

#ifdef __INV_DEBUG__
	int64_t start = TNOWMS;
#endif
	int ret = 0;
    
    AdjustClient(key);
    redisReply* reply = exec2p("SET", "SET %b %b", key.c_str(), key.size(), value.c_str(), value.size());
	if (reply == NULL)
    {
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
        << _client->getIp() << ":port:" << _client->getPort()  << ":key:"<< key << ":value.size:"<< value.size << endl;
    }
#endif

    return ret;
}

int INV_CoRedis::remove(const string &key) {
	
	int ret = 0;
    
    AdjustClient(key);
	redisReply *reply = exec1p("DEL", "DEL %b", key.c_str(), key.size());
	if(reply == NULL) {
		FDLOG("error") << "INV_CoRedis remove:" << key << " failed!"<< endl;
    	ret = -1;
    }

    freeReply(reply);

    return ret;
}

int64_t INV_CoRedis::incr(const string &key) 
{
    AdjustClient(key);

	redisReply *reply = exec1p("INCR", "INCR %b", key.c_str(), key.size());
	if(reply == NULL) {
		FDLOG("error") << "INV_CoRedis incr:" << key << " failed" << endl;
    	return -1;
    }

    int ret = reply->integer;
    
    freeReply(reply);

    return ret;
}

int64_t INV_CoRedis::decr(const string &key) {
    AdjustClient(key);
    redisReply *reply = exec1p("DECR", "DECR %b", key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis decr:" << key << " failed" << endl;
        return -1;
    }

    int ret = reply->integer;
    
    freeReply(reply);

    return ret;
}

int INV_CoRedis::keys(const string &start, const string &end, int num, vector<string> &keys) {
    int ret = 0;
    string nums = INV_Util::tostr(num);
    redisReply *reply = exec3p("KEYS", "KEYS %b %b %b", 
                start.c_str(), start.size(), end.c_str(), end.size(), nums.c_str(), nums.size());
    if(reply == NULL) 
    {
    	FDLOG("error") << "INV_CoRedis keys:" << start << " " << end << " failed!" << endl;
    	return -1;
    }

    do {
    	if(reply->type == REDIS_REPLY_NIL) 
        {
    		//不存在的key
    		ret = -6;
    		break;
    	}
    	else if(reply->type == REDIS_REPLY_ARRAY) 
        {
            for (unsigned int i = 0; i < reply->elements; i++) 
            {
                if(reply->element[i]->type == REDIS_REPLY_STRING)
                {
                    keys.push_back(string(reply->element[i]->str, reply->element[i]->len));
                }
        	}
	    }
	    else 
        {
	    	FDLOG("error") << "INV_CoRedis keys reply->type:" << reply->type
                << " ip:" << _client->getIp()
                << " port:" << _client->getPort()
                << endl;
	    	ret = -1;
	    }

    }while(0);

    freeReply(reply);

    return reply->elements; 
}

int INV_CoRedis::rpush(const std::string &key, const std::string &value)
{
    int ret = 0;
    AdjustClient(key);
	
    redisReply *reply = exec2p("RPUSH", "RPUSH %b %b", key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
	    FDLOG("error") << "INV_CoRedis rpush:" << key << " failed!" << endl;
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

int INV_CoRedis::lset(const std::string &key, int index, const std::string &value)
{
    int ret = 0; 
    AdjustClient(key);

	string command = "LSET %b "+INV_Util::tostr(index)+" %b";
	redisReply *reply = exec2p("LSET", command.c_str(), key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
	    FDLOG("error") << "INV_CoRedis lset:" << key << " failed!" << endl;
		return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "lset:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);

	return ret;
}

int INV_CoRedis::llen(const std::string &key)
{
    int ret = 0;
    AdjustClient(key);

    string command = "LLEN %b";
    redisReply *reply = exec1p("LLEN", command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
    	FDLOG("error") << "INV_CoRedis llen:" << key  << " failed!" << endl;
    	return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

	return ret;
}

int INV_CoRedis::lpop(const std::string &key,std::string &value)
{
    int ret = 0;
    AdjustClient(key);

    redisReply *reply = exec1p("LPOP", "LPOP %b", key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis lpop:" << key  << " failed!" << endl;
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

int INV_CoRedis::blpop(const std::string& key, std::string& value)
{
    int ret = 0;
    AdjustClient(key);
    
    string cmdfmt = "BLPOP %b " + INV_Util::tostr(_timeout);
    redisReply *reply = exec1p("BLPOP", cmdfmt.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis blpop:" << key  << " failed!" << endl;
        return -1;
    }

    if(reply->type == REDIS_REPLY_NIL) {
        //不存在的key
        ret = -6;
    }
    if(reply->type == REDIS_REPLY_ARRAY) {
        ret = -6;
        if (reply->elements >=  2 && reply->element[0]->type == REDIS_REPLY_STRING && 
            reply->element[1]->type == REDIS_REPLY_STRING && key == string(reply->element[0]->str, reply->element[0]->len))
        {
            ret = 0;
            value = string(reply->element[1]->str, reply->element[1]->len);
        }
    }
    else {
        ret = -1;
    }

    freeReply(reply);

    return ret;
}

int INV_CoRedis::lindex(const std::string &key, int index, std::string &value)
{
    int ret = 0;
    AdjustClient(key);

    string command = "LINDEX %b "+INV_Util::tostr(index);
    redisReply *reply = exec1p("LINDEX", command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis lindex:" << key  << " failed!"  << endl;
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

int INV_CoRedis::lrange(const std::string &key, int start, int end, std::vector<string> &values)
{
    int ret = 0;
    AdjustClient(key);

	string range = INV_Util::tostr(start)+" "+INV_Util::tostr(end);
    string command = "LRANGE %b "+range;
    redisReply *reply = exec1p("LRANGE", command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
    	FDLOG("error") << "INV_CoRedis lrange:" << key  << ":"  << start  << ":"  << end  << " failed!" << endl;
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

int INV_CoRedis::lrem(const std::string &key, const std::string &value)
{
    int ret = 0;
    AdjustClient(key);

    string command = "LREM %b 1 %b";
    redisReply *reply = exec2p("LREM", command.c_str(), key.c_str(), key.size(), value.c_str(), value.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis lrem:" << key << " failed!" << endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "lrem:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);

    return ret;
}

void* INV_CoRedis::getcommand(const string& key, const vector<string>& vtKey)
{
    AdjustClient(key);

	string strCmd = "HMGET ";
	strCmd += key;
	for(size_t i = 0; i < vtKey.size(); i++)
	{
		strCmd += " " + vtKey[i];
	}

	return exec0p("HMGET", strCmd.c_str() );
}

int INV_CoRedis::zadd(const std::string &key, int64_t score, const string &member)
{
    int ret = 0;
    AdjustClient(key);

    string sScore = INV_Util::tostr(score);

    redisReply *reply = NULL;
    if(_ssdb)
    {
        reply = exec3p("ZSET", "zset %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        member.c_str(), member.size());
    }
    else
    {
        reply = exec3p("ZADD", "zadd %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        member.c_str(), member.size());
    }

    if(reply == NULL){
        FDLOG("error") << "INV_CoRedis zadd:" << key  << " " << score << " " << member << " failed!"<< endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "zadd:" << key << " reply->type:" << reply->type << " status:" << status << endl);

    freeReply(reply);

    return ret;
}

int INV_CoRedis::zadd(const std::string &key, int32_t score, int32_t member)
{
    int ret = 0;
    AdjustClient(key);

    string sScore = INV_Util::tostr(score);
    string sMember = INV_Util::tostr(member);

    redisReply *reply = NULL;
    if(_ssdb)
    {
        reply = exec3p("ZSET", "zset %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        sMember.c_str(), sMember.size());
    }
    else
    {
        reply = exec3p("ZADD", "zadd %b %b %b", 
        key.c_str(), key.size(),
        sScore.c_str(), sScore.size(), 
        sMember.c_str(), sMember.size());
    }

    if(reply == NULL){
        FDLOG("error") << "INV_CoRedis zadd:" << key  << " " << score << " " << member << " failed!" << endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "zadd:" << key << " reply->type:" << reply->type << " status:" << status << endl);

    freeReply(reply);

    return ret;
}

int INV_CoRedis::zrevrange(const std::string &key, int32_t start, int32_t stop, vector<int32_t> &ids)
{
    int ret = 0;
    AdjustClient(key);

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

    redisReply *reply = exec3p("ZREVRANGE", command.c_str(), 
        key.c_str(), key.size(), 
        sStart.c_str(), sStart.size(), 
        sStop.c_str(), sStop.size()
        );
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zrevrange:" << key  << " "  << sStart  << " "  << sStop  << " failed!" << endl;
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

int INV_CoRedis::zrevrange(const std::string &key, int32_t start, int32_t stop, vector<string> &members)
{
    int ret = 0;
    AdjustClient(key);

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

    redisReply *reply = exec3p("ZREVRANGE", command.c_str(), 
        key.c_str(), key.size(), 
        sStart.c_str(), sStart.size(), 
        sStop.c_str(), sStop.size()
        );
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zrevrange:" << key  << " "  << sStart  << " "  << sStop  << " failed!" << endl;
        return -1;
    }

    //int count = 0;
    if(reply->type == REDIS_REPLY_ARRAY) {
        //便利结果

        for (unsigned int i = 0; i < reply->elements; i++) {
            if(reply->element[i]->type == REDIS_REPLY_STRING){
                //count = atoi(reply->element[i]->str);
                members.push_back(string(reply->element[i]->str, reply->element[i]->len));
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

int INV_CoRedis::zrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids)
{
    int ret = 0;
    AdjustClient(key);

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

    redisReply *reply = exec4p("ZRANGEBYSCORE", command.c_str(), 
        key.c_str(), key.size(), 
        sMin.c_str(), sMin.size(), 
        sMax.c_str(), sMax.size(), 
        sLimit.c_str(), sLimit.size()
        );
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zrangebyscore:" << key  << " "  << sMin  << " "  << sMin  << " " << sLimit  << " failed!" << endl;
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


int INV_CoRedis::zrevrangebyscore(const std::string &key, int32_t min, int32_t max, int32_t limit, vector<int32_t> &ids)
{
    int ret = 0;
    AdjustClient(key);

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
    
    redisReply *reply = exec4p("ZREVRANGEBYSCORE", command.c_str(), 
        key.c_str(), key.size(), 
        sMax.c_str(), sMax.size(), 
        sMin.c_str(), sMin.size(), 
        sLimit.c_str(), sLimit.size()
        );
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zrevrangebyscore:" << key  << " "  << sMin  << " "  << sMin  << " " << sLimit  << " failed!" << endl;
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

int INV_CoRedis::zcard(const std::string &key)
{
    int ret = 0;
    AdjustClient(key);

    string command = "ZCARD %b";
    redisReply *reply = exec1p("ZCARD", command.c_str(), key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zcard:" << key  << " failed!" << endl;
        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_CoRedis::zcount(const std::string &key, int32_t score)
{
    int ret = 0;
    AdjustClient(key);

    string command = "ZCOUNT %b 0 "+INV_Util::tostr(score);
    redisReply *reply = exec1p("ZCOUNT", command.c_str(), 
        key.c_str(), key.size());
    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis zcount:" << key  << " failed!" << endl;
        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_CoRedis::zremrangebyscore(const std::string &key, int32_t score)
{
    int ret = 0;
    AdjustClient(key);

    string min = INV_Util::tostr(score);
    string max = min;

    string command = "zremrangebyscore %b %b %b";
    redisReply *reply = exec3p("ZREMRANGEBYSCORE", command.c_str(), 
        key.c_str(), key.size(), 
        min.c_str(), min.size(), 
        max.c_str(), max.size());

    if(reply == NULL) {
        FDLOG("error") << "INV_CoRedis lrem:" << key << " " << min << " " << max << " failed!" << endl;
        return -1;
    }

    string status = reply->str == NULL ? "" : reply->str;
    LOGMSG(InvRollLogger::DEBUG_LOG, "zremrangebyscore:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

    freeReply(reply);


    return ret;
}

int INV_CoRedis::zrank(const std::string &key, const string &member)
{
    int ret = 0;
    AdjustClient(key);

    redisReply *reply = exec2p("ZRANK", "ZRANK %b %b", 
        key.c_str(), key.size(), member.c_str(), member.size());
    if(reply == NULL) {
        FDLOG("error") << "|INV_CoRedis zrank:" << key  << "|member:" << member << " failed!" << endl;
        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_CoRedis::zrevrank(const std::string &key, const string &member)
{
    int ret = 0;
    AdjustClient(key);

    redisReply *reply = exec2p("ZREVRANK", "ZREVRANK %b %b", 
        key.c_str(), key.size(), member.c_str(), member.size());
    if(reply == NULL) {
        FDLOG("error") << "|INV_CoRedis zrank:" << key  << "|member:" << member << " failed!" << endl;
        return -1;
    }

    if(reply->type == REDIS_REPLY_INTEGER) {
        ret = reply->integer;
    }

    freeReply(reply);

    return ret;
}

int INV_CoRedis::hashset(const std::string &key, const std::vector<std::string>& field, const std::vector<std::string>& value)
{
	int ret = 0;
    if(field.size() != value.size())
	{
		return -2;
	}

    AdjustClient(key);
    
    std::vector<const char*> argv;
	argv.reserve(field.size() + value.size() + 2);
	std::vector<size_t> argvlen;
	argvlen.reserve(field.size() + value.size() + 2);

	static char mgetcmd[] = "HMSET";
	argv.push_back(mgetcmd);
	argvlen.push_back(sizeof(mgetcmd) - 1);
	argv.push_back(key.c_str());
	argvlen.push_back(key.size());

	for(size_t i = 0; i < field.size(); i++) {
		argv.push_back(field[i].c_str()), argvlen.push_back(field[i].size());
		argv.push_back(value[i].c_str()), argvlen.push_back(value[i].size());
	}

	redisReply *reply = execmp(string("HMSET ") + key, argv.size(), &(argv[0]), &(argvlen[0]));
	if(reply == NULL) {
		return -1;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "hashset:" << key << " reply->type:" << reply->type << " status:" << status << ":" << ret << endl);

	freeReply(reply);

	return ret;
}

int INV_CoRedis::hashget(const std::string &key, const std::vector<std::string>& field, std::vector<std::string>& values)
{
    int ret = -1;

    AdjustClient(key);
    
    std::vector<const char*> argv;
	argv.reserve(field.size() + 2);
	std::vector<size_t> argvlen;
	argvlen.reserve(field.size() + 2);

	static char mgetcmd[] = "HMGET";
	argv.push_back(mgetcmd);
	argvlen.push_back(sizeof(mgetcmd) - 1);
	argv.push_back(key.c_str());
	argvlen.push_back(key.size());

	for(size_t i = 0; i < field.size(); i++) {
		argv.push_back(field[i].c_str()), argvlen.push_back(field[i].size());
	}

	redisReply *reply = execmp(string("HMGET ") + key, argv.size(), &(argv[0]), &(argvlen[0]));
	int32_t ret_type = 0;
	if(reply == NULL) {
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

int INV_CoRedis::hashset(const std::string &key, const std::string& field, const std::string& value)
{
	/*
	int ret = 0;

    AdjustClient(key);
    redisReply* reply = exec3p("HSET", "HSET %b %b %b", key.c_str(), key.size(), field.c_str(), field.size(), value.c_str(), value.size());
	if (reply == NULL)
    {
        return -1;
	}

	string status = reply->str == NULL ? "" : reply->str;
	LOGMSG(InvRollLogger::DEBUG_LOG, "set:" << key << " reply->type:" << reply->type << " status:" << status << endl);

    freeReply(reply);

	return ret;
	*/
	vector<std::string> fields;
	vector<std::string> values;
	fields.push_back(field);
	values.push_back(value);
	return hashset(key, fields, values);
}

int INV_CoRedis::hashget(const std::string &key, const std::string& field, std::string& value)
{
	/*
	int ret = 0;
    AdjustClient(key);
    redisReply* r = exec2p("HGET", "HGET %b", key.c_str(), key.size(), field.c_str(), field.size());
    if (r == NULL)
    {
        ret = -1;
    }
    else if(r->type == REDIS_REPLY_NIL) {
		//不存在的key
		ret = -6;
	}
	else if(r->type == REDIS_REPLY_STRING && r->str != NULL) {
        value = string(r->str, r->len);
    }
    else {
    	ret = -1;
    }

    if (r)
    {
        freeReply(r);
    }
	
	return ret;
	*/
	vector<std::string> values;
	vector<std::string> fields;
	fields.push_back(field);
	int ret = hashget(key, fields, values);
	if (0 != ret)
	{
		return ret;
	}
	if (values.size() != 1)
	{
		return -2;
	}

	value = values[0];
	return 0;
}


