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
#include <iostream>

using namespace std;

#define MAX_EVENTS 1024
string httpRes =
    "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 1048576\r\n\r\n123456";
struct Con
{
    string readed;
    size_t written;
    bool writeEnabled;
    Con ( ) : written ( 0 ), writeEnabled ( false )
    { }
};
map<int, Con> cons;

int SetNonBlock ( int fd )
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
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof ( raddr );
    int cfd = accept ( fd, ( struct sockaddr * ) &raddr, &rsz );
    sockaddr_in peer, local;
    socklen_t alen = sizeof ( peer );
    int r = getpeername ( cfd, ( sockaddr * ) &peer, &alen );
    printf ( "accept a connection from %s\n", inet_ntoa ( raddr.sin_addr ));
    SetNonBlock ( cfd );
    updateEvents ( efd, cfd, EPOLLIN, EPOLL_CTL_ADD );
}

void SendResponse ( int efd, int fd )
{
    Con &con = cons[ fd ];
    if (!con.readed.length()) {
        if (con.writeEnabled) {
            updateEvents(efd, fd, EPOLLIN, EPOLL_CTL_MOD);
            con.writeEnabled = false;
        }
        return;
    }
    size_t left = httpRes.length ( ) - con.written;
    int wd = 0;
    while (( wd = ::write ( fd, httpRes.data ( ) + con.written, left )) > 0 )
    {
        con.written += wd;
        left -= wd;
        printf ( "write %d bytes left: %lu\n", wd, left );
    };
    if ( left == 0 )
    {
        close ( fd );
        cons.erase ( fd );
        return;
    }
    if ( wd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK ))
    {
        if ( !con.writeEnabled )
        {
            updateEvents ( efd, fd, EPOLLIN | EPOLLOUT, EPOLL_CTL_MOD );
            con.writeEnabled = true;
        }
        return;
    }
    if ( wd <= 0 )
    {
        printf ( "write error for %d: %d %s\n", fd, errno, strerror (errno));
        close ( fd );
        cons.erase ( fd );
    }
}

void HandleRead ( int efd, int fd )
{
    char buf[4096];
    int n = 0;
    while (( n = ::read ( fd, buf, sizeof buf )) > 0 )
    {
        printf ( "read %d bytes\n", n );
        string &readed = cons[ fd ].readed;
        readed.append ( buf, n );
        if ( readed.length ( ) > 4 )
        {
            if ( readed.substr ( readed.length ( ) - 2, 2 ) == "\n\n"
                || readed.substr ( readed.length ( ) - 4, 4 ) == "\r\n\r\n" )
            {
                //当读取到一个完整的http请求，测试发送响应
                SendResponse ( efd, fd );
            }
        }
    }
    if ( n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK ))
        return;
    //实际应用中，n<0应当检查各类错误，如EINTR
    if ( n < 0 )
    {
        printf ( "read %d error: %d %s\n", fd, errno, strerror (errno));
    }
    close ( fd );
    cons.erase ( fd );
}

void HandleWrite ( int efd, int fd )
{
    SendResponse ( efd, fd );
}

int main ( int argc, char **argv )
{
    int port = 50000;
    int epollfd = epoll_create ( 1 );
    int listenSock = socket ( AF_INET, SOCK_STREAM, 0 );

    struct sockaddr_in addr;
    memset ( &addr, 0, sizeof addr );
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( port );
    addr.sin_addr.s_addr = INADDR_ANY;

    bind ( listenSock, ( struct sockaddr * ) &addr, sizeof ( struct sockaddr ));
    listen ( listenSock, 1024 );
    SetNonBlock ( listenSock );
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
                    cout << "handle accept" << endl;
                    HandleAccept ( epollfd, fd );
                }
                else
                {
                    cout << "handle read" << endl;
                    HandleRead ( epollfd, fd );
                }
            }
            else if ( event & EPOLLOUT )
            {
                cout << "handle write" << endl;
                HandleWrite ( epollfd, fd );
            }
            else
            {
                // TODO: HANDLE ERROR
            }
        }
    }

    return 0;
}
