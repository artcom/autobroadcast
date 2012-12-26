#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include "autobroadcast.h"

ssize_t broadcast(unsigned short thePort, const void *theSendData, size_t theSendDataLength) {
    struct sockaddr_in destinationEndpoint;
    struct ifaddrs *firstInterfaceAddress=0, *curInterfaceAddress;
    int udpSocket = 0;
    int on = 1;

    ssize_t returnValue = -1;
    do { /* we use a fake do/while loop as a finally construct */
        /* send UDP DGRAM */ 
        if ((udpSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            break;
        }
        setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

        /* iterate over all interfaces */
        if (getifaddrs(&firstInterfaceAddress) == -1) {
            break;
        } 
        /* Walk through linked list, maintaining head pointer so we can free list later */

        for (curInterfaceAddress = firstInterfaceAddress; curInterfaceAddress != NULL; curInterfaceAddress = curInterfaceAddress->ifa_next) {
            if (curInterfaceAddress->ifa_addr == NULL)
                continue;
            if ((curInterfaceAddress->ifa_flags & IFF_BROADCAST) == 0)
                continue;
            if (curInterfaceAddress->ifa_addr->sa_family == AF_INET) {
                memcpy(&destinationEndpoint, curInterfaceAddress->ifa_broadaddr, sizeof(destinationEndpoint));
                destinationEndpoint.sin_port = htons(thePort);
                returnValue = sendto(udpSocket, theSendData, theSendDataLength, 
                                         0, (struct sockaddr *) &destinationEndpoint, 
                                         sizeof(destinationEndpoint)); 
                if (returnValue == -1) { /* something when wrong */
                    break;
                }
            }
        }
    } while (0);

    /* finally - clean up */
    if (firstInterfaceAddress) {
        freeifaddrs(firstInterfaceAddress);
    }

    if (udpSocket) {
        close(udpSocket);
    }
    return returnValue;
}

