#include "Task.hpp"


Task::Task()
{
    m_u8TaskID = m_u8NextTaskID;
    m_u8NextTaskID++;
    m_bIsFinished = false;

}

uint8_t Task::SendMessage(uint64_t Msj[3], uint8_t ID)
{
    for(int i = 0; i<3 ; i++){
    *(m_pWTaskMsj+i) = Msj[i];
    }

    (m_pSTaskMsj)->TaskToCall = ID;                 // Writes to the Scheduler who to call
    (m_pSTaskMsj+ID)->DirDataRead= this->m_pWTaskMsj;  // Sends the pointer where the info is stored in the scheduler

    return(NO_ERR);
}
