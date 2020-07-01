#A RTOS kernel with so many features
A fullt developed kernel for RTOS

#HOW ??
The goal is to develop a fully functional RTOS kernel which is featured as most
RTOS features as Mutex and Semaphores which is inter-therad communication.
It is efficeint as a FreeRTOS.
This kernel is general purpose for now.
This kernel is like on the features of [qpc-framework](https://state-machine.com) library
embedded lessons make it as more likely as the features but i have tried to add more light weight
and remove complexity of the code.
To run this you need to have to start-up code for your Embedded-board i had EK-TM4C123GXL which had
the ARM Cortex-M4 processor with many hardware features. This code can easliy can be compiled on the
IAR workbench it is suitable for for that workbench. If you use as same board as mine you can download
the start-up code from site mentioned above. otherwise can write your own.
This project has some files like promot.c which is not a part of RTOS if you want to check them. And
Luck to you for making your own Kernel!!!..


### Direcotries strcture
  ** include        (source header file for all functions)
  ** src            (source code file for all program)
  ** include/wisu.h (this file can be included to run all the functions of RTOS)

a work in progress by Mani
