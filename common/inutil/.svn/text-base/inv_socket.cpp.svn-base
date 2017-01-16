#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cassert>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "util/inv_socket.h"
#include "util/inv_util.h"

namespace inv
{

INV_Socket::INV_Socket() : _sock(INVALID_SOCKET), _bOwner(true), _iDomain(AF_INET)
{
}

INV_Socket::~INV_Socket()
{
    if(_bOwner)
    {
        close();
    }
}

void INV_Socket::init(int fd, bool bOwner, int iDomain)
{
    if(_bOwner)
    {
        close();
    }

    _sock       = fd;
    _bOwner     = bOwner;
    _iDomain    = iDomain;
}

void INV_Socket::createSocket(int iSocketType, int iDomain)
{
    assert(iSocketType == SOCK_STREAM || iSocketType == SOCK_DGRAM);
    close();

    _iDomain    = iDomain;
    _sock       = socket(iDomain, iSocketType, 0);

    if(_sock < 0)
    {
        _sock = INVALID_SOCKET;
        throw INV_Socket_Exception("[INV_Socket::createSocket] create socket error! :" + string(strerror(errno)));
    }
}

void INV_Socket::getPeerName(string &sPeerAddress, uint16_t &iPeerPort)
{
    assert(_iDomain == AF_INET);

    struct sockaddr stPeer;
    bzero(&stPeer, sizeof(struct sockaddr));
    socklen_t iPeerLen = sizeof(sockaddr);

    getPeerName(&stPeer, iPeerLen);

    char sAddr[INET_ADDRSTRLEN] = "\0";
    struct sockaddr_in *p = (struct sockaddr_in *)&stPeer;

    inet_ntop(_iDomain, &p->sin_addr, sAddr, sizeof(sAddr));

    sPeerAddress= sAddr;
    iPeerPort   = ntohs(p->sin_port);
}

void INV_Socket::getPeerName(string &sPathName)
{
    assert(_iDomain == AF_LOCAL);

    struct sockaddr_un stSock;
    bzero(&stSock, sizeof(struct sockaddr_un));
    socklen_t iSockLen = sizeof(stSock);
    getPeerName((struct sockaddr *)&stSock, iSockLen);

    sPathName = stSock.sun_path;
}

void INV_Socket::getPeerName(struct sockaddr *pstPeerAddr, socklen_t &iPeerLen)
{
    if(getpeername(_sock, pstPeerAddr, &iPeerLen) < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::getPeerName] getpeername error", errno);
    }
}

void INV_Socket::getSockName(string &sSockAddress, uint16_t &iSockPort)
{
    assert(_iDomain == AF_INET);

    struct sockaddr stSock;
    bzero(&stSock, sizeof(struct sockaddr));
    socklen_t iSockLen = sizeof(sockaddr);

    getSockName(&stSock, iSockLen);

    char sAddr[INET_ADDRSTRLEN] = "\0";
    struct sockaddr_in *p = (struct sockaddr_in *)&stSock;

    inet_ntop(_iDomain, &p->sin_addr, sAddr, sizeof(sAddr));

    sSockAddress = sAddr;
    iSockPort = ntohs(p->sin_port);
}

void INV_Socket::getSockName(string &sPathName)
{
    assert(_iDomain == AF_LOCAL);

    struct sockaddr_un stSock;
    bzero(&stSock, sizeof(struct sockaddr_un));
    socklen_t iSockLen = sizeof(stSock);
    getSockName((struct sockaddr *)&stSock, iSockLen);

    sPathName = stSock.sun_path;
}

void INV_Socket::getSockName(struct sockaddr *pstSockAddr, socklen_t &iSockLen)
{
    if(getsockname(_sock, pstSockAddr, &iSockLen) < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::getSockName] getsockname error", errno);
    }
}

int INV_Socket::accept(INV_Socket &tcSock, struct sockaddr *pstSockAddr, socklen_t &iSockLen)
{
    assert(tcSock._sock == INVALID_SOCKET);

    int ifd;

    while ((ifd = ::accept(_sock, pstSockAddr, &iSockLen)) < 0 && errno == EINTR);

    tcSock._sock    = ifd;
    tcSock._iDomain = _iDomain;

    return tcSock._sock;
}

void INV_Socket::parseAddr(const string &sAddr, struct in_addr &stSinAddr)
{
    int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
    if(iRet < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::parseAddr] inet_pton error", errno);
    }
    else if(iRet == 0)
    {
        struct hostent stHostent;
        struct hostent *pstHostent;
        char buf[2048] = "\0";
        int iError;

        gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);

        if (pstHostent == NULL)
        {
            throw INV_Socket_Exception("[INV_Socket::parseAddr] gethostbyname_r error! :" + string(hstrerror(iError)));
        }
        else
        {
            stSinAddr = *(struct in_addr *) pstHostent->h_addr;
        }
    }
}

void INV_Socket::bind(const string &sServerAddr, int port)
{
    assert(_iDomain == AF_INET);

    struct sockaddr stBindAddr;
    bzero(&stBindAddr, sizeof(struct sockaddr));

    struct sockaddr_in *p = (struct sockaddr_in *)&stBindAddr;

    p->sin_family   = _iDomain;
    p->sin_port     = htons(port);

    if (sServerAddr == "")
    {
        p->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
    	parseAddr(sServerAddr, p->sin_addr);
    }

    try
    {
        bind(&stBindAddr, sizeof(stBindAddr));
    }
    catch(...)
    {
        throw INV_Socket_Exception("[INV_Socket::bind] bind '" + sServerAddr + ":" + INV_Util::tostr(port) + "' error", errno);
    }
}

void INV_Socket::bind(const char *sPathName)
{
    assert(_iDomain == AF_LOCAL);

    unlink(sPathName);

    struct sockaddr_un stBindAddr;
    bzero(&stBindAddr, sizeof(struct sockaddr_un));
    stBindAddr.sun_family = _iDomain;
    strncpy(stBindAddr.sun_path, sPathName, sizeof(stBindAddr.sun_path));

    try
    {
        bind((struct sockaddr *)&stBindAddr, sizeof(stBindAddr));
    }
    catch(...)
    {
        throw INV_Socket_Exception("[INV_Socket::bind] bind '" + string(sPathName) + "' error", errno);
    }
}

void INV_Socket::bind(struct sockaddr *pstBindAddr, socklen_t iAddrLen)
{
	int iReuseAddr = 1;

    setSockOpt(SO_REUSEADDR, (const void *)&iReuseAddr, sizeof(int), SOL_SOCKET);

    if(::bind(_sock, pstBindAddr, iAddrLen) < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::bind] bind error", errno);
    }
}

void INV_Socket::close()
{
    if (_sock != INVALID_SOCKET)
    {
        ::close(_sock);
        _sock = INVALID_SOCKET;
    }
}

int INV_Socket::connectNoThrow(const string &sServerAddr, uint16_t port)
{
    assert(_iDomain == AF_INET);

    if (sServerAddr == "")
    {
        throw INV_Socket_Exception("[INV_Socket::connect] server address is empty error!");
    }

    struct sockaddr stServerAddr;
    bzero(&stServerAddr, sizeof(stServerAddr));

    struct sockaddr_in *p = (struct sockaddr_in *)&stServerAddr;

    p->sin_family = _iDomain;
    parseAddr(sServerAddr, p->sin_addr);
    p->sin_port = htons(port);

    return connect(&stServerAddr, sizeof(stServerAddr));
}

int INV_Socket::connectNoThrow(struct sockaddr* addr)
{
	assert(_iDomain == AF_INET);

	return connect(addr, sizeof(struct sockaddr));
}

void INV_Socket::connect(const string &sServerAddr, uint16_t port)
{
    int ret = connectNoThrow(sServerAddr, port);

    if(ret < 0)
    {
        throw INV_SocketConnect_Exception("[INV_Socket::connect] connect error", errno);
    }
}

void INV_Socket::connect(const char *sPathName)
{
    int ret = connectNoThrow(sPathName);
    if(ret < 0)
    {
        throw INV_SocketConnect_Exception("[INV_Socket::connect] connect error", errno);
    }
}

int INV_Socket::connectNoThrow(const char *sPathName)
{
    assert(_iDomain == AF_LOCAL);

    struct sockaddr_un stServerAddr;
    bzero(&stServerAddr, sizeof(struct sockaddr_un));
    stServerAddr.sun_family = _iDomain;
    strncpy(stServerAddr.sun_path, sPathName, sizeof(stServerAddr.sun_path));

    return connect((struct sockaddr *)&stServerAddr, sizeof(stServerAddr));
}

int INV_Socket::connect(struct sockaddr *pstServerAddr, socklen_t serverLen)
{
    return ::connect(_sock, pstServerAddr, serverLen);

}

void INV_Socket::listen(int iConnBackLog)
{
    if (::listen(_sock, iConnBackLog) < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::listen] listen error", errno);
    }
}

int INV_Socket::recv(void *pvBuf, size_t iLen, int iFlag)
{

    return ::recv(_sock, pvBuf, iLen, iFlag);
}

int INV_Socket::send(const void *pvBuf, size_t iLen, int iFlag)
{
    uint32_t sent = 0;

    while (sent < iLen) {
        uint32_t b = ::send(_sock, (char *)pvBuf + sent, iLen - sent, iFlag);
        if (b <= 0) {
            return b;
        }
        sent += b;
    }

    return sent;
    //return ::send(_sock, pvBuf, iLen, iFlag);
}

int INV_Socket::recvfrom(void *pvBuf, size_t iLen, string &sFromAddr, uint16_t &iFromPort, int iFlags)
{
    struct sockaddr stFromAddr;
    socklen_t iFromLen = sizeof(struct sockaddr);
    struct sockaddr_in *p = (struct sockaddr_in *)&stFromAddr;

    bzero(&stFromAddr, sizeof(struct sockaddr));

    int iBytes = recvfrom(pvBuf, iLen, &stFromAddr, iFromLen, iFlags);
    if (iBytes >= 0)
    {
        char sAddr[INET_ADDRSTRLEN] = "\0";

        inet_ntop(_iDomain, &p->sin_addr, sAddr, sizeof(sAddr));

        sFromAddr = sAddr;
        iFromPort = ntohs(p->sin_port);
    }

    return iBytes;
}

int INV_Socket::recvfrom(void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags)
{
    return ::recvfrom(_sock, pvBuf, iLen, iFlags, pstFromAddr, &iFromLen);
}

int INV_Socket::sendto(const void *pvBuf, size_t iLen, const string &sToAddr, uint16_t port, int iFlags)
{
    struct sockaddr stToAddr;
    struct sockaddr_in *p = (struct sockaddr_in *)&stToAddr;

    bzero(&stToAddr, sizeof(struct sockaddr));

    p->sin_family = _iDomain;

    if (sToAddr == "")
    {
        p->sin_addr.s_addr = htonl(INADDR_BROADCAST);
    }
    else
    {
        parseAddr(sToAddr, p->sin_addr);
    }

    p->sin_port = htons(port);

    return sendto(pvBuf, iLen, &stToAddr, sizeof(stToAddr), iFlags);
}

int INV_Socket::sendto(const void *pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlags)
{
    return ::sendto(_sock, pvBuf, iLen, iFlags, pstToAddr, iToLen);
}

void INV_Socket::shutdown(int iHow)
{
    if (::shutdown(_sock, iHow) < 0)
    {
        throw INV_Socket_Exception("[INV_Socket::shutdown] shutdown error", errno);
    }
}

void INV_Socket::setblock(bool bBlock)
{
    assert(_sock != INVALID_SOCKET);

    setblock(_sock, bBlock);
}

int INV_Socket::setSockOpt(int opt, const void *pvOptVal, socklen_t optLen, int level)
{
    return setsockopt(_sock, level, opt, pvOptVal, optLen);
}

int INV_Socket::getSockOpt(int opt, void *pvOptVal, socklen_t &optLen, int level)
{
    return getsockopt(_sock, level, opt, pvOptVal, &optLen);
}

void INV_Socket::setNoCloseWait()
{
	linger stLinger;
	stLinger.l_onoff = 1;  //在close socket调用后, 但是还有数据没发送完毕的时候容许逗留
	stLinger.l_linger = 0; //容许逗留的时间为0秒

    if(setSockOpt(SO_LINGER, (const void *)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setNoCloseWait] error", errno);
    }
}

void INV_Socket::setCloseWait(int delay)
{
	linger stLinger;
	stLinger.l_onoff = 1;  //在close socket调用后, 但是还有数据没发送完毕的时候容许逗留
	stLinger.l_linger = delay; //容许逗留的时间为delay秒

    if(setSockOpt(SO_LINGER, (const void *)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setCloseWait] error", errno);
    }
}

void INV_Socket::setCloseWaitDefault()
{
	linger stLinger;
	stLinger.l_onoff  = 0;
	stLinger.l_linger = 0;

    if(setSockOpt(SO_LINGER, (const void *)&stLinger, sizeof(linger), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setCloseWaitDefault] error", errno);
    }
}

void INV_Socket::setTcpNoDelay()
{
    int flag = 1;

    if(setSockOpt(TCP_NODELAY, (char*)&flag, int(sizeof(int)), IPPROTO_TCP) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setTcpNoDelay] error", errno);
    }
}

void INV_Socket::setKeepAlive()
{
    int flag = 1;
    if(setSockOpt(SO_KEEPALIVE, (char*)&flag, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setKeepAlive] error", errno);
    }
}

void INV_Socket::setSendBufferSize(int sz)
{
    if(setSockOpt(SO_SNDBUF, (char*)&sz, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setSendBufferSize] error", errno);
    }
}

int INV_Socket::getSendBufferSize()
{
    int sz;
    socklen_t len = sizeof(sz);
    if(getSockOpt(SO_SNDBUF, (void*)&sz, len, SOL_SOCKET) == -1 || len != sizeof(sz))
    {
        throw INV_Socket_Exception("[INV_Socket::getSendBufferSize] error", errno);
    }

    return sz;
}

void INV_Socket::setRecvBufferSize(int sz)
{
    if(setSockOpt(SO_RCVBUF, (char*)&sz, int(sizeof(int)), SOL_SOCKET) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setRecvBufferSize] error", errno);
    }
}

int INV_Socket::getRecvBufferSize()
{
    int sz;
    socklen_t len = sizeof(sz);
    if(getSockOpt(SO_RCVBUF, (void*)&sz, len, SOL_SOCKET) == -1 || len != sizeof(sz))
    {
        throw INV_Socket_Exception("[INV_Socket::getRecvBufferSize] error", errno);
    }

    return sz;
}

void INV_Socket::setblock(int fd, bool bBlock)
{
    int val = 0;

    if ((val = fcntl(fd, F_GETFL, 0)) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setblock] fcntl [F_GETFL] error", errno);
    }

    if(!bBlock)
    {
        val |= O_NONBLOCK;
    }
    else
    {
        val &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, val) == -1)
    {
        throw INV_Socket_Exception("[INV_Socket::setblock] fcntl [F_SETFL] error", errno);
    }
}

void INV_Socket::createPipe(int fds[2], bool bBlock)
{
    if(::pipe(fds) != 0)
    {
        throw INV_Socket_Exception("[INV_Socket::createPipe] error", errno);
    }

    try
    {
        setblock(fds[0], bBlock);
        setblock(fds[1], bBlock);
    }
    catch(...)
    {
        ::close(fds[0]);
        ::close(fds[1]);
        throw;
    }
}

vector<string> INV_Socket::getLocalHosts()
{
    vector<string> result;

    INV_Socket ts;
    ts.createSocket(SOCK_STREAM, AF_INET);

    int cmd = SIOCGIFCONF;

    struct ifconf ifc;

    int numaddrs = 10;

    int old_ifc_len = 0;

    while(true)
    {
        int bufsize = numaddrs * static_cast<int>(sizeof(struct ifreq));
        ifc.ifc_len = bufsize;
        ifc.ifc_buf = (char*)malloc(bufsize);
        int rs = ioctl(ts.getfd(), cmd, &ifc);

        if(rs == -1)
        {
            free(ifc.ifc_buf);
            throw INV_Socket_Exception("[INV_Socket::getLocalHosts] ioctl error", errno);
        }
        else if(ifc.ifc_len == old_ifc_len)
        {
            break;
        }
        else
        {
            old_ifc_len = ifc.ifc_len;
        }
    
        numaddrs += 10;
        free(ifc.ifc_buf);
    }

    numaddrs = ifc.ifc_len / static_cast<int>(sizeof(struct ifreq));
    struct ifreq* ifr = ifc.ifc_req;
    for(int i = 0; i < numaddrs; ++i)
    {
        if(ifr[i].ifr_addr.sa_family == AF_INET)
        {
            struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(&ifr[i].ifr_addr);
    	    if(addr->sin_addr.s_addr != 0)
    	    {
                char sAddr[INET_ADDRSTRLEN] = "\0";
                inet_ntop(AF_INET, &(*addr).sin_addr, sAddr, sizeof(sAddr));
    	        result.push_back(sAddr);
    	    }
        }
    }

    free(ifc.ifc_buf);

    return result;
}


}
