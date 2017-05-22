//
// Created by wp on 5/22/17.
//

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <signal.h>
#include "glog/logging.h"

using namespace std;

#define MAX_EVENTS 10

int SetNoBlock ( int fd )
{
    int flags = fcntl ( fd, F_GETFL, 0 );
    fcntl ( fd, F_SETFL, flags | O_NONBLOCK );
    return 0;
}

void updateEvents ( int efd, int fd, int events, int op )
{
    struct epoll_event ev;
    memset ( &ev, 0, sizeof ( ev ));
    ev.events = events;
    ev.data.fd = fd;
    printf ( "%s fd %d events read %d write %d\n",
             op == EPOLL_CTL_MOD ? "mod" : "add", fd, ev.events & EPOLLIN, ev.events & EPOLLOUT );
    epoll_ctl ( efd, op, fd, &ev );
}

void HandleAccept ( int efd, int fd )
{

}

void HandleRead ( int efd, int fd )
{

}

void SendResponse ( int efd, int fd )
{

}

void HandleWrite ( int efd, int fd )
{

}

int main ( int argc, char **argv )
{
    google::InitGoogleLogging ( argv[ 0 ] );
    google::SetLogDestination ( google::GLOG_INFO, "../LOG/epoll/raw_epoll" );

    int port = 80;
    int epollfd = epoll_create ( 1 );
    int listenSock = socket ( AF_INET, SOCK_STREAM, 0 );

    struct sockaddr_in addr;
    memset ( &addr, 0, sizeof addr );
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( port );
    addr.sin_addr.s_addr = INADDR_ANY;

    bind ( listenSock, ( struct sockaddr * ) &addr, sizeof ( struct sockaddr ));
    listen ( listenSock, 1024 );
    SetNoBlock ( listenSock );
    updateEvents ( epollfd, listenSock, EPOLLIN, EPOLL_CTL_ADD );

    struct epoll_event ev, events[MAX_EVENTS];
    for ( ;; )
    {
        int nfds = epoll_wait ( epollfd, events, MAX_EVENTS, -1 );

        for ( int i = 0; i < nfds; i++ )
        {
            int fd = events[ i ].data.fd;
            int event = events[ i ].events;
            if ( event & ( EPOLLIN | EPOLLERR ))
            {
                if ( fd == listenSock )
                {
                    HandleAccept ( epollfd, fd );
                }
                else
                {
                    HandleRead ( epollfd, fd );
                }
            }
            else if ( event & EPOLLOUT )
            {
                HandleWrite ( epollfd, fd );
            }
            else
            {
            }
        }
    }

    return 0;
}
