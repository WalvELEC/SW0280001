/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Timer.c
 * History:
 *  - 05 feb 2020 1.0 Davoli - First release
 * S32K_Timer.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_Timer.h"
#include "S32K_SysPin.h"

/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/
#define COUNTER_OFF   0U
#define __weak __attribute__ ((weak))

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#ifndef MySysTmr_H
    #error Module S32_Timer need Object typed lpit and named MySysTmr
#endif
// Settings Link from this module and Object lpit named MySysTmr
#define INST_NUM			INST_MYSYSTMR
#define Init_Config			MySysTmr_InitConfig
#define Init_CHannel		MySysTmr_ChnConfig0
#define CHannel_Num			0x00
#define HandlerPtr			MySysTick_Handler
#ifdef MCU_S32K116
#define LPIT_Channel_IRQ	LPIT0_IRQn
#endif
#ifdef MCU_S32K144
#define LPIT_Channel_IRQ	LPIT0_Ch0_IRQn
#endif

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct S_TIMER_SCHEDULE
{
    uint32_t scheduleCounter;
    uint32_t scheduleCounterTh;
    uint32_t delayCounterTh;
    uint32_t counterTh;
    uint32_t Ton;
    uint32_t Toff;
    void (*scheduleFunction)( uint8_t index );
} sTimerSchedule;

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/
static void MySysTick_Handler(void);

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
static sTimerSchedule Application_Schedule[TASK_NUM];
static uint32_t Timer_tickTime;

/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/
/*
//------------------------------------------------------------------------------
__weak void Application_TickUpdate( void )
{

}
*/
//------------------------------------------------------------------------------
static void MySysTick_Handler(void)
{
  // Clear LPIT channel flag
#ifdef TEST_DEBUG_CODE_SPY  
  CodeSpy_DEBUG_Set(GPIO_SYS_TICK);
#endif  
  LPIT_DRV_ClearInterruptFlagTimerChannels(INST_NUM, (1u << CHannel_Num));
  Application_TickUpdate();
#ifdef TEST_DEBUG_CODE_SPY
  CodeSpy_DEBUG_Clear(GPIO_SYS_TICK);
#endif  
  return;
}
//------------------------------------------------------------------------------
extern void S32K_Timer_Start( void )
{
	LPIT_DRV_StartTimerChannels(INST_NUM, (1u << CHannel_Num));
}
//------------------------------------------------------------------------------
extern void S32K_Timer_Stop( void )
{
	LPIT_DRV_StopTimerChannels(INST_NUM, (1u << CHannel_Num));
}

//------------------------------------------------------------------------------
extern wres_t S32K_Timer_Init(uint32_t microSecondTimerValue )
{
	wres_t res;
    LPIT_DRV_Init(INST_NUM, &Init_Config);
	Init_CHannel.period = microSecondTimerValue;
	if (STATUS_SUCCESS == LPIT_DRV_InitChannel(INST_NUM, CHannel_Num,&Init_CHannel))
	{
		INT_SYS_InstallHandler(LPIT_Channel_IRQ, HandlerPtr, (isr_t *)0);
		//LPIT_DRV_StartTimerChannels(INST_NUM, (1 << CHannel_Num));
		Timer_tickTime = microSecondTimerValue;
		res = WRES_OK;
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}

//------------------------------------------------------------------------------
extern void Timer_ScheduleTickUpdate( s_S32K_TimerTaskId TakId )
{
	sTimerSchedule *timerSchedule;
	timerSchedule =&Application_Schedule[TakId];
    timerSchedule->scheduleCounter++;

    return;
}
//------------------------------------------------------------------------------
extern void Timer_ScheduleTickClear( void )
{
  uint8_t TakId; 
    for (TakId = 0U; TakId < TASK_NUM; TakId++)
    {
      Application_Schedule[TakId].scheduleCounter = 0;
    }
    return;
}

//------------------------------------------------------------------------------
extern void Timer_ScheduleInit( s_S32K_TimerTaskId TakId, uint32_t scheduleCounterTh, uint32_t delayCounterTh, bool immediateStart, void (*scheduleFunction)( uint8_t index ) )
{
	sTimerSchedule *timerSchedule;
	timerSchedule =&Application_Schedule[TakId];

	timerSchedule->scheduleCounterTh = scheduleCounterTh;
    timerSchedule->delayCounterTh = delayCounterTh;
    timerSchedule->scheduleFunction = scheduleFunction;
    timerSchedule->scheduleCounter = COUNTER_OFF;
    
    if ( true == immediateStart )
    {
        timerSchedule->counterTh = timerSchedule->delayCounterTh;
    }
    else
    {
        timerSchedule->counterTh = timerSchedule->delayCounterTh + timerSchedule->scheduleCounterTh;
    }
    
    return;
}


//------------------------------------------------------------------------------
extern void Timer_ScheduleExecute( s_S32K_TimerTaskId TakId , uint8_t index )
{
    uint32_t auxScheduleTimeTh;

    sTimerSchedule *timerSchedule;
	timerSchedule =&Application_Schedule[TakId];
    
    auxScheduleTimeTh = timerSchedule->counterTh;
    auxScheduleTimeTh = auxScheduleTimeTh / Timer_tickTime;
   
    if ( timerSchedule->scheduleCounter >= auxScheduleTimeTh )
    {
        timerSchedule->scheduleCounter = timerSchedule->scheduleCounter - auxScheduleTimeTh;
        timerSchedule->counterTh = timerSchedule->scheduleCounterTh;
      	timerSchedule->scheduleFunction( index );
    }
    
    return;
}
