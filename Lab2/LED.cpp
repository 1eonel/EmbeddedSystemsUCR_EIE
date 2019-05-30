#include "LED.hpp"

LED::LED(uint16_t i_BITN, uint8_t Set_Priority)
{
    m_u8Priority = Set_Priority;
    m_u16BITN = i_BITN;
}

uint8_t LED::run()
{

    //#########################
    // Blink code Assuming PORT2
    P2->OUT ^= m_u16BITN;
    //#########################
    return(NO_ERR);
}

uint8_t LED::setup()
{
    //LED Setup, assuming PORT2
    // - P2.0 is connected to the RGB LED

    P2->DIR |= m_u16BITN; //Red LED
    P2->OUT &= m_u16BITN; // Initialize the LED Value
 //   Task::m_pTaskMsj = i_Scheduler.AssignMemoryToTask ;
    return(NO_ERR);
}

