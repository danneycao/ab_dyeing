#ifndef	__INV_EPOLLER_H_
#define __INV_EPOLLER_H_

#include <sys/epoll.h>
#include <cassert>

namespace inv
{

class INV_Epoller
{
public:

	INV_Epoller(bool bEt = true);

	~INV_Epoller();

	void create(int max_connections);

	void add(int fd, long long data, __uint32_t event);

	void mod(int fd, long long data, __uint32_t event);

	void del(int fd, long long data, __uint32_t event);

	int wait(int millsecond);

	struct epoll_event& get(int i) { assert(_pevs != 0); return _pevs[i]; }

protected:

	void ctrl(int fd, long long data, __uint32_t events, int op);

protected:
  
    int _iEpollfd;

	int	_max_connections;

	struct epoll_event *_pevs;


    bool _et;
};

}
#endif

