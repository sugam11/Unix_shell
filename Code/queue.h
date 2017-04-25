#include<stdio.h>
#include<stdlib.h>
#include"queuedef.h"
extern Queue * createQueue(void);
extern void Dequeue(Queue *Q);
extern void Enqueue(Queue *Q , char *element, int childs);
extern void printQ(Queue *Q);
