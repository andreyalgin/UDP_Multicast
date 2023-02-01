#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 256

struct Bridge
{
    int multicast;
    int unicast;
    struct sockaddr_in multicastAddr;
    struct sockaddr_in unicastAddr;
    pthread_t multicastThread;
    pthread_t unicastThread;
    void *multicastRet;
    void *unicastRet;
};

void initUdpMulticast(struct Bridge *bridge, char *ip, int port, bool loopback)
{
    bridge->multicast = -1;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket");

        return;
    }

    unsigned int optVal = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &optVal, sizeof(optVal)) < 0)
    {
        perror("setsockopt");
        close(fd);

        return;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(fd);

        return;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(ip);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        close(fd);

        return;
    }

    if(loopback == false)
    {
        char loopCh = 0;
        if(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopCh, sizeof(loopCh)) < 0)
        {
            perror("setsockopt");
            close(fd);

            return;
        }
    }

    addr.sin_addr.s_addr = inet_addr(ip);

    bridge->multicast = fd;
    bridge->multicastAddr = addr;
}
