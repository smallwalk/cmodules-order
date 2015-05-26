project(examples)

link_directories(${PROJECT_BUILD_DIR}/lib)
include_directories(${PROJECT_BUILD_DIR}/include)

# Create build rules for all the simple examples that only require a
# single file.

foreach(prog basic-1 basic-2 binlog-browser binlog-hunter)
  ADD_EXECUTABLE(${prog} ${prog}.cpp)
  TARGET_LINK_LIBRARIES(${prog} replication_static mysqlclient)
endforeach()

