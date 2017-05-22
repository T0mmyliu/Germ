//
// Created by wp on 5/22/17.
//


#include <iostream>
#include <glog/logging.h>

using namespace std;

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  google::SetLogDestination(google::GLOG_INFO,"./LOG/epoll/raw_epoll");

  LOG(INFO) << "Hello,GLOG!";

  return 0;
}
