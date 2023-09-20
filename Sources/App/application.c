#include "wtypes.h"
#include "application.h"
#include "S32K_Timer.h"
#include "S32K_SysPin.h"
#include "S32K_Adc.h"

static s_bspcan_bitRate Application_baudRate;
static uint32_t Application_canResTerm;
static uint8_t Application_sourceAddress;
static void Application_IO_Main( uint8_t index );

static uint32_t datoRead_01;
static uint32_t datoRead_02;
/**************************************************************************************************
 *  FUNCTION PROTOTYPES                                                                           *
 **************************************************************************************************/
static wres_t Application_BaudRateGet( uint32_t baudRateIn, s_bspcan_bitRate *baudRateOut );
//static wres_t Application_BaudRateSet( s_bspcan_bitRate baudRateIn, uint32_t *baudRateOut );
static void Application_paramToRAMUpdate( void );
static void Application_paramFromRAMUpdate( void );
static wres_t Application_sourceAddressEval( uint32_t sourceAddressIn, uint8_t *sourceAddressOut );
uint8_t Application_sourceAddressGet( void );
uint32_t Application_ParamGet( void );
void Application_Task ( uint8_t index );

//------------------------------------------------------------------------------
static wres_t Application_BaudRateGet( uint32_t baudRateIn, s_bspcan_bitRate *baudRateOut )
{
    wres_t retVal;
    
    uint32_t aux;
    
    retVal = WRES_OK;
    aux = ( baudRateIn >> 16UL ) & ( 0x0000000FUL );
    switch ( aux )
    {
    case 0x01U:
        *baudRateOut = CAN_BIT_RATE_800K;
        break;
    case 0x02U:
        *baudRateOut = CAN_BIT_RATE_500K;
        break;        
    case 0x03U:
        *baudRateOut = CAN_BIT_RATE_250K;
        break;        
    case 0x04U:
        *baudRateOut = CAN_BIT_RATE_125K;
        break;        
    case 0x05U:
        *baudRateOut = CAN_BIT_RATE_1M;
        break;
    case 0x06U:
        *baudRateOut = CAN_BIT_RATE_50K;
        break;        
    case 0x07U:
        *baudRateOut = CAN_BIT_RATE_20K;
        break;        
    case 0x08U:
        *baudRateOut = CAN_BIT_RATE_10K;
        break;        
    default:
        *baudRateOut = CAN_BIT_RATE_250K;
        retVal = WRES_ERROR_INVALID_PARAMETER;
        break;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t Application_sourceAddressEval( uint32_t sourceAddressIn, uint8_t *sourceAddressOut )
{
    wres_t retVal;
    
    uint8_t aux;
    
    retVal = WRES_OK;
    aux = (uint8_t)( sourceAddressIn );
    if ( ( 0xFF != aux ) && ( 0xFE != aux ) )
    {
        *sourceAddressOut = aux;
        retVal = WRES_OK;
    }
    else
    {
        *sourceAddressOut = SAEJ_DEFAULT_SA;
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
//static wres_t Application_BaudRateSet( s_bspcan_bitRate baudRateIn, uint32_t *baudRateOut )
//{
//    wres_t retVal;
//    
//    retVal = WRES_OK;
//    switch ( baudRateIn )
//    {
//    case CAN_BIT_RATE_1M:
//        *baudRateOut = 0x05U;
//        break;      
//    case CAN_BIT_RATE_800K:
//        *baudRateOut = 0x01U;
//        break;
//    case CAN_BIT_RATE_500K:
//        *baudRateOut = 0x02U;
//        break;        
//    case CAN_BIT_RATE_250K:
//        *baudRateOut = 0x03U;
//        break;        
//    case CAN_BIT_RATE_125K:
//        *baudRateOut = 0x04U;
//        break;        
//    case CAN_BIT_RATE_50K:
//        *baudRateOut = 0x06U;
//        break;        
//    case CAN_BIT_RATE_20K:
//        *baudRateOut = 0x07U;
//        break;        
//    case CAN_BIT_RATE_10K:
//        *baudRateOut = 0x08U;
//        break;        
//    default:
//        *baudRateOut = 0x03U;
//        retVal = WRES_ERROR_INVALID_PARAMETER;
//        break;
//    }
//    
//    return retVal;
//}

//------------------------------------------------------------------------------
static void Application_paramToRAMUpdate( void )
{
    uint32_t datoOut;
    
    //Application_BaudRateSet( Application_baudRate, &datoOut );
    
    datoOut = Application_canResTerm;
    
    MemData_SetRAMItem( ADDR_APPLICATION_BAUD_RATE, datoOut );
    return;
}

//------------------------------------------------------------------------------
static void Application_paramFromRAMUpdate( void )
{
    uint32_t datoRead;
    
    if ( 0U == MemData_GetRAMItem( ADDR_APPLICATION_BAUD_RATE, &datoRead ) )
    {
        //Application_BaudRateGet( datoRead, &Application_baudRate );
        Application_canResTerm = datoRead & 0x000000F0UL;
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Application_paramFromEEUpdate( void )
{
    uint32_t auxBoot1;
    uint32_t auxBoot2;

    if ( ( 0U == MemData_GetEepromItem( ADDR_BOOT_SET_DATA_01, &datoRead_01 ) ) 
      && ( 0U == MemData_GetEepromItem( ADDR_BOOT_SET_DATA_02, &datoRead_02 ) ) )
    {
        if ( datoRead_01 == datoRead_02 )
        {
            if ( WRES_OK != Application_BaudRateGet( datoRead_01, &Application_baudRate ) )
            {
                MemData_ErrorSet( MEM_DATA_ERROR_INVALID_BAUD );    
            }
            if ( WRES_OK != Application_sourceAddressEval( datoRead_01, &Application_sourceAddress ) )
            {
                MemData_ErrorSet( MEM_DATA_ERROR_INVALID_SA );
            }
        }
        else
        {
            MemData_ErrorSet( MEM_DATA_ERROR_INVALID_APP_PARAM );
            auxBoot1 = datoRead_01 & 0x00FF0000u;
            auxBoot2 = datoRead_02 & 0x00FF0000u;
            if ( auxBoot1 == auxBoot2 )
            {
                if ( WRES_OK != Application_BaudRateGet( datoRead_01, &Application_baudRate ) )
                {
                    MemData_ErrorSet( MEM_DATA_ERROR_INVALID_BAUD );    
                }              
            }
            else
            {
                MemData_ErrorSet( MEM_DATA_ERROR_INVALID_BAUD );    
            }
            auxBoot1 = datoRead_01 & 0x000000FFu;
            auxBoot2 = datoRead_02 & 0x000000FFu;            
            if ( auxBoot1 == auxBoot2 )
            {
                if ( WRES_OK != Application_sourceAddressEval( datoRead_01, &Application_sourceAddress ) )
                {
                    MemData_ErrorSet( MEM_DATA_ERROR_INVALID_SA );
                }
            }
            else
            {
                MemData_ErrorSet( MEM_DATA_ERROR_INVALID_SA );
            }              
        }
    }
    else
    {
        MemData_ErrorSet( MEM_DATA_ERROR_INVALID_APP_PARAM );
    }
        
    
    return;
}
//------------------------------------------------------------------------------
static void Application_IO_Main( uint8_t index )
{
    (void)index;
   //PINS_DRV_TogglePins(TP_GPIO_PORT, (1 << LED0_PIN_TP_2));
    CAN_Main_K144();
    //ADC_Main_K144();
    Supply_Main();
    Digital_Main();
    Roller_Main();

    return;
}

//------------------------------------------------------------------------------
uint8_t Application_sourceAddressGet( void )
{
    return Application_sourceAddress;
}

//------------------------------------------------------------------------------
uint32_t Application_ParamGet( void )
{
    return datoRead_01;
}

//------------------------------------------------------------------------------
void Application_Task ( uint8_t index )
{
    (void)(index);
    Saej_Main(0);
    Application_IO_Main(0);
    Flash_Main(0);
}
//------------------------------------------------------------------------------
wres_t Application_Init( void )
{
    wres_t InitRes;
    uint8_t i;
    Flash_Init();
    Digital_Init();
    Supply_Init();
    Roller_Init();
    Saej_Init();

    Application_sourceAddress = SAEJ_DEFAULT_SA;
    Application_baudRate = CAN0_BAUD_RATE_BPS;
    Application_canResTerm = DEFAULT_CAN_RES_TERM;
    
//    CAN_ModuleEnable( CAN_MODULE_0, CAN0_BAUD_RATE_BPS );

    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        //CAN_StoredMsgUpdate( 0U, i, 0x18DA23F9UL, CAN_SOURCE_ADDRESS_CHECK_YES );  //TLE SPI Rx
        //CAN_StoredMsgUpdate( CAN_MSG_MLX_0, i, 0x18DA22FAUL, CAN_SOURCE_ADDRESS_CHECK_YES );  //MLX_SPICmd Rx X
        //CAN_StoredMsgUpdate( CAN_MSG_MLX_1, i, 0x18DA23FAUL, CAN_SOURCE_ADDRESS_CHECK_YES );  //MLX_SPICmd Rx Y
        //CAN_StoredMsgUpdate( 3U, i, 0x18DA24F9UL, CAN_SOURCE_ADDRESS_CHECK_YES );  //Flash access
    }
    
    Timer_ScheduleInit( TASK_APP_INDEX, TASK_APP_PERIOD, TASK_APP_DELAY, true, &Application_Task );
    Application_paramToRAMUpdate();
    MemData_Init();

        Application_paramFromRAMUpdate();
        Application_paramFromEEUpdate();
    InitRes = S32K_CanDma_Init( CAN_JOY_OUT ,INST_CANCOM1,&canCom1_InitConfig0, Application_baudRate );
    if ( WRES_OK == InitRes )
    {
        CAN_Init_K144( Application_sourceAddress );
        //CAN_ModuleEnable( CAN_MODULE_0, Application_baudRate );
        if ( CAN_RES_TERM_ENABLED == Application_canResTerm )
        {
            S32K_SysPin_ClearOutputPins( GPIO_OUT_CAN_TERM );
        }
        else
        {
            S32K_SysPin_SetOutputPins( GPIO_OUT_CAN_TERM );
        }
    }
    return InitRes;
}

//------------------------------------------------------------------------------
void Application_Main( void )
{

    static bool first_entry_in_main = true;

    if ( true == first_entry_in_main )
    {
        first_entry_in_main = false;
    }
    Timer_ScheduleExecute( TASK_APP_INDEX, 0U );
    return;
}
                     
