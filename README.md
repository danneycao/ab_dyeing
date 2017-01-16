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
