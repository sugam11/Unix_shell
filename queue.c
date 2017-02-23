#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"queuedef.h"
Queue * createQueue()
{
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        Q->elements = (char**)malloc(sizeof(char*)*10);
        Q->size = 0;
        Q->capacity = 10;
        Q->front = 0;
        Q->rear = -1;
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

void Enqueue(Queue *Q , char *element)
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
        Q->elements[Q->rear] = (char *) malloc((sizeof element + 1)* sizeof(char));
        strcpy(Q->elements[Q->rear], element);
        return;
}

void printQ(Queue *Q)
{
    int i=Q->front;
    int j=Q->size;
    while(j!=0)
    {
        puts(Q->elements[i]);
        j--;
        i++;
        if(i == Q->capacity)
        {
                i = 0;
        }
    }
    return;
}
