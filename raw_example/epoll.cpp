//
// Created by wp on 5/22/17.
//


#include <iostream>
#include <sys/socket.h>
#include <glog/logging.h>

using namespace std;

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::GLOG_INFO,"../LOG/epoll/raw_epoll");







  return 0;
}
