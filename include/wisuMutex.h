#ifndef WISU_MUTEX
#define WISU_MUTEX

#include "WSSet.h"
#include "wisu.h"
#include "WSthread.h"


/*Initialise a mutex structure*/
typedef struct {
  WSSet WSMutex_waitingSet;
  uint8_t volatile locks;
  uint8_t volatile threadPrio;
  uint8_t ceiling;
}WSMutex;

/*initialising function*/
void WSMutex_init(WSMutex *const me,uint_fast16_t ceiling);

/*Lock equiring function for a task/thread*/
bool WSMutex_lock(WSMutex *const me,uint_fast16_t timeout);

/*Unlock function to thread*/
void WSMutex_unlock(WSMutex *const me);

/*Scheduling for Mutex*/
void WSMutex_ScheduleFIFO(WSMutex *const me);
#endif