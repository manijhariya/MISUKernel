#include "wisuQueue.h"
/*New node which is going to add in the queue*/
struct QNode* QNode_init(void *const thread_pointer){
	struct QNode *temp = (struct QNode*) malloc(sizeof(struct QNode));
	temp->thread = thread_pointer;
	temp->next = NULL;
        return temp;
}
/*WS*/
void WXQueue_init(WSQueue *const me){
	me->front = NULL;
	me->rear = NULL;
}

bool WSQueue_insert(WSQueue *const me,void *const thread_pointer){
		struct QNode *temp = QNode_init(thread_pointer);
		if(me->rear == NULL){
			me->rear = me->front = temp;
			return true;
		}	
		else{
				me->rear->next = temp;
				me->rear = temp;
				return true;
			}
	//	return 1;
}
bool WSQueue_delete(WSQueue *const me){
		if(me->front == NULL)
			return true;
		else{
				struct QNode *temp = me->front;
				me->front = me->front->next;
				if(me->front == NULL){
					me->rear = NULL;
					}
				free(temp);				
				return true;
			}
	//	return 1;
}
