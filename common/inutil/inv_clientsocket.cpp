#include <cerrno>
#include <iostream>
#include "util/inv_clientsocket.h"
#include "util/inv_epoller.h"
#include "util/inv_util.h"

namespace inv
{

void INV_TCPClient::close()
{
    _socket.close();
}

int INV_TCPClient::checkSocket()
{
    if(!_socket.isValid())
    {
        try
        {
            if(_port == 0)
            {
                _socket.createSocket(SOCK_STREAM, AF_LOCAL);
            }
            else
            {
                _socket.createSocket(SOCK_STREAM, AF_INET);

            }

            _socket.setblock(false);

            try
            {
                if(_port == 0)
                {
                    _socket.connect(_ip.c_str());
                }
                else
                {
                    _socket.connect(_ip, _port);
                }
            }
            catch(INV_SocketConnect_Exception &ex)
            {
                if(errno != EINPROGRESS)
                {
                    _socket.close();
                    return EM_CONNECT;
                }
            }

            if(errno != EINPROGRESS)
            {
                _socket.close();
                return EM_CONNECT;
            }

            INV_Epoller epoller(false);
            epoller.create(1);
            epoller.add(_socket.getfd(), 0, EPOLLOUT);
            int iRetCode = epoller.wait(_timeout);
            if (iRetCode < 0)
            {
                _socket.close();
                return EM_SELECT;
            }
            else if (iRetCode == 0)
            {
                _socket.close();
                return EM_TIMEOUT;
            }

            _socket.setblock(true);
        }
        catch(INV_Socket_Exception &ex)
        {
            _socket.close();
            return EM_SOCKET;
        }
    }
    return EM_SUCCESS;
}

int INV_TCPClient::send(const char *sSendBuffer, size_t iSendLen)
{
    int iRet = checkSocket();
    if(iRet < 0)
    {
        return iRet;
    }

    iRet = _socket.send(sSendBuffer, iSendLen);
    if(iRet < 0)
    {
        _socket.close();
        return EM_SEND;
    }

    return EM_SUCCESS;
}

int INV_TCPClient::recv(char *sRecvBuffer, size_t &iRecvLen)
{
    int iRet = checkSocket();
    if(iRet < 0)
    {
        return iRet;
    }

    INV_Epoller epoller(false);
    epoller.create(1);
    epoller.add(_socket.getfd(), 0, EPOLLIN);

    uint32_t recved = 0;
    while (recved < iRecvLen) {
        int iRetCode = epoller.wait(_timeout);
        if (iRetCode < 0)
        {
            _socket.close();
            return EM_SELECT;
        }
        else if (iRetCode == 0)
        {
            _socket.close();
            return EM_TIMEOUT;
        }

        epoll_event ev  = epoller.get(0);
        if(ev.events & EPOLLIN)
        {
            int iLen = _socket.recv((void*)(sRecvBuffer+recved), iRecvLen-recved);
            if (iLen < 0)
            {
                _socket.close();
                return EM_RECV;
            }
            else if (iLen == 0)
            {
                _socket.close();
                return EM_CLOSE;
            }

            recved += iLen;
            continue;
        }
        else
        {
            _socket.close();
        }

        return EM_SELECT;

        
    }

    return EM_SUCCESS;
}

}
