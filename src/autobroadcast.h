#ifndef AUTOBROADCAST_H
#define AUTOBROADCAST_H

#include <sys/types.h>

ssize_t broadcast(unsigned short port, const void *buf, size_t len);

#endif /* AUTOBROADCAST_H */
