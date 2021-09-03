/*
 * AM2302.h
 *
 *  Created on: 02-09-2021
 *      Author: Gonzalo
 */

#ifndef AM2302_H_
#define AM2302_H_

#define STACKSIZETEMP 2048 // Tamaño de stack para task

#define AM2302THRESH 5000 //Threshold para 1 o 0 de AM2302
//0 son ~80us y 1 ~120us. Como el clock es 48MHz:
//80*48 = 3840 ticks.
//120*48 = 5760 ticks

void ISR_GPIO_23(uint_least8_t index);

void ISR_Timer_Temp(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask);

void readAM2302TaskFunc(UArg arg0, UArg arg1);

void init_Timer_Temp();

void init_mbx_Temp();

void init_sem_Temp();

void init_task_Temp();

void init_GPIO_Temp();
#endif /* AM2302_H_ */
