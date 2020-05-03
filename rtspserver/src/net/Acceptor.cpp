#include "net/Acceptor.h"
#include "net/SocketsOps.h"
#include "base/Logging.h"
#include "base/New.h"

Acceptor* Acceptor::createNew(UsageEnvironment* env, const Ipv4Address& addr)
{
#ifndef CUSTOM_NEW
    return new Acceptor(env, addr);
#else
    return New<Acceptor>::allocate(env, addr);
#endif
}

Acceptor::Acceptor(UsageEnvironment* env, const Ipv4Address& addr) :
    mEnv(env),
    mAddr(addr),
    mSocket(sockets::createTcpSock()),
    mNewConnectionCallback(NULL)
{
    mSocket.setReuseAddr(1);
    mSocket.bind(mAddr);
    mAcceptIOEvent = IOEvent::createNew(mSocket.fd(), this);
    mAcceptIOEvent->setReadCallback(readCallback);
    mAcceptIOEvent->enableReadHandling();
}

Acceptor::~Acceptor()
{
    if(mListenning)
        mEnv->scheduler()->removeIOEvent(mAcceptIOEvent);
#ifndef CUSTOM_NEW
    delete mAcceptIOEvent;
#else
    Delete::release(mAcceptIOEvent);
#endif
}

void Acceptor::listen()
{
    mListenning = true;
    mSocket.listen(1024);
    mEnv->scheduler()->addIOEvent(mAcceptIOEvent);
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb, void* arg)
{
    mNewConnectionCallback = cb;
    mArg = arg;
}

void Acceptor::readCallback(void* arg)
{
    Acceptor* acceptor = (Acceptor*)arg;
    acceptor->handleRead();
}

void Acceptor::handleRead()
{
    int connfd = mSocket.accept();
    LOG_DEBUG("client connect: %d\n", connfd);
    if(mNewConnectionCallback)
        mNewConnectionCallback(mArg, connfd);
}
