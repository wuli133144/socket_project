
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

 #define  err_exit(m)\
 do\
 { \
 	perror(m);\
 	exit(0);\
 }while(0);

void *Malloc(size_t size){
    void *p=NULL;
    if((p=malloc(size))==NULL){
    	err_exit("malloc");
    }
  return p;
}


typedef struct node{
  int data;
  struct node *next;

}t_node,*p_listnode;


void init_list(p_listnode *head){
	  if((*head)==NULL){
	  *head=(t_node *)Malloc(sizeof(t_node));
  	  (*head)->next=NULL;
  	
	  }else{
	  	return;
	  }
}
  

void insert(p_listnode head,int ls){

  if(head==NULL){
  	 head=(t_node *)Malloc(sizeof(t_node));
  	 (head)->next=NULL;
  	 (head)->data=ls;
  }else{
    t_node *temp=(t_node *)Malloc(sizeof(t_node));
    temp->data=ls;
    temp->next=NULL;
    while((head)->next!=NULL){
    	(head)=(head)->next;
    } 
   (head)->next=temp;
  }
  

}


void show(t_node *head){
  head=head->next;

  while(head->next!=NULL){
  	printf("%d \n", head->data);
  	head=head->next;
  }
  printf("%d \n", head->data);
 
}


void show1(t_node *head){

  if(head==NULL){
    return;
  }
  while(head->next!=NULL){
    printf("%d \n", head->data);
    head=head->next;
  }
  printf("%d \n", head->data);
 
}




int main(){

	t_node *p=NULL;
	init_list(&p);
	insert(p,12);
	insert(p,12);
	insert(p,12);
	insert(p,12);
  show1(p);

	return 0;
}
