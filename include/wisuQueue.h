#ifndef __WISU_Q
#define __WISU_Q

#include "wisuStd.h"
struct QNode{
	void *thread;
	struct QNode *next;
};
typedef struct{
	struct QNode *front,*rear;
}WSQueue;
	 
struct QNode *QNode_init(void *const thread_pointer);
	 
void WSQueue_init(WSQueue *const me);

_Bool WSQueue_insert(WSQueue *const me,void *const thread_pointer);
 
_Bool WSQueue_delete(WSQueue *const me);
#endif
