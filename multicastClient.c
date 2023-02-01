#include "common.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Not enough arguments !\n");
        printf("arguments: <multicast ip> <multicast port>\n");

        return 1;
    }

    struct Bridge bridge;
    initUdpMulticast(&bridge, argv[1], atoi(argv[2]), true);

    if(bridge.multicast < 0)
    {
        perror("initUdpMulticast");

        return 1;
    }

    while(1)
    {
        const char *message = "Hello from multicaster !";
        int nbytes = sendto(bridge.multicast, message, strlen(message), 0, (struct sockaddr*) &bridge.multicastAddr, sizeof(bridge.multicastAddr));

        if(nbytes < 0)
        {
            perror("sendto");

            return 1;
        }

        char buffer[BUFFER_SIZE];
        struct sockaddr_in multicastRemote;
        socklen_t addrLen = sizeof(multicastRemote);
        int len = recvfrom(bridge.multicast, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &multicastRemote, &addrLen);

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
