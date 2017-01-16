# ab_dyeing
as a dyeing server

ab_dyeing为基于thrift协议构建的thrift_server主要用于
1：基于灰度发布的ab_test染色;
2：基于bucket可配置的比例分配；
3：基于mysql的快照读取；
4：基于kafka的流水更新；
5：基于shared_ptr的无锁设计;
6：thrift_work线程绑定类实体，实现work单线程，进行业务逻辑处理，避免不必要的锁竞争。

client采用py_unit进行编写，编译进行单元测试。

协议如下rpc如下：
service ABDyeingService {
	ABDyeingReply dyeing(1:string uid, 2:ABDyeingRequest req),
	
    ABDyeingReply DyeingLan(1:string uid, 2:ABDyeingLanRequest req),

    ABDyeingBatchReply BatchDyeing(1:ABDyeingBatchRequest batch_reqs),
    
    ABDyeingBatchReply BatchDyeingLan(1:ABDyeingBatchLanRequest batch_reqs),
}

client py_unit如下：
#!/usr/bin/python
# encoding: utf-8

import sys, glob
sys.path.append('gen-py')

import unittest

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.protocol import TCompactProtocol
from thrift.transport.TTransport import TMemoryBuffer

from thrift import TSerialization
from thrift.TSerialization import deserialize
from thrift.TSerialization import serialize

from ab_dyeing import *
from ab_dyeing.ttypes import *

def SerializeThriftMsg(msg, protocol_type=TBinaryProtocol.TBinaryProtocol):
  msg.validate()
  transportOut = TTransport.TMemoryBuffer()
  protocolOut = protocol_type(transportOut)
  msg.write(protocolOut)
  return transportOut.getvalue()

def DeserializeThriftMsg(msg, data, protocol_type=TBinaryProtocol.TBinaryProtocol):
  transportIn = TTransport.TMemoryBuffer(data)
  protocolIn = protocol_type(transportIn)
  msg.read(protocolIn)
  msg.validate()
  return msg

class TestAbdyeing(unittest.TestCase):

    def setUp(self):
        mysocket = TSocket.TSocket('localhost', 8893)
        #mysocket = TSocket.TSocket('106.75.9.6', 8893)
        transport = TTransport.TFramedTransport(mysocket)
        protocol = TBinaryProtocol.TBinaryProtocol(transport)

        self.client = ABDyeingService.Client(protocol)
        self.transport = transport
        self.transport.open()

    def tearDown(self):
       self.transport.close()

    def test_AbdyeingDyeingLan(self):
        ab_dy_req= ABDyeingLanRequest()
        ab_dy_req.app = "hotoday"
        ab_dy_req.app_ver = "2.1.3"
        ab_dy_req.app_lan = "english"
        types = ["ad", "news","dy_article", "un_know"]
        ab_dy_req._types= types
        print('ab_dy_lan_req(%s) \n' % ab_dy_req)

        uid="01011606301403364801000000104800"
        ab_dy_resp = self.client.DyeingLan(uid, ab_dy_req)
        print ('uid(%r) \n' % uid)
        print('ab_dy_lan_resp(%r) \n' % ab_dy_resp)

    def test_AbdyeingDyeing(self):
        ab_dy_req= ABDyeingRequest()
        ab_dy_req.app = "coolpad"
        ab_dy_req.app_ver = ""
        types = ["ad", "news","dy_article", "un_know"]
        ab_dy_req._types= types
        print('ab_dy_req(%s) \n' % ab_dy_req)

        uid="01011606301403364801000000104800"
        ab_dy_resp = self.client.dyeing(uid, ab_dy_req)
        print ('uid(%r) \n' % uid)
        print('ab_dy_resp(%r) \n' % ab_dy_resp)

    def test_AbdyeingBatchDyeing(self):
        ab_dy_req= ABDyeingRequest()
        ab_dy_req.app = "emui"
        ab_dy_req.app_ver = "1.1.1"
        types = ["ad", "news","dy_article", "un_know"]
        ab_dy_req._types= types

        ab_batch_dy_reqs = ABDyeingBatchRequest()

        uid_reqs = []
        count = 0
        while (count < 10):
            uid_req_info = ABDyeingUidReqInfo()
            uid_req_info.uid = "112233" + str(count*10)
            uid_req_info._uid_info = ab_dy_req
            uid_reqs.append(uid_req_info)
            count +=1

        ab_batch_dy_reqs._uid_req_infos = uid_reqs
        print('ab_batch_dy_reqs(%s) \n' % ab_batch_dy_reqs)
        ab_batch_resp = self.client.BatchDyeing(ab_batch_dy_reqs)
        print('ab_batch_resp(%r) \n' % ab_batch_resp)


    def test_AbdyeingBatchDyeingLan(self):
        ab_dy_req= ABDyeingLanRequest()
        ab_dy_req.app = "hotoday"
        ab_dy_req.app_ver = "2.1.3"
        ab_dy_req.app_lan = "English"
        types = ["ad", "news", "biz", "dy_article"]
        ab_dy_req._types= types
        print('ab_dy_lan_req(%s) \n' % ab_dy_req)

        ab_batch_dy_reqs = ABDyeingBatchLanRequest()

        uid_reqs = []
        count = 0
        while (count < 10):
            uid_req_info = ABDyeingUidLanReqInfo()
            uid_req_info.uid = "112233" + str(count*10)
            uid_req_info._uid_lan_info = ab_dy_req
            uid_reqs.append(uid_req_info)
            count +=1

        ab_batch_dy_reqs._uid_lan_req_infos = uid_reqs
        print('ab_batch_dy_lan_reqs(%s) \n' % ab_batch_dy_reqs)
        ab_batch_resp = self.client.BatchDyeingLan(ab_batch_dy_reqs)
        print('ab_batch_lan_resp(%r) \n' % ab_batch_resp)

if __name__ =='__main__':
    unittest.main()
