#ifndef __CB_H__
#define __CB_H__

 
/* Circular buffer object */
typedef struct {
    int         size;   /* maximum number of elements           */
    int         start;  /* index of oldest element              */
    int         end;    /* index at which to write new element  */
    int         count;    /* index at which to write new element  */
    ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;

void cbInit(CircularBuffer *cb, int size,void * buff);
void cbFree(CircularBuffer *cb);
int cbIsFull(CircularBuffer *cb);
int cbIsEmpty(CircularBuffer *cb);
void cbWrite(CircularBuffer *cb, ElemType *elem);
void cbRead(CircularBuffer *cb, ElemType *elem);
void cbPeek(CircularBuffer *cb, int *newest);
void getindexes(CircularBuffer *cb, double sumvalue, int *indexes, int *num);
#endif
