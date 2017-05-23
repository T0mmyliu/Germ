//
// Created by wp on 5/23/17.
//

#ifndef GERM_POLLER_H
#define GERM_POLLER_H

#include "channel.hpp"

namespace Germ
{
namespace Net
{

/*
 * 底层epoll的封装
 */
class Poller : noncopyable
{
public:
    // TODO: 加注释
    int Poll (int const& timeoutMs, ChannelListPtr &activeChannels );

    // TODO: 加注释
    int addChannel ( const Channel &channel );

    // TODO: 加注释
    int removeChannel ( const Channel &channel );

    // TODO: 加注释
    int updateChannel ( const Channel &channel );
};

}
}

#endif //GERM_POLLER_H
