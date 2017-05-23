//
// Created by wp on 5/23/17.
//

#ifndef GERM_CHANNEL_H
#define GERM_CHANNEL_H

namespace Germ
{
namespace Net
{

/*
 * 通道 对fd的封装
 */
typedef std::function<void ( )> EventCallback;

class Channel : noncopyable
{
public:
    void HandleRead ( );

    void HandleWrite ( );

    void SetReadCallBack ( const EventCallback &cb );

    void SetWriteCallBack ( const EventCallback &cb );

    void EnableRead ( );

    void DisableRead ( );

    void EnableWrite ( );

    void DisableWrite ( );

private:
    const int mFd;
    EventCallback mReadCallBack;
    EventCallback mWriteCallBack;
};

typedef std::vector <std::shared_ptr<Channel>> ChannelListPtr;

}
}

#endif //GERM_CHANNEL_H
