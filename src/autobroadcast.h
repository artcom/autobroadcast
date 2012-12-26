#ifndef AUTOBROADCAST_H
#define AUTOBROADCAST_H

#include <sys/types.h>

ssize_t broadcast(unsigned short thePort, const void *theSendData, size_t theSendDataLength);

#endif /* AUTOBROADCAST_H */
