# A RTOS kernel with many features
A fully developed kernel for RTOS

# HOW ??
The goal is to develope a fully functional RTOS kernel which is featured as most
RTOS features such as Mutex and Semaphores which are type of inter-therad communication.
This RTOSKernel is efficeint as a FreeRTOS Kernel.
This kernel is general purpose for now.
This kernel is consist of features like [qpc-framework](https://state-machine.com) library
have.MISUKernel have more light weight and it also remove complexity of the code in qpc-framework.
To run this you need to have start-up code for your Embedded-board. I had EK-TM4C123GXL which has
ARM Cortex-M4 processor with many hardware features. This code can easliy can be compiled on the
IAR workbench it is suitable for that workbench. You can download
the start-up code for EK-TM4C123GXL mcu from site mentioned above. otherwise you can write your own.
This project has some files like promot.c which is not a part of RTOS you can also check them. Good
Luck to you for making your own Kernel!!!..


### Direcotries strcture
  ** include        (source header file for all functions)
  ** src            (source code file for all program)
  ** include/wisu.h (this file can be included to run all the functions of RTOS)

a work in progress by Mani
