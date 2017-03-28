

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

#include <sys/types.h>






typedef struct task_worker{
  void *(*process)(void *arg);//处理函数
  void *arg;
  struct task_worker *next;

}t_task;


typedef struct {

   pthread_mutex_t lock;
   pthread_cond_t cond_lock;
   t_task *queue_task;
   int shutdown;
   pthread_t *threadid;
   int max_thread_num;
   int cur_queue_size;


}t_task_pool;


//添加任务

int pool_add_task(void *(*proc)(void *arg),void *arg);

void *pthread_routine(void *);

static t_task_pool *pool=NULL;
//

//init_pool
void pool_init(int max_num){
 
  pool=(t_task_pool *)malloc(sizeof(t_task_pool));
  
  pthread_mutex_init(&(pool->lock),NULL);
  pthread_cond_init(&(pool->cond_lock),NULL);

  pool->queue_task=NULL;
  pool->max_thread_num=max_num;
  pool->cur_queue_size=0;
  pool->shutdown=0;
  pool->threadid=(pthread_t *)malloc(sizeof(pthread_t)*(pool->max_thread_num));
  int i=0;
  for(i=0;i<pool->max_thread_num;i++){
  	 pthread_create(&(pool->threadid[i]),NULL,pthread_routine,NULL);
  }


}


int pool_add_task(void *(*proc)(void *arg),void *arg){

     t_task *newtask=(t_task *)malloc(sizeof(t_task));
     newtask->process=proc;
     newtask->arg=arg;

     newtask->next=NULL;

     pthread_mutex_lock(&(pool->lock));//加锁
     t_task *ptast=pool->queue_task;
     
     if(ptast!=NULL){
       while(ptast->next!=NULL){
       	  ptast=ptast->next;
       }
       ptast->next=newtask;//添加任务
     }else{
      pool->queue_task=newtask;

     }
     pool->cur_queue_size++;
     pthread_mutex_unlock(&(pool->lock));//解锁

     pthread_cond_signal(&(pool->cond_lock));//唤醒等待线程
   
    return 0;

  }

  int pool_destroy(){

     if(pool->shutdown==1){
     	 return -1;
     }
     pool->shutdown=1;
     pthread_cond_broadcast(&(pool->cond_lock));
     
     int i;
     for(i=0;i<pool->max_thread_num;i++){
      pthread_join(pool->threadid[i],NULL);
     }
     free(pool->threadid);
     t_task *p=NULL;
     while(pool->queue_task!=NULL){
      p=pool->queue_task;
      pool->queue_task=pool->queue_task->next;
      free(p);
     }

     pthread_mutex_destroy(&(pool->lock));
     pthread_cond_destroy(&(pool->cond_lock));
     free(pool);
     pool=NULL;
     return 	0;

  }



  void * pthread_routine(void *arg){

   while(1){

       pthread_mutex_lock(&(pool->lock));
       while(pool->cur_queue_size==0&&pool->shutdown!=1){//当前任务为0

         pthread_cond_wait(&(pool->cond_lock),&(pool->lock));

       }
     
     if(pool->shutdown){

     	pthread_mutex_unlock(&(pool->lock));
     	pthread_exit(NULL);
     }

     pool->cur_queue_size--;//Decrese task

     t_task *head=pool->queue_task;
     pool->queue_task=pool->queue_task->next;
     pthread_mutex_unlock(&(pool->lock));
     
     ////handler function
     (*(head->process))(head->arg);//执行函数

     free(head);
     head=NULL;
    
   }

   pthread_exit(NULL);
  }


//处理函数
  void * pro(void *arg){
   
   int *p=(int *)&arg;
    printf("  %d\n",*p);
    sleep(1);
    return NULL;

  }

  int main(void){

  	pool_init(3);

  	int *num=(int *)malloc(sizeof(int)*10);
  	int i=0;

  	for (i = 0; i < 10; ++i)
  	{
  		/* code */
  		pool_add_task(pro,(void *)&num[i]);
      
  	}

  	sleep(5);
  	pool_destroy();
  	return 0;

  }