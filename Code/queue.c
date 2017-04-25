#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include"queuedef.h"
Queue * createQueue()
{
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        Q->size = 0;
        Q->capacity = 10;
        Q->front = 0;
        Q->rear = -1;
		/*for(int i =0;i<10;i++)
		{
			Q->comm[i] = (char*)malloc(sizeof(char*));
			Q->status[i] = (char*)malloc(sizeof(char*));
		}*/
        return Q;
}

void Dequeue(Queue *Q)
{
        if(Q->size!=0)
        {
                Q->size--;
                Q->front++;
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}


void printQ(Queue *Q)
{
    int i=Q->front;
    int j=Q->size;
    printf("\n");
    while(j!=0)
    {
        printf("command = %s , ", Q->comm[i]);
		printf("with status = %s\n", Q->status[i]);
        j--;
        i++;
        if(i == Q->capacity)
        {
                i = 0;
        }
    }
    return;
}

void Enqueue(Queue *Q , char *ele, int childs)
{
        if(Q->size == Q->capacity)
        {		
                Dequeue(Q);
        }
        Q->size++;
        Q->rear++;
        if(Q->rear == Q->capacity)
        {
                Q->rear = 0;
				
        }
        strcpy(Q->comm[Q->rear],ele);
		if(childs)
			Q->status[Q->rear] = "FAILURE";
		else
			Q->status[Q->rear] = "SUCCESS";
		//printQ(Q);
        return;
}
