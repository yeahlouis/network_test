#include "louis_util.h"

#define IP 127.0.0.1

#if 0
static ssize_t         /* read n bytes from a descriptor */
readn(int fd, void *vptr, size_t n)
{ 
  size_t  nleft;
  ssize_t nread; 
  char *ptr;
    
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
	if ( (nread = read(fd, ptr, nleft)) < 0) {
	  if (errno == EINTR) nread = 0; /* and call read() again */
	  else return (-1);
	} else if (nread == 0) {
	  break;    /* EOF */
	}
	nleft -= nread;
	ptr += nread;
  } /* end of   while (nleft > 0) */
  return (n - nleft);  /* return >= 0 */
} 
#endif




int 
main(int argc, char **argv) 
{
  int sockfd;
  struct sockaddr_in servaddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
	  fprintf(stderr, "socket error!\n");
	  exit(-1);
  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;

  servaddr.sin_port = htons(SERV_PORT);



  if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
	fprintf(stderr, "inet_pton error!\n");
	close(sockfd);
	exit(-1);
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
	fprintf(stderr, "connect error:%m\n");
	close(sockfd);
	exit(-1);
  } else {
	fprintf(stdout, "connect success!\n");
  }
  
#if 0
  char buf[1024];
  int n;
  n = readn(sockfd, buf, sizeof(buf) - 1);
  buf[n] = '\0';
  printf("buf = %s\n", buf);
#else  
  str_cli(stdin, sockfd);
#endif
  exit(0);

}

