project (binlog-hunter)

set(CMAKE_CXX_FLAGS "-g3 -std=c++0x")

include_directories("/home/work/cmodule-order/mysql-replication-listener/include")
include_directories("/home/work/mysql-5.6.24/include")
include_directories("/home/work/cmodule-order/glog/glog/include")

link_directories("/home/work/cmodule-order/mysql-replication-listener/lib")
link_directories("/home/work/mysql-5.6.24/libmysql")
link_directories("/home/work/cmodule-order/glog/glog/lib")

ADD_EXECUTABLE(binlog-hunter binlog-hunter.cpp cache.cpp cache_key.cpp conf.cpp)
TARGET_LINK_LIBRARIES(binlog-hunter replication mysqlclient glog)
