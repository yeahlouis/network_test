#include "louis_util.h"

#define SOCK_ERR -1

static ssize_t read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];



static ssize_t
my_read(int fd, char *ptr)
{
  if (read_cnt <= 0) {
  again:
	if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
	  if (errno == EINTR)
		goto again;
	  return (-1);
	} else if (0 == read_cnt){
	  return (0);
	}
	read_ptr = read_buf;
  } /*   if (read_cnt <= 0) */

  read_cnt--;
  *ptr = *read_ptr++;
  return (1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char c, *ptr;
  
  ptr = vptr;
  for (n = 1; n < maxlen; n++) {
	if ( (rc = my_read(fd, &c)) == 1) {
	  *ptr++ = c;
	  if (c == '\n')
		break;         /* newline is stored, like fgets() */
	} else if (rc == 0) {
	  *ptr = 0;
	  return (n - 1); /* EOF, n - 1 bytes were read */
	} else {
	  return (-1); /* error, errno set by read() */
	}
  } /* end of for */

  *ptr = 0; /* null terminate like fgets() */
  return (n);
}


ssize_t 
readlinebuf(void **vptrptr)
{
  if (read_cnt)
	*vptrptr = read_ptr;
  return (read_cnt);
}



ssize_t         /* read n bytes from a descriptor */
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



ssize_t      /* write n byte to a descriptor */
writen(int fd, const void *vptr,size_t n)
{
  size_t  nleft;
  ssize_t nwritten;
  const char *ptr;
 
  ptr = vptr;
  nleft = n;

  while (nleft > 0){
	if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
	  if (nwritten < 0 && errno == EINTR) 
		nwritten = 0; /* and call write() again */
	  else
		return (-1); /* error */
	}
	nleft -= nwritten;
	ptr += nwritten;
  } /* end of while (nleft > 0) */
  return (n);
}


ssize_t
own_listen(int fd, int backlog)
{
  ssize_t ret = 0;
  char *ptr;
  
  /* 4can override 2nd argument with environment variable */
  if ( (ptr = getenv("LISTENQ")) != NULL)
	backlog = atoi(ptr);

  ret = listen(fd, backlog);
  return ret;
}


int
str_echo(int sockfd)
{
  ssize_t n;
  char buf[MAXLINE];

 again:
  while ( (n = read(sockfd, buf, MAXLINE)) > 0) {
	if (writen(sockfd, buf, n) < 0) {
	  return -1;
	}
  } /* end of while */

  if (n < 0 && errno == EINTR)
	goto again;
  else 
	return (-1);

}


int 
str_cli(FILE *fp, int sockfd) 
{
  char sendline[MAXLINE], recvline[MAXLINE];

  while (fgets(sendline, MAXLINE, fp) != NULL) {
	if (writen(sockfd, sendline, strlen(sendline)) < 0) {
	  break;
	}
	if (readline(sockfd, recvline, MAXLINE) == 0) {
	  return -1;
	}
	fputs(recvline, stdout);
  } /* end of while loop */

  return 0;
}


int tcp_socket_init(int *host_sockfd, char *host_ip, char *host_port)   
{
  int ret = 0;   
  int sockfd;   


  struct sockaddr_in servaddr;   

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {   
	fprintf(stderr, "socket error!\n");   
	return SOCK_ERR;   
  }


  memset(&servaddr, 0, sizeof(servaddr));   
  servaddr.sin_family = AF_INET;   
  servaddr.sin_port = htons(atoi(host_port));   
  if (inet_pton(AF_INET, host_ip, &servaddr.sin_addr) <= 0) {   
    fprintf(stderr, "inet_pton error!\n");   
    close(sockfd);   
    return SOCK_ERR;   
  }

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {   
    fprintf(stderr, "connect error:%m\n");  
    close(sockfd);
    return SOCK_ERR;
  }

  *host_sockfd = sockfd;
  return ret;
}







