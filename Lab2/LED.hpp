/*
 * LED.hpp
 *
 *  Created on: Aug 31, 2016
 *      Author: eortiz
 */

#ifndef LED_HPP_
#define LED_HPP_
#define __NOP __nop
#include "Scheduler.hpp"
#include <ti/devices/msp432p4xx/inc/msp.h>
//#include "msp.h"
#include "Task.hpp"
//#include "Scheduler.cpp"

class LED : public Task
{
    public:
        LED(uint16_t i_BITN, uint8_t Set_Priority);
        uint16_t m_u16BITN;
        virtual uint8_t run(void);
        virtual uint8_t setup(void);
    protected:
    private:
};

#endif /* LED_HPP_ */
