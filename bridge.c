#include "common.h"

void initUdpUnicast(struct Bridge *bridge, char *ip, int bridgePort, int clientPort)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        perror("socket");

        return;
    }

    struct sockaddr_in bridgeAddr;
    memset(&bridgeAddr, 0, sizeof(bridgeAddr));
    bridgeAddr.sin_family = AF_INET;
    bridgeAddr.sin_addr.s_addr = inet_addr(ip);
    bridgeAddr.sin_port = htons(bridgePort);

    if(bind(fd, (struct sockaddr*) &bridgeAddr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        close(fd);

        return;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(clientPort);

    bridge->unicast = fd;
    bridge->unicastAddr = addr;
}

void *multicastHandler(void *arg)
{
    while (1)
    {
        struct Bridge *bridge = (struct Bridge*)arg;
        char buffer[BUFFER_SIZE];
        struct sockaddr_in multicastRemote;
        socklen_t addrLen = sizeof(multicastRemote);
        int len = recvfrom(bridge->multicast, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &multicastRemote, &addrLen);

        if (len < 0)
        {
            perror("recvfrom");

            return NULL;
        }

        buffer[len] = '\0';
        puts(buffer);

        len = sendto(bridge->unicast, buffer, len, 0, (struct sockaddr *) &bridge->unicastAddr, sizeof(bridge->unicastAddr));

        if(len < 0)
        {
            perror("sendto");

            return NULL;
        }
    }

    return NULL;
}

void *unicastHandler(void *arg)
{
    while (1)
    {
        struct Bridge *bridge = (struct Bridge*)arg;
        char buffer[BUFFER_SIZE];
        struct sockaddr_in unicastRemote;
        socklen_t addrLen = sizeof(unicastRemote);
        int len = recvfrom(bridge->unicast, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &unicastRemote, &addrLen);

        if (len < 0)
        {
            perror("recvfrom");

            return NULL;
        }

        buffer[len] = '\0';
        puts(buffer);

        len = sendto(bridge->multicast, buffer, len, 0, (struct sockaddr *) &bridge->multicastAddr, sizeof(bridge->multicastAddr));

        if(len < 0)
        {
            perror("sendto");

            return NULL;
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 6)
    {
        printf("Not enough arguments !\n");
        printf("arguments: <multicast ip> <multicast port> <unicast ip> <unicast bridge port> <unicast client port>\n");

        return 1;
    }

    struct Bridge bridge;

    initUdpMulticast(&bridge, argv[1], atoi(argv[2]), true);

    if(bridge.multicast < 0)
    {
        perror("initUdpMulticast");

        return 1;
    }

    initUdpUnicast(&bridge, argv[3], atoi(argv[4]), atoi(argv[5]));

    if(bridge.unicast < 0)
    {
        perror("initUdpUnicast");

        return 1;
    }

    pthread_create(&bridge.multicastThread, NULL, multicastHandler, (void*)&bridge);
    pthread_create(&bridge.unicastThread, NULL, unicastHandler, (void*)&bridge);

    pthread_join(bridge.multicastThread, &bridge.multicastRet);
    pthread_join(bridge.unicastThread, &bridge.unicastRet);

    return 0;
}
