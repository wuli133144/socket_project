

#include "common_header.h"




int main(){

 
   int listenfd,connectfd;
  
    fd_set rfds;//can read

    fd_set all_fds;//all fds

    SOCK serve,client;

    socklen_t ssize;//the size of SOCK

	 listenfd=Socket(AF_INET,SOCK_STREAM,0);
       
      bzero(&serve,sizeof(serve));
       //memset()
      init_sockaddr_in(&serve);//init sockaddr_in

     Bind(listenfd,(struct sockaddr *)&serve,sizeof(struct sockaddr));
      
     Listen(listenfd,LISTEN_MAX);
     ssize=sizeof(SOCK);
     
     FD_ZERO(&all_fds);//初始化
     





     
    // signal(SIGCHLD,sig_handler);

	return 0;
}