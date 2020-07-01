#ifndef __WISU_SEMA
#define __WISU_SEMA

#include "WSSet.h"
#include "wisuStd.h"
#include "wisu.h"
#include "WSthread.h"

typedef struct{
  WSSet WSSema_waitingSet;
  uint16_t volatile count;      //Up-down or on-off counter
  uint16_t max_count;          //for binary or counting semaphore
}WSSema;

void WSSema_init(WSSema *const me,
            uint_fast16_t count,
            uint_fast16_t max_count);

bool WSSema_wait(WSSema *me,
            uint16_t const wait);

bool WSSema_tryWait(WSSema *const me);

bool WSsema_signal(WSSema *const me);
#endif