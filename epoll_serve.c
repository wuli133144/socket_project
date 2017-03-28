


#include <stdio.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <pthread.h>


#define MAXLINE  10
#define OPEN_MAX 100
#define QLISTEN 32
#define BUFSIZE 4096


/****************************************/
 typedef struct task{
   int fd;
   struct task *next;
   
}t_task;
/*****************************************/

/************************************/
typedef struct user_data{
 char buf[BUFSIZE];
 int cur_size;
 int fd;
}t_udata;
/*****************************************/

# define err_exit(m) \
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)


void * readtask(void *);

void * writetask(void *);

struct epoll_event ev,events[OPEN_MAX];

int epfd;

/******************************************************/
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond =PTHREAD_COND_INITIALIZER ;
/*******************************************************/

t_task *ptask=NULL,*readtail=NULL,*writehead=NULL;

void setnonblock(int sock){

    int opt;
    opt=fcnt(sock,F_GETFL);
    if(opt<0){
    	err_exit("nonblock");
    }
    opt|=O_NONBLOCK;
    if((fcnt(sock,F_SETFL,opt))<0){
    	err_exit("fcnt");
    }

} 

//简单设置
void set_fl(int fd,int flag){
	int val;
    if((val=fcnt(fd,F_GETFL,0))<0){
    	err_exit("fcnt error");
    }
    val|=flag;
    if((fcnt(fd,F_SETFL,val)<0)){
    	err_exit("fcnt error");
    }
}
/*******************************/


/*****************************************/
int Socket(int ip,int inet,int flag)
{    
	 int listenfd;
	 if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
	 	 err_exit("socket failed!");
	 }
	 return listenfd;
}

/*****************************************/

//init_sockaddr_in()
void init_sockaddr_in(SOCK *Psock){
  
  bzero(Psock,sizeof(SOCK));
  Psock->sin_family=AF_INET;

  Psock->sin_port=PORT;
  Psock->sin_addr.s_addr=inet_addr(ADDR);

}
/******************************/

  int Bind(int listenfd,struct sockaddr *psock,size_t size){
     if((bind(listenfd,psock,size))==-1){
      	err_exit("listen error");
      }
     return 1;
  }
/****************************************/


//
 int Listen(int listenfd,int max)
 {

      if((listen(listenfd,max))<0){
       err_exit("listen error");
      }
    return 1;
 }

 //
int Accept(int listenfd,struct sockaddr * addr,size_t size){
	int connectfd;
	if((connectfd=accept(listenfd,addr,&size))<0){
          	err_exit("accept failed!");
          }
    return connectfd;
}


/***************************************************/
int Connect(int fd,t_sockaddr *addr,socklen_t size){
	int connectfd;
  if((connectfd=connect(fd,addr,size))<0){
  	 err_exit("connectfd failed");
  }
  return connectfd;

}
/****************************************************/


int Pthread_create(pthread_t *thread,pthread_attr_t * attr,void *(*start_routine)(void *),void *arg){

  if((pthread_create(thread,attr,start_routine,arg))<0){
  	 err_exit("pthread_create error");
  }
 return 1;
}
/*******************************/


int main(){
  
  int i;
  int max;
  int listenfd;
  int connfd;
  int sockfd;
  int nfds;
  pthread_t pid1,pid2;
  
  t_task *new_task=NULL;
  t_udata *data=NULL;
  
  socklen_t len;


  pthread_create(&pid1,NULL,readtask,NULL);
  pthread_create(&pid2,NULL,writetask,NULL);
  

  ///////////////////////////////////////////////////////
   struct sockaddr_in clientaddr;
   struct sockaddr_in serveaddr;

   listenfd=Socket(AF_INET,SOCK_STREAM,0);
   setnonblock(listenfd);
   
   bzero(&serveaddr,sizeof(serveaddr));

   serveaddr.sin_family=AF_INET;
   inet_aton("120.0.0.1",&(serveaddr.sin_addr));

   Bind(listenfd,(struct sockaddr *)&serveaddr,sizeof(struct sockaddr));
   
   Listen(listenfd,OPEN_MAX);
   ////////////////////////////////////////////////////////


   /*********************************************/
   epfd=epoll_create(OPEN_MAX);

   ev.data.fd=listenfd;
   ev.events=EPOLLIN|EPOLLET;

   epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
  
  /*************************************************/

   
  max=0;

  for(;;){

     nfds=epoll_wait(epfd,events,OPEN_MAX,500);

     for(i=0;i<nfds;i++){
     	  if(events[i].data.fd==listenfd){
     	  	  connfd=Accept(listenfd,(struct sockaddr *)&clientaddr,sizeof(struct sockaddr));
              setnonblock(connfd);
              
              ev.data.fd=connfd;
              ev.events=EPOLLIN|EPOLLET;
              epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
     	  }
     	  else if(events[i].events&EPOLLIN){//有数据可读
     	  	if((sockfd=events[i].data.fd)<0){
     	  		  continue;
     	  	}
     	  	new_task=(t_task *)malloc(sizeof(t_task));
     	  	new_task->fd=sockfd;
     	  	new_task->next=NULL;

     	  	pthread_mutex_lock(&mutex);//加锁
             
             if(readtask==NULL){
             	readtask=new_task;
             	readtail=new_task;
             }else{

             	while(readtask->next!=NULL){
             		readtask=readtask->next;
             	}
             	readtask->next=new_task;
             	  readtail=new_task;
             }
             
           pthread_cond_broadcast(&cond);
           pthread_mutex_unlock(&mutex);//加锁     	  	


     	  }else if(events[i].events&EPOLLOUT){//WRITE

     	  	 data=(t_udata *)events.data.ptr;
     	  	 sockfd=data->fd;
     	  	 write(sockfd,data->buf,data->cur_size);
             free(data);
             ev.data.fd=sockfd;
             ev.events=EPOLLIN|EPOLLET;

             epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
     	  }

     }



  }


	return 0;
}


void *readtask(void *arg){
    
    int fd=-1;
    size_t n;
    t_udata *data=NULL;

    while(1){

      pthread_mutex_lock(&mutex);
       while(readtask==NULL){
       	 pthread_cond_wait(&(cond),&(mutex));
       }
       
       fd=readtask->fd; 

       t_task *temp=readtask;

       readtask=readtask->next;

       free(temp);
       

      pthread_mutex_unlock(&mutex);
      data=(t_udata *)malloc(sizeof(t_udata));
      data->fd=fd;

      if((n=read(fd,data->buf,sizeof(data->buf)))<0){
      	 if(errno==ECONNRESET){
      	 	close(fd);
      	 }else{
      	 	 printf("errno" );
      	 }
      	 if(data!=NULL){
      	 	free(data);
      	 }
      }else if(n==0){
      	close(fd);
      	 if(data!=NULL){
      	 	free(data);
      	 }
      }else {

        data->cur_size=n;
        ev.data.ptr=data;
        ev.events=EPOLLOUT|EPOLLET;
        epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);

      }


    }


}