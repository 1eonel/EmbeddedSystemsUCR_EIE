/*
 * functions.cpp
 *
 *  Created on: 
 *      Author:
 */

#include "msp.h"
#include </home/cerva08/Documents/Incrustados/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/driverlib.h>
//#include </home/leo/Dropbox/UCR/EmbeddedSystems/IE-1119/msp432_driverlib_3_21_00_05/driverlib/MSP432P4xx/driverlib.h>
#include "HAL_I2C.h"
#include "HAL_OPT3001.h"

#include <stdint.h>
#include <stdbool.h>
/***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "handler.h"
/***************************************************************/


/*
 * LampSelection function: Green(1), Red(2), or Blue(3)
 * Buttons:
 * S1 = color selection
 * S2 = end color selection
 * Enters selection mode until S2 es pressed
*/
void LampsSelection(){
    int l_icounter = 0;
    bool l_bSelecMode = true;
    /*Enter Selection Mode*/
    while (l_bSelecMode)
    {
        /* Button S1 */
        if (!(P5IN & GPIO_PIN1)) //P5.1 input
        {
            /* Button delay (para detectar el toque) */
            while (l_icounter <1000){
                l_icounter++;
            }
            l_icounter = 0;
            g_ilights++;
            //XORS para cambiar estados, se enciente y se apaga
            switch (g_ilights){
                case 1: /*Green*/
                    P2->OUT = BIT4;
                    __delay_cycles(1100000);
                    P2->OUT ^= BIT4;
                    break;
                case 2: /*Red*/
                    P2->OUT = BIT6;
                    __delay_cycles(1100000);
                    P2->OUT ^= BIT6;
                    break;
                case 3: /*Blue*/
                    P5->OUT = BIT6;
                    __delay_cycles(1100000);
                    P5->OUT ^= BIT6;
                    break;
                case 4: /*Restart light count*/
                    g_ilights = 0;
                    break;
            }
        }
        /* Button S2 */
        if (!(P3IN & GPIO_PIN5)) /*P3.5* input*/
        {
            l_bSelecMode = false; /*Selection mode ends*/
        }
    }
}

void SetUp(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer


    /*Sound Vector Initializing*/
    for (int i = 0; i < 5; i++){
        g_vfsound[i] = 400.0;
    }


    //Pins as OUTPUTS, P2.4, P2.6 & P5.6
    P2->DIR = BIT4 | BIT6;
    P5->DIR = BIT6;


    //Configure P4.3 (Mic) for ADC (As analogic input)
    P4->SEL0 = BIT3;
    P4->SEL1 = BIT3;
    P4->DIR &= ~BIT3; /*0 only on position 3, keeps other bits intact (MASK)*/

    /*Hacer sin Driver Lib!!*/
    MAP_GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN5);
    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN1);
    MAP_GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN1);

    /*Sets up Interruptions on P4.1*/
    P4->IE |= BIT1; /*P4.1 interrupt enabled*/
    P4->IES |= BIT1; /*P4.1 Hi/lo edge*/
    P4->IFG &= ~BIT1;

    // Initialize I2C communication
    Init_I2C_GPIO();
    I2C_init();

    // Initialize OPT3001 digital light sensor
    OPT3001_init();



    /*TIMER32_2 in one shot mode*/
    TIMER32_2->CONTROL = TIMER32_CONTROL_ONESHOT | TIMER32_CONTROL_SIZE
                | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE
                | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;

    /*TIMER32_1 in rep mode*/
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0
                    | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE
                    | TIMER32_CONTROL_ENABLE;

    //ADC Configuration
        /*Control 0*/
        ADC14->CTL0 = ADC14_CTL0_PDIV_0 | /*Predivider: predivides clk source by 1*/
                      ADC14_CTL0_SHS_0 | /*sample-and-hold source select*/
                      ADC14_CTL0_DIV_7  | /*clk divider: divides by 7*/
                      ADC14_CTL0_SSEL__MCLK | /*clk source select = MCLK*/
                      ADC14_CTL0_SHT0_1 | /*8=number of ADC14CLK cycles in the sampling period for registers*/
                      ADC14_CTL0_ON | /*ADC14 on*/
                      ADC14_CTL0_SHP; /*sample-and-hold pulse-mode. source of the sampling signal= from sampling timer*/

        /*Memory Control 0*/
        ADC14->MCTL[0] = ADC14_MCTLN_INCH_10 | /*Input channel*/
                         ADC14_MCTLN_VRSEL_0; /*combinations of V(R+) and V(R-) srcs & buffer selection and buffer on or off. = V(R+) = AVCC, V(R-) = AVSS*/

        ADC14->CTL0 = ADC14->CTL0 |
                      ADC14_CTL0_ENC; /*Can be modified only when ADC14ENC = 0.*/

        ADC14->IER0 = ADC14_IER0_IE0; /*Interrupt enable.*/

        __delay_cycles(100000); //POTENCIALMENTE BORRARLO
}



/*
 *Initial 3 blinks
 */
void InitialBlink(){
    /*Repeat 6 times for 3 ons & 3 offs = 3blinks*/
    for(int l_iIndex = 0;l_iIndex<6;l_iIndex++){
        // P2.6 y P5.6
        //XOR for blink
        P2->OUT ^= BIT6;
        P5->OUT ^= BIT6;
        __delay_cycles(1100000);
    }
}
/*
 * Determines Initial State:
 *
 * If Lux is under 30 and room is dark, -> Lights are turned on
 *
 * Else, room is not dark -> do nothing.
 */
void InitialState(){

    if (g_fMeasureLux < 30){
        LightsOn();
        TIMER32_2->LOAD = 0x055D4A80; //30s,3Mhz
        NVIC_SetPriority(T32_INT2_IRQn, 2); //
        NVIC_EnableIRQ(T32_INT2_IRQn);

        TIMER32_1->LOAD = 0x000249F0; //0.05s,3Mhz
        NVIC_SetPriority(T32_INT1_IRQn, 3);
        NVIC_EnableIRQ(T32_INT1_IRQn);
        NVIC_SetPriority(ADC14_IRQn, 3);
        NVIC_EnableIRQ(ADC14_IRQn);
    }
}

/*
 * Lights On
 */

void LightsOn()
{
    /*Turns on lights*/
    switch (g_ilights){
    case 1: //P2.4 Green
        P2->OUT = BIT4;
        break;
    case 2: //P2.6 Red
        P2->OUT = BIT6;
        break;
    case 3: //P5.6 Blue
        P5->OUT = BIT6;
        break;
    }
}

void LightsOff(){
    /*Turns  Lights off*/
    switch (g_ilights){
    case 1:
        P2->OUT = 0;
        break;
    case 2:
        P2->OUT = 0;
        break;
    case 3:
        P5->OUT = 0;
        break;
    }
}

/*
 * ADC Samples
 */
void ADCSamples(){

    /*Stores ADC Samples in Vector and calculates AVG*/

    g_vfsamples[g_isample_count] = g_fADCResult;
    g_isample_count++;
    g_fsample_avg += g_fADCResult;

    if (g_isample_count > 20){
        g_fsample_avg = g_fsample_avg / 20;
        g_isample_count = 0;

        for (int j = 0; j < 5; j++){
            g_fkid_noise += g_vfsound[j];
        }
        g_fkid_noise = g_fkid_noise / 5.0;
        Sound();
    }
}



/* Sound
 *
 * Stores ADCSamples in vector
 * Calcs avg of 4 oldest samples
 * Oldest sample is eliminated
 * if, last sample is grater than the last 4 and room is dark: (kid is
 *      Activate ADC interruption (Lights On)
 */
void Sound(){



    for (g_isound_count = 0; g_isound_count < 4; g_isound_count++){

        g_vfsound[4 - g_isound_count] = g_vfsound[3 - g_isound_count]; //Shift samples in vector

        g_favg += g_vfsound[4 - g_isound_count]; // sum of sound samples
    }

    g_vfsound[0] = g_fsample_avg; // Store newest sample in pos [0]

    g_favg = g_favg / 5.0; // Average of last 5 samples


    //Newest Sample vs Average Comparation:

    if (g_vfsound[0] >  1.5*g_favg)
    {
        LightsOn();

        if (g_iverify == 0){ /* verify = off, activates lights for 30 seconds */
            g_fstoreavg =  1.5*g_favg;
            TIMER32_2->LOAD = 0x055D4A80; // 30s
            NVIC_SetPriority(T32_INT2_IRQn, 2);
            NVIC_EnableIRQ(T32_INT2_IRQn);
        }
        g_iverify++;
    }
}
