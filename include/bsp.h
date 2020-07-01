#ifndef __BSP_H__
#define __BSP_H__

#include "wisu.h"
#include "wisuStd.h"

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 1000U
/* on-board LEDs */
#define LED_RED   (1U << 1)
#define LED_BLUE  (1U << 2)
#define LED_GREEN (1U << 3)
#define BTN_SWI   (1U << 4)

//extern WXSema SWI_sema;

void GPIOPortF_IRQHandler(void);
void BSP_init(void);

void BSP_ledRedOn(void);
void BSP_ledRedOff(void);

void BSP_ledBlueOn(void);
void BSP_ledBlueOff(void);

void BSP_ledGreenOn(void);
void BSP_ledGreenOff(void);

void BSP_allOff(void);

void BSP_sendMorseCode(uint32_t bitmask);

void WS_onAssert(char const *module, int loc);
#endif // __BSP_H__
