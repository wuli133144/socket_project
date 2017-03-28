

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//互斥量
//条件变量

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond =PTHREAD_COND_INITIALIZER;


void * fun1(void *);
void * fun2(void *);

int i=1;

int main(){

   pthread_t t_a;
   pthread_t t_b;

    pthread_create(&t_b,NULL,fun2,NULL);

    pthread_create(&t_a,NULL,fun1,NULL);
  
   
   pthread_join(t_a,NULL);
   pthread_join(t_b,NULL);

   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);

	return 0;
}

void * fun1(void *arg){

   for(i=1;i<50;i++){

        pthread_mutex_lock(&mutex);

         if(i%3==0){
         	pthread_cond_signal(&cond);
         }else{
         	printf("thread1  %d\n",i);
         }

        pthread_mutex_unlock(&mutex);
        sleep(1);

   }


}

void *fun2(void *arg){
   
   while(i<50){
      pthread_mutex_lock(&mutex);

      //if(i%3!=0){
       	  pthread_cond_wait(&cond,&mutex);
      //  }
       printf("thread2  %d\n", i);

      pthread_mutex_unlock(&mutex);
       sleep(1);
   }



}