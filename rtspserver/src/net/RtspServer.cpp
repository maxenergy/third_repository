#include <algorithm>
#include <assert.h>
#include <stdio.h>

#include "net/RtspServer.h"
#include "base/New.h"

RtspServer* RtspServer::createNew(UsageEnvironment* env, Ipv4Address& addr)
{

#ifndef CUSTOM_NEW
    return new RtspServer(env, addr);
#else
    return New<RtspServer>::allocate(env, addr);
#endif
}

RtspServer::RtspServer(UsageEnvironment* env, const Ipv4Address& addr) :
    TcpServer(env, addr)
{
    mTriggerEvent = TriggerEvent::createNew(this);
    mTriggerEvent->setTriggerCallback(triggerCallback);

    mMutex = Mutex::createNew();
}

RtspServer::~RtspServer()
{
#ifndef CUSTOM_NEW
    delete mTriggerEvent;
    delete mMutex;
#else
    Delete::release(mTriggerEvent);
    Delete::release(mMutex);
#endif
}

void RtspServer::handleNewConnection(int connfd)
{
    RtspConnection* conn = RtspConnection::createNew(this, connfd);
    conn->setDisconnectionCallback(disconnectionCallback, this);
    mConnections.insert(std::make_pair(connfd, conn));
}

void RtspServer::disconnectionCallback(void* arg, int sockfd)
{
    RtspServer* rtspServer = (RtspServer*)arg;
    rtspServer->handleDisconnection(sockfd);
}

void RtspServer::handleDisconnection(int sockfd)
{
    MutexLockGuard mutexLockGuard(mMutex);
    mDisconnectionlist.push_back(sockfd);
    mEnv->scheduler()->addTriggerEvent(mTriggerEvent);
}

bool RtspServer::addMeidaSession(MediaSession* mediaSession)
{
    if(mMediaSessions.find(mediaSession->name()) != mMediaSessions.end())
        return false;

    mMediaSessions.insert(std::make_pair(mediaSession->name(), mediaSession));

    return true;
}

MediaSession* RtspServer::loopupMediaSession(std::string name)
{
    std::map<std::string, MediaSession*>::iterator it = mMediaSessions.find(name);
    if(it == mMediaSessions.end())
        return NULL;
    
    return it->second;
}

std::string RtspServer::getUrl(MediaSession* session)
{
    char url[200];

    snprintf(url, sizeof(url), "rtsp://%s:%d/%s", sockets::getLocalIp().c_str(),
                mAddr.getPort(), session->name().c_str());

    return std::string(url);
}

void RtspServer::triggerCallback(void* arg)
{
    RtspServer* rtspServer = (RtspServer*)arg;
    rtspServer->handleDisconnectionList();
}

void RtspServer::handleDisconnectionList()
{
    MutexLockGuard mutexLockGuard(mMutex);

    for(std::vector<int>::iterator it = mDisconnectionlist.begin(); it != mDisconnectionlist.end(); ++it)
    {
        int sockfd = *it;
        std::map<int, RtspConnection*>::iterator _it = mConnections.find(sockfd);
        assert(_it != mConnections.end());
		
#ifndef CUSTOM_NEW
        delete _it->second;
#else
        Delete::release(_it->second);
#endif
        mConnections.erase(sockfd);
    }

    mDisconnectionlist.clear();
}
