#ifndef _queuedef
#define _queuedef

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        char comm[10][100];
		char *status[10];
}Queue;

#endif
