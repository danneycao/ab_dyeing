#ifndef __INV_SHM_H__
#define __INV_SHM_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "util/inv_ex.h"

namespace inv
{

struct INV_Shm_Exception : public INV_Exception
{
    INV_Shm_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_Shm_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_Shm_Exception() throw() {};
};

class INV_Shm
{
public:

    INV_Shm(bool bOwner = false) : _bOwner(bOwner),_shmSize(0),_shmKey(0),_bCreate(true), _pshm(NULL),_shemID(-1) {}

    INV_Shm(size_t iShmSize, key_t iKey, bool bOwner = false);

    ~INV_Shm();

    void init(size_t iShmSize, key_t iKey, bool bOwner = false);


    bool iscreate()     {return _bCreate;}


    void *getPointer() {return _pshm;}


    key_t getkey()  {return _shmKey;}


    int getid()     {return _shemID;}


    size_t size()   {return _shmSize;}


    int detach();

    int del();

protected:


    bool            _bOwner;


    size_t          _shmSize;


    key_t           _shmKey;


    bool            _bCreate;


    void            *_pshm;


    int             _shemID;

};

}

#endif
