#ifndef _queuedef
#define _queuedef

typedef struct Queue
{
    int capacity;
    int size;
    int front;
    int rear;
    char **elements;
}Queue;
#endif
