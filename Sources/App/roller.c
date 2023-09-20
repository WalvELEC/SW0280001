/*
 * roller.c
 *
 *  Created on: 22 jun 2018
 *      Author: PoliD
 */

//------------------------------------------------------------------------------

#include "application.h"

//------------------------------------------------------------------------------
#define ROLLER_REDUNDANCE_TH_MIN   ( 4500U )
#define ROLLER_REDUNDANCE_TH_MAX   ( 5500U )

//------------------------------------------------------------------------------
#define ROLLER_STATUS_NORMAL       ( 0x01U )
#define ROLLER_STATUS_ERROR        ( 0xFFU )
//------------------------------------------------------------------------------
typedef struct Roller_obj_e
{
    uint16_t in;
    uint16_t inRed;
    uint8_t adcRefIndex;
    uint8_t adcRefIndexRed;
    uint16_t minA;
    uint16_t maxA;
    uint16_t minB;
    uint16_t maxB;
    uint16_t minARed;
    uint16_t maxARed;
    uint16_t minBRed;
    uint16_t maxBRed;    
    uint16_t errThA;
    uint16_t errThB;
    uint16_t errThARed;
    uint16_t errThBRed;
    uint16_t out;
    bool waitNeutralAtStartup;
    w_error_entry_t error[ROLLER_ERROR_NUM];
    w_status_t status;
} Roller_obj_t;

//------------------------------------------------------------------------------
static Roller_obj_t Roller_obj[ROLLER_NUM_MAX];
static uint16_t Roller_cfg;
static uint8_t Roller_error_cfg;
uint8_t Roller_signalMainIndex[ROLLER_NUM_MAX];
static uint8_t Roller_signalRedundanceIndex[ROLLER_NUM_MAX];
static void Roller_OutputEval( uint8_t rollerIndex, uint16_t setpointMax );

//------------------------------------------------------------------------------
uint16_t  Roller_MinAGet (uint8_t Roller)
{
  return Roller_obj[Roller].minA;
}
//------------------------------------------------------------------------------
uint16_t  Roller_MinBGet (uint8_t Roller)
{
  return Roller_obj[Roller].minB;

}
//------------------------------------------------------------------------------
uint16_t  Roller_MaxAGet (uint8_t Roller)
{
  return Roller_obj[Roller].maxA;

}
//------------------------------------------------------------------------------
uint16_t  Roller_MaxBGet (uint8_t Roller)
{
  return Roller_obj[Roller].maxB;
}
//------------------------------------------------------------------------------
#define ROLLER_PARAM_INDEX_SGN_MIN_A      ( 0U )
#define ROLLER_PARAM_INDEX_SGN_MAX_A      ( 1U )
#define ROLLER_PARAM_INDEX_SGN_ERR_TH_A   ( 2U )
#define ROLLER_PARAM_INDEX_SGN_MIN_B      ( 3U )
#define ROLLER_PARAM_INDEX_SGN_MAX_B      ( 4U )
#define ROLLER_PARAM_INDEX_SGN_ERR_TH_B   ( 5U )
#define ROLLER_PARAM_INDEX_RED_MIN_A      ( 6U )
#define ROLLER_PARAM_INDEX_RED_MAX_A      ( 7U )
#define ROLLER_PARAM_INDEX_RED_ERR_TH_A   ( 8U )
#define ROLLER_PARAM_INDEX_RED_MIN_B      ( 9U )
#define ROLLER_PARAM_INDEX_RED_MAX_B      ( 10U )
#define ROLLER_PARAM_INDEX_RED_ERR_TH_B   ( 11U )
#define ROLLER_PARAM_INDEX_NUM            ( 12U )

#define ADDR_INDEX_MIN_A      ( 0U ) 
#define ADDR_INDEX_MAX_A      ( 1U )
#define ADDR_INDEX_ERR_TH_A   ( 2U )
#define ADDR_INDEX_MIN_B      ( 3U )
#define ADDR_INDEX_MAX_B      ( 4U )
#define ADDR_INDEX_ERR_TH_B   ( 5U )
#define ADDR_INDEX_NUM_MAX    ( 6U )

static bool Roller_IsInNeutral( uint8_t rollerIndex );
static bool Roller_RedundanceCheck( uint8_t rollerIndex );
static bool Roller_ErrorCheck( uint8_t rollerIndex );
static void Roller_InputEval( uint8_t rollerIndex );
static void Roller_CfgUpdate( void );
static void Roller_paramToRAMUpdate( uint8_t status );
static void Roller_paramFromRAMUpdate( void );

//------------------------------------------------------------------------------
static bool Roller_IsInNeutral( uint8_t rollerIndex )
{
    uint8_t aux;
    bool retVal;
    
    aux = 0U;
    retVal = true;
    if ( ( Roller_obj[rollerIndex].in <= Roller_obj[rollerIndex].minA ) || ( Roller_obj[rollerIndex].in >= Roller_obj[rollerIndex].minB ) )
    {
        aux++;
    }
//    if ( ROLLER_INPUT_NOT_CONNECTED != Roller_obj[rollerIndex].adcRefIndexRed )
//    {
//        if ( ( Roller_obj[rollerIndex].inRed <= Roller_obj[rollerIndex].minA ) || ( Roller_obj[rollerIndex].inRed >= Roller_obj[rollerIndex].minB ) )
//        {
//            aux++;
//        }
//    }
    if ( 0U < aux )
    {
        retVal = false;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static bool Roller_RedundanceCheck( uint8_t rollerIndex )
{
    bool retVal;
    
    retVal = false;
    if ( ( ( Roller_obj[rollerIndex].in + Roller_obj[rollerIndex].inRed ) < ROLLER_REDUNDANCE_TH_MIN )
      || ( ( Roller_obj[rollerIndex].in + Roller_obj[rollerIndex].inRed ) > ROLLER_REDUNDANCE_TH_MAX ) )
    {
        retVal = true;
    }
    return retVal;  
}

//------------------------------------------------------------------------------
static bool Roller_ErrorCheck( uint8_t rollerIndex )
{
    uint8_t aux;
    uint8_t i;
    bool retVal;
    
    aux = 0U;
    retVal = false;
    for ( i=0U; i<ROLLER_ERROR_NUM; i++ )
    {
        if ( true == Roller_obj[rollerIndex].error[i].active )
        { 
            aux++;
        }
    }
    if ( aux > 0U )
    {
        retVal = true;
    }
    
    return retVal;
}
    
//------------------------------------------------------------------------------
static void Roller_InputEval( uint8_t rollerIndex )
{
    bool signalOutOfRange;
    uint16_t auxSignalIn;
    uint16_t auxSignalRed;
    
    if ( ROLLER_INPUT_NOT_CONNECTED != Roller_obj[rollerIndex].adcRefIndex )
    {
        auxSignalIn = ADC_GetValue_mV( Roller_obj[rollerIndex].adcRefIndex );
        signalOutOfRange = ( ( auxSignalIn < Roller_obj[rollerIndex].errThA ) || ( auxSignalIn > Roller_obj[rollerIndex].errThB ) );
        if ( false == signalOutOfRange )
        {
            Roller_obj[rollerIndex].in = auxSignalIn;
        }
        if ( ROLLER_INPUT_NOT_CONNECTED != Roller_obj[rollerIndex].adcRefIndexRed )
        {
            auxSignalRed = ADC_GetValue_mV( Roller_obj[rollerIndex].adcRefIndexRed );
            signalOutOfRange = ( ( auxSignalRed < Roller_obj[rollerIndex].errThARed ) || ( auxSignalRed > Roller_obj[rollerIndex].errThBRed ) );      
            if ( false == signalOutOfRange )
            {
                Roller_obj[rollerIndex].inRed = auxSignalRed;
            }
            Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_HIGH], ( ( auxSignalIn > Roller_obj[rollerIndex].errThB ) || ( auxSignalRed > Roller_obj[rollerIndex].errThBRed ) ), ERROR_RESET_WITH_CNT ); 
            Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_LOW], ( ( auxSignalIn < Roller_obj[rollerIndex].errThA ) || ( auxSignalRed < Roller_obj[rollerIndex].errThARed ) ), ERROR_RESET_WITH_CNT );             
            Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_REDUNDANCE], Roller_RedundanceCheck( rollerIndex ), ERROR_RESET_WITH_CNT );
        }
        else
        {
            Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_HIGH], ( auxSignalIn > Roller_obj[rollerIndex].errThB ), ERROR_RESET_WITH_CNT ); 
            Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_LOW], ( auxSignalIn < Roller_obj[rollerIndex].errThA ), ERROR_RESET_WITH_CNT );             
            Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_REDUNDANCE] );
        } 
        Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VBB], ( SUPPLY_VBB_PROT_ERR_NO != Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) ) , ERROR_RESET_IMMEDIATE ); 
        Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VCC], ( SUPPLY_VCC_5VDC_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_ERROR ) ) , ERROR_RESET_IMMEDIATE ); 
        Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VCC_EXT], ( SUPPLY_VCC_5VDC_EXT_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_EXT_ERROR ) ) , ERROR_RESET_IMMEDIATE ); 
        Error_Update( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_MEMORY], ( ( MEM_DATA_NO_ERROR != MemData_ErrorGet() ) ) , ERROR_RESET_IMMEDIATE );         
    }
    else
    {
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_HIGH] ); 
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VALUE_LOW] );             
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_REDUNDANCE] );        
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VBB] ); 
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VCC] ); 
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_VCC_EXT] ); 
        Error_Reset( &Roller_obj[rollerIndex].error[ROLLER_ERROR_INDEX_MEMORY] );
    }        
    
    return;   
}

//------------------------------------------------------------------------------
//static void Roller_OutputEval( uint8_t rollerIndex, uint16_t setpointMax )
//{
//    uint32_t aux;
//  
//    if ( ROLLER_INPUT_NOT_CONNECTED == Roller_obj[rollerIndex].adcRefIndex )
//    {
//        Roller_obj[rollerIndex].out = 0xFFFF;
//    }
//    else
//    {
//        if ( true == Roller_ErrorCheck( rollerIndex ) )
//        {
//            Roller_obj[rollerIndex].out = 0xFFFE;
//        }
//        else if ( true == Roller_IsInNeutral( rollerIndex ) )
//        {
//            Roller_obj[rollerIndex].out = setpointMax;
//        }
//        else if ( Roller_obj[rollerIndex].in >= Roller_obj[rollerIndex].maxB )
//        {
//            Roller_obj[rollerIndex].out = setpointMax * 2U;
//        }
//        else if ( Roller_obj[rollerIndex].in <= Roller_obj[rollerIndex].maxA ) 
//        {
//            Roller_obj[rollerIndex].out = 0U;
//        }
//        else if ( Roller_obj[rollerIndex].in >= Roller_obj[rollerIndex].minB )
//        {
//            aux = (uint32_t)( setpointMax ) * (uint32_t)( Roller_obj[rollerIndex].in - Roller_obj[rollerIndex].minB );
//            aux = aux / (uint32_t)( Roller_obj[rollerIndex].maxB - Roller_obj[rollerIndex].minB ); 
//            aux = aux + setpointMax;
//            Roller_obj[rollerIndex].out = (uint16_t)( aux );
//        }
//        else if ( Roller_obj[rollerIndex].in <= Roller_obj[rollerIndex].minA )
//        {
//            aux = (uint32_t)( setpointMax ) * (uint32_t)( Roller_obj[rollerIndex].minA - Roller_obj[rollerIndex].in );
//            aux = aux / (uint32_t)( Roller_obj[rollerIndex].minA - Roller_obj[rollerIndex].maxA ); 
//            aux = setpointMax - aux;
//            Roller_obj[rollerIndex].out = (uint16_t)( aux );
//        }          
//        else
//        {
//            Roller_obj[rollerIndex].out = 0xFFFF;
//        }
//    }
//    
//    return;
//}

static void Roller_OutputEval( uint8_t rollerIndex, uint16_t setpointMax )
{
    uint32_t aux;
  
    if ( ROLLER_INPUT_NOT_CONNECTED == Roller_obj[rollerIndex].adcRefIndex )
    {
        Roller_obj[rollerIndex].out = 0xFFFFU;
        Roller_obj[rollerIndex].status.value = ROLLER_STATUS_NORMAL;
    }
    else
    {
        switch ( Roller_obj[rollerIndex].status.value )
        {
        case ROLLER_STATUS_ERROR:
            Roller_obj[rollerIndex].out = 0xFFFEU;
            if ( false == Roller_ErrorCheck( rollerIndex ) )
            {
                if ( true == Roller_IsInNeutral( rollerIndex ) )
                {
                    Roller_obj[rollerIndex].status.value = ROLLER_STATUS_NORMAL;
                }
            }
            break;
        case ROLLER_STATUS_NORMAL:
            if ( true == Roller_ErrorCheck( rollerIndex ) )
            {
                Roller_obj[rollerIndex].status.value = ROLLER_STATUS_ERROR;
            }
            else if ( true == Roller_IsInNeutral( rollerIndex ) )
            {
                Roller_obj[rollerIndex].out = setpointMax;
            }
            else if ( Roller_obj[rollerIndex].in >= Roller_obj[rollerIndex].maxB )
            {
                Roller_obj[rollerIndex].out = setpointMax * 2U;
            }
            else if ( Roller_obj[rollerIndex].in <= Roller_obj[rollerIndex].maxA ) 
            {
                Roller_obj[rollerIndex].out = 0U;
            }
            else if ( Roller_obj[rollerIndex].in >= Roller_obj[rollerIndex].minB )
            {
                aux = (uint32_t)( setpointMax ) * (uint32_t)( Roller_obj[rollerIndex].in - Roller_obj[rollerIndex].minB );
                aux = aux / (uint32_t)( Roller_obj[rollerIndex].maxB - Roller_obj[rollerIndex].minB ); 
                aux = aux + setpointMax;
                Roller_obj[rollerIndex].out = (uint16_t)( aux );
            }
            else if ( Roller_obj[rollerIndex].in <= Roller_obj[rollerIndex].minA )
            {
                aux = (uint32_t)( setpointMax ) * (uint32_t)( Roller_obj[rollerIndex].minA - Roller_obj[rollerIndex].in );
                aux = aux / (uint32_t)( Roller_obj[rollerIndex].minA - Roller_obj[rollerIndex].maxA ); 
                aux = setpointMax - aux;
                Roller_obj[rollerIndex].out = (uint16_t)( aux );
            }          
            else
            {
                Roller_obj[rollerIndex].out = 0xFFFFU;
            }
            break;
        default:
            break;
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Roller_CfgUpdate( void )
{
    uint8_t i;

    /* Roller_cfg è formattato in questo modo:
       una terna di bit per ongi roller, nell'ordine:

       b0b1b2 : roller w
       b3b4b5 : roller z
       b6b7b8 : roller j
       b9b10b11 : roller q
       b12b13b14 : roller v

       per ogni terna: x00 - roller non collegato
                       x01 - roller dritto
                       x11 - roller ridondato
                       x10 - invalid
                       1xx - roller senza attesa in neutro
                       0xx - roller con attesa in neutro 
    */
  
    for ( i=0U; i<ROLLER_NUM_MAX; i++ )
    {
        if ( ( 0x0001U << ( 3U*i ) ) == ( Roller_cfg & ( 0x0001U << ( 3U*i ) ) ) )
        {
            Roller_obj[i].adcRefIndex = Roller_signalMainIndex[i];
            if ( ( 0x0002U << ( 3U*i ) ) == ( Roller_cfg & ( 0x0002U << ( 3U*i ) ) ) )  
            {
                Roller_obj[i].adcRefIndexRed = Roller_signalRedundanceIndex[i];
            }
            else
            {
                Roller_obj[i].adcRefIndexRed = ROLLER_INPUT_NOT_CONNECTED;
            }
        }
        else
        {
            Roller_obj[i].adcRefIndex = ROLLER_INPUT_NOT_CONNECTED;  
            Roller_obj[i].adcRefIndexRed = ROLLER_INPUT_NOT_CONNECTED;  
        }

        if ( ( 0x0004U << ( 3U*i ) ) == ( Roller_cfg & ( 0x0004U << ( 3U*i ) ) ) )
        {
            Roller_obj[i].waitNeutralAtStartup = false;
        }
        else
        {
            Roller_obj[i].waitNeutralAtStartup = true;
        }
        if ( true == Roller_obj[i].waitNeutralAtStartup )
        {
            Roller_obj[i].status.value = ROLLER_STATUS_ERROR;
        }
        else
        {
            Roller_obj[i].status.value = ROLLER_STATUS_NORMAL;
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Roller_paramToRAMUpdate( uint8_t status )
{
#ifndef DJW_MEM_DISABLED
    uint32_t aux;
    uint8_t j;
    
    if ( W_INIT == status )
    {
        MemData_SetRAMItem( ADDR_ROLLER_CFG, (uint32_t)( (uint32_t)( Roller_cfg ) | (uint32_t)( Roller_error_cfg << 16U ) ) );
        aux = (uint32_t)( Roller_signalMainIndex[ROLLER_W] ) | 
              (uint32_t)( (uint32_t)Roller_signalMainIndex[ROLLER_Z] << 8U  ) | 
              (uint32_t)((uint32_t) Roller_signalMainIndex[ROLLER_J] << 16U ) | 
              (uint32_t)( (uint32_t)Roller_signalMainIndex[ROLLER_Q] << 24U );
        MemData_SetRAMItem( ADDR_ROLLER_SGN_INDEX_1_4, aux );
        aux = (uint32_t)( Roller_signalMainIndex[ROLLER_V] );
        MemData_SetRAMItem( ADDR_ROLLER_SGN_INDEX_5_8, aux );
        aux = (uint32_t)( Roller_signalRedundanceIndex[ROLLER_W] ) | 
              (uint32_t)( (uint32_t)Roller_signalRedundanceIndex[ROLLER_Z] << 8U  ) | 
              (uint32_t)( (uint32_t)Roller_signalRedundanceIndex[ROLLER_J] << 16U ) | 
              (uint32_t)( (uint32_t)Roller_signalRedundanceIndex[ROLLER_Q] << 24U );
        MemData_SetRAMItem( ADDR_ROLLER_RED_INDEX_1_4, aux );
        aux = (uint32_t)( Roller_signalRedundanceIndex[ROLLER_V] );    
        MemData_SetRAMItem( ADDR_ROLLER_RED_INDEX_5_8, aux );

        for ( j=0U; j<ROLLER_NUM_MAX; j++ )
        {
            MemData_SetRAMItem( ADDR_ROLLER_W_MIN_A    + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].minA )   | ( Roller_obj[j].minARed   << 16U ) ) );
            MemData_SetRAMItem( ADDR_ROLLER_W_MAX_A    + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].maxA )   | ( Roller_obj[j].maxARed   << 16U ) ) );
            MemData_SetRAMItem( ADDR_ROLLER_W_ERR_TH_A + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].errThA ) | ( Roller_obj[j].errThARed << 16U ) ) );
            MemData_SetRAMItem( ADDR_ROLLER_W_MIN_B    + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].minB )   | ( Roller_obj[j].minBRed   << 16U ) ) );
            MemData_SetRAMItem( ADDR_ROLLER_W_MAX_B    + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].maxB )   | ( Roller_obj[j].maxBRed   << 16U ) ) );
            MemData_SetRAMItem( ADDR_ROLLER_W_ERR_TH_B + ( j * ADDR_INDEX_NUM_MAX ), ( (uint32_t)( Roller_obj[j].errThB ) | ( Roller_obj[j].errThBRed << 16U ) ) );
        }
    }
    else
    {
        MemData_SetRAMItem( ADDR_ROLLER_W_SGN, (uint32_t)Roller_obj[ROLLER_W].in );
        MemData_SetRAMItem( ADDR_ROLLER_Z_SGN, (uint32_t)Roller_obj[ROLLER_Z].in );
        MemData_SetRAMItem( ADDR_ROLLER_J_SGN, (uint32_t)Roller_obj[ROLLER_J].in );
        MemData_SetRAMItem( ADDR_ROLLER_Q_SGN, (uint32_t)Roller_obj[ROLLER_Q].in );
        MemData_SetRAMItem( ADDR_ROLLER_V_SGN, (uint32_t)Roller_obj[ROLLER_V].in );
        MemData_SetRAMItem( ADDR_ROLLER_W_RED, (uint32_t)Roller_obj[ROLLER_W].inRed );
        MemData_SetRAMItem( ADDR_ROLLER_Z_RED, (uint32_t)Roller_obj[ROLLER_J].inRed );
        MemData_SetRAMItem( ADDR_ROLLER_J_RED, (uint32_t)Roller_obj[ROLLER_Z].inRed );
        MemData_SetRAMItem( ADDR_ROLLER_Q_RED, (uint32_t)Roller_obj[ROLLER_Q].inRed );
        MemData_SetRAMItem( ADDR_ROLLER_V_RED, (uint32_t)Roller_obj[ROLLER_V].inRed );
    }                            
#endif    
    return;
}

//------------------------------------------------------------------------------
static void Roller_paramFromRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED  
    uint32_t datoRead;  
    uint8_t j;
  
    if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_CFG, &datoRead ) )
    {
        Roller_cfg = (uint16_t)( datoRead );
        Roller_error_cfg = (uint8_t)( datoRead >> 16U );
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_SGN_INDEX_1_4, &datoRead ) )
    {
        Roller_signalMainIndex[ROLLER_W] = (uint8_t)( datoRead );
        Roller_signalMainIndex[ROLLER_Z] = (uint8_t)( datoRead >> 8U );
        Roller_signalMainIndex[ROLLER_J] = (uint8_t)( datoRead >> 16U );
        Roller_signalMainIndex[ROLLER_Q] = (uint8_t)( datoRead >> 24U );
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_SGN_INDEX_5_8, &datoRead ) )
    {
        Roller_signalMainIndex[ROLLER_V] = (uint8_t)( datoRead );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_RED_INDEX_1_4, &datoRead ) )
    {
        Roller_signalRedundanceIndex[ROLLER_W] = (uint8_t)( datoRead );
        Roller_signalRedundanceIndex[ROLLER_Z] = (uint8_t)( datoRead >> 8U );
        Roller_signalRedundanceIndex[ROLLER_J] = (uint8_t)( datoRead >> 16U );
        Roller_signalRedundanceIndex[ROLLER_Q] = (uint8_t)( datoRead >> 24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_RED_INDEX_5_8, &datoRead ) )
    {
        Roller_signalRedundanceIndex[ROLLER_V] = (uint8_t)( datoRead );
    }    

    for ( j=0U; j<ROLLER_NUM_MAX; j++ )
    {
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_MIN_A + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].minA = (uint16_t)( datoRead );
            Roller_obj[j].minARed = (uint16_t)( datoRead >> 16U );
        }
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_MAX_A + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].maxA = (uint16_t)( datoRead );
            Roller_obj[j].maxARed = (uint16_t)( datoRead >> 16U );
        }          
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_ERR_TH_A + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].errThA = (uint16_t)( datoRead );
            Roller_obj[j].errThARed = (uint16_t)( datoRead >> 16U );
        }
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_MIN_B + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].minB = (uint16_t)( datoRead );
            Roller_obj[j].minBRed = (uint16_t)( datoRead >> 16U );
        }
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_MAX_B + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].maxB = (uint16_t)( datoRead );
            Roller_obj[j].maxBRed = (uint16_t)( datoRead >> 16U );
        }          
        if ( 0U == MemData_GetRAMItem( ADDR_ROLLER_W_ERR_TH_B + ( j * ADDR_INDEX_NUM_MAX ), &datoRead ) )
        {
            Roller_obj[j].errThB = (uint16_t)( datoRead );
            Roller_obj[j].errThBRed = (uint16_t)( datoRead >> 16U );
        }        
    }
    
    Roller_CfgUpdate();

#endif    
    return;
    
}

//------------------------------------------------------------------------------
uint16_t Roller_GetOutput( uint8_t rollerIndex )
{
    return Roller_obj[rollerIndex].out;
}

//------------------------------------------------------------------------------
bool Roller_ErrorGet( uint8_t rollerIndex, uint8_t errorIndex )
{
    bool retVal;
    
    retVal = false;
    if ( ( 0x01U << rollerIndex ) == ( Roller_error_cfg & ( 0x01U << rollerIndex ) ) )
    {
        retVal = Error_IsActive( Roller_obj[rollerIndex].error[errorIndex] );
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
void Roller_Init( void )
{
  static const uint16_t ROLLER_PARAM[ROLLER_NUM_MAX][ROLLER_PARAM_INDEX_NUM] = 
  {
    {ROLLER_W_SGN_MIN_A, ROLLER_W_SGN_MAX_A, ROLLER_W_SGN_ERR_TH_A, ROLLER_W_SGN_MIN_B, ROLLER_W_SGN_MAX_B, ROLLER_W_SGN_ERR_TH_B,
    ROLLER_W_RED_MIN_A, ROLLER_W_RED_MAX_A, ROLLER_W_RED_ERR_TH_A, ROLLER_W_RED_MIN_B, ROLLER_W_RED_MAX_B, ROLLER_W_RED_ERR_TH_B},
    {ROLLER_Z_SGN_MIN_A, ROLLER_Z_SGN_MAX_A, ROLLER_Z_SGN_ERR_TH_A, ROLLER_Z_SGN_MIN_B, ROLLER_Z_SGN_MAX_B, ROLLER_Z_SGN_ERR_TH_B,
    ROLLER_Z_RED_MIN_A, ROLLER_Z_RED_MAX_A, ROLLER_Z_RED_ERR_TH_A, ROLLER_Z_RED_MIN_B, ROLLER_Z_RED_MAX_B, ROLLER_Z_RED_ERR_TH_B},
    {ROLLER_J_SGN_MIN_A, ROLLER_J_SGN_MAX_A, ROLLER_J_SGN_ERR_TH_A, ROLLER_J_SGN_MIN_B, ROLLER_J_SGN_MAX_B, ROLLER_J_SGN_ERR_TH_B,
    ROLLER_J_RED_MIN_A, ROLLER_J_RED_MAX_A, ROLLER_J_RED_ERR_TH_A, ROLLER_J_RED_MIN_B, ROLLER_J_RED_MAX_B, ROLLER_J_RED_ERR_TH_B},
    {ROLLER_Q_SGN_MIN_A, ROLLER_Q_SGN_MAX_A, ROLLER_Q_SGN_ERR_TH_A, ROLLER_Q_SGN_MIN_B, ROLLER_Q_SGN_MAX_B, ROLLER_Q_SGN_ERR_TH_B,
    ROLLER_Q_RED_MIN_A, ROLLER_Q_RED_MAX_A, ROLLER_Q_RED_ERR_TH_A, ROLLER_Q_RED_MIN_B, ROLLER_Q_RED_MAX_B, ROLLER_Q_RED_ERR_TH_B},
    {ROLLER_V_SGN_MIN_A, ROLLER_V_SGN_MAX_A, ROLLER_V_SGN_ERR_TH_A, ROLLER_V_SGN_MIN_B, ROLLER_V_SGN_MAX_B, ROLLER_V_SGN_ERR_TH_B,
    ROLLER_V_RED_MIN_A, ROLLER_V_RED_MAX_A, ROLLER_V_RED_ERR_TH_A, ROLLER_V_RED_MIN_B, ROLLER_V_RED_MAX_B, ROLLER_V_RED_ERR_TH_B},
  };
  
  static const uint32_t ROLLER_ERROR_VALUE[ROLLER_ERROR_NUM] = 
  {
    ROLLER_ERROR_VALUE_VALUE_HIGH,
    ROLLER_ERROR_VALUE_VALUE_LOW,
    ROLLER_ERROR_VALUE_REDUNDANCE,
    ROLLER_ERROR_VALUE_VBB,
    ROLLER_ERROR_VALUE_VCC,
    ROLLER_ERROR_VALUE_VCC_EXT,
//    ROLLER_ERROR_VALUE_SAFETY,
    ROLLER_ERROR_VALUE_MEMORY,
  };
  static const uint32_t ROLLER_ERROR_CNT_TH[ROLLER_ERROR_NUM] = 
  {
    ROLLER_ERROR_CNT_TH_OUT_OF_RANGE_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_OUT_OF_RANGE_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_REDUNDANCE_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_VBB_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_VCC_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_VCC_EXT_US / SCHEDULE_COUNTER_TH_IO,
//    ROLLER_ERROR_CNT_TH_SAFETY_US / SCHEDULE_COUNTER_TH_IO,
    ROLLER_ERROR_CNT_TH_MEMORY_US / SCHEDULE_COUNTER_TH_IO,
  };
  static const uint8_t ROLLER_ADC_REF_INDEX_SGN[ROLLER_NUM_MAX] = 
  {
    ROLLER_W_IN_SGN,
    ROLLER_Z_IN_SGN,
    ROLLER_J_IN_SGN,
    ROLLER_Q_IN_SGN,
    ROLLER_V_IN_SGN,
  };
  static const uint8_t ROLLER_ADC_REF_INDEX_RED[ROLLER_NUM_MAX] = 
  {
    ROLLER_W_IN_RED,
    ROLLER_Z_IN_RED,
    ROLLER_J_IN_RED,
    ROLLER_Q_IN_RED,
    ROLLER_V_IN_RED,
  };
  
    uint8_t i;
    uint8_t j;
    
    for ( i=0U; i<ROLLER_NUM_MAX; i++ )
    {
        Roller_obj[i].in = 2500U;
        Roller_obj[i].out = 0xFFFFU;
        Roller_obj[i].minA = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_MIN_A];
        Roller_obj[i].maxA = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_MAX_A];
        Roller_obj[i].minB = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_MIN_B];
        Roller_obj[i].maxB = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_MAX_B];
        Roller_obj[i].minARed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_MIN_A];
        Roller_obj[i].maxARed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_MAX_A];
        Roller_obj[i].minBRed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_MIN_B];
        Roller_obj[i].maxBRed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_MAX_B];        
        Roller_obj[i].errThA = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_ERR_TH_A];
        Roller_obj[i].errThB = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_SGN_ERR_TH_B];
        Roller_obj[i].errThARed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_ERR_TH_A];
        Roller_obj[i].errThBRed = ROLLER_PARAM[i][ROLLER_PARAM_INDEX_RED_ERR_TH_B]; 
        Roller_signalMainIndex[i] = ROLLER_ADC_REF_INDEX_SGN[i];
        Roller_signalRedundanceIndex[i] = ROLLER_ADC_REF_INDEX_RED[i];        
        for ( j=0U; j<ROLLER_ERROR_NUM; j++ )
        {
            Roller_obj[i].error[j].value = ROLLER_ERROR_VALUE[j];
            Roller_obj[i].error[j].cntTh = ROLLER_ERROR_CNT_TH[j];
            Roller_obj[i].error[j].active = false;
            Roller_obj[i].error[j].occurrence = 0U;
            Roller_obj[i].error[j].cnt = 0U;
            Roller_obj[i].error[j].en = true;
        }
    }
    
    Roller_cfg = ROLLER_CFG_DEFAULT;
    Roller_error_cfg = ROLLER_ERROR_CFG_DEFAULT;
    Roller_CfgUpdate();

    Roller_paramToRAMUpdate( W_INIT );
    
    return;
}

//------------------------------------------------------------------------------
void Roller_Main( void )
{
    uint8_t i;
    
    static bool first_entry_in_main = true;
    
    if ( true == first_entry_in_main )
    {
        Roller_paramFromRAMUpdate();
        first_entry_in_main = false;
    }    
    
    for ( i=0U; i<ROLLER_NUM_MAX; i++ )
    {
        Roller_InputEval( i );
        Roller_OutputEval( i, 1000U );
    }
    
    Roller_paramToRAMUpdate( W_MAIN );
    
    return;
}
             
