#ifndef __INV_MEM_QUEUE_H__
#define __INV_MEM_QUEUE_H__

#include <string>
#include "util/inv_shm.h"
#include "util/inv_thread.h"

using namespace std;

namespace inv
{

class INV_MemQueue
{
public:

 
    INV_MemQueue();


    ~INV_MemQueue(){}


    void create(void *pAddr, size_t iSize);


    void connect(void *pAddr, size_t iSize);


    bool pop_front(string &sOut);


    bool push_back(const string &sIn);


    bool push_back(const char *pvIn, size_t iSize);


    bool isFull(size_t iSize);



    size_t getFreeSize();


    bool isEmpty();


    size_t elementCount();


    size_t queueSize();


    size_t memSize() const {return _size;};

protected:

    void update(void* iModifyAddr, size_t iModifyValue);


    void update(void* iModifyAddr, bool bModifyValue);


    void doUpdate(bool bUpdate = false);

protected:


    struct CONTROL_BLOCK
    {
        size_t iMemSize; 
        size_t iTopIndex;
        size_t iBotIndex;
        size_t iPushCount;
        size_t iPopCount;      
    }__attribute__((packed));


    struct tagModifyData
    {
        size_t  _iModifyAddr;
        char    _cBytes;
        size_t  _iModifyValue;
    }__attribute__((packed));


    struct tagModifyHead
    {
        char            _cModifyStatus;
        size_t          _iNowIndex;
        tagModifyData   _stModifyData[5];
    }__attribute__((packed));


    CONTROL_BLOCK   *_pctrlBlock;


    void            *_paddr;


    size_t          _size;

 
    tagModifyHead   *_pstModifyHead;


};

class INV_ShmQueue : public INV_ThreadLock
{
public:
    INV_ShmQueue(){}

    bool initialize();

    void push_back(const string &data);

    bool pop_front(string &data, size_t millsecond = 0);
        
public:
    void setShmKey(const key_t ShmKey) { _shmKey = ShmKey; }

    void setShmSize(const size_t ShmSize) { _shmSize = ShmSize; }

    key_t getShmKey() { return _shmKey; }

    size_t getShmSize() { return _shmSize; }

    size_t getQueueSize();
        
private:
    key_t _shmKey;
    size_t  _shmSize;
    INV_Shm _shm;
    INV_MemQueue _shmQueue;
};

}

#endif
