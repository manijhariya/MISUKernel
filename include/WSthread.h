#ifndef __WISU_Thread
#define __WISU_Thread

#include "WSSet.h"

/* Thread Control Block (TCB) */
/*
OSThread is struct consist of ** sp pointer for the function of thread
**timeout delay down-counter for which it has tp wait before start most of
it is always 0 delay.
**prio is priority for premitive it is going to be -1 for not premitive type of scheduling

*/

typedef struct{
  uint32_t *sp;
  uint8_t prio;
  uint8_t Initprio;
  uint32_t timeout;
  bool signal;
}WSActive;

extern WSActive *WS_Active_Thread[WS_MAX_Active + 1U];

typedef struct {
    WSActive super;
} WSThread;


typedef struct{
  WSActive * volatile WS_curr; /* pointer to the current thread */
  WSActive * volatile WS_next; /* pointer to the next thread to run */

  uint8_t volatile actPrio;
  uint8_t volatile lockPrio;
  uint8_t volatile lockHolder;
  uint8_t volatile intNest;
  WSActive * idleThread;
  WSSet WS_readySet; /* bitmask of threads that are ready to run */
}WS_OSAttr;

extern WS_OSAttr WSK_Attr;

/*Callback to initiase the OS idle thread*/
void WS_init(void *stkSto, uint32_t stkSize);

/* callback to handle the idle condition */
void WSThread_onIdle(void);

/* this function must be called with interrupts DISABLED */
void WSThread_sched(void);

/* transfer control to the RTOS to run the threads */
void WS_run(void);

/* blocking delay */
void WSThread_delay(uint32_t ticks);

/* process all timeouts */
void WSThread_tick(void);

/* callback to configure and start interrupts */
void WS_onStartup(void);

typedef void (*WSThreadHandler)();
/*OS thread start */
void WSThread_start(
    WSThread *me,
    uint8_t prio, /* thread priority */
    uint8_t initialPrio,
    WSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize);

void WS_StackInit(WSActive *super,
                  WSThreadHandler threadHandler,
                  void *stkSto,uint32_t stkSize);
#endif