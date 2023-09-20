/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Timer.h
 * Brief description
 */
#ifndef S32K_TIMER_H_
#define S32K_TIMER_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include "wtypes.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/
#define TIMER_PERIOD_1US   1000UL
/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/
typedef enum {
	SYS_TASK_MEASURE_DISABLED,
	SYS_TASK_MEASURE_WAIT_TO_START,
	SYS_TASK_MEASURE_BUSY,
	SYS_TASK_MEASURE_DONE,
} s_Systimer_measure;
/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct S_TASK_MEASURE
{
    uint32_t TaskId;
    uint32_t Ton;
    uint32_t Toff;
} sTaskMeasure;
/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/
extern wres_t S32K_Timer_Init(uint32_t microSecondTimerValue );
extern void S32K_Timer_Start( void );
extern void S32K_Timer_Stop( void );
extern void Application_TickUpdate( void );
extern void Timer_ScheduleInit( s_S32K_TimerTaskId TakId, uint32_t scheduleCounterTh, uint32_t delayCounterTh, bool immediateStart, void (*scheduleFunction)( uint8_t index ) );
extern void Timer_ScheduleTickUpdate( s_S32K_TimerTaskId TakId );
extern void Timer_ScheduleExecute( s_S32K_TimerTaskId TakId , uint8_t index );
extern void Timer_ScheduleTickClear( void );

/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/
// 	Macro definition example
//	#define TASK_0_INDEX  TASK_0_ID
//	#define TASK_0_PERIOD 20000
//	#define TASK_0_DELAY  1000

//	#define TASK_1_INDEX  TASK_1_ID
//	#define TASK_1_PERIOD 20000
//	#define TASK_1_DELAY  3000

//	#define TASK_2_INDEX  TASK_2_ID
//	#define TASK_2_PERIOD 5000
//	#define TASK_2_DELAY  0

// 	Init example
//	S32K_Timer_Init( TIMER_PERIOD_1US);
//	Timer_ScheduleInit( TASK_0_INDEX, TASK_0_PERIOD, TASK_0_DELAY, true, &Task_0_Main );
//	Timer_ScheduleInit( TASK_1_INDEX, TASK_1_PERIOD, TASK_1_DELAY, true, &Task_1_Main );
//	Timer_ScheduleInit( TASK_2_INDEX, TASK_2_PERIOD, TASK_2_DELAY, true, &Task_2_Main );

// 	Main example
//	Timer_ScheduleExecute( TASK_0_INDEX, 0U );
//	Timer_ScheduleExecute( TASK_1_INDEX, 0U );
//	Timer_ScheduleExecute( TASK_2_INDEX, 0U );

//	Application_TickUpdate example
//void Application_TickUpdate( void )
//{
//    Timer_ScheduleTickUpdate( TASK_0_INDEX );
//    Timer_ScheduleTickUpdate( TASK_1_INDEX );
//    Timer_ScheduleTickUpdate( TASK_2_INDEX );
//}

#endif /* S32K_TIMER_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
