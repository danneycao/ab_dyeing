#ifndef __INV_SOCKET_H
#define __INV_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <sys/un.h>
#include "util/inv_ex.h"
using namespace std;

namespace inv
{

struct INV_Socket_Exception : public INV_Exception
{
    INV_Socket_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_Socket_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_Socket_Exception() throw() {};
};

struct INV_SocketConnect_Exception : public INV_Socket_Exception
{
    INV_SocketConnect_Exception(const string &buffer) : INV_Socket_Exception(buffer){};
    INV_SocketConnect_Exception(const string &buffer, int err) : INV_Socket_Exception(buffer, err){};
    ~INV_SocketConnect_Exception() throw() {};
};

class INV_Socket
{
public:

    INV_Socket();

    virtual ~INV_Socket();

    void init(int fd, bool bOwner, int iDomain = AF_INET);

    void setOwner(bool bOwner)  { _bOwner = bOwner; }

    void setDomain(int iDomain) { _iDomain = iDomain; }

    void createSocket(int iSocketType = SOCK_STREAM, int iDomain = AF_INET);

    int getfd() const { return _sock; }

    bool isValid() const { return _sock != INVALID_SOCKET; }

    void close();

    void getPeerName(string &sPeerAddress, uint16_t &iPeerPort);

    void getPeerName(string &sPathName);

    void getSockName(string &sSockAddress, uint16_t &iSockPort);

    void getSockName(string &sPathName);

    int setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level = SOL_SOCKET);

    int getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level = SOL_SOCKET);

    int accept(INV_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen);

    void bind(const string &sServerAddr, int port);

    void bind(const char *sPathName);

    void connect(const string &sServerAddr, uint16_t port);

    void connect(const char *sPathName);

    int connectNoThrow(const string &sServerAddr, uint16_t port);

    int connectNoThrow(struct sockaddr* addr);

    int connectNoThrow(const char *sPathName);

    void listen(int connBackLog);

    int  recv(void *pvBuf, size_t iLen, int iFlag = 0);


    int  send(const void *pvBuf, size_t iLen, int iFlag = 0);

    int recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags = 0);

    int recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags = 0);

    int sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t iToPort, int iFlags = 0);

    int sendto(const void *pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlags = 0);

    void shutdown(int iHow);

    void setblock(bool bBlock = false);

    void setNoCloseWait();

    void setCloseWait(int delay = 30);

    void setCloseWaitDefault();

    void setTcpNoDelay();

    void setKeepAlive();

    int getRecvBufferSize();

    void setRecvBufferSize(int sz);

    int getSendBufferSize();

    void setSendBufferSize(int sz);

    static vector<string> getLocalHosts();

    static void setblock(int fd, bool bBlock);

    static void createPipe(int fds[2], bool bBlock);

    static void parseAddr(const string &sAddr, struct in_addr &stAddr);

    void bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen);

protected:

    int connect(struct sockaddr *pstServerAddr, socklen_t serverLen);   

    void getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen);

    void getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen);

private:

protected:
    static const int INVALID_SOCKET = -1;

    int  _sock;

    bool _bOwner;

    int  _iDomain;
};

}
#endif
