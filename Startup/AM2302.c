/*
 * AM2302_reading.c
 *
 *  Created on: 31-08-2021
 *      Author: Gonzalo
 */

#include <xdc/std.h>
#include <stdio.h>
#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>
#include "hal_assert.h"
/* Driver configuration */
#include <ti_drivers_config.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <Startup/AM2302.h>

#include <Profiles/environmentalSensingService.h>
extern GPTimerCC26XX_Handle timerTempHandle; //Handle para timer de temperatura

extern Semaphore_Struct semTempStruct; // Handle y estructura para semaforo para adquisicion de temperatura.
extern Semaphore_Handle semTempHandle;

extern Mailbox_Handle mbxTempHandle;

#pragma DATA_ALIGN(readAM2302TaskStack, 8);
extern Task_Struct readAM2302Task;
static uint8_t readAM2302TaskStack[STACKSIZETEMP];

//extern UART2_Handle uart;
//extern char msg[10];


void ISR_GPIO_23(uint_least8_t index)
{

    uint32_t value = GPTimerCC26XX_getFreeRunValue(timerTempHandle); // Gaurdar valor de timer
    Mailbox_post(mbxTempHandle, &value, BIOS_NO_WAIT); // Postear en mailbox valor de timer obtenido

}

void ISR_Timer_Temp(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask)
{
    GPIO_disableInt (CONFIG_GPIO_23); // Desactivar GPIO ints
    Semaphore_post(semTempHandle);
}

void readAM2302TaskFunc(UArg arg0, UArg arg1)
{

    GPIO_write(CONFIG_GPIO_23, 1); //Set high para partir

    uint32_t temperatura[16];
    uint32_t humedad[17];
    uint32_t checksum[8];
    int valorTemperatura;
    int valorHumedad;
    int valorChecksum;
    int i;

    while (1) {
        GPIO_setConfig(CONFIG_GPIO_23, GPIO_CFG_OUT_STD); // Settear como salida y en HIGH
        GPIO_write(CONFIG_GPIO_23, 1); //Set high para partir

        Task_sleep(1000000/Clock_tickPeriod); // Esperar para volver a partir.



        GPIO_write(CONFIG_GPIO_23, 0); //Set LOW para partir adquisicion.
        GPIO_write(CONFIG_GPIO_LED_0_CONST, 1);
        Task_sleep(5000/Clock_tickPeriod); //Esperar 5 ms para asegurar recepcion de instruccion para partir
        GPTimerCC26XX_start(timerTempHandle);
        GPIO_write(CONFIG_GPIO_23, 1); //Set HIGH para empezar a escuchar AM2302



        //Cambiar GPIO23 a input para leer
        GPIO_setConfig(CONFIG_GPIO_23, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);// Hacer pin input con PU y INT en ambos sentidos.
        GPIO_enableInt(CONFIG_GPIO_23); // Activar Interrupts



        /*Usando semaforos seguimos el flujo de la adquisicion de datos.
        *Se compara el tiempo transcurrido entre cada falling edge del pin de comunicacion del AM2302.
        *Luego de terminar la adquisicion usando hwi se da el semaforo para que el task lo procese y determinar si son 1 o 0.*/


        Semaphore_pend(semTempHandle, BIOS_WAIT_FOREVER);
        GPIO_write(CONFIG_GPIO_LED_0_CONST, 0);

        //Extraer DATOS de queue y convertirlos a bytes
        //Se guardan los valores del timer en el mailbox.
        //Para extraer el valor de la temperatura hay que sacar la diferencia de tiempo.
        //RH data

       /* --- Humedad --- */
       Mailbox_pend(mbxTempHandle, &i, BIOS_NO_WAIT); //eliminar primer timing
       //memcpy(msg, "\r\nRH:   \r\n", 10); //LSB first
       //UART2_write(uart, &msg, 10, NULL);
       for (i=0; i<17; i++){
            //memset(msg,' ', 10);
            Mailbox_pend(mbxTempHandle, &humedad[i],BIOS_NO_WAIT);
            //sprintf(msg,"%d - ", humedad[i]);
            //UART2_write(uart, &msg, 10, NULL);
        }

        /* --- Temperatura --- */
        //memcpy(msg, "\r\nTemp: \r\n", 10); //LSB first
        //UART2_write(uart, &msg, 10, NULL);
        for (i=0; i<16; i++){
            //memset(msg,' ', 10);
            Mailbox_pend(mbxTempHandle, &temperatura[i],BIOS_NO_WAIT);
            //sprintf(msg,"%d - ", temperatura[i]);
            //UART2_write(uart, &msg, 10, NULL);
        }

        /* --- CheckSum --- */ //LSB first
        //memcpy(msg, "\r\nCSum: \r\n", 10); //LSB first
        //UART2_write(uart, &msg, 10, NULL);
        for (i=0; i<8; i++){
            //memset(msg,' ', 10);
            Mailbox_pend(mbxTempHandle, &checksum[i],BIOS_NO_WAIT);
            //sprintf(msg,"%d - ", checksum[i]);
            //UART2_write(uart, &msg, 10, NULL);
        }






        /* --- Calcular valores --- */
        valorChecksum = 0;
        valorHumedad = 0;
        valorTemperatura = 0;

        //Humedad
        for (i = 1; i<17; i++){ //Calcular distancia temporal para ver si es un 1 o 0 en binario y pasarlo a decimal.
            if (humedad[i]-humedad[i-1]>=AM2302THRESH){
                valorHumedad += (1<<16-i); // 1<<16-i es equivalente a decir 2^(16-i)
            }
        }
        EnvironmentalSensing_SetParameter( ENVIRONMENTALSENSING_RELATIVEHUMIDITY_ID, 2, &valorHumedad );
        //memcpy(msg, "\r\nRH:   \r\n", 10); //Print value
        //UART2_write(uart, &msg, 10, NULL);
        //memset(msg,' ', 10);
        //sprintf(msg,"%i", valorHumedad);
        //UART2_write(uart, &msg, 10, NULL);

        //Temperatura

        for(i = 1; i<16; i++){ // Calcular distancia temporal para ver si es un 1 o 0 en binario y pasarlo a decimal.
            if (temperatura[i]-temperatura[i-1]>=AM2302THRESH){
                valorTemperatura += (1<<15-i);
            }
        }
        if (temperatura[0] - humedad[16] >= AM2302THRESH){
            valorTemperatura *= -1; // Ver si la temperatura es negativa o positiva.
        }

        //Actualizar valor atributo BLE

        EnvironmentalSensing_SetParameter( ENVIRONMENTALSENSING_TEMPERATURECELSIUS_ID, 2, &valorTemperatura );

        //memcpy(msg, "\r\nTemp: \r\n", 10); //Print value
        //UART2_write(uart, &msg, 10, NULL);
        //memset(msg,' ', 10);
        //sprintf(msg,"%i", valorTemperatura);
        //UART2_write(uart, &msg, 10, NULL);

        //Checksum
        if (checksum[0] - temperatura[15] >=AM2302THRESH){
            valorChecksum += (1<<7);
        }
        for (i = 1; i<8; i++){ //Calcular distancia temporal para ver si es un 1 o 0 en binario y pasarlo a decimal.
            if (checksum[i]-checksum[i-1]>=AM2302THRESH){
                valorChecksum += (1<<7-i);
            }
        }








    }
}

void init_Timer_Temp(){ // Iniciar timer para medicion de temperatura con AM2302
    GPTimerCC26XX_Params timerTempParams;
    GPTimerCC26XX_Params_init(&timerTempParams);
    timerTempParams.width = GPT_CONFIG_32BIT;
    timerTempParams.mode = GPT_MODE_ONESHOT;
    timerTempParams.direction = GPTimerCC26XX_DIRECTION_UP;
    timerTempParams.matchTiming = GPTimerCC26XX_SET_MATCH_NEXT_CLOCK;
    timerTempParams.debugStallMode = GPTimerCC26XX_DEBUG_STALL_ON;
    timerTempHandle = GPTimerCC26XX_open(CONFIG_GPTIMER_0_CONST, &timerTempParams);

    while(timerTempHandle == NULL);
    GPTimerCC26XX_setLoadValue(timerTempHandle, 48000000);
    GPTimerCC26XX_registerInterrupt(timerTempHandle, ISR_Timer_Temp, GPT_INT_TIMEOUT); //Register INT
    GPTimerCC26XX_enableInterrupt(timerTempHandle, GPT_INT_TIMEOUT); // Enable INT
}

void init_mbx_Temp(){ // Iniciar mailbox para adquisicion de temperatura con AM2302
    Mailbox_Params mbxParams;
    Mailbox_Params_init(&mbxParams);
    mbxTempHandle = Mailbox_create(sizeof(uint32_t), 42, &mbxParams, NULL);
}

void init_sem_Temp(){ // Iniciar semaforo para adquisicion de temperatura con AM2302
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semTempStruct, 0, &semParams);
    semTempHandle = Semaphore_handle(&semTempStruct);
}

void init_task_Temp(){ // Iniciar task para adquisicion de temperatura con AM2302 para TIRTOS
    Task_Params readAM2302TaskParams;
    Task_Params_init(&readAM2302TaskParams);
    readAM2302TaskParams.stackSize = STACKSIZETEMP;
    readAM2302TaskParams.priority = 1;
    readAM2302TaskParams.stack = &readAM2302TaskStack;
    Task_construct(&readAM2302Task, readAM2302TaskFunc, &readAM2302TaskParams, NULL);
}

void init_GPIO_Temp(){ // Iniciar GPIO para adquisicion de temperatura con AM2302
    GPIO_setConfig(CONFIG_GPIO_23, GPIO_CFG_OUT_STD); // Settear al principio como salida
    GPIO_setCallback(CONFIG_GPIO_23, ISR_GPIO_23); // Funcion para interrupt del pin
}
