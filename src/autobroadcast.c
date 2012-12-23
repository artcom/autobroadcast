#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "autobroadcast.h"

#define BUFLEN 512
#define SRV_IP "192.168.90.62"

ssize_t broadcast(unsigned short port, const void *buf, size_t len) {
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return -1;
    }
    
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

