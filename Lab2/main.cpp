//#include "msp.h"
#include <CalcDiff.hpp>
#include <ColorRect.hpp>
#include <LcdDriver/Crystalfontz128x128_ST7735.hpp>
#include "main.hpp"
#include "Scheduler.hpp"
#include "Task.hpp"
#include "LED.hpp"

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>
#include "Defines.hpp"

// ##########################
// Global/Static declarations
// ##########################

uint8_t Task::m_u8NextTaskID = 1;             // - Init task ID (static variable)
volatile static uint64_t g_SystemTicks = 0;   // - The system counter.
Scheduler g_MainScheduler;                    // - Instantiate a Scheduler

/* Graphic library context */
Graphics_Context g_sContext;
/* ADC results buffer */
uint64_t resultsBuffer[3];

Graphics_Rectangle g_rect;

// #########################
//          MAIN
// #########################

void main(void)
{
    /* Halting WDT and disabling master interrupts */
        MAP_WDT_A_holdTimer();
        MAP_Interrupt_disableMaster();

        /* Set the core voltage level to VCORE1 */
        MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

        /* Set 2 flash wait states for Flash bank 0 and 1*/
        MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
        MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

        /* Initializes Clock System */
        MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
        MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

        /* Initializes display */
        Crystalfontz128x128_Init();

        /* Set default screen orientation */
        Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

        /* Initializes graphics context */
        Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

        g_rect.xMin = 0;
        g_rect.yMin =0;
        g_rect.xMax = 128;
        g_rect.yMax = 64;



    // - Instantiate two new Tasks
        ColorRect color(g_sContext, g_rect);
        CalcDiff calcdiff(1);

    //*************************************************************************
      //Memory assigns for each task
    //*************************************************************************
      color.m_pWTaskMsj = g_MainScheduler.AssignMemoryToTask(3U);
      calcdiff.m_pWTaskMsj = g_MainScheduler.AssignMemoryToTask(3U);


      // - Run the set up instructions
      Setup();


      // - Attach the Tasks to the Scheduler;
      g_MainScheduler.attach(OneShot,&color, 500);
      g_MainScheduler.attach(Periodic,&calcdiff, 100);

      // - Run the Setup for the scheduler and all tasks
      g_MainScheduler.setup();


      // - Main Loop
      while(1)
      {
          __wfe(); // Wait for Event
          if(g_SystemTicks != g_MainScheduler.m_u64ticks)
          {
              //- Only execute the tasks if one tick has passed.
              g_MainScheduler.m_u64ticks = g_SystemTicks;
              color.SendMessage(resultsBuffer, 2);
              g_MainScheduler.run();
          }
      }

}
// Setup function for the application
// @input - none
// @output - none
// **********************************
void Setup(void)
{
    // ****************************
    //         DEVICE CONFIG
    // ****************************
    // - Disable WDT
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // ****************************
    //         PORT CONFIG
    // ****************************
    // - P1.0 is connected to the Red LED
    // - This is the heart beat indicator.
    P1->DIR |= BIT0; //Red LED

    // ****************************
    //       TIMER CONFIG
    // ****************************
    // - Configure Timer32_1  with MCLK (3Mhz), Division by 1, Enable the interrupt, Periodic Mode
    // - Enable the interrupt in the NVIC
    // - Start the timer in UP mode.
    // - Re-enable interrupts

    TIMER32_1->LOAD = TIMER32_COUNT; //~1ms ---> a 3Mhz
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_PRESCALE_0 | TIMER32_CONTROL_MODE | TIMER32_CONTROL_IE | TIMER32_CONTROL_ENABLE;
    NVIC_SetPriority(T32_INT1_IRQn,1);
    NVIC_EnableIRQ(T32_INT1_IRQn);
    __enable_irq();


    Graphics_fillRectangle(&g_sContext, &g_rect);

    /* Configures Pin 4.0, 4.2, and 6.1 as ADC input */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);


    /* Initializing ADC (ADCOSC/64/8) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8,0);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat)
     *  with internal 2.5v reference */

    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);

    MAP_ADC14_configureConversionMemory(ADC_MEM0,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

    MAP_ADC14_configureConversionMemory(ADC_MEM1,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

    MAP_ADC14_configureConversionMemory(ADC_MEM2,
            ADC_VREFPOS_AVCC_VREFNEG_VSS,
            ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

    /* Enabling the interrupt when a conversion on channel 2 (end of sequence)
        *  is complete and enabling conversions */
       MAP_ADC14_enableInterrupt(ADC_INT2);

       /* Enabling Interrupts */
       MAP_Interrupt_enableInterrupt(INT_ADC14);
       MAP_Interrupt_enableMaster();

       /* Setting up the sample timer to automatically step through the sequence
        * convert.
        */
       MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

       /* Triggering the start of the sample */
       MAP_ADC14_enableConversion();
       MAP_ADC14_toggleConversionTrigger();



    return;
}

extern "C"
{
    // - Handle the Timer32 Interrupt
    void T32_INT1_IRQHandler(void)
    {
        TIMER32_1->INTCLR = 0U;
        P1->OUT ^= BIT0; // - Toggle the heart beat indicator (1ms)
        g_SystemTicks++;
        return;
    }



void ADC14_IRQHandler(void)
{
    __disable_irq();
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    /* Checks for ADC_MEM2 conversion completed */
    if(status & ADC_INT2)
    {
        /* Store ADC14 conversion results */
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        resultsBuffer[2] = ADC14_getResult(ADC_MEM2);
    }
    __enable_irq();
}
}
