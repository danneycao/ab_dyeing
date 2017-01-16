#ifndef _INV_CLIENTSOCKET_H__
#define _INV_CLIENTSOCKET_H__

#include "util/inv_socket.h"
#include <sstream>

namespace inv
{

class INV_ClientSocket
{
public:

    INV_ClientSocket() : _port(0),_timeout(3000) {}

    virtual ~INV_ClientSocket(){}

    INV_ClientSocket(const string &sIp, int iPort, int iTimeout) { init(sIp, iPort, iTimeout); }

    void init(const string &sIp, int iPort, int iTimeout)
    {
        _socket.close();
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
};

class INV_TCPClient : public INV_ClientSocket
{
public:

    INV_TCPClient(){}

    INV_TCPClient(const string &sIp, int iPort, int iTimeout) : INV_ClientSocket(sIp, iPort, iTimeout)
    {
    }

    int send(const char *sSendBuffer, size_t iSendLen);

    int recv(char *sRecvBuffer, size_t &iRecvLen);

    void close();

    bool isValid()
    {
        return _socket.isValid();
    }

protected:

    int checkSocket();
};

}

#endif
