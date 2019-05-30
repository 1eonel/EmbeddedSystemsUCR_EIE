#ifndef ColorRect_HPP_
#define ColorRect_HPP_

//#define __NOP __nop
#include <ti/devices/msp432p4xx/inc/msp.h>
#include "Task.hpp"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.hpp"
#include <stdio.h>
#include "Defines.hpp"
#include "Scheduler.hpp"

#include "Task.hpp"


class ColorRect : public Task
{
    public:
        ColorRect(Graphics_Context context, Graphics_Rectangle rect);
        uint16_t m_u16BITN;
        Graphics_Context g_sContext;
        Graphics_Rectangle g_rect;
        virtual uint8_t run(void);
        virtual uint8_t setup(void);
    protected:
    private:
};

#endif /* ColorRect_HPP_ */
