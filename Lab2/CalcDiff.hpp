#ifndef CalcDiff_HPP_
#define CalcDiff_HPP_

//#define __NOP __nop
//#include "msp.h"
#include "Defines.hpp"
#include "Task.hpp"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.hpp"
#include <stdio.h>
#include "Defines.hpp"
#include "Scheduler.hpp"
//#include "msp.h"
#include "Task.hpp"


class CalcDiff : public Task
{
    public:
        CalcDiff(uint16_t i_BITN);
        //Paint(uint16_t);
        uint16_t m_u16BITN;
        uint64_t g_u8OldLine;
        virtual uint8_t run(void);
        virtual uint8_t setup(void);
    protected:
    private:
};

#endif /* CalcDiff_HPP_ */
