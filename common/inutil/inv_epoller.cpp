#include "util/inv_epoller.h"
#include <unistd.h>

namespace inv
{

INV_Epoller::INV_Epoller(bool bEt)
{
	_iEpollfd   = -1;
	_pevs       = NULL;
    _et         = bEt;
	_max_connections = 1024;
}

INV_Epoller::~INV_Epoller()
{
	if(_pevs != NULL)
	{
		delete[] _pevs;
		_pevs = NULL;
	}

	if(_iEpollfd > 0)
	{
		close(_iEpollfd);
	}
}

void INV_Epoller::ctrl(int fd, long long data, __uint32_t events, int op)
{
	struct epoll_event ev;
	ev.data.u64 = data;
    if(_et)
    {
        ev.events   = events | EPOLLET;
    }
    else
    {
        ev.events   = events;
    }

	epoll_ctl(_iEpollfd, op, fd, &ev);
}

void INV_Epoller::create(int max_connections)
{
	_max_connections = max_connections;

	_iEpollfd = epoll_create(_max_connections + 1);

	if(_pevs != NULL)
	{
		delete[] _pevs;
	}

	_pevs = new epoll_event[_max_connections + 1];
}

void INV_Epoller::add(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_ADD);
}

void INV_Epoller::mod(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_MOD);
}

void INV_Epoller::del(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_DEL);
}

int INV_Epoller::wait(int millsecond)
{
	return epoll_wait(_iEpollfd, _pevs, _max_connections + 1, millsecond);
}

}

