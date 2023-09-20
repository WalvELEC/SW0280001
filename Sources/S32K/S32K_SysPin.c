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
#include "S32K_SysPin.h"
#include "S32K_Timer.h"
#define __weak __attribute__ ((weak))
/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#ifndef CLOCK_MANAGER_CONFIG_CNT
    #error Module S32_SysPin need Object typed clock_manager and named clockMan1
#endif
#ifndef NUM_OF_CONFIGURED_PINS
    #error Module S32_SysPin need Object typed pinmux
#endif

// This Driver manage only interrupt on port PORT_IRQn
#ifdef MCU_S32K144
#define PORT_IRQn	PORTA_IRQn
#endif


/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/


/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/
static uint32_t GetGpioBase(const GPIO_Type * const GpioPort);
extern wres_t PINS_DRV_CheckConfig_GPIO(uint32_t pinCount,
                       const pin_settings_config_t config[],uint8_t *ErrorIndex);
uint32_t GetIntPin( uint32_t In);
//__weak void SysPinIsrManage( s_GPIO_Port Port, uint32_t Pin );
void SysPinIsrManage( s_GPIO_Port Port, uint32_t Pin );
extern void SysPinIsr( void );

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
#define SYNC_TIMEOUT_2MS 1500u
static uint32_t EnIntPort[PORT_INSTANCE_COUNT] ={0u,0u,0u,0u,0u};
static uint32_t MY_GPIO_BASE_ADDRS[PORT_INSTANCE_COUNT]={ PORTA_BASE, PORTB_BASE, PORTC_BASE, PORTD_BASE, PORTE_BASE };
static uint32_t AuxPortValue;
static uint8_t Value;
static uint8_t ValueIn;
static uint8_t PinValue;
static uint32_t ActBase;

/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/
uint8_t edgeCounter;
uint8_t tlfIntFlag;
uint8_t tlfStatusCheckWIP;
uint8_t tlfGoToRecovery;
uint8_t tlfMONSFValToCAN;

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/

static uint32_t GetGpioBase(const GPIO_Type * const GpioPort)
{
   GPIO_Type * const MY_GPIO_BASE_PTRS[PORT_INSTANCE_COUNT]= { PTA, PTB, PTC, PTD, PTE };
   uint8_t i;
   uint32_t res = 0u;
   
   for (i = 0U; i < 5u; i++)
   {
	   if ( MY_GPIO_BASE_PTRS[i] == GpioPort)
	   {
		   res =  MY_GPIO_BASE_ADDRS[i];
	   }
   }
   
   return res;
}
/*
#define GPIO_BASE_ADDRS                          { PTA_BASE, PTB_BASE, PTC_BASE, PTD_BASE, PTE_BASE }
/// Array initializer of GPIO peripheral base pointers
#define GPIO_BASE_PTRS                           { PTA, PTB, PTC, PTD, PTE }
*/
extern wres_t PINS_DRV_CheckConfig_GPIO(uint32_t pinCount,
                       const pin_settings_config_t config[],uint8_t *ErrorIndex)
{
    uint8_t i;
    uint8_t j;

	wres_t res =WRES_OK;
 	uint8_t LastPinIndex = 0xFFu;
 	bool PinFound ;

   for (i = 0U; i < (uint8_t)GPIO_NUM; i++)
   {
	    PinFound =false;
	    ActBase = GetGpioBase(GpioLIst[i].GpioPort);
        for (j = 0U; j < pinCount; j++)
        {
        	if (( ActBase == (uint32_t )config[j].base) && ( GpioLIst[i].PinNumber == config[j].pinPortIdx))
        	{   // Pin Found
        		PinFound=true;
        		if (( PORT_MUX_AS_GPIO != config[j].mux)||( GpioLIst[i].direction != config[j].direction))
        		{   // Pin wrong configuration
        			res = WRES_ERROR_INVALID_MODE;
        			LastPinIndex =i;
                                *ErrorIndex = LastPinIndex;
                                break;
        		}
        	}

        }
        if (false == PinFound)
        {
			res = WRES_ERROR_UNINITIALIZED;
			LastPinIndex =i;
        }
    }
    *ErrorIndex = LastPinIndex;
    return res;
}
//------------------------------------------------------------------------------
extern wres_t S32K_SysPin_Init( void )
{
    wres_t res;
    uint8_t ErrorIndex;
    res = PINS_DRV_CheckConfig_GPIO((uint32_t)NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr,&ErrorIndex);
    (void)ErrorIndex;
    DEV_ASSERT(WRES_OK == res);
	if (STATUS_SUCCESS == CLOCK_SYS_Init(CM_ConfigsArr, CM_CONFIG_NUM,
			CM_CallbacksArr, CM_CALLBACK_CNT))
	{

		if (STATUS_SUCCESS == CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT))
		{
                    if (STATUS_SUCCESS == PINS_DRV_Init((uint32_t)PM_NUM_OF_CONF_PINS, PM_InitConfigArr))
			{
				res = WRES_OK;
			}
			else
			{
				res = WRES_ERROR_INVALID_MODE;
			}
		}
		else
		{
			res = WRES_ERROR_UNINITIALIZED;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}

uint32_t GetIntPin( uint32_t In)
{
	uint8_t i;
	uint32_t res = 0xFFu;
	uint32_t Aux;
	for (i = 0U; i < 16u; i++)
	{
	  Aux = In & (1u<<i);
	  if ( Aux >0u )
	  {
		res = i;
		break;
	  }
	}
	return res;
}
//------------------------------------------------------------------------------
//__weak void SysPinIsrManage( s_GPIO_Port Port, uint32_t Pin )
//{
//	(void)(Port);
//	(void)(Pin);
//}

void SysPinIsrManage( s_GPIO_Port Port, uint32_t Pin )
{
	(void)(Port);
	(void)(Pin);
        edgeCounter++;
        if ( edgeCounter < 12U )
        {
            if ( 0U == tlfStatusCheckWIP )
            {    
                if ( 1U < edgeCounter ) 
                {
                    tlfIntFlag = 0x01;
                    tlfStatusCheckWIP = 1U;
                }
            }
        }
}

extern void SysPinIsr( void )
{
//	s_S32K_SysPin_GPioID PinId =2;
	uint32_t AuxPort;
	uint32_t AuxPin;
	uint8_t i;
	for (i = 0U; i < PORT_INSTANCE_COUNT; i++)
	{
		AuxPort = PINS_DRV_GetPortIntFlag((PORT_Type *)MY_GPIO_BASE_ADDRS[i]) & EnIntPort[i] ;
		if (AuxPort!= 0u){
		  AuxPin = GetIntPin(AuxPort);
		  if (AuxPin<16u){
			  SysPinIsrManage((s_GPIO_Port)i,AuxPin);
			  PINS_DRV_ClearPinIntFlagCmd(  ((PORT_Type *)MY_GPIO_BASE_ADDRS[i]), AuxPin);
			  break;
		  }
		  else
		  {
			  DEV_ASSERT(false);
		  }

		}
	}
}

//------------------------------------------------------------------------------
extern wres_t S32K_SysPin_InstallInterrupt( s_S32K_SysPin_GPioID PinId,port_interrupt_config_t Event )
{
	// Install buttons ISR
	wres_t res;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_INPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			ActBase = GetGpioBase(GpioLIst[PinId].GpioPort);
			PINS_DRV_SetPinIntSel((PORT_Type *)ActBase, GpioLIst[PinId].PinNumber, Event);
			INT_SYS_InstallHandler(PORT_IRQn, &SysPinIsr, NULL);
			if (PTA == GpioLIst[PinId].GpioPort){EnIntPort[0] = EnIntPort[0] | (1u<<GpioLIst[PinId].PinNumber);}
			if (PTB == GpioLIst[PinId].GpioPort){EnIntPort[1] = EnIntPort[1] | (1u<<GpioLIst[PinId].PinNumber);}
			if (PTC == GpioLIst[PinId].GpioPort){EnIntPort[2] = EnIntPort[2] | (1u<<GpioLIst[PinId].PinNumber);}
			if (PTD == GpioLIst[PinId].GpioPort){EnIntPort[3] = EnIntPort[3] | (1u<<GpioLIst[PinId].PinNumber);}
			if (PTE == GpioLIst[PinId].GpioPort){EnIntPort[4] = EnIntPort[4] | (1u<<GpioLIst[PinId].PinNumber);}
			res = WRES_OK;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}
//------------------------------------------------------------------------------
extern wres_t S32K_SysPin_SetOutputPins(s_S32K_SysPin_GPioID PinId)
{
	wres_t res;
        uint32_t PortValue;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_OUTPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			PINS_DRV_SetPins((GPIO_Type *)GpioLIst[PinId].GpioPort, (pins_channel_type_t)1u<<GpioLIst[PinId].PinNumber);
                        PinValue = 0x0u;
			PortValue = PINS_DRV_ReadPins((GPIO_Type *)GpioLIst[PinId].GpioPort);
			PinValue = (uint8_t)GetBit(PortValue, GpioLIst[PinId].PinNumber);
                        if (0x01u == PinValue)
                        {
			  res = WRES_OK;
                        }
                        else
                        {
                          res = WRES_ERROR;
                        }
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}
extern wres_t S32K_SysPin_ClearOutputPins(s_S32K_SysPin_GPioID PinId)
{
	wres_t res;
        uint32_t PortValue;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_OUTPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			PINS_DRV_ClearPins((GPIO_Type *)GpioLIst[PinId].GpioPort,(uint32_t)1u<<GpioLIst[PinId].PinNumber);
                        PinValue = 0x01u;
			PortValue = PINS_DRV_ReadPins((GPIO_Type *)GpioLIst[PinId].GpioPort);
			PinValue = (uint8_t)GetBit(PortValue, GpioLIst[PinId].PinNumber);
                        if (0x00u == PinValue)
                        {
			  res = WRES_OK;
                        }
                        else
                        {
                          res = WRES_ERROR;
                        }
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}

extern wres_t S32K_SysPin_ToggleOutputPins(s_S32K_SysPin_GPioID PinId)
{
	wres_t res;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_OUTPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			PINS_DRV_TogglePins((GPIO_Type *)GpioLIst[PinId].GpioPort,(uint32_t)1u<<GpioLIst[PinId].PinNumber);
			res = WRES_OK;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}
extern void S32K_SysPin_EnableGpioIRQ(void )
{
	INT_SYS_EnableIRQ(PORT_IRQn);
}
extern void S32K_SysPin_DisableGpioIRQ(void )
{
	INT_SYS_DisableIRQ(PORT_IRQn);
}
extern wres_t S32K_SysPin_GetInputPins(s_S32K_SysPin_GPioID PinId, uint8_t *Value)
{
	wres_t res;
	uint32_t PortValue;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_INPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			PortValue = PINS_DRV_ReadPins((GPIO_Type *)GpioLIst[PinId].GpioPort);
			*Value = (uint8_t)GetBit(PortValue, GpioLIst[PinId].PinNumber);
			res = WRES_OK;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;

}
extern wres_t S32K_SysPin_GetOutputPins(s_S32K_SysPin_GPioID PinId, uint8_t *Value)
{
	wres_t res;
	uint32_t PortValue;
	if (GPIO_NUM > PinId)
	{
		if (GPIO_OUTPUT_DIRECTION != GpioLIst[PinId].direction)
		{
			res = WRES_ERROR_INVALID_MODE;
		}
		else
		{
			PortValue = PINS_DRV_ReadPins((GPIO_Type *)GpioLIst[PinId].GpioPort);
			*Value = (uint8_t)GetBit(PortValue, GpioLIst[PinId].PinNumber);
			res = WRES_OK;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
    return res;
}
//------------------------------------------------------------------------------
extern void S32K_SysPin_WaitForSync( bool SkipNext )
{
//  uint32_t SampleInCnt;
  uint32_t AuxPortValue;
  uint8_t Value;
  uint8_t ValueIn;
  
  AuxPortValue = PINS_DRV_ReadPins((GPIO_Type *)PTD);
  ValueIn = (uint8_t)GetBit(AuxPortValue, 5u);
  Value = ValueIn;
  S32K_Timer_Stop();
//  SampleInCnt =0;
#ifdef TEST_DEBUG_CODE_SPY
  CodeSpy_DEBUG_Set(GPIO_OUT_TPTASK);
#endif  
  while (Value == ValueIn)
  {
    AuxPortValue = PINS_DRV_ReadPins((GPIO_Type *)PTD);
    Value = (uint8_t)GetBit(AuxPortValue, 5u);
  }
  if (SkipNext){ Application_TickUpdate();}
 
  S32K_Timer_Start();
#ifdef TEST_DEBUG_CODE_SPY
  CodeSpy_DEBUG_Clear(GPIO_OUT_TPTASK);
#endif  
}

//------------------------------------------------------------------------------
extern void CodeSpy_DEBUG_Set(s_S32K_SysPin_GPioID PinId)
{
  S32K_SysPin_SetOutputPins(PinId);

  return;  
}
//------------------------------------------------------------------------------
extern void CodeSpy_DEBUG_Clear(s_S32K_SysPin_GPioID PinId)
{
  S32K_SysPin_ClearOutputPins(PinId);

  return;
}

/**************************************************************************************************
 * EOF                                                                                            *
 **************************************************************************************************/
