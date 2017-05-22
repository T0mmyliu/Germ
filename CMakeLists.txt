cmake_minimum_required(VERSION 2.8)
project(Germ)

set(LINKER_LIBS "")

# glog
include("cmake/FindGlog.cmake")
#find_package(lib_glog REQUIRED)
include_directories(SYSTEM ${GLOG_INCLUDE_DIRS})
list(APPEND LINKER_LIBS ${GLOG_LIBRARIES})

# gflags
include("cmake/FindGFlags.cmake")
#find_package(GFlags REQUIRED)
include_directories(SYSTEM ${GFLAGS_INCLUDE_DIRS})
list(APPEND LINKER_LIBS ${GFLAGS_LIBRARIES})

add_executable(binary/epoll epoll.cpp)
target_link_libraries(binary/epoll ${LINKER_LIBS})
