#include <stdio.h>
#include <cstdint>
#include <getopt.h>
#include <event.h>
#include <string>
#include <time.h>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/async/TEvhttpClientChannel.h>

#include "util/inv_util.h"
#include "util/inv_timeprovider.h"
#include "util/inv_config.h"
#include "../proto/gen-cpp/ABDyeingService.h"

typedef uint16_t uint16;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::async;

int main(int argc, char* argv[]) {
  inv::INV_Config config;
  config.parseFile(argv[1]);

  std::string server_ip = config.get("/main/[ip]");
  uint16 server_port = ::atoi(config.get("/main/[port]").data());
  std::cout << "ip: " << server_ip << ", port: " << server_port << std::endl;

  boost::shared_ptr<TSocket> sock(new TSocket(server_ip, server_port));
  boost::shared_ptr<TTransport> transport(new TFramedTransport(sock));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  inveno::ABDyeingServiceClient client(protocol);
  try {
    transport->open();
  } catch (std::exception& e) {
    std::cerr << "open socket error: " << e.what() << std::endl;
    return -1;
  }

  std::string uid = config.get("/main/[uid]");
  inveno::ABDyeingRequest req;
  req.app = config.get("/main/[app]");
  std::string type = config.get("/main/[type]");
  if (type == "article_type") {
    req._types.push_back("article_type");
    req._types.push_back("ad_type");
    req._types.push_back( "un_know");
  } else if (type == "ad_type") {
    req._types.push_back("article_type");
    req._types.push_back("ad_type");
    req._types.push_back( "un_know");
  } else {
    req._types.push_back("article_type");
    req._types.push_back("ad_type");
    req._types.push_back( "un_know");
  }
  inveno::ABDyeingReply reply;
  try {
    client.dyeing(reply, uid, req);
  } catch (std::exception& e) {
    std::cerr << "dyeing exception: " << e.what() << std::endl;
    return -1;
  }

  if (reply.status == 0) {
    std::cout << "item size: " << reply._entries.size() << std::endl;
    for (int i = 0; i < (int)reply._entries.size(); ++i) {
      const inveno::ABDyeingEntry& e = reply._entries[i];
      std::cout << "type: " << e.type << ", configid: " << e.config_id
                << std::endl;
    }
  } else {
    std::cerr << "reply error, status: " << reply.status << std::endl;
    return -1;
  }
  return 0;
}
