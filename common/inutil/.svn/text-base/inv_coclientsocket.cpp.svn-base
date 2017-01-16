#include <cerrno>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "util/inv_coclientsocket.h"
#include "util/inv_epoller.h"
#include "util/inv_util.h"
#include "util/inv_timeprovider.h"

#include "co_routine.h"

namespace inv
{

void INV_CoTCPClient::close()
{
    _connected = false;
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
}

int INV_CoTCPClient::checkSocket()
{
    int ret = 0;
    
    if (_connecting)
    {
        return EM_CONNECT;
    }

    if(!_connected)
    {
        _connecting = true;
        if (_fd == -1)
        {
            if (_port == 0)
            {
                _fd = socket(AF_LOCAL, SOCK_STREAM, 0);
            }
            else
            {
                _fd = socket(AF_INET, SOCK_STREAM, 0);
            }

            if (_fd == -1)
            {
                _connecting = false;
                return EM_SOCKET;
            }
        }

        try 
        {
            INV_Socket::setblock(_fd, false);
        }
        catch(INV_Socket_Exception &ex)
        {
            ::close(_fd);
            _fd = -1;

            _connecting = false;
            return EM_SOCKET;
        }

        if (_port == 0)
        {
            struct sockaddr_un addr;
            bzero(&addr, sizeof(struct sockaddr_un));
            addr.sun_family = AF_LOCAL;
            strncpy(addr.sun_path, _ip.c_str(), sizeof(addr.sun_path));

            ret = ::connect(_fd, (struct sockaddr*)&addr, sizeof(addr));
        }
        else
        {
            struct sockaddr_in addr;
            bzero(&addr,sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(_port);
            int nIP = 0;
            const char* pszIP = _ip.c_str();
            if( !pszIP || '\0' == *pszIP
                    || 0 == strcmp(pszIP,"0") || 0 == strcmp(pszIP,"0.0.0.0")
                    || 0 == strcmp(pszIP,"*"))
            {
                nIP = htonl(INADDR_ANY);
            }
            else
            {
                nIP = inet_addr(pszIP);
            }
            addr.sin_addr.s_addr = nIP;

            ret = ::connect(_fd, (struct sockaddr*)&addr, sizeof(addr));
        }
        
        if (co_is_coroutine())
        {
            if (ret != 0)
            {
                if (errno != EALREADY && errno != EINPROGRESS)
                {
                    ::close(_fd);
                    _fd = -1;

                    _connecting = false;
                    return EM_CONNECT;
                }
                
                struct pollfd pf = { 0 };
                pf.fd = _fd; 
                pf.events = (POLLOUT|POLLERR|POLLHUP);
                co_poll(co_get_epoll_ct(), &pf, 1, _timeout);
                if (pf.revents & POLLTIMEOUT)
                {
                    ::close(_fd);
                    _fd = -1;

                    _connecting = false;
                    return EM_CONNECT;
                }

                //check connect
                int error = 0;
                uint32_t socklen = sizeof(error);
                errno = 0;
                ret = getsockopt(_fd, SOL_SOCKET, SO_ERROR,(void *)&error,  &socklen);
                if ( ret == -1 ) 
                {    
                    ::close(_fd);
                    _fd = -1; 

                    _connecting = false;
                    return EM_CONNECT;
                }    
                if (error) 
                {    
                    errno = error;
                    ::close(_fd);
                    _fd = -1; 

                    _connecting = false;
                    return EM_CONNECT;
                }
            }
        }
        else
        {
            if (ret != 0)
            {
                if(errno != EINPROGRESS)
                {
                    ::close(_fd);
                    _fd = -1;
                    _connecting = false;
                    return EM_CONNECT;
                }

                INV_Epoller epoller(false);
                epoller.create(1);
                epoller.add(_fd, 0, EPOLLOUT);
                int iRetCode = epoller.wait(_timeout);
                if (iRetCode < 0)
                {
                    ::close(_fd);
                    _fd = -1;
                    _connecting = false;
                    return EM_SELECT;
                }
                else if (iRetCode == 0)
                {
                    ::close(_fd);
                    _fd = -1;
                    _connecting = false;
                    return EM_CONNECT;
                }

                int error = 0;
                uint32_t socklen = sizeof(error);
                ret = getsockopt(_fd, SOL_SOCKET, SO_ERROR,(void *)&error,  &socklen);
                if ( ret == -1 ) 
                {    
                    ::close(_fd);
                    _fd = -1; 

                    _connecting = false;
                    return EM_CONNECT;
                }    
                if (error) 
                {    
                    errno = error;
                    ::close(_fd);
                    _fd = -1; 

                    _connecting = false;
                    return EM_CONNECT;
                }
            }
        }

        _connected = true;
    }

    _connecting = false;
    return EM_SUCCESS;
}

int INV_CoTCPClient::send(const char *sSendBuffer, size_t iSendLen)
{
    int ret = EM_SUCCESS;
    int iRet = checkSocket();
    if(iRet < 0)
    {
        return iRet;
    }
    
    if (!co_is_coroutine())
    {
        uint32_t sent = 0;
        while (sent < iSendLen) 
        {
            int32_t b = ::send(_fd, (char *)sSendBuffer + sent, iSendLen - sent, 0);
            if (b < 0 && (errno == EAGAIN || errno == EINTR))
            {
                INV_Epoller epoller(false);
                epoller.create(1);
                epoller.add(_fd, 0, EPOLLOUT);

                int iRetCode = epoller.wait(_timeout);
                if (iRetCode < 0)
                {
                    ret = EM_SELECT;
                    goto err;
                }
                else if (iRetCode == 0)
                {
                    ret = EM_TIMEOUT;
                    goto err;
                }
                continue;  
            }
            else if (b <= 0) {
                ret = EM_SEND;
                goto err;
            }
            sent += b;
        }
    }
    else
    {
        int64_t begin = TNOWMS;
        size_t sentLen = 0;
        while (sentLen < iSendLen)
        {
            int timeout = _timeout - (int)(TNOWMS - begin);
            struct pollfd pf = { 0 };
            pf.fd = _fd; 
            pf.events = (POLLOUT|POLLERR|POLLHUP);
            co_poll(co_get_epoll_ct(), &pf, 1, timeout > 0 ? timeout : 0);
            if (pf.revents & POLLERR || pf.revents & POLLHUP)
            {
                ret = EM_SELECT;
                goto err;
            }

            if (pf.revents & POLLTIMEOUT)
            {
                ret = EM_TIMEOUT;
                goto err;
            }

            int iLen = ::send(_fd, sSendBuffer + sentLen, iSendLen - sentLen, 0);
            if (iLen < 0)
            {
                if (errno == EAGAIN)
                {
                    continue;
                }
                ret = EM_SEND;
                goto err;
            }
            sentLen += iLen;
        }
    }
    
    return EM_SUCCESS;

err:
    _connected = false;
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
    return ret;
}

int INV_CoTCPClient::recvn(char *sRecvBuffer, size_t &iRecvLen)
{
    int ret = EM_SUCCESS;
    int iRet = checkSocket();
    if(iRet < 0)
    {
        return iRet;
    }
    
    if (!co_is_coroutine())
    {
        INV_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_fd, 0, EPOLLIN);

        uint32_t recved = 0;
        while (recved < iRecvLen) {
            int iRetCode = epoller.wait(_timeout);
            if (iRetCode < 0)
            {
                ret = EM_SELECT;
                goto err;
            }
            else if (iRetCode == 0)
            {
                ret = EM_TIMEOUT;
                goto err;
            }

            epoll_event ev  = epoller.get(0);
            if(ev.events & EPOLLIN)
            {
                int iLen = ::recv(_fd, (void*)(sRecvBuffer+recved), iRecvLen-recved, 0);
                if (iLen < 0)
                {
                    ret = EM_RECV;
                    goto err;
                }
                else if (iLen == 0)
                {
                    ret = EM_CLOSE;
                    goto err;
                }

                recved += iLen;
                continue;
            }

            ret = EM_SELECT;
            goto err;
        }
    }
    else
    {
        int64_t begin = TNOWMS;
        size_t recvLen = 0;
        while (recvLen < iRecvLen)
        {
            int timeout = _timeout - (int)(TNOWMS - begin);
            struct pollfd pf = { 0 };
            pf.fd = _fd; 
            pf.events = (POLLIN|POLLERR|POLLHUP);
            co_poll(co_get_epoll_ct(), &pf, 1, timeout > 0 ? timeout : 0);
            if (pf.revents & POLLERR || pf.revents & POLLHUP)
            {
                ret = EM_SELECT;
                goto err;
            }

            if (pf.revents & POLLTIMEOUT)
            {
                ret = EM_TIMEOUT;
                goto err;
            }

            int iLen = ::recv(_fd, sRecvBuffer + recvLen, iRecvLen - recvLen, 0);
            if (iLen < 0)
            {
                if (errno == EAGAIN)
                {
                    continue;
                }
                ret = EM_RECV;
                goto err;
            }
            else if (iLen == 0)
            {
                ret = EM_CLOSE;
                goto err;
            }
            recvLen += iLen;
        }
    }

    return EM_SUCCESS;

err:
    _connected = false;
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
    return ret;
}

int INV_CoTCPClient::recv(char *sRecvBuffer, size_t &iRecvLen)
{
    int ret = EM_SUCCESS;
    int iRet = checkSocket();
    if(iRet < 0)
    {
        return iRet;
    }
    
    if (!co_is_coroutine())
    {
        INV_Epoller epoller(false);
        epoller.create(1);
        epoller.add(_fd, 0, EPOLLIN);

        int iRetCode = epoller.wait(_timeout);
        if (iRetCode < 0)
        {
            ret = EM_SELECT;
            goto err;
        }
        else if (iRetCode == 0)
        {
            ret = EM_TIMEOUT;
            goto err;
        }

        epoll_event ev  = epoller.get(0);
        if(!(ev.events & EPOLLIN))
        {
            ret = EM_SELECT;
            goto err;
        }
        
        int iLen = ::recv(_fd, (void*)sRecvBuffer, iRecvLen, 0);
        if (iLen < 0)
        {
            ret = EM_RECV;
            goto err;
        }
        else if (iLen == 0)
        {
            ret = EM_CLOSE;
            goto err;
        }
        iRecvLen = iLen;
    }
    else
    {
        int iLen = 0;
        struct pollfd pf = { 0 };
        pf.fd = _fd; 
        pf.events = (POLLIN|POLLERR|POLLHUP);
        co_poll(co_get_epoll_ct(), &pf, 1, _timeout);
        if (pf.revents & POLLERR || pf.revents & POLLHUP)
        {
            ret = EM_SELECT;
            goto err;
        }

        if (pf.revents & POLLTIMEOUT)
        {
            ret = EM_TIMEOUT;
            goto err;
        }

        iLen = ::recv(_fd, sRecvBuffer, iRecvLen, 0);
        if (iLen < 0)
        {
            ret = EM_RECV;
            goto err;
        }
        else if (iLen == 0)
        {
            ret = EM_CLOSE;
            goto err;
        }
        iRecvLen = iLen;
    }

    return EM_SUCCESS;

err:
    _connected = false;
    if (_fd != -1)
    {
        ::close(_fd);
        _fd = -1;
    }
    return ret;
}

}
