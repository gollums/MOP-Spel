#include "queue.h"
#include <stdlib.h>
#include "geometricobject.h"

POBJECT objArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

/*returns the head*/
POBJECT peek() {
   return objArray[front];
}

/*returns true if itemCount == 0*/
bool isEmpty() {
   return itemCount == 0;
}

/*returns true if itemCount == MAX (512)*/
bool isFull() {
   return itemCount == MAX;
}

/*Returns itemCount*/
int size() {
   return itemCount;
}  

/*Adds a node in the end of the queue*/
void insert(POBJECT data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       
      objArray[++rear] = data;
      itemCount++;
   }
}

int remove() {
   POBJECT data = objArray[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return (int)data;  
}

/*
void init_queue(queue *q){
    q->rear=q->front=NULL;
}

int empty(queue *q){
    if(q->rear==NULL)
        return 0;
    return 1;
}

void insert(queue *q, int x1, int y1, int r1){
    node *p;
    p = (node *)malloc(sizeof(node));
    p->x = x1;
    p->y = y1;
    p->radius = r1;
    p->next == NULL;
    
    if(q->rear==NULL){
        q->front = q->rear = p;
    }else{
        (q->rear)->next = p;
        q->rear = p;
    }
}

int remove(queue *q){
    int x;
    node *u;
    u = q->front;
    
    if(q->rear==q->front){
        init_queue(q);
    }else{
        q->front = (q->front)->next;
    }
    free(u);
    return x;
}
*/