

#ifndef  ____SERVER_PORT

#define  ____SERVER_PORT

//jackwu
//time:

#include <stdio.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>



#define     PORT 1234
#define 	LISTEN_MAX  10

#define  ADDR  "127.0.0.1"

#define BUFSIZE    4096


#define SOCK      struct sockaddr_in 

#define  SOCK_ADDR  struct sockaddr 

#define socklen       sizeof(struct sockaddr)

typedef  struct sockaddr       t_sockaddr;


#define  convert(type,b)   ((type *)&(b))


char sendbuf[BUFSIZE] ;


# define err_exit(m) \
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)



#define  MAX(a,b)   ((a)>(b)?(a):(b))

#define  swap(a,b)\
	   do\
	   {\
              a=a^b;\
              b=b^a;\
              a=a^b;\
  	     }while(0);



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

 #define null    \
         do{}while(0)
#endif