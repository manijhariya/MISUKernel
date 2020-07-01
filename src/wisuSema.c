#include "wisuSema.h"

WS_DEFINE_FILE

uint16_t signal=1;
void WSSema_init(WSSema *const me,
            uint_fast16_t count,
            uint_fast16_t max_count){
   //Intialising the semaphore for the thread Processing
   me->count     = (uint16_t)count;
   me->max_count = (uint16_t)max_count;
}

bool WSSema_wait(WSSema *const me,
                 uint16_t const wait){
    //Continue thread wait
    WS_CRIT_START_
    bool signaled = true;
    if(me->count > 0U){
      --me->count;
    }
    else{
        WSActive *temp = (WSActive *)WSK_Attr.WS_curr;
        uint_fast8_t p = temp->prio;
        temp->signal = false;
        WSSet_insert(&(me)->WSSema_waitingSet,p);
        WSSet_remove(&WSK_Attr.WS_readySet,p);
        
        WSThread_sched();
      
        WS_CRIT_STOP_
        
        __asm( 
              "   ISB   \n");  
        WS_NOP(temp);     //Uncomment this line if Sema not able to block
        // Send this thread in waiting till timeout it is going to return for timeout
        WS_CRIT_START_
          
        if(!(temp->signal)){ 
          if(WSSet_hasBits(&(me)->WSSema_waitingSet,p)){
            //Still in waiting set send check if timeout is over
            WSSet_remove(&(me)->WSSema_waitingSet,p);
            signaled = false; 
          }
          else{
            //signaled variable
            --(me)->count;
          }
        } 
        else{
          WS_ASSERT(!WSSet_hasBits(&(me)->WSSema_waitingSet,p));
           --(me)->count;
        }
        WS_CRIT_STOP_
    }   
    return signaled;
}

bool WSSema_tryWait(WSSema *const me){
  //try wait 
  return 0;
}

bool WSsema_signal(WSSema *const me){
  // Signal the semaphore to run like hell!!!!
    bool signaled = true;
    uint32_t p;
    if((me)->count < (me)->max_count){
        ++(me)->count;
        if(WSSet_notempty(&(me)->WSSema_waitingSet)){
        WSSet_findMax(&(me)->WSSema_waitingSet,p);
        WS_Active_Thread[p]->signal = true;
        
        WSSet_remove(&(me)->WSSema_waitingSet,p);
        WSSet_insert(&WSK_Attr.WS_readySet,p);
        }
      }
      else{
          signaled = false;
      } 
    return signaled;
}
