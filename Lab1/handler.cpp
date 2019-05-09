#include "msp.h"
#include </home/cerva08/Documents/Incrustados/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/driverlib.h>
//#include </home/leo/Dropbox/UCR/EmbeddedSystems/IE-1119/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/driverlib.h>
#include "HAL_I2C.h"
#include "HAL_OPT3001.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "handler.h"



/****** Interruptions Handler ******/
extern "C"
{
//Resets ADC reads
void T32_INT1_IRQHandler(void)
{
    __disable_irq();
    TIMER32_1->INTCLR = 0U;
    P2->OUT ^= BIT1 + BIT2;  //P2.1 y P2.2
    ADC14->CTL0 = ADC14->CTL0 | ADC14_CTL0_SC; // Start
    __enable_irq();
    return;
}

// Detects new sample
void ADC14_IRQHandler(void)
{
    __disable_irq();
    g_iADC14Result = 8000 - ADC14->MEM[0];
    g_iADC14Result = abs(g_iADC14Result); //valor absoluto
    g_fADCResult = float(g_iADC14Result); //parse

    ADCSamples();
    ADC14->CLRIFGR0 = ADC14_CLRIFGR0_CLRIFG0;
    __enable_irq();
    return;
}

// Turns off lamps after a given time
void T32_INT2_IRQHandler(void)
{
    __disable_irq();
    TIMER32_2->INTCLR = 0U;
    __enable_irq();

    if (g_fkid_noise > g_fstoreavg)
    {
        if (g_fMeasureLux < 15)
        {   //Noise & Dark room
            TIMER32_2->LOAD = 0x055D4A80; //~30s ---> a 3Mhz
            NVIC_SetPriority(T32_INT2_IRQn, 2);
            NVIC_EnableIRQ(T32_INT2_IRQn);
            return;
        }
        else
        {   //Noise and Light room
            g_iverify = 0;
            LightsOff();
            return;
        }
    }
    //Silence
    g_iverify = 0;
    LightsOff();
    return;
}


/* Joystick Button interruption
 *
 * with lights on = turn off
 * with lights off = turn on 30 seconds
 */
void PORT4_IRQHandler(void){

    __disable_irq();
    P4->IFG &= ~BIT1; //button interruption enb
    __enable_irq();

    //If lights are on -> mini wait  and turn off
    if (P5->OUT & BIT6 || P2->OUT & BIT4 || P2->OUT & BIT6)
    {
        g_fkid_noise = 0.0;
        TIMER32_2->LOAD = 0x000004000;
        NVIC_SetPriority(T32_INT2_IRQn, 2);
        NVIC_EnableIRQ(T32_INT2_IRQn);
    }
    else
    {
        //If none of the lights are on -> turn on for 30 seconds
        LightsOn();
        TIMER32_2->LOAD = 0x055D4A80;
        NVIC_SetPriority(T32_INT2_IRQn, 2);
        NVIC_EnableIRQ(T32_INT2_IRQn);
    }
}
}
