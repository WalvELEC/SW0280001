/*
 * supply.c
 *
 *  Created on: 05 jul 2018
 *      Author: PoliD
 */

#include "application.h"

//------------------------------------------------------------------------------
//Vbb Prot
#define SUPPLY_VBB_PROT_AN_IN         ( ADC_IN_24V_SYS )
#define SUPPLY_VBB_PROT_TH_HYST_mV    ( 500U )
#define SUPPLY_VBB_PROT_TH_LO_mV      ( 7550U )
#define SUPPLY_VBB_PROT_TH_HI_mV      ( 31300U )
#define SUPPLY_VBB_PROT_TH_CAN_OFF_mV ( 3500U ) 

#define SUPPLY_VBB_PROT_STATUS_NOMINAL        ( 0x00U )                   
#define SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_LO  ( 0x01U )                   
#define SUPPLY_VBB_PROT_STATUS_LO             ( 0x02U ) 
#define SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_HI  ( 0x03U )                   
#define SUPPLY_VBB_PROT_STATUS_HI             ( 0x04U )   
#define SUPPLY_VBB_PROT_STATUS_LO_TO_CAN_OFF  ( 0x05U )
#define SUPPLY_VBB_PROT_STATUS_CAN_OFF        ( 0x06U )

#define SUPPLY_VBB_PROT_ERR_CNT_TH            ( 400000U / SCHEDULE_COUNTER_TH_IO )
#define SUPPLY_VBB_PROT_ERR_CAN_OFF_CNT_TH    ( 100000U / SCHEDULE_COUNTER_TH_IO )

//Vcc 5VDC
#define SUPPLY_VCC_5VDC_AN_IN        ( ADC_IN_VREF_ADC0 )
#define SUPPLY_VCC_5VDC_TH_HYST_mV   ( 50U )
#define SUPPLY_VCC_5VDC_TH_LO_mV     ( 2400U )
#define SUPPLY_VCC_5VDC_TH_HI_mV     ( 2600U )

#define SUPPLY_VCC_5VDC_STATUS_NOMINAL        ( 0x00U )                   
#define SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_LO  ( 0x01U )                   
#define SUPPLY_VCC_5VDC_STATUS_LO             ( 0x02U )                   
#define SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_HI  ( 0x03U )                   
#define SUPPLY_VCC_5VDC_STATUS_HI             ( 0x04U )

#define SUPPLY_VCC_5VDC_ERR_CNT_TH  ( 400000U / SCHEDULE_COUNTER_TH_IO )

//Vcc 5VDC EXT
#define SUPPLY_VCC_5VDC_EXT_AN_IN        ( ADC_IN_5V_SENSOR )
#define SUPPLY_VCC_5VDC_EXT_TH_HYST_mV   ( 100U )
#define SUPPLY_VCC_5VDC_EXT_TH_LO_mV     ( 4800U )
#define SUPPLY_VCC_5VDC_EXT_TH_HI_mV     ( 5200U )

#define SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL        ( 0x00U )                   
#define SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_LO  ( 0x01U )                   
#define SUPPLY_VCC_5VDC_EXT_STATUS_LO             ( 0x02U )                   
#define SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_HI  ( 0x03U )                   
#define SUPPLY_VCC_5VDC_EXT_STATUS_HI             ( 0x04U )

#define SUPPLY_VCC_5VDC_EXT_ERR_CNT_TH  ( 400000U / SCHEDULE_COUNTER_TH_IO )

#define SUPPLY_ERR_MASK ( 0x03U )

//------------------------------------------------------------------------------
typedef struct w_supply_e
{
    uint16_t value;
    uint8_t adcRefIndex;
    uint16_t errHysteresis;
    uint16_t errThHigh;
    uint16_t errThLow;
    uint8_t error;
    w_status_t status;
} w_supply_t;

//------------------------------------------------------------------------------
static w_supply_t Supply_vbbProt;
static w_supply_t Supply_vcc;
static w_supply_t Supply_vccExt;

//------------------------------------------------------------------------------
static wres_t Supply_VbbProtInit( void )
{
    Supply_vbbProt.adcRefIndex = SUPPLY_VBB_PROT_AN_IN;
    Supply_vbbProt.errHysteresis = SUPPLY_VBB_PROT_TH_HYST_mV;
    Supply_vbbProt.errThHigh = SUPPLY_VBB_PROT_TH_HI_mV;
    Supply_vbbProt.errThLow = SUPPLY_VBB_PROT_TH_LO_mV;
    Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_NO;
    Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL;
    Supply_vbbProt.status.previous = SUPPLY_VBB_PROT_STATUS_NOMINAL;
    Supply_vbbProt.status.cnt = 0U;
    Supply_vbbProt.status.cntTh = SUPPLY_VBB_PROT_ERR_CNT_TH;    
    
    return WRES_OK;
}

//------------------------------------------------------------------------------
static wres_t Supply_VccInit( void )
{
    Supply_vcc.adcRefIndex = SUPPLY_VCC_5VDC_AN_IN;
    Supply_vcc.errHysteresis = SUPPLY_VCC_5VDC_TH_HYST_mV;
    Supply_vcc.errThHigh = SUPPLY_VCC_5VDC_TH_HI_mV;
    Supply_vcc.errThLow = SUPPLY_VCC_5VDC_TH_LO_mV;
    Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_NO;
    Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL;
    Supply_vcc.status.previous = SUPPLY_VCC_5VDC_STATUS_NOMINAL;
    Supply_vcc.status.cnt = 0U;
    Supply_vcc.status.cntTh = SUPPLY_VCC_5VDC_ERR_CNT_TH;    
    
    return WRES_OK;
}

//------------------------------------------------------------------------------
static wres_t Supply_VccExtInit( void )
{
    Supply_vccExt.adcRefIndex = SUPPLY_VCC_5VDC_EXT_AN_IN;
    Supply_vccExt.errHysteresis = SUPPLY_VCC_5VDC_EXT_TH_HYST_mV;
    Supply_vccExt.errThHigh = SUPPLY_VCC_5VDC_EXT_TH_HI_mV;
    Supply_vccExt.errThLow = SUPPLY_VCC_5VDC_EXT_TH_LO_mV;
    Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_NO;
    Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
    Supply_vccExt.status.previous = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
    Supply_vccExt.status.cnt = 0U;
    Supply_vccExt.status.cntTh = SUPPLY_VCC_5VDC_EXT_ERR_CNT_TH;    
    
    return WRES_OK;
}

//------------------------------------------------------------------------------
static wres_t Supply_VbbProtMain( void )
{
    wres_t retVal;
  
    retVal = WRES_OK;
    
    Supply_vbbProt.value = ADC_GetValue_mV( Supply_vbbProt.adcRefIndex );
    
    switch ( Supply_vbbProt.status.value )
    {
    case SUPPLY_VBB_PROT_STATUS_NOMINAL:
        if ( Supply_vbbProt.value <= ( Supply_vbbProt.errThLow -  Supply_vbbProt.errHysteresis ) ) 
        {
            Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_LO;
            Supply_vbbProt.status.cnt = 0U;
        }
        else if ( Supply_vbbProt.value >= ( Supply_vbbProt.errThHigh +  Supply_vbbProt.errHysteresis ) ) 
        {
            Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_HI;
            Supply_vbbProt.status.cnt = 0U;
        }   
        else {}
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_NO;
        break;
    case SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_LO:
        if ( Supply_vbbProt.value > ( Supply_vbbProt.errThLow -  Supply_vbbProt.errHysteresis ) ) 
        {
            Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL;
            Supply_vbbProt.status.cnt = 0U;
        }
        else
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= Supply_vbbProt.status.cntTh ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_LO;
                Supply_vbbProt.status.cnt = 0U;
            }
        }    
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_NO;        
        break;
    case SUPPLY_VBB_PROT_STATUS_LO:
        if ( Supply_vbbProt.value >= ( Supply_vbbProt.errThLow +  Supply_vbbProt.errHysteresis ) ) 
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= Supply_vbbProt.status.cntTh ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL;
                Supply_vbbProt.status.cnt = 0U;
            }
        } 
        else 
        {
            if ( Supply_vbbProt.value <= ( SUPPLY_VBB_PROT_TH_CAN_OFF_mV - Supply_vbbProt.errHysteresis ) )
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_LO_TO_CAN_OFF;
            }
            Supply_vbbProt.status.cnt = 0U;
        }
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_LO;        
        break;
    case SUPPLY_VBB_PROT_STATUS_NOMINAL_TO_HI:
        if ( Supply_vbbProt.value < ( Supply_vbbProt.errThHigh +  Supply_vbbProt.errHysteresis ) ) 
        {
            Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL;
            Supply_vbbProt.status.cnt = 0U;
        }
        else
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= Supply_vbbProt.status.cntTh ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_HI;
                Supply_vbbProt.status.cnt = 0U;
            }
        }
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_NO;
        break;
    case SUPPLY_VBB_PROT_STATUS_HI:
        if ( Supply_vbbProt.value <= ( Supply_vbbProt.errThHigh -  Supply_vbbProt.errHysteresis ) ) 
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= Supply_vbbProt.status.cntTh ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_NOMINAL;
                Supply_vbbProt.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vbbProt.status.cnt = 0U;
        }   
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_HI;        
        break; 
    case SUPPLY_VBB_PROT_STATUS_LO_TO_CAN_OFF:
        if ( Supply_vbbProt.value > ( SUPPLY_VBB_PROT_TH_CAN_OFF_mV -  Supply_vbbProt.errHysteresis ) ) 
        {
            Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_LO;
            Supply_vbbProt.status.cnt = 0U;
        }
        else
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= SUPPLY_VBB_PROT_ERR_CAN_OFF_CNT_TH ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_CAN_OFF;
                Supply_vbbProt.status.cnt = 0U;
            }
        }    
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_LO;        
        break;        
    case SUPPLY_VBB_PROT_STATUS_CAN_OFF:
        if ( Supply_vbbProt.value >= ( SUPPLY_VBB_PROT_TH_CAN_OFF_mV +  Supply_vbbProt.errHysteresis ) ) 
        {
            ++Supply_vbbProt.status.cnt;
            if ( Supply_vbbProt.status.cnt >= Supply_vbbProt.status.cntTh ) 
            {
                Supply_vbbProt.status.value = SUPPLY_VBB_PROT_STATUS_LO;
                Supply_vbbProt.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vbbProt.status.cnt = 0U;
        }
        Supply_vbbProt.error = SUPPLY_VBB_PROT_ERR_LO | SUPPLY_VBB_PROT_ERR_CAN_OFF;        
        break;        
    default:
        break;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t Supply_VccMain( void )
{
    wres_t retVal;
  
    retVal = WRES_OK;
    
    Supply_vcc.value = ADC_GetValue_mV( Supply_vcc.adcRefIndex );
    
    switch ( Supply_vcc.status.value )
    {
    case SUPPLY_VCC_5VDC_STATUS_NOMINAL:
        if ( Supply_vcc.value <= ( Supply_vcc.errThLow -  Supply_vcc.errHysteresis ) ) 
        {
            Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_LO;
            Supply_vcc.status.cnt = 0U;
        }
        else if ( Supply_vcc.value >= ( Supply_vcc.errThHigh +  Supply_vcc.errHysteresis ) ) 
        {
            Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_HI;
            Supply_vcc.status.cnt = 0U;
        }
        else {}
        Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_NO;
        break;
    case SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_LO:
        if ( Supply_vcc.value > ( Supply_vcc.errThLow -  Supply_vcc.errHysteresis ) ) 
        {
            Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL;
            Supply_vcc.status.cnt = 0U;
        }
        else
        {
            ++Supply_vcc.status.cnt;
            if ( Supply_vcc.status.cnt >= Supply_vcc.status.cntTh ) 
            {
                Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_LO;
                Supply_vcc.status.cnt = 0U;
            }
        }    
        Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_NO;        
        break;
    case SUPPLY_VCC_5VDC_STATUS_LO:
        if ( Supply_vcc.value >= ( Supply_vcc.errThLow +  Supply_vcc.errHysteresis ) ) 
        {
            ++Supply_vcc.status.cnt;
            if ( Supply_vcc.status.cnt >= Supply_vcc.status.cntTh ) 
            {
                Supply_vcc.status.value =SUPPLY_VCC_5VDC_STATUS_NOMINAL;
                Supply_vcc.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vcc.status.cnt = 0U;
        }
        Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_LO;        
        break;
    case SUPPLY_VCC_5VDC_STATUS_NOMINAL_TO_HI:
        if ( Supply_vcc.value < ( Supply_vcc.errThHigh +  Supply_vcc.errHysteresis ) ) 
        {
            Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL;
            Supply_vcc.status.cnt = 0U;
        }
        else
        {
            ++Supply_vcc.status.cnt;
            if ( Supply_vcc.status.cnt >= Supply_vcc.status.cntTh ) 
            {
                Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_HI;
                Supply_vcc.status.cnt = 0U;
            }
        }
        Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_NO;
        break;
    case SUPPLY_VCC_5VDC_STATUS_HI:
        if ( Supply_vcc.value <= ( Supply_vcc.errThHigh -  Supply_vcc.errHysteresis ) ) 
        {
            ++Supply_vcc.status.cnt;
            if ( Supply_vcc.status.cnt >= Supply_vcc.status.cntTh ) 
            {
                Supply_vcc.status.value = SUPPLY_VCC_5VDC_STATUS_NOMINAL;
                Supply_vcc.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vcc.status.cnt = 0U;
        }   
        Supply_vcc.error = SUPPLY_VCC_5VDC_ERR_HI;        
        break;      
    default:
        break;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t Supply_VccExtMain( void )
{
    wres_t retVal;
  
    retVal = WRES_OK;
    
    Supply_vccExt.value = ADC_GetValue_mV( Supply_vccExt.adcRefIndex );
    
    switch ( Supply_vccExt.status.value )
    {
    case SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL:
        if ( Supply_vccExt.value <= ( Supply_vccExt.errThLow -  Supply_vccExt.errHysteresis ) ) 
        {
            Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_LO;
            Supply_vccExt.status.cnt = 0U;
        }
        else if ( Supply_vccExt.value >= ( Supply_vccExt.errThHigh +  Supply_vccExt.errHysteresis ) ) 
        {
            Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_HI;
            Supply_vccExt.status.cnt = 0U;
        }
        else 
        {
          // Do Nothing
        }
        Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_NO;
        break;
    case SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_LO:
        if ( Supply_vccExt.value > ( Supply_vccExt.errThLow -  Supply_vccExt.errHysteresis ) ) 
        {
            Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
            Supply_vccExt.status.cnt = 0U;
        }
        else
        {
            ++Supply_vccExt.status.cnt;
            if ( Supply_vccExt.status.cnt >= Supply_vccExt.status.cntTh ) 
            {
                Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_LO;
                Supply_vccExt.status.cnt = 0U;
            }
        }    
        Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_NO;        
        break;
    case SUPPLY_VCC_5VDC_EXT_STATUS_LO:
        if ( Supply_vccExt.value >= ( Supply_vccExt.errThLow +  Supply_vccExt.errHysteresis ) ) 
        {
            ++Supply_vccExt.status.cnt;
            if ( Supply_vccExt.status.cnt >= Supply_vccExt.status.cntTh ) 
            {
                Supply_vccExt.status.value =SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
                Supply_vccExt.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vccExt.status.cnt = 0U;
        }
        Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_LO;        
        break;
    case SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL_TO_HI:
        if ( Supply_vccExt.value < ( Supply_vccExt.errThHigh +  Supply_vccExt.errHysteresis ) ) 
        {
            Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
            Supply_vccExt.status.cnt = 0U;
        }
        else
        {
            ++Supply_vccExt.status.cnt;
            if ( Supply_vccExt.status.cnt >= Supply_vccExt.status.cntTh ) 
            {
                Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_HI;
                Supply_vccExt.status.cnt = 0U;
            }
        }
        Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_NO;
        break;
    case SUPPLY_VCC_5VDC_EXT_STATUS_HI:
        if ( Supply_vccExt.value <= ( Supply_vccExt.errThHigh -  Supply_vccExt.errHysteresis ) ) 
        {
            ++Supply_vccExt.status.cnt;
            if ( Supply_vccExt.status.cnt >= Supply_vccExt.status.cntTh ) 
            {
                Supply_vccExt.status.value = SUPPLY_VCC_5VDC_EXT_STATUS_NOMINAL;
                Supply_vccExt.status.cnt = 0U;
            }
        } 
        else 
        {
            Supply_vccExt.status.cnt = 0U;
        }   
        Supply_vccExt.error = SUPPLY_VCC_5VDC_EXT_ERR_HI;        
        break;      
    default:
        break;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static void Supply_paramToRAMUpdate( void )
{
    MemData_SetRAMItem( ADDR_VBB_PROT_VALUE, (uint32_t)Supply_vbbProt.value );
    MemData_SetRAMItem( ADDR_VCC_VALUE, (uint32_t)Supply_vcc.value );
    MemData_SetRAMItem( ADDR_VCC_EXT_VALUE, (uint32_t)Supply_vccExt.value );
    
    return;
}

//------------------------------------------------------------------------------
uint8_t Supply_ErrorGet( uint8_t index )
{
    uint8_t retVal;

    retVal = 0U;
    if ( SUPPLY_VBB_PROT_ERROR == index )
    {
        retVal = Supply_vbbProt.error;
    }
    else
    if ( SUPPLY_VCC_5VDC_ERROR == index )
    {
        retVal = Supply_vcc.error;
    }
    else if ( SUPPLY_VCC_5VDC_EXT_ERROR == index )
    {
        retVal = Supply_vccExt.error;
    }
    else 
    {
      // Do Nothing
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
uint16_t Supply_VbbProtValueGet( void )
{
    return Supply_vbbProt.value;
}

//------------------------------------------------------------------------------
uint16_t Supply_VccValueGet( void )
{
    return Supply_vcc.value;
}

//------------------------------------------------------------------------------
uint16_t Supply_VccExtValueGet( void )
{
    return Supply_vccExt.value;
}

//------------------------------------------------------------------------------
wres_t Supply_Init( void )
{
    Supply_VbbProtInit();
    Supply_VccInit();
    Supply_VccExtInit();
    
    return WRES_OK;
}

//------------------------------------------------------------------------------
wres_t Supply_Main( void )
{
    Supply_VbbProtMain();
    Supply_VccMain();
    Supply_VccExtMain();
    Supply_paramToRAMUpdate();
    
    return WRES_OK;
}
