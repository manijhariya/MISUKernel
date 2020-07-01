#ifndef WISU_STD
#define WISU_STD
#include <stdint.h>
#include <stdlib.h>

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define WS_CRIT_START_ __disable_irq();

#define WS_CRIT_STOP_  __enable_irq();

#define WS_NOP(me) \
  while(!(me)->signal)

#define WS_MAX_Active 32

#define WISU_TIMEOUT_WAITING_ 0xFFFF

#define true 1

#define false 0

typedef _Bool bool;

#endif
