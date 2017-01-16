#include "util/inv_shm.h"
#include <cassert>
#include <errno.h>

namespace inv
{

INV_Shm::INV_Shm(size_t iShmSize, key_t iKey, bool bOwner)
{
    init(iShmSize, iKey, bOwner);
}

INV_Shm::~INV_Shm()
{
    if(_bOwner)
    {
        detach();
    }
}

void INV_Shm::init(size_t iShmSize, key_t iKey, bool bOwner)
{
    assert(_pshm == NULL);

    _bOwner     = bOwner;

    if ((_shemID = shmget(iKey, iShmSize, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    {
        _bCreate = false;
        if ((_shemID = shmget(iKey, iShmSize, 0666)) < 0)
        {
            throw INV_Shm_Exception("[INV_Shm::init()] shmget error", errno);
        }
    }
    else
    {
        _bCreate    = true;
    }

    if ((_pshm = shmat(_shemID, NULL, 0)) == (char *) -1)
    {
        throw INV_Shm_Exception("[INV_Shm::init()] shmat error", errno);
    }

    _shmSize = iShmSize;
    _shmKey = iKey;
}

int INV_Shm::detach()
{
    int iRetCode = 0;
    if(_pshm != NULL)
    {
        iRetCode = shmdt(_pshm);

        _pshm = NULL;
    }

    return iRetCode;
}

int INV_Shm::del()
{
    int iRetCode = 0;
    if(_pshm != NULL)
    {
        iRetCode = shmctl(_shemID, IPC_RMID, 0);

        _pshm = NULL;
    }

    return iRetCode;
}

}
