#include "louis_util.h"



void 
sig_chld(int signo)
{
  pid_t pid;
  int   stat;

  while( (pid = waitpid(-1, &stat, WNOHANG)) > 0) 
	printf("child %d terminated\n", pid);
  return ;
}



Sigfunc *
signal(int signo, Sigfunc *func)
{
  struct sigaction act, oact;

  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif	
  } else {
#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif	
  }
  
  if (sigaction(signo, &act, &oact) < 0) {
	return (SIG_ERR);
  }
  return (oact.sa_handler);

}



int 
main(int argc, char **argv) 
{
  int listenfd, connfd;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;

  void sig_chld(int);


  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  if (listenfd < 0) {
	  fprintf(stderr, "socket error!\n");
	  exit(-1);
  }

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);




  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  listen(listenfd, 1024);

  signal(SIGCHLD, sig_chld);

  for ( ; ;) {
	clilen = sizeof(cliaddr);
	connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
	if ( connfd < 0) {
	  if (errno == EINTR) {
		continue;
	  } else {
		fprintf(stderr, "accept error!"); 
		exit(-1);
	  }
	}

	if ( (childpid = fork()) == 0) {
	  close(listenfd);
	  if (str_echo(connfd) < 0) {
		exit(-1);
	  }
	  exit(0);
	}

	close(connfd);
  }

  




}

