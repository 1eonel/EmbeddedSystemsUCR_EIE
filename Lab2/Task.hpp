#ifndef TASK_HPP_
#define TASK_HPP_

//#include "Scheduler.hpp"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <string>
#include "Defines.hpp"

using namespace std;

struct st_MsjInfo {
    uint8_t TaskToCall;
    uint64_t * DirDataRead;
};


class Task
{
    public:
        Task();                         // Defines operation type
        st_MsjInfo * m_pSTaskMsj;       //  Communication with the Scheduler
        uint64_t * m_pRTaskMsj;         // Pointer to mem address for task to READ.
        uint64_t * m_pWTaskMsj;         // Pointer to mem address for task to WRITE.
        uint8_t m_u8TaskID;

        bool m_bMailBox;    // Flag to detect if the task has a message and then read RTaskMsj.

        uint8_t SendMessage(uint64_t Msj[3], uint8_t ID);
        virtual uint8_t     run(void){return(0);};
        virtual uint8_t     setup(void){return(0);};

        bool                IsTaskFinished(void){return m_bIsFinished;};
        uint8_t             GetTaskPriority(void) {return m_u8Priority;};
        void                SetTaskPriority(uint8_t i_u8NewPriority){m_u8Priority = i_u8NewPriority;};
    private:
       static uint8_t m_u8NextTaskID;
    protected:
       uint8_t m_u8Priority;
       bool m_bIsFinished;
};

#endif /* TASK_HPP_ */
