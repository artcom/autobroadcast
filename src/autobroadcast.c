#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>
#include "autobroadcast.h"

#define SRV_IP "192.168.90.255"

ssize_t broadcast(unsigned short port, const void *buf, size_t len) {
    struct sockaddr_in si_other;
    struct ifaddrs *ifaddr, *ifa;
    int s, i, slen=sizeof(si_other);
    int  on = 1;
    int family;
    char host[NI_MAXHOST];

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
            s = getnameinfo(ifa->ifa_ifu.ifu_broadaddr, sizeof(struct sockaddr_in),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("\taddress: <%s>\n", host);
        }
    }

    freeifaddrs(ifaddr);  

    /* send UDP DGRAM */ 
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
      return -1;
    }
    
    if (sendto(s, buf, len, 0, &si_other, slen)==-1) {
        return -1;
    }
    
    close(s);
    return 0;
}

