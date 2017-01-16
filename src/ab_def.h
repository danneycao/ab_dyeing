#pragma once

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>  
#include <algorithm>
#include <chrono>

#include "log/inv_log.h"
#include "util/inv_logger.h"
#include "util/inv_thread_rwlock.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(T) \
  T(const T&); \
  void operator=(const T&)
#endif

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

namespace inv {

struct kafkaFreshQueue
{
	std::unordered_map<std::string, uint64_t> kq;
    INV_ThreadRWLocker lock;
};
	
std::chrono::time_point<std::chrono::high_resolution_clock> TimeStampMs();
class ProcessCostTimeMonitor {
    public:
        ProcessCostTimeMonitor(const std::string& process_name)
            : time_begin_(TimeStampMs()),
              process_name_(process_name) {

                  TLOGINFO(__FILE__<<"-"<<__LINE__ << ": Enter: "
                           << process_name << std::endl);
            }

        ~ProcessCostTimeMonitor() {
             TLOGINFO(__FILE__<<"-"<<__LINE__<< ": Leave: " << process_name_
                << " TIME_COST = " 
                << std::chrono::duration_cast<std::chrono::microseconds>(TimeStampMs() - time_begin_).count() 
                << "us" << std::endl);
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(ProcessCostTimeMonitor);

        std::chrono::time_point<std::chrono::high_resolution_clock> time_begin_;
        std::string process_name_;
};

}
