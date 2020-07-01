#include "wisu.h"

void GPIOPortF_IRQHandler();
void main_blinky2();

uint32_t stack_blinky2[40];
WSThread blinky2;

uint32_t stack_IRQHandler[40];
static WSSema SWI_sema;

WSThread IRQHandler;

uint32_t stack_blinky1[40];
WSThread blinky1;
void main_blinky1() {
    while (1) {
        /*uint32_t volatile i;
        for (i = 1500U; i != 0U; --i) {
            BSP_ledGreenOn();
            BSP_ledGreenOff();
        }*/
        BSP_sendMorseCode(0xA8EEE2A0U); /* "SOS" */
        WSThread_delay(1U); /* block for 1 tick */
    }
}

uint32_t stack_blinky3[40];
WSThread blinky3;
void main_blinky3() {
    while (1) {
        /*BSP_ledRedOn();
        WSThread_delay(BSP_TICKS_PER_SEC / 3U);
        BSP_ledRedOff();
        WSThread_delay(BSP_TICKS_PER_SEC * 3U / 5U); */
        BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
        BSP_sendMorseCode(0xE22A3800U); /* "TEST" */
        WSThread_delay(5U);
    }
}

int main() {
    BSP_init();    
    WSSema_init(&SWI_sema,
                0U,
                1U);
    
    /* start blinky1 thread */
     WSThread_start(&blinky1,
                   5U, /* priority */
                   6U,
                   &main_blinky1,
                   stack_blinky1, sizeof(stack_blinky1));
    
    WSThread_start(&IRQHandler,
                   4U, /* priority */
                   1U,
                   &GPIOPortF_IRQHandler,
                   stack_IRQHandler, sizeof(stack_IRQHandler));

    /* start blinky2 thread */
      WSThread_start(&blinky2,
                    3U, /* priority */
                    4U,
                    &main_blinky2,
                    stack_blinky2, sizeof(stack_blinky2));

    /* start blinky3 thread */
    WSThread_start(&blinky3,
                   1U, /* priority */
                   1U,
                   &main_blinky3,
                   stack_blinky3, sizeof(stack_blinky3));
    
    /* transfer control to the RTOS to run the threads */
    WS_run();
}
void GPIOPortF_IRQHandler(void) {
  while(1){
    if((GPIOF_AHB->RIS & BTN_SWI) != 0U) {
        WSsema_signal(&SWI_sema);
    }
      GPIOF_AHB->ICR = 0xFFU;
      WSThread_delay(2U);
  }
}

void main_blinky2() {
    while (1) {
        uint32_t volatile i;
        WSSema_wait(&SWI_sema,WISU_TIMEOUT_WAITING_);
        for (i = 3*1500U; i != 0U; --i) {
            BSP_ledBlueOn();
            BSP_ledBlueOff();
        }
        WSThread_delay(50U); /* block for 50 ticks */
    }
}