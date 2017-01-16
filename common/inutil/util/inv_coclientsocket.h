#ifndef _INV_COCLIENTSOCKET_H__
#define _INV_COCLIENTSOCKET_H__

#include "util/inv_socket.h"
#include <sstream>
#include <stdio.h>

namespace inv
{

class INV_CoClientSocket
{
public:

    INV_CoClientSocket() : _port(0),_timeout(3000), _fd(-1) {}

    virtual ~INV_CoClientSocket()
    {
        if (_fd != -1)
        {
            close(_fd);
            _fd = -1;
        }
    }

    INV_CoClientSocket(const string &sIp, int iPort, int iTimeout) { init(sIp, iPort, iTimeout); }

    void init(const string &sIp, int iPort, int iTimeout)
    {
        if (_fd != -1)
        {
            close(_fd);
            _fd = -1;
        }

        _ip         = sIp;
        _port       = iPort;
        _timeout    = iTimeout;
    }

    virtual int send(const char *sSendBuffer, size_t iSendLen) = 0;

    virtual int recv(char *sRecvBuffer, size_t &iRecvLen) = 0;

    enum
    {
        EM_SUCCESS  = 0,
        EM_SEND     = -1,
        EM_SELECT   = -2,
        EM_TIMEOUT  = -3,
        EM_RECV     = -4,
        EM_CLOSE    = -5,
        EM_CONNECT  = -6,
        EM_SOCKET   = -7
    };

    string getIp()
    {
        return _ip;
    }

    int getPort()
    {
        return _port;
    }

    int getTimeout()
    {
        return _timeout;
    }

protected:

    INV_Socket   _socket;

    string      _ip;

    int         _port;

    int         _timeout;

    int         _fd;
};

class INV_CoTCPClient : public INV_CoClientSocket
{
public:

    INV_CoTCPClient() : _connected(false), _connecting(false){}

    INV_CoTCPClient(const string &sIp, int iPort, int iTimeout) : INV_CoClientSocket(sIp, iPort, iTimeout)
    {
        _connected = false;
        _connecting = false;
    }

    int connect() { return checkSocket(); }

    int send(const char *sSendBuffer, size_t iSendLen);

    int recvn(char *sRecvBuffer, size_t &iRecvLen);

    int recv(char *sRecvBuffer, size_t &iRecvLen);

    void close();

    bool isValid()
    {
        return (_fd != -1);
    }

protected:

    int checkSocket();

    bool _connected;   
    bool _connecting;   //防止协程多次connect
};

}

#endif
