
#include "Cpu.h"
#include "S32K_ObjectConfig.h"
#include "S32K_flexSpi.h"
#include "S32K_LpSpi.h"
#include "S32K_CanDma.h"
#include "S32K_Flash.h"
#include "S32K_Adc.h"
#include "S32K_Timer.h"
#include "S32K_SysPin.h"
#include "TLF_Spi.h"
#include "mlx.h"
#include "S32K_Driver.h"

#include "AppTranslate.h"
#include "application.h"

#define FLEXIO_SPI_MASTER  S32K_FLEXSPI_ID_0
// Timer Definitions

static void Task_Driver ( uint8_t index );
static void Task_WDT ( uint8_t index );


extern wres_t S32K_Driver_Init( void )
{
    wres_t InitRes;
    LED_DutyInit();

    InitRes =S32K_SysPin_Init();
    DEV_ASSERT(WRES_OK == InitRes);
    InitRes = S32K_Flash_Init();  //Flash_Init(); Da FARE
    DEV_ASSERT(WRES_OK == InitRes);

    
    InitRes = S32K_Adc_Init( INST_ADC_0, &ADC_0_InitConfig0);
    DEV_ASSERT(WRES_OK == InitRes);
    InitRes = S32K_Adc_Init( INST_ADC_1, &ADC_1_InitConfig0);
    DEV_ASSERT(WRES_OK == InitRes);

    InitRes = S32K_FlexSpi_Init(FLEXIO_SPI_MASTER, S32K_FLEX_SPI_MASTER , INST_FLEXIO_M4, &flexio_M4_MasterConfig0,NULL);
    DEV_ASSERT(WRES_OK == InitRes);

    //InitRes = S32K_Can_Init( CAN_JOY_OUT , INST_CAN_OUT_JOY,&CAN_OUT_JOY_Config0);
    //InitRes = S32K_Can_Init( CAN_JOY_OUT , INST_CANCOM1,&canCom1_InitConfig0);

    InitRes = S32K_SysPin_SetOutputPins( GPIO_OUT_CAN_TERM );
    DEV_ASSERT(WRES_OK == InitRes);

    InitRes = S32K_LpSpi_Init(SPI_MLX_1,S32K_LP_SPI_MASTER,LPSPI_MLX_1,&LPSPI_MLX_1_MasterConfig0,NULL);
    DEV_ASSERT(WRES_OK == InitRes);
    InitRes = S32K_LpSpi_MasterSetDelay(SPI_MLX_1,1u,2u,1u);
    DEV_ASSERT(WRES_OK == InitRes);

    InitRes = S32K_LpSpi_Init(SPI_MLX_2,S32K_LP_SPI_MASTER,LPSPI_MLX_2,&LPSPI_MLX_2_MasterConfig0,NULL);
    DEV_ASSERT(WRES_OK == InitRes);
    InitRes = S32K_LpSpi_MasterSetDelay(SPI_MLX_2,1u,2u,1u);
    DEV_ASSERT(WRES_OK == InitRes);

    InitRes = S32K_LpSpi_Init(SPI_TLF,S32K_LP_SPI_MASTER,LPSPI_TLF,&LPSPI_TLF_MasterConfig0,NULL);
    DEV_ASSERT(WRES_OK == InitRes);
    InitRes = S32K_LpSpi_MasterSetDelay(SPI_TLF,1u,2u,1u);
    DEV_ASSERT(WRES_OK == InitRes);
    TLF_Spi_Init();
    MLX_Init();

    S32K_Timer_Init( TIMER_PERIOD_1US);
    //InitRes = S32K_CanDma_Init( CAN_JOY_OUT ,INST_CANCOM1,&canCom1_InitConfig0);
    //CheckInitError( InitRes);

    Timer_ScheduleInit( TASK_DRIVER_INDEX, TASK_DRIVER_PERIOD, TASK_DRIVER_DELAY, true, &Task_Driver );
    Timer_ScheduleInit( TASK_WDT_INDEX, TASK_WDT_PERIOD, TASK_WDT_DELAY, true, &Task_WDT );
    //Timer_ScheduleInit( TASK_MLX_0_INDEX, TASK_MLX_PERIOD, TASK_MLX_0_DELAY, true, &MLX_Main );
    //Timer_ScheduleInit( TASK_MLX_1_INDEX, TASK_MLX_PERIOD, TASK_MLX_1_DELAY, true, &MLX_Main );

//    S32K_Led_Init();
//    S32K_Led_Stop( 0x00 );
//    S32K_Led_Stop( 0x01 );
//    S32K_Led_Stop( 0x02 );
//    S32K_Led_Stop( 0x03 );
    
    return InitRes;
}
//------------------------------------------------------------------------------
//  S32K_driver :  Task_Driver
//            
//------------------------------------------------------------------------------
static void Task_Driver ( uint8_t index )
{
   (void)(index);
    wres_t Res;
#ifdef TEST_DEBUG_CODE_SPY    
    CodeSpy_DEBUG_Set(GPIO_OUT_TPTASK);
    CodeSpy_DEBUG_Clear(GPIO_OUT_TPTASK);
#endif    
    Res = S32K_Adc_Task(ADC0_GROUP_0);
    Res = S32K_Adc_Task(ADC1_GROUP_0);
    Res = S32K_Adc_Task(ADC0_GROUP_1);
    Res = S32K_Adc_Task(ADC1_GROUP_1);
#ifdef TEST_DEBUG_CODE_SPY
    CodeSpy_DEBUG_Set(GPIO_OUT_TPTASK);
#endif    
    Res = S32K_SysPin_ToggleOutputPins(GPIO_OUT_VOTING_SYNC);
    Res = S32K_Adc_Start(ADC0_GROUP_0);

    Res = S32K_Adc_Start(ADC1_GROUP_0);
    MLX_StartupMain();
    MLX_Main( MLX_1 );
    MLX_Main( MLX_0 );
#ifdef TEST_DEBUG_CODE_SPY
    CodeSpy_DEBUG_Clear(GPIO_OUT_TPTASK);
#endif    
}
//------------------------------------------------------------------------------
//  S32K_driver :  Task_WDT
//            
//------------------------------------------------------------------------------
static void Task_WDT ( uint8_t index )
{
    (void)(index);
    wres_t Res;
#ifdef TEST_DEBUG_CODE_SPY
    CodeSpy_DEBUG_Set(GPIO_OUT_TPTASK);
#endif    
    Res = S32K_Adc_Start(ADC0_GROUP_1);
    Res = S32K_Adc_Start(ADC1_GROUP_1);
    TLF_Spi_Main(0x00);
#ifdef TEST_DEBUG_CODE_SPY
    CodeSpy_DEBUG_Clear(GPIO_OUT_TPTASK);
#endif    
}
//------------------------------------------------------------------------------
//  S32K_driver :  S32K_Driver_Task
//            
//------------------------------------------------------------------------------
extern void S32K_Driver_Task( void )
{
    Timer_ScheduleExecute( TASK_DRIVER_INDEX, 0U );
    Timer_ScheduleExecute( TASK_WDT_INDEX, 0U );
}
