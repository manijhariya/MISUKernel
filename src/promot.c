#include "uart.h"
#include <string.h>
void returnKey();
void checkCmd(const char* cmd);
void LED_program();

void promotSetup(void){
  UART_init();
  returnKey();
  char currentChar;
  while(1){
    char cmd[256] = {'\0'};
    int cur_len=0;
    for(;strlen(cmd)<256;) {
		currentChar = readChar();
                if(currentChar == '\r'){      //return processing
                    printChar('\r');
                    printChar('\n');
                    checkCmd(cmd);
                    returnKey();
                    break;
                }
                else{
                  if(currentChar == 0x7F){   //backspace processing
                    if(cur_len <= 0){}
                    else{
                      cur_len-=1;
                      cmd[cur_len] = '\0';
                      printChar(currentChar);
                    }
                  }
                  else{
                    cmd[cur_len] = currentChar;
                    cur_len+=1;
                    printChar(currentChar);
                  } 
                }
              //  cur_len = strlen(cmd);
	}
    //  printString("Not enough space (buffer is full for the current command)");
  }
}
void returnKey(){
  printString("root@powermcu:");
}
void checkCmd(const char* cmd){
  // got the command to check in this function if it is in the OS or not !! 
  if(!strcmp(cmd,("whoami"))){
    printString("This is PowerMCU\r\n");
    return;
  }  
  if(!strcmp(cmd,("help"))){
     printString("Working on commands\r\n");
     return;
  }   
  if(!strcmp(cmd,("ledcall"))){
     LED_program();
     return;
  }   
  if(!strcmp(cmd,("exit"))){
     return;//to exit from whole function but it is a mcu so just going to return it
  }
  else{
     printString("Command Not found please try another Command Or for more commands try:\r\n\r\nhelp or cmd --help \r\n");
      return;
  }
}
void LED_program(){
  char c;
  printString("Welcome to LEDGlow Program::\r\nPress 'q' to exit\r\n");
  while(1)
  {
    printString("Enter r g or b:\r\n");
    c = readChar();
    printChar(c);
    printChar('\r');
    printChar('\n');
    switch(c){
    case 'r':
        BSP_allOff();
        BSP_ledRedOn();
        break;
    case 'b':
        BSP_allOff();
        BSP_ledBlueOn();
        break;
    case 'g':
        BSP_allOff();
        BSP_ledGreenOn(); 
        break;
    case 'q':
        BSP_allOff();
        return;
    default: 
        BSP_allOff();
        break;
    }
  }
}