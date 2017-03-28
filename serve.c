

#include "common_header.h"





#define MSG  "hello world!"


//处理客户端
typedef  struct client_info
{
 int connfd;
 struct sockaddr_in client;
 
}t_client_info;

//客户处理请求函数

void child_handler(int connectfd,struct sockaddr_in client){
 
 char msgbuf[BUFSIZE];
 bzero(msgbuf,BUFSIZE);

 snprintf(msgbuf,"%s",inet_ntoa(client.sin_addr));
 write(connectfd,msgbuf,sizeof(msgbuf));
 
  return ;


}

void *pthread_fun(void *arg){
  
  t_client_info *info=(t_client_info *)arg;

 child_handler(info->connfd,info->client);

 free info;
 pthread_exit(NULL);

}



void sig_handler(int sig){
  

  pid_t pid;

  while((pid=waitpid(-1,NULL,NULL))>0){

       printf("child pid=%d\n", pid); 
     }

}

int main()
{
   

   int listenfd,connectfd;
    
    pthread_t threadid;

    SOCK serve,client;

    socklen_t ssize;//the size of SOCK

	  listenfd=Socket(AF_INET,SOCK_STREAM,0);
       
      bzero(&serve,sizeof(serve));
       //memset()
     init_sockaddr_in(&serve);//init sockaddr_in

     Bind(listenfd,(struct sockaddr *)&serve,sizeof(struct sockaddr));
      
     Listen(listenfd,LISTEN_MAX);
     ssize=sizeof(SOCK);
     
     signal(SIGCHLD,sig_handler);

     while(1){
          
         connectfd=Accept(listenfd,(t_sockaddr *)&client,sizeof(t_sockaddr));
         
          t_client_info *info=(t_client_info *)malloc(sizeof(t_client_info));

          info->connfd=connectfd;
          memcpy(&info->client,&client,sizeof(t_sockaddr));
         

          Pthread_create(&threadid,NULL,pthread_fun,(void *)info);
          
     }

   //printf(MSG);
	return 0 ;
}
