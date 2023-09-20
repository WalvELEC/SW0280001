#include "S32K_Driver.h"
#include "S32K_SysPin.h"
//#include "SafetySlave.h"
#include "S32K_Timer.h"
#include "application.h"

//------------------------------------------------------------------------------
void Application_TickUpdate( void )
{
    Timer_ScheduleTickUpdate( TASK_DRIVER_INDEX ); //TASK_0_ID
    Timer_ScheduleTickUpdate( TASK_WDT_INDEX );    //TASK_1_ID
    Timer_ScheduleTickUpdate( TASK_APP_INDEX );    //TASK_4_ID
//    Timer_ScheduleTickUpdate( TASK_SAFETY_INDEX ); //TASK_5_ID
    LED_DutyUpdate();
}


int main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
    S32K_Driver_Init();
//    SafetySlave_Init();
    Application_Init();
   S32K_SysPin_ClearOutputPins(GPIO_OUT_CAN_STB);
    S32K_Timer_Start();
  for(;;)
  {
    	S32K_Driver_Task();
    	Application_Main();
//    	SafetySlave_Task();
    }
}

