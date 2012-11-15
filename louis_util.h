#ifndef __LOUIS_UTIL_H
#define __LOUIS_UTIL_H



#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/wait.h>



#define LISTENQ 1024



#ifndef MAXLINE
#define MAXLINE 4096
#endif
#ifndef BUFFSIZE
#define BUFFSIZE 8192
#endif



#ifndef HAVE_BZERO
#define bzero(ptr,n) memset(ptr, 0x00, n)
#endif


typedef void Sigfunc(int);


#define SERV_PORT 9877
#define SERV_PORT_STR "9877"

#endif /* __LOUIS_UTIL_H */
