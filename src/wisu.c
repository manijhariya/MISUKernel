#include <stdint.h>
#include "WSthread.h"
#include "WSAssert.h"
#include "TM4C123GH6PM.h"
#include "bsp.h"
#include "WSThread.h"

#define LED_RED   (1U << 1)
WS_DEFINE_FILE

WS_OSAttr WSK_Attr;
WSSet WS_delaySet;
static WSActive idleThread;
uint32_t stack_idleThread[40];

WSActive *WS_Active_Thread[WS_MAX_Active + 1U] = {NULL}; 

void WS_onStartup(void) {
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set the SysTick interrupt priority (highest) */
    NVIC_SetPriority(SysTick_IRQn, 0U);
    
}

void WSThread_onIdle(void) {
  while(1){
    BSP_ledRedOn();
    BSP_ledRedOff();
  }
}


void WS_init(void *stkSto, uint32_t stkSize) {
    /* set the PendSV interrupt priority to the lowest level 0xFF */
    
    *(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16);
    /* start idleThread thread */
    WS_CRIT_START_
    WSK_Attr.lockPrio = 0U;
    idleThread.prio = 0U;
    idleThread.Initprio = 0U;
    
    WS_StackInit(&idleThread,
                 &WSThread_onIdle,
                 stack_idleThread,sizeof(stack_idleThread));
    WSK_Attr.idleThread = &idleThread;
    WS_CRIT_STOP_
}

void WSThread_sched_RR(void){
    uint32_t currPrio = WSK_Attr.WS_curr->prio;
    
    ++currPrio;
    
    if(currPrio == WS_MAX_Active || currPrio == 0U){
      currPrio = 0U;
    }
    WSK_Attr.WS_next = WS_Active_Thread[currPrio];
    
    if (WSK_Attr.WS_next != WSK_Attr.WS_curr) {
    *(uint32_t volatile *)0xE000ED04 = (1U << 28);
   } 
}

void WSThread_sched(void) {
    /*Priority based schedular in increasing order!!!*/
  
    uint32_t currPrio;
    WSActive *next;
    
    WSSet_findMax(&WSK_Attr.WS_readySet,currPrio);
    
    if ((uint_fast8_t)WSK_Attr.lockPrio >= currPrio ) {
        currPrio = (uint_fast8_t)WSK_Attr.lockPrio; 
        if(currPrio != 0U){
          WS_ASSERT(WSSet_hasBits(&WSK_Attr.WS_readySet,currPrio));
          }
        }
    next = WS_Active_Thread[currPrio];  
    
    if(currPrio > (uint_fast8_t)WSK_Attr.actPrio){
        WSK_Attr.WS_next = next;
    }    
    else{
      WSK_Attr.WS_next = (WSActive *)0;
      currPrio = 0U;
    }
    
    /* trigger PendSV, if needed */
    
    if (WSK_Attr.WS_next != WSK_Attr.WS_curr) {
    *(uint32_t volatile *)0xE000ED04 = (1U << 28);
   }
}

void WS_run(){
    /* callback to configure and start interrupts */
    
    WS_onStartup();
      WS_init(stack_idleThread,sizeof(stack_idleThread));
      __disable_irq();
      WSThread_sched();
      __enable_irq();
      
    /* the following code should never execute */
      
      for(;;){
          WSThread_onIdle();
      }
  }

void WSThread_tick() {
        WSSet workingSet = WS_delaySet;
        while (WSSet_notempty(&workingSet)) {
        uint32_t bit;  
        WSSet_findMax(&workingSet,bit);
        WSActive *t = WS_Active_Thread[bit];
          
          --t->timeout;
          if (t->timeout == 0U) {
          WSSet_insert(&WSK_Attr.WS_readySet,bit);  /* insert to set */
          WSSet_remove(&WS_delaySet,bit); /* remove from set */
          }
          WSSet_remove(&workingSet,bit); /* remove from working set */
     }
}

void WSThread_delay(uint32_t ticks) {
    __disable_irq();

    WSK_Attr.WS_curr->timeout = ticks;
    WSSet_remove(&WSK_Attr.WS_readySet,WSK_Attr.WS_curr->prio);
    WSSet_insert(&WS_delaySet,WSK_Attr.WS_curr->prio);
    
    WSThread_sched();
    __enable_irq();
}

void WSThread_start(
    WSThread *me,
    uint8_t prio, /* thread priority */
    uint8_t initialPrio,
    WSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize)
{
    /* priority must be in ragne
    * and the priority level must be unused */

   // WS_Require((prio > 0) && (prio <= WS_MAX_Active));
    
    me->super.prio = prio;
    me->super.Initprio = prio;
 
    WS_StackInit(&(me)->super,
                 threadHandler,
                 stkSto,stkSize);
    
    WS_Active_Thread[prio] = &(me)->super;
    WSSet_insert(&WSK_Attr.WS_readySet,me->super.prio);
    

}

void WS_StackInit(WSActive *super,
                  WSThreadHandler threadHandler,
                  void *stkSto,uint32_t stkSize){

     /* round down the stack top to the 8-byte boundary
    * NOTE: ARM Cortex-M stack grows down from hi -> low memory
    */
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;
    
    
    *(--sp) = (1U << 24);               /* xPSR */
    *(--sp) = (uint32_t)threadHandler; /* PC */
    *(--sp) = 0x0000000EU; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */

    /* additionally, fake registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */

    /* save the top of the stack in the thread's attibute */

    #if (__ARM_FP != 0)          /* if VFP available... */
    *(--sp) = 0xFFFFFFFDU;   /* exception return with VFP state */
    *(--sp) = 0xAAAAAAAAU;   /* stack "aligner" */
#endif                       /* VFP available */

    /* save the top of the stack in the thread's attibute */
    super->sp = sp;

    /* pre-fill the unused part of the stack with 0xDEADBEEF */
    stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U) >> 3) + 1U) << 3);
    for (; sp >= stk_limit; --sp) {
        *sp = 0xDEADBEEFU;
    }

}

#define VAL(x) #x
#define STRINGIFY(x) VAL(x)
#define NVIC_ICSR    0xE000ED04
#define QF_BASEPRI   0x3F

#define WSK_CURR       0
#define WSK_NEXT       4
#define WSK_ACT_PRIO   8
#define WSK_IDLE_THR   12
#define Thread_SP 0
#define Thread_Prio  4

#define VAL(x) #x
#define STRINGIFY(x) VAL(x)

__stackless
void PendSV_Handler(void) {
__asm (

    /* Prepare some constants before entering a critical section... */
    "  LDR     r3,=WSK_Attr   \n"
    "  LDR     r2,=" STRINGIFY(NVIC_ICSR) "\n" /* Interrupt Control and State */
    "  MOVS    r1,#1            \n"
    "  LSLS    r1,r1,#27        \n"            /* r0 := (1 << 27) (UNPENDSVSET bit) */

    /*<<<<<<<<<<<<<<<<<<<<<<< CRITICAL SECTION BEGIN <<<<<<<<<<<<<<<<<<<<<<<<*/

    "  MOVS    r0,#" STRINGIFY(QF_BASEPRI) "\n"
    "  CPSID   i                \n" /* selectively disable interrutps with BASEPRI */
    "  MSR     BASEPRI,r0       \n" 
    "  CPSIE   i                \n" 
                           

    /* The PendSV exception handler can be preempted by an interrupt,
    * which might pend PendSV exception again. The following write to
    * ICSR[27] un-pends any such spurious instance of PendSV.
    */
    "  STR     r1,[r2]          \n" /* ICSR[27] := 1 (unpend PendSV) */

    /* Check WSK_attr.next, which contains the pointer to the next thread */
    "  LDR     r0,[r3,#" STRINGIFY(WSK_NEXT) "]\n" /* r1 := QXK_attr_.next */
    "  CMP     r0,#0            \n" /* is (WSK_attr.next == 0)? */
    "  BEQ     PendSV_return    \n" /* branch if (WSK_attr.next == 0) */

    /* Load pointers into registers... */
    "  MOV     r12,r0           \n"                   /* save WSK_Attr.WS_next in r12 */
    "  LDR     r2,[r0,#" STRINGIFY(Thread_SP) "]\n"   /* r2 := WSK_Attr.WS_next->Thread_SP*/
    "  LDR     r1,[r3,#" STRINGIFY(WSK_CURR) "]\n"    /* r1 := WSK_Attr.WS_curr */

    "  CMP     r1,#0            \n" /* (WSK_Attr.WS_curr != 0)? */
    "  BNE     PendSV_save_ex   \n" /* branch if (current thread is extended) */

    "  CMP     r2,#0            \n" /* (WSK_attr.WS_next->ThreadSP != 0)? */
    "  BNE     PendSV_save_ao   \n" 

   "PendSV_activate:           \n"
#if (__ARM_FP != 0)                 /* if VFP available... */
    "  PUSH    {r0,lr}          \n" /* ... push lr (EXC_RETURN) plus stack-aligner */
#endif     
    "PendSV_save_ao:            \n"

    "  PUSH    {r4-r11}         \n" /* save r4-r11 on top of the exception frame */
#if (__ARM_FP != 0)                 /* if VFP available... */
    "  TST     lr,#(1 << 4)     \n" /* is it return with the VFP exception frame? */
    "  IT      EQ               \n" /* if lr[4] is zero... */
    "  VSTMDBEQ sp!,{s16-s31}   \n" /* ... save VFP registers s16..s31 */

    "  PUSH    {r0,lr}          \n" /* save the "aligner" and the EXC_RETURN value */
#endif                              /* VFP available */

    "  CMP     r2,#0            \n"
    "  BNE     PendSV_restore_ex\n" 
    /* otherwise continue to restoring next AO-thread... */
      
    "PendSV_restore_ao:         \n"
    "  MOVS    r0,#0            \n"
    "  STR     r0,[r3,#" STRINGIFY(WSK_CURR) "]\n" 
    /* don't clear WSK_Attr.WS_next, as it might be needed for AO activation */

#if (__ARM_FP != 0)                 
    "  POP     {r0,lr}          \n" /* restore alighner and EXC_RETURN into lr */
    "  TST     lr,#(1 << 4)     \n" /* is it return to the VFP exception frame? */
    "  IT      EQ               \n" /* if EXC_RETURN[4] is zero... */
    "  VLDMIAEQ sp!,{s16-s31}   \n" /* ... restore VFP registers s16..s31 */
#else
    "  BIC     lr,lr,#(1 << 2)  \n" /* make sure MSP is used */
#endif
    "  POP     {r4-r11}         \n" /* restore r4-r11 from the next thread's stack */

    "  MOV     r0,r12           \n" 
    "  MOVS    r2,#" STRINGIFY(Thread_Prio) "\n" /* r2 := offset of .prio into QActive */
    "  LDRB    r0,[r0,r2]       \n" 
    "  LDRB    r2,[r3,#" STRINGIFY(WSK_ACT_PRIO) "]\n" 
    "  CMP     r2,r0            \n"
    "  BCC     PendSV_activate  \n" 

    /* otherwise no activation needed... */
    "  MOVS    r0,#0            \n"
    "  STR     r0,[r3,#" STRINGIFY(WSK_NEXT) "]\n" 

#ifdef QXK_ON_CONTEXT_SW
    "  MOVS    r0,r1            \n" /* r0 := WSK_Attr.WS_curr */
    "  MOV     r1,r12           \n" /* r1 := WSK_Attr_.WS_next */
    "  LDR     r2,[r3,#" STRINGIFY(QXK_IDLE_THR) "]\n" /* r2 := idle thr ptr */
    "  CMP     r1,r2            \n"
    "  BNE     PendSV_onContextSw1 \n" /* if (next != idle) call onContextSw */
    "  MOVS    r1,#0            \n" /* otherwise, next := NULL */
    "PendSV_onContextSw1:        \n"
    "  PUSH    {r1,lr}          \n" /* save the aligner + exception lr */
    "  BL      QXK_onContextSw  \n" /* call WSK_onContextSw() */
    "  POP     {r1,r2}          \n" /* restore the aligner + lr into r2 */
    "  MOV     lr,r2            \n" /* restore the exception lr */
#endif 

    /* re-enable interrupts and return from PendSV */
    "PendSV_return:             \n"

    "  MOVS    r0,#0            \n"
    "  MSR     BASEPRI,r0       \n" /* enable interrupts (clear BASEPRI) */
    /*>>>>>>>>>>>>>>>>>>>>>>>> CRITICAL SECTION END >>>>>>>>>>>>>>>>>>>>>>>>>*/
    "  BX      lr               \n" /* return to the preempted AO-thread */

      
    "PendSV_save_ex:            \n"
    "  MRS     r0,PSP           \n" /* r0 := Process Stack Pointer */
                              
    "  ISB                      \n" /* reset pipeline after fetching PSP */
    "  STMDB   r0!,{r4-r11}     \n" /* save r4-r11 on top of the exception frame */
#if (__ARM_FP != 0)                 /* if VFP available... */
    "  TST     lr,#(1 << 4)     \n" /* is it return with the VFP exception frame? */
    "  IT      EQ               \n" /* if lr[4] is zero... */
    "  VSTMDBEQ r0!,{s16-s31}   \n" /* ... save VFP registers s16..s31 */
    "  STMDB   r0!,{r1,lr}      \n" /* save the "aligner" and the EXC_RETURN value */
#endif                              /* VFP available */

    /* store the SP of the current extended-thread */
    "  STR     r0,[r1,#" STRINGIFY(Thread_SP) "]\n" 
    "  MOV     r0,r12           \n"     

    "  CMP     r2,#0            \n"
    "  BEQ     PendSV_restore_ao\n" /* branch if (WSK_Attr.WS_next->ThreadSP == 0) */
    /* otherwise continue to restoring next extended-thread... */
      
    "PendSV_restore_ex:         \n"
#ifdef QXK_ON_CONTEXT_SW
    "  MOVS    r0,r1            \n" /* r0 := WSK_attr.WS_curr */
    "  MOV     r1,r12           \n" /* r1 := WSK_Attr.WS_next */
    "  LDR     r2,[r3,#" STRINGIFY(QXK_IDLE_THR) "]\n" /* r2 := idle thr ptr */
    "  CMP     r0,r2            \n"
    "  BNE     PendSV_onContextSw2 \n" /* if (curr != idle) call onContextSw */
    "  MOVS    r0,#0            \n" 
    "PendSV_onContextSw2:       \n"
    "  LDR     r3,=QXK_onContextSw \n"
    "  BLX     r3               \n" 

    /* restore the AAPCS-clobbered registers after a functin call...  */
    "  LDR     r3,=QXK_attr_    \n"
    "  LDR     r0,[r3,#" STRINGIFY(QXK_NEXT) "]\n" 
    "  LDR     r2,[r0,#" STRINGIFY(QMACTIVE_OSOBJ) "]\n" 
#endif 

    "  STR     r0,[r3,#" STRINGIFY(WSK_CURR) "]\n" /* WSK_Attr.WS_curr := r0 (QXK_Attr.WS_next) */
    "  MOVS    r0,#0            \n"
    "  STR     r0,[r3,#" STRINGIFY(WSK_NEXT) "]\n" /* WSK_Attr.WS_next := 0 */

    /* exit the critical section */
      
    "  MOVS    r1,#0            \n"
    "  MSR     BASEPRI,r1       \n" /* enable interrupts (clear BASEPRI) */
#if (__ARM_FP != 0)                 /* if VFP available... */
    "  LDMIA   r2!,{r1,lr}      \n" /* restore aligner and EXC_RETURN into lr */
    "  TST     lr,#(1 << 4)     \n" /* is it return to the VFP exception frame? */
    "  IT      EQ               \n" /* if lr[4] is zero... */
    "  VLDMIAEQ r2!,{s16-s31}   \n" /* ... restore VFP registers s16..s31 */
#else
    "  ORR     lr,lr,#(1 << 2)  \n" /* make sure PSP is used */
#endif                              /* VFP available */
    "  LDMIA   r2!,{r4-r11}     \n" /* restore r4-r11 from the next thread's stack */


    /* set the PSP to the next thread's SP */
    "  MSR     PSP,r2           \n" /* Process Stack Pointer := r2 */

    "  BX      lr               \n" /* return to the next extended-thread */
    );
}
