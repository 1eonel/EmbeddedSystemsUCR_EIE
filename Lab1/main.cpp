#include "msp.h"
#include </home/cerva08/Documents/Incrustados/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/driverlib.h>
#include "HAL_I2C.h"
#include "HAL_OPT3001.h"

#include <stdint.h>
#include <stdbool.h>
#include <functions.h>
#include <handler.h>



/**
 * main.cpp
 */


/* Global Variables */

int g_isample_count = 0;
int g_isound_count = 0;
int g_iADC14Result = 0U;
int g_iverify = 0;
int g_ilights = 1;

float g_fstoreavg = 0.0;
float g_fMeasureLux = 0.0;
float g_fADCResult = 0.0;
float g_fsample_avg = 0.0;
float g_favg = 0.0;
float g_vfsound[5];
float g_vfsamples[10];
float g_fkid_noise = 0.0;


    void main(void)
    {
        /* Initial Conditions */
        P2->OUT = 0;
        P5->OUT = 0;
        SetUp();
        __disable_irq();
        /*Select lights with user input*/
        LampsSelection();
        __enable_irq();

        /*Initial Blink*/
        InitialBlink();
        g_fMeasureLux = OPT3001_getLux();

        /*Initial State*/
        InitialState();
        NVIC_SetPriority(PORT4_IRQn, 0);
        NVIC_EnableIRQ(PORT4_IRQn);

        /**/
        while (1){
            g_fMeasureLux = OPT3001_getLux();

            if (g_fMeasureLux > 30){ // Room is not dark

                TIMER32_1->LOAD = 0x000249F0; //0.05s —-> 3Mhz
                NVIC_SetPriority(T32_INT1_IRQn, 3);
                NVIC_EnableIRQ(T32_INT1_IRQn);
                /*ADC14*/
                NVIC_SetPriority(ADC14_IRQn, 3);
                NVIC_EnableIRQ(ADC14_IRQn);
            }
        }
    }
//
