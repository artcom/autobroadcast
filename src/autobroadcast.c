#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include "autobroadcast.h"

ssize_t broadcast(unsigned short port, const void *buf, size_t len) {
    struct sockaddr_in si_other;
    struct ifaddrs *ifaddr, *ifa;
    int s, i, slen=sizeof(si_other);
    int  on = 1;
    int family;
    char host[NI_MAXHOST];

    /* send UDP DGRAM */ 
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    /* iterate over all interfaces */
    if (getifaddrs(&ifaddr) == -1) {
        return -1;
    } 
    /* Walk through linked list, maintaining head pointer so we can free list later */

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        if ((ifa->ifa_flags & IFF_BROADCAST) == 0)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            memcpy(&si_other, ifa->ifa_addr, sizeof(si_other));
            si_other.sin_port = htons(port);

            if (sendto(s, buf, len, 0, (struct sockaddr *) &si_other, slen)==-1) {
                return -1;
            }
        }
    }

    freeifaddrs(ifaddr);  

    close(s);
    return 0;
}

