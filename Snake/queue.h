#import <stdbool.h>

#define NULL "\0"
#define MAX 512

/*returns true if itemCount == 0*/
bool isEmpty();

/*returns true if itemCount == MAX (512)*/
bool isFull();

/*Returns itemCount*/
int size();
/*Adds a node in the end of the queue*/
void insert(data);

int remove();