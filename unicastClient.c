#include "common.h"

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("Not enough arguments !\n");
        printf("arguments: <unicast ip> <unicast bridge port> <unicast client port>\n");

        return 1;
    }

    char* unicastIp = argv[1];
    int unicastBridgePort = atoi(argv[2]);   
    int unicastClientPort = atoi(argv[3]);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        perror("socket");

        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(unicastIp);
    addr.sin_port = htons(unicastClientPort);

    if(bind(fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        close(fd);

        return -1;
    }

    struct sockaddr_in bridgeAddr;
    memset(&bridgeAddr, 0, sizeof(bridgeAddr));
    bridgeAddr.sin_family = AF_INET;
    bridgeAddr.sin_addr.s_addr = inet_addr(unicastIp);
    bridgeAddr.sin_port = htons(unicastBridgePort);

    while(1)
    {
        const char *message = "Hello from unicaster !";
        int nbytes = sendto(fd, message, strlen(message), 0, (struct sockaddr*) &bridgeAddr, sizeof(bridgeAddr));

        if(nbytes < 0)
        {
            perror("sendto");

            return 1;
        }

        char buffer[BUFFER_SIZE];
        struct sockaddr_in unicastRemote;
        socklen_t addrLen = sizeof(unicastRemote);
        int len = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &unicastRemote, &addrLen);

        if (len < 0)
        {
            perror("recvfrom");

            return 1;
        }

        buffer[len] = '\0';
        puts(buffer);

        sleep(1);
    }

    return 0;
}
