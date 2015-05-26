project (binlog-hunter)

find_path(Replication_INCLUDE_DIR binlog_api.h)

ADD_EXECUTABLE(binlog-hunter binlog-hunter.cpp cache.cpp cache_key.cpp conf.cpp) 
TARGET_LINK_LIBRARIES(binlog-hunter replication_static mysqlclient memcached glog)

