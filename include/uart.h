//Clkdiv = 16
//Baudrate = 9,600
//Clkrate = 20,000,000
#ifndef __UART_H__
#define __UART_H__
#define UART (*((unsigned int *)(0x400FE618)))
#include "lm4f120h5qr.h"

void UART_init();
char readChar(void);
void printChar(char c);
void printString(char *string);
#endif