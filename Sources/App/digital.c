/*
 * adc.c
 *
 *  Created on: 14 jun 2018
 *      Author: PoliD
 */


#include "application.h"

//------------------------------------------------------------------------------
#define DIGITAL_TH_ON         ( DIGITAL_TH_ON_US  / SCHEDULE_COUNTER_TH_IO )
#define DIGITAL_TH_OFF        ( DIGITAL_TH_OFF_US / SCHEDULE_COUNTER_TH_IO )
#define DIGITAL_TH_RED        ( DIGITAL_TH_RED_US / SCHEDULE_COUNTER_TH_IO )

#define DIGITAL_STATUS_ERROR  ( 0U )
#define DIGITAL_STATUS_OFF    ( 1U )
#define DIGITAL_STATUS_ON     ( 2U )

//------------------------------------------------------------------------------
typedef struct digital_ch_e
{
    uint8_t value;
    uint8_t vslueRed;
    uint8_t adcRefIndex;
    uint8_t adcRefIndexRed;
    uint8_t status;
    uint8_t inputType;
    uint8_t redType;
    bool waitValueNeed;
    uint8_t startingValue;
    uint16_t cnt;
    uint16_t cntRed;
    uint16_t thOn;
    uint16_t thOff;
} digital_ch_t;

//------------------------------------------------------------------------------
static digital_ch_t Digital_In[DIGITAL_INPUT_NUM] = 
{
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_0, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV_VBB, DIGITAL_IN_OFF_TH_MV_VBB},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_1, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_2, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV_VBB, DIGITAL_IN_OFF_TH_MV_VBB}, //M3
    //{DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_2, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV}, //M2    
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_3, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_4, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_5, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_6, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_7, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_8, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV_VBB, DIGITAL_IN_OFF_TH_MV_VBB}, //M3
    //{DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_8, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV}, //M2
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_9, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
//    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_10, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI,                            true, DIGITAL_IN_LEVEL_LO, 0U,     DIGITAL_IN_ON_TH_MV,     DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_10, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV,     DIGITAL_IN_OFF_TH_MV}, //CHECK_MISRA
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_11, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_12, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
    {DIGITAL_IN_OFF, DIGITAL_IN_OFF, DIGITAL_IN_ADC_REF_13, DIGITAL_IN_NOT_CONNECTED, DIGITAL_STATUS_ERROR, DIGITAL_ON_LEVEL_HI, DIGITAL_RED_TYPE_DISABLED, true, DIGITAL_IN_LEVEL_LO, 0U, 0U, DIGITAL_IN_ON_TH_MV, DIGITAL_IN_OFF_TH_MV},
};

static uint32_t Digital_cfgInputType;
static uint32_t Digital_cfgWaitNeed;
static uint32_t Digital_cfgWaitValue;
static uint32_t Digital_inputMonitor_1_16;
static uint32_t Digital_cfgSetup_1_2;
static uint32_t Digital_cfgSetup_3_4;
static uint32_t Digital_cfgSetup_5_6;
static uint32_t Digital_cfgSetup_7_8;
static uint32_t Digital_cfgSetup_9_10;
static uint32_t Digital_cfgSetup_11_12;
static uint32_t Digital_cfgSetup_13_14;
static bool Digital_inputExternalError;

static bool Digital_InputErrorCheck( void );
static void Digital_InputEval(digital_ch_t *dgt_in);
static void Digital_paramToRAMUpdate( uint8_t status );
static void Digital_paramFromRAMUpdate( void );
static void Digital_CfgUpdate( void );

//------------------------------------------------------------------------------
static bool Digital_InputErrorCheck( void )
{
    bool retVal;
    
    retVal = false;
    if ( SUPPLY_VBB_PROT_ERR_NO != Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) )
    {
        retVal = true;
    }
    if ( SUPPLY_VCC_5VDC_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_ERROR ) )
    {
        retVal = true;
    }
    if ( SUPPLY_VCC_5VDC_EXT_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_EXT_ERROR ) ) 
    {
        retVal = true;
    }
    if ( MEM_DATA_NO_ERROR != MemData_ErrorGet() )
    {
        retVal = true;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static void Digital_InputEval(digital_ch_t *dgt_in)
{
    uint16_t adcRef = ADC_GetValue_mV( dgt_in->adcRefIndex );
    uint16_t adcRefRed = ADC_GetValue_mV( dgt_in->adcRefIndexRed );
  
    if ( DIGITAL_IN_NOT_CONNECTED == dgt_in->adcRefIndex  )
    {
        dgt_in->value = DIGITAL_IN_OFF;
    }
    else
    {
        switch ( dgt_in->status ) {
        case DIGITAL_STATUS_ERROR:
            dgt_in->value = DIGITAL_IN_ERROR;
            if ( false == Digital_inputExternalError ) 
            {
                if ( true == dgt_in->waitValueNeed )
                {
                    if ( DIGITAL_IN_LEVEL_HI == dgt_in->startingValue )
                    {
                        if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType ) 
                        {
                        if ( adcRef >= dgt_in->thOn )
                            {
                                 ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                                {
                                    dgt_in->cnt = 0U;
                                    dgt_in->status = DIGITAL_STATUS_OFF;
                                    dgt_in->value = DIGITAL_IN_OFF;
                                }
                            }
                            else
                            {
                                dgt_in->cnt = 0U;
                            }
                        } 
                        else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType )
                        {
                            if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed >= dgt_in->thOn ) )
                            {
                                ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                                {
                                    dgt_in->cnt = 0U;
                                    dgt_in->status = DIGITAL_STATUS_OFF;
                                    dgt_in->value = DIGITAL_IN_OFF;
                                }
                            }
                            else
                            {
                                dgt_in->cnt = 0U;
                            }                          
                        }
                        else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                        {
                            if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed <= dgt_in->thOff ) )
                            {
                                ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                                {
                                dgt_in->cnt = 0U;
                                dgt_in->status = DIGITAL_STATUS_OFF;
                                dgt_in->value = DIGITAL_IN_OFF;
                            }
                        }
                        else
                        {
                            dgt_in->cnt = 0U;
                        }
                    }
                    else
                    {
                        }
                    }
                    else
                    {
                        if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType ) 
                        {
                            if ( adcRef <= dgt_in->thOff )
                            {
                                ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                            {
                                dgt_in->cnt = 0U;
                                dgt_in->status = DIGITAL_STATUS_OFF;
                                dgt_in->value = DIGITAL_IN_OFF;
                            }
                        }
                        else
                        {
                            dgt_in->cnt = 0U;
                            }              
                        }
                        else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType ) 
                        {
                            if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed <= dgt_in->thOff ) )
                            {
                                ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                                {
                                    dgt_in->cnt = 0U;
                                    dgt_in->status = DIGITAL_STATUS_OFF;
                                    dgt_in->value = DIGITAL_IN_OFF;
                                }
                            }
                            else
                            {
                                dgt_in->cnt = 0U;
                            }                          
                        }
                        else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                        {
                            if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed >= dgt_in->thOn ) )
                            {
                                ++dgt_in->cnt;
                                if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                                {
                                    dgt_in->cnt = 0U;
                                    dgt_in->status = DIGITAL_STATUS_OFF;
                                    dgt_in->value = DIGITAL_IN_OFF;
                                }
                            }
                            else
                            {
                                dgt_in->cnt = 0U;
                            }                          
                        }
                        else
                        {
                        }              
                    }
                }
                else
                {
                    dgt_in->cnt = 0U;
                    dgt_in->status = DIGITAL_STATUS_OFF;
                    dgt_in->value = DIGITAL_IN_OFF;
                }
            }
//            dgt_in->value = DIGITAL_IN_OFF;
            break;
        case DIGITAL_STATUS_OFF:
            if ( DIGITAL_ON_LEVEL_HI == dgt_in->inputType )
            {
                if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType )
                {
                    if ( adcRef >= dgt_in->thOn )
                    {
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_ON )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_ON;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                    }
                }
                else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed >= dgt_in->thOn ) ) 
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_ON )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_ON;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }
                    }                  
                }
                else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;                      
                    }
                    else if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_ON )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_ON;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else 
                {
                }
            }
            else
            {
                if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType )
                {
                    if ( adcRef <= dgt_in->thOff )
                    {
                      ++dgt_in->cnt;
                    if ( dgt_in->cnt >= DIGITAL_TH_ON )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->status = DIGITAL_STATUS_ON;
                    }
                }
                else
                {
                    dgt_in->cnt = 0U;
                }          
            }
                else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( adcRef <= dgt_in->thOff )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_ON )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_ON;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_ON )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_ON;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }
                    }                  
                }
                else 
                {
                }
            }
            dgt_in->value = DIGITAL_IN_OFF;
            if ( true == Digital_inputExternalError )
            {
                dgt_in->cnt = 0U;
                dgt_in->cntRed = 0U;
                dgt_in->status = DIGITAL_STATUS_ERROR;
            }
            break;
        case DIGITAL_STATUS_ON:
            if ( DIGITAL_ON_LEVEL_HI == dgt_in->inputType )
            {
                if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType )
                {
                    if ( adcRef <= dgt_in->thOff )
                    {
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                    }
                }
                else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                            dgt_in->cnt = 0U;
                            dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else
                {
                  // Do Nothing
                }
            }
            else
            {
                if ( DIGITAL_RED_TYPE_DISABLED == dgt_in->redType )
                {
                    if ( adcRef >= dgt_in->thOn )
                    {
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                           dgt_in->cnt = 0U;
                           dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                    }          
                }
                else if ( DIGITAL_RED_TYPE_NOT_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                           dgt_in->cnt = 0U;
                           dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else if ( DIGITAL_RED_TYPE_INVERTED == dgt_in->redType )
                {
                    if ( ( adcRef <= dgt_in->thOff ) && ( adcRefRed >= dgt_in->thOn ) )
                    {
                        dgt_in->cnt = 0U;
                        dgt_in->cntRed = 0U;
                    }
                    else if ( ( adcRef >= dgt_in->thOn ) && ( adcRefRed <= dgt_in->thOff ) )
                    {
                        dgt_in->cntRed = 0U;
                        ++dgt_in->cnt;
                        if ( dgt_in->cnt >= DIGITAL_TH_OFF )
                        {
                           dgt_in->cnt = 0U;
                           dgt_in->status = DIGITAL_STATUS_OFF;
                        }
                    }
                    else
                    {
                        dgt_in->cnt = 0U;
                        ++dgt_in->cntRed;
                        if ( dgt_in->cntRed >= DIGITAL_TH_RED )
                        {
                            dgt_in->cntRed = 0U;
                            dgt_in->status = DIGITAL_STATUS_ERROR;
                        }                        
                    }                  
                }
                else
                {
                }
            }
            dgt_in->value = DIGITAL_IN_ON;
            if ( true == Digital_inputExternalError )
            {
                dgt_in->cnt = 0U;
                dgt_in->cntRed = 0U;
                dgt_in->status = DIGITAL_STATUS_ERROR;
            }
            break;        
      default:
          break;
      }
  }
  
  return;
}

//------------------------------------------------------------------------------
static void Digital_paramToRAMUpdate( uint8_t status )
{
#ifndef DJW_MEM_DISABLED  
    if ( W_INIT == status )
    {
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_INPUT_TYPE, Digital_cfgInputType );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_WAIT_NEED, Digital_cfgWaitNeed );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_WAIT_VALUE, Digital_cfgWaitValue );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_1_2, Digital_cfgSetup_1_2 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_3_4, Digital_cfgSetup_3_4 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_5_6, Digital_cfgSetup_5_6 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_7_8, Digital_cfgSetup_7_8 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_9_10, Digital_cfgSetup_9_10 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_11_12, Digital_cfgSetup_11_12 );
        MemData_SetRAMItem( ADDR_DIGITAL_CFG_SETUP_13_14, Digital_cfgSetup_13_14 );
    }
    else
    {
        MemData_SetRAMItem( ADDR_DIGITAL_INPUT_MONITOR_1_16, Digital_inputMonitor_1_16 );
    }

#endif
    return;
}

//------------------------------------------------------------------------------
static void Digital_paramFromRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED  
    uint32_t datoRead;
    
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_INPUT_TYPE, &datoRead ) )
    {
        Digital_cfgInputType = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_WAIT_NEED, &datoRead ) )
    {
        Digital_cfgWaitNeed = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_WAIT_VALUE, &datoRead ) )
    {
        Digital_cfgWaitValue = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_1_2, &datoRead ) )
    {
        Digital_cfgSetup_1_2 = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_3_4, &datoRead ) )
    {
        Digital_cfgSetup_3_4 = (uint32_t)( datoRead ); 
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_5_6, &datoRead ) )
    {
        Digital_cfgSetup_5_6 = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_7_8, &datoRead ) )
    {
        Digital_cfgSetup_7_8 = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_9_10, &datoRead ) )
    {
        Digital_cfgSetup_9_10 = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_11_12, &datoRead ) )
    {
        Digital_cfgSetup_11_12 = (uint32_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_DIGITAL_CFG_SETUP_13_14, &datoRead ) )
    {
        Digital_cfgSetup_13_14 = (uint32_t)( datoRead );
    }
#endif    
    return;
}

//------------------------------------------------------------------------------
static void Digital_CfgUpdate( void )
{
    uint8_t i;
    
    for ( i=0U; i<DIGITAL_INPUT_NUM; i++ )
    {
        if ( ( (uint32_t)( 1UL << i ) ) == ( Digital_cfgInputType & ( (uint32_t)( 1UL << i ) ) ) )
        {
            Digital_In[i].inputType = DIGITAL_ON_LEVEL_HI;
        }
        else
        {
            Digital_In[i].inputType = DIGITAL_ON_LEVEL_LO;
        }
        if ( ( (uint32_t)( 1UL << i ) ) == ( Digital_cfgWaitNeed & ( (uint32_t)( 1UL << i ) ) ) )
        {
            Digital_In[i].waitValueNeed = true;
        }
        else
        {
            Digital_In[i].waitValueNeed = false;
        }
        if ( ( (uint32_t)( 1UL << i ) ) == ( Digital_cfgWaitValue & ( (uint32_t)( 1UL << i ) ) ) )
        {
            Digital_In[i].startingValue = DIGITAL_IN_LEVEL_HI;
        }
        else
        {
            Digital_In[i].startingValue = DIGITAL_IN_LEVEL_LO;
        }        
    }
    Digital_In[7U].waitValueNeed = false;
    Digital_In[0u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_1_2        ) & 0x1Fu );
    Digital_In[0u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_1_2 >> 5u  ) & 0x1Fu );
    Digital_In[0u].redType =        (uint8_t)( ( Digital_cfgSetup_1_2 >> 10u ) & 0x03u );
    Digital_In[1u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_1_2 >> 16u ) & 0x1Fu );
    Digital_In[1u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_1_2 >> 21u ) & 0x1Fu );
    Digital_In[1u].redType =        (uint8_t)( ( Digital_cfgSetup_1_2 >> 26u ) & 0x03u );        

    Digital_In[2u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_3_4        ) & 0x1Fu );
    Digital_In[2u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_3_4 >> 5u  ) & 0x1Fu );
    Digital_In[2u].redType =        (uint8_t)( ( Digital_cfgSetup_3_4 >> 10u ) & 0x03u );
    Digital_In[3u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_3_4 >> 16u ) & 0x1Fu );
    Digital_In[3u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_3_4 >> 21u ) & 0x1Fu );
    Digital_In[3u].redType =        (uint8_t)( ( Digital_cfgSetup_3_4 >> 26u ) & 0x03u );        

    Digital_In[4u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_5_6        ) & 0x1Fu );
    Digital_In[4u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_5_6 >> 5u  ) & 0x1Fu );
    Digital_In[4u].redType =        (uint8_t)( ( Digital_cfgSetup_5_6 >> 10u ) & 0x03u );
    Digital_In[5u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_5_6 >> 16u ) & 0x1Fu );
    Digital_In[5u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_5_6 >> 21u ) & 0x1Fu );
    Digital_In[5u].redType =        (uint8_t)( ( Digital_cfgSetup_5_6 >> 26u ) & 0x03u );        
  
    Digital_In[6u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_7_8        ) & 0x1Fu );
    Digital_In[6u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_7_8 >> 5u  ) & 0x1Fu );
    Digital_In[6u].redType =        (uint8_t)( ( Digital_cfgSetup_7_8 >> 10u ) & 0x03u );
    Digital_In[7u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_7_8 >> 16u ) & 0x1Fu );
    Digital_In[7u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_7_8 >> 21u ) & 0x1Fu );
    Digital_In[7u].redType =        (uint8_t)( ( Digital_cfgSetup_7_8 >> 26u ) & 0x03u );        

    Digital_In[8u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_9_10        ) & 0x1Fu );
    Digital_In[8u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_9_10 >> 5u  ) & 0x1Fu );
    Digital_In[8u].redType =        (uint8_t)( ( Digital_cfgSetup_9_10 >> 10u ) & 0x03u );
    Digital_In[9u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_9_10 >> 16u ) & 0x1Fu );
    Digital_In[9u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_9_10 >> 21u ) & 0x1Fu );
    Digital_In[9u].redType =        (uint8_t)( ( Digital_cfgSetup_9_10 >> 26u ) & 0x03u );        

    Digital_In[10u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_11_12        ) & 0x1Fu );
    Digital_In[10u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_11_12 >> 5u  ) & 0x1Fu );
    Digital_In[10u].redType =        (uint8_t)( ( Digital_cfgSetup_11_12 >> 10u ) & 0x03u );
    Digital_In[11u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_11_12 >> 16u ) & 0x1Fu );
    Digital_In[11u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_11_12 >> 21u ) & 0x1Fu );
    Digital_In[11u].redType =        (uint8_t)( ( Digital_cfgSetup_11_12 >> 26u ) & 0x03u );        

    Digital_In[12u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_13_14        ) & 0x1Fu );
    Digital_In[12u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_13_14 >> 5u  ) & 0x1Fu );
    Digital_In[12u].redType =        (uint8_t)( ( Digital_cfgSetup_13_14 >> 10u ) & 0x03u );
    Digital_In[13u].adcRefIndex =    (uint8_t)( ( Digital_cfgSetup_13_14 >> 16u ) & 0x1Fu );
    Digital_In[13u].adcRefIndexRed = (uint8_t)( ( Digital_cfgSetup_13_14 >> 21u ) & 0x1Fu );
    Digital_In[13u].redType =        (uint8_t)( ( Digital_cfgSetup_13_14 >> 26u ) & 0x03u );        
  
    return;
}

//------------------------------------------------------------------------------
uint8_t Digital_GetValue( uint8_t index )
{
    uint8_t i;
    uint8_t retVal;
    
    retVal = 0xFFu;
    for ( i=0U; i<DIGITAL_INPUT_NUM; i++ )
    {
        //if ( index == Digital_In[i].adcRefIndex )
        if ( index == i )
        {
            retVal = Digital_In[i].value;
            break;
        }
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
bool Digital_GetExternalError( void )
{
    return Digital_inputExternalError;
}

//------------------------------------------------------------------------------
void Digital_Init( void )
{
    Digital_cfgInputType = DEFAULT_CFG_INPUT_TYPE;
    Digital_cfgWaitNeed = DEFAULT_CFG_WAIT_NEED;
    Digital_cfgWaitValue = DEFAULT_CFG_WAIT_VALUE;
    Digital_inputMonitor_1_16 = 0x00000000U;
    Digital_inputExternalError = false;

    Digital_cfgSetup_1_2   = (uint32_t)( DIGITAL_IN_0_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_0_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_0_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_1_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_1_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_1_RED_TYPE   & 0x03u ) << 26u );
    Digital_cfgSetup_3_4   = (uint32_t)( DIGITAL_IN_2_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_2_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_2_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_3_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_3_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_3_RED_TYPE   & 0x03u ) << 26u );
    Digital_cfgSetup_5_6   = (uint32_t)( DIGITAL_IN_4_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_4_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_4_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_5_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_5_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_5_RED_TYPE   & 0x03u ) << 26u );    
    Digital_cfgSetup_7_8   = (uint32_t)( DIGITAL_IN_6_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_6_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_6_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_7_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_7_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_7_RED_TYPE   & 0x03u ) << 26u );  
    Digital_cfgSetup_9_10  = (uint32_t)( DIGITAL_IN_8_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_8_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_8_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_9_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_9_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_9_RED_TYPE   & 0x03u ) << 26u );
    Digital_cfgSetup_11_12 = (uint32_t)( DIGITAL_IN_10_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_10_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_10_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_11_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_11_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_11_RED_TYPE   & 0x03u ) << 26u );   
    Digital_cfgSetup_13_14 = (uint32_t)( DIGITAL_IN_12_INPUT_MAIN & 0x1Fu ) |
                             (uint32_t)( ( DIGITAL_IN_12_INPUT_RED  & 0x1Fu ) <<  5u ) | 
                             (uint32_t)( ( DIGITAL_IN_12_RED_TYPE   & 0x03u ) << 10u ) |
                             (uint32_t)( ( DIGITAL_IN_13_INPUT_MAIN & 0x1Fu ) << 16u ) |   
                             (uint32_t)( ( DIGITAL_IN_13_INPUT_RED  & 0x1Fu ) << 21u ) |
                             (uint32_t)( ( DIGITAL_IN_13_RED_TYPE   & 0x03u ) << 26u ); 
    Digital_paramToRAMUpdate( W_INIT );
    
    return;
}

//------------------------------------------------------------------------------
void Digital_Main( void )
{
    uint8_t i;

    static bool first_entry_in_main = true;

    if ( true == first_entry_in_main )
    {
        Digital_paramFromRAMUpdate();
        Digital_CfgUpdate();
        first_entry_in_main = false;
    }
    
    Digital_inputMonitor_1_16 = 0x00000000U;
    Digital_inputExternalError = Digital_InputErrorCheck();
    for ( i=0U; i<DIGITAL_INPUT_NUM; i++ )
    {
        Digital_InputEval( &Digital_In[i] );
        
        Digital_inputMonitor_1_16 |= ( (uint32_t)Digital_In[i].value << ( i * 2U ) );
    }
    
    Digital_paramToRAMUpdate( W_MAIN );
                         
  return;	
}
