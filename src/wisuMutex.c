#include "wisuMutex.h"

WS_DEFINE_FILE

/*initialising function*/
void WSMutex_init(WSMutex *const me,uint_fast16_t ceiling){
 // WS_ASSERT()
  me->ceiling = (uint8_t)ceiling;         /*Priority ceiling*/
  me->locks = 0U;
  me->threadPrio = 0U;
}

/*Lock equiring function for a task/thread*/
bool WSMutex_lock(WSMutex *const me,uint_fast16_t timeout){
  bool locked = true;
  
  WS_CRIT_START_
  
  WSActive *temp = (WSActive *)WSK_Attr.WS_curr;
  if(me->locks == 0U){
    me->locks = 1U;  /*lock is acquire by another thread*/
    if(me->ceiling != 0){
      temp->prio = me->ceiling;
      WS_Active_Thread[temp->prio] = temp;
      WSSet_remove(&WSK_Attr.WS_readySet,temp->Initprio);
      WSSet_insert(&WSK_Attr.WS_readySet,temp->prio);
    }
    me->threadPrio = (uint8_t)temp->Initprio;
  }
    else if(me->threadPrio == temp->Initprio){
      // Locks should be in limit
      WS_ASSERT(me->locks >= 0xFF);
      ++(me)->locks;
    }
    else{
      WS_ASSERT(me->threadPrio != 0U);
      if(me->ceiling != 0U){      
        WS_ASSERT(WS_Active_Thread[me->ceiling] != (WSActive *)0);
      }
      uint_fast8_t p;
      p = temp->prio;
      WSSet_remove(&WSK_Attr.WS_readySet,p);
      WSSet_insert(&(me)->WSMutex_waitingSet,p);
      
      WSThread_sched();
     
      WS_CRIT_STOP_
      // Erase any pipeline in Prcoessor.
       __asm( 
              "   ISB   \n");  
        WS_NOP(temp);

      WS_CRIT_START_
        if(!(temp->signal)){
          // Mutex is not signaled 
          if(WSSet_hasBits(&me->WSMutex_waitingSet,p)){
              WSSet_remove(&me->WSMutex_waitingSet,p);
              locked = false;
          }
        }
        else{
               WS_ASSERT(!WSSet_hasBits(&(me)->WSMutex_waitingSet,p));
        }
    }
    WS_CRIT_STOP_
    return locked;
}

void WSMutex_unlock(WSMutex *const me){

  WSActive *temp = (WSActive *)WSK_Attr.WS_curr;
  WS_CRIT_START_
  if(me->locks == 1U){
    /* It is locked so let's unlock it*/
    if(me->ceiling != 0){
      WSSet_remove(&WSK_Attr.WS_readySet,temp->prio);
      WSSet_insert(&WSK_Attr.WS_readySet,temp->Initprio);
      temp->prio = temp->Initprio;
    }
    me->threadPrio = 0U;
    if(WSSet_notempty(&me->WSMutex_waitingSet)){
      uint_fast8_t p;
      WSSet_findMax(&me->WSMutex_waitingSet,p);
      temp = WS_Active_Thread[p];
      
      /*we got new mutex to lock critical section let's lock*/
      if(me->ceiling != 0){
        temp->prio = me->ceiling;
        WS_Active_Thread[temp->prio] = temp;
      }
      me->threadPrio = (uint8_t)temp->prio;
      WSSet_insert(&WSK_Attr.WS_readySet,temp->prio);
      WSSet_remove(&(me)->WSMutex_waitingSet,temp->prio);
    }
    else{
      /*threre is not waiting thread for this section*/
      me->locks = 0U;
      if(me->ceiling != 0U){
          WS_Active_Thread[me->ceiling] = (WSActive *)me;
      }
    }
  }
  else{
    /*decrement lock by one level*/
    --me->locks;
  }
    WS_CRIT_STOP_
}