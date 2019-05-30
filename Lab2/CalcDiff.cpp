#include "CalcDiff.hpp"

CalcDiff::CalcDiff(uint16_t i_BITN)
{
    m_u16BITN = i_BITN;
}

uint8_t CalcDiff::run()
{
    uint64_t l_u8Msj[3];
    double l_puntero = *(m_pRTaskMsj+2);

    double l_preCurrentLine = (l_puntero*(-32.0/1665.0)) + 222.3663; //Reference to set half of the LCD
    uint64_t l_u8CurrentLine = l_preCurrentLine;


    //Checks for relative position to send message and color properly
    if(g_u8OldLine != l_u8CurrentLine){
        if(l_u8CurrentLine >= g_u8OldLine){
            l_u8Msj[0] = l_u8CurrentLine;
            l_u8Msj[1] = g_u8OldLine;
            l_u8Msj[2] = GRAPHICS_COLOR_BLUE;
            SendMessage(l_u8Msj, T_ColorRect);
        } else {
            l_u8Msj[0] = g_u8OldLine;
            l_u8Msj[1] = l_u8CurrentLine;
            l_u8Msj[2] = GRAPHICS_COLOR_CHOCOLATE;
            SendMessage(l_u8Msj, T_ColorRect);
        }
    }

    g_u8OldLine = l_u8CurrentLine;

    return(NO_ERR);
}

uint8_t CalcDiff::setup()
{
    g_u8OldLine = 64;
    return(NO_ERR);
}
