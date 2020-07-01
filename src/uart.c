#include "uart.h"

void UART_init(){
  UART |= (1<<0);
  SYSCTL_RCGCGPIO_R |= (1<<0);

  GPIO_PORTA_DEN_R |= (1<<0) | (1<<1);
  GPIO_PORTA_AFSEL_R |= (1<<1) | (1<<0);
  GPIO_PORTA_PCTL_R = (1<<0) | (1<<4);
  
  UART0_CTL_R |= (1<<0);
  UART0_IBRD_R = 104;
  UART0_FBRD_R = 11;
  UART0_LCRH_R = (0x3 << 5);
  UART0_CC_R = 0x0;
  UART0_CTL_R = (1<<0) | (1<<9) | (1<<8);
}

char readChar(void){
  char c;
  while(!(UART0_FR_R & (1<<6)));
  c = UART0_DR_R;
  return c;
}
void printChar(char c){
  while(!(UART0_FR_R & (1<<7)));
  UART0_DR_R = (c);
  return;
}
void printString(char * string)
{
  while(*string)
  {
    printChar(*(string++));
  }
}