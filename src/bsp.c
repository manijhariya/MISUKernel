/* Board Support Package (BSP) for the EK-TM4C123GXL board */
#include <stdint.h>  /* Standard integers. WG14/N843 C99 Standard */

#include "bsp.h"
#include "wisu.h"
#include "wisuMutex.h"

static uint32_t volatile l_tickCtr;

static WSMutex Morse_mutex;

void SysTick_Handler(void) {
    WSThread_tick();
    __disable_irq();
     WSThread_sched();
    __enable_irq();

}

void BSP_init(void) {
    SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF */
    SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
    GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN);
    
    GPIOF_AHB->DIR &= ~(BTN_SWI);
    GPIOF_AHB->DEN |= (BTN_SWI);
    GPIOF_AHB->PUR |= (BTN_SWI);

    GPIOF_AHB->IS &= ~(BTN_SWI);
    GPIOF_AHB->IBE &= ~(BTN_SWI);
    GPIOF_AHB->IEV &= ~(BTN_SWI);
    GPIOF_AHB->IM |= (BTN_SWI);   
      
   WSMutex_init(&Morse_mutex, 6U);   
}

void BSP_ledRedOn(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = LED_RED;
}

void BSP_ledRedOff(void) {
    GPIOF_AHB->DATA_Bits[LED_RED] = 0U;
}

void BSP_ledBlueOn(void) {
    //GPIOF_AHB->DATA_Bits[LED_BLUE] = LED_BLUE;
    GPIOF_AHB->DATA ^= LED_BLUE;
}

void BSP_ledBlueOff(void) {
    GPIOF_AHB->DATA_Bits[LED_BLUE] = 0U;
}

void BSP_ledGreenOn(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = LED_GREEN;
}

void BSP_ledGreenOff(void) {
    GPIOF_AHB->DATA_Bits[LED_GREEN] = 0U;
}
void BSP_allOff(void) {
  BSP_ledRedOff();
  BSP_ledBlueOff();
  BSP_ledGreenOff();
}

void BSP_sendMorseCode(uint32_t bitmask) {
    uint32_t volatile delay_ctr;
    enum { DOT_DELAY = 150 };
    //MUTEX
    WSMutex_lock(&Morse_mutex,
                 WISU_TIMEOUT_WAITING_); /* timeout for waiting */
    
    for (; bitmask != 0U; bitmask <<= 1) {
        if ((bitmask & (1U << 31)) != 0U) {
            BSP_ledGreenOn();
        }
        else {
            BSP_ledGreenOff();
        }
        for (delay_ctr = DOT_DELAY;
             delay_ctr != 0U; --delay_ctr) {
        }
    }
    BSP_ledGreenOff();
    for (delay_ctr = 7*DOT_DELAY;
         delay_ctr != 0U; --delay_ctr) {
    }

    //MUTEX
    WSMutex_unlock(&Morse_mutex);
}

void WS_onAssert(char const *module, int loc) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)loc;    /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}
void Q_onAssert(char const *module,int loc){
  (void)module;
}
