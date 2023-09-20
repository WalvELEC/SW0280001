/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_flexio.c
 * History:
 *  - 29 gen 2020 1.0 Davoli - First release
 * S32K_can.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_flexSpi.h"
#include "S32K_Timer.h"
#include "string.h"

/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/
static status_t FlexSpiStatus;

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#if FLEX_SPI_TRANSFER_SIZE > 0u
#define S32K_FLEX_SPI_TRANSFER_SIZE FLEX_SPI_TRANSFER_SIZE
#else
#define S32K_FLEX_SPI_TRANSFER_SIZE 8UL
#endif

#ifndef FLEX_SPI_NUM
#define S32K_FLEX_SPI_OBJ_NUM 0L
#else
#define S32K_FLEX_SPI_OBJ_NUM FLEX_SPI_NUM
#endif

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
  //------------------ Local -----------------------------------
  flexio_device_state_t flexIODeviceState;
  flexio_spi_master_state_t flexIOSPIState; //Master/Slave
  bool open;
  //------------------ Config ----------------------------------
  S32K_flexSpi_spimode_t SpiMode;
  uint32_t instanceNum;
  const flexio_spi_master_user_config_t * userMasterConfigPtr;
  const flexio_spi_slave_user_config_t * userSlaveConfigPtr;
} s_FlexSpi;

typedef struct {
	s_FlexSpi * FlexSpiPtr;
}s_FlexSpi_Config;

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/
/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
#if S32K_FLEX_SPI_OBJ_NUM == 1u
static s_FlexSpi FlexSpi_0;
static s_FlexSpi_Config FlexSpi_Obj_Config[S32K_FLEX_SPI_OBJ_NUM]={
		{&FlexSpi_0},
};
#endif
#if S32K_FLEX_SPI_OBJ_NUM == 2u
static s_FlexSpi FlexSpi_0;
static s_FlexSpi FlexSpi_1;
static s_FlexSpi_Config FlexSpi_Obj_Config[S32K_FLEX_SPI_OBJ_NUM]={
		{&FlexSpi_0},
		{&FlexSpi_1},
};
#endif
/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/
extern wres_t S32K_FlexSpi_Init( s_FlexSpi_SpiId ObjId,
		S32K_flexSpi_spimode_t SpiMode,
		uint32_t instanceNum,
		const flexio_spi_master_user_config_t *userMasterConfigPtr,
		const flexio_spi_slave_user_config_t *userSlaveConfigPtr
		)
{
  wres_t res;

  if (S32K_FLEX_SPI_OBJ_NUM > (uint8_t)ObjId)
  {

	s_FlexSpi *Config;
	Config = FlexSpi_Obj_Config[ObjId].FlexSpiPtr;


	Config->SpiMode =SpiMode;
	Config->instanceNum = instanceNum;
	Config->userMasterConfigPtr = userMasterConfigPtr;
	Config->userSlaveConfigPtr = userSlaveConfigPtr;
        if (S32K_FLEX_SPI_MASTER == Config->SpiMode ){
                if( STATUS_SUCCESS  == FLEXIO_DRV_InitDevice(Config->instanceNum, &Config->flexIODeviceState))
                {
                        if( STATUS_SUCCESS  == FLEXIO_SPI_DRV_MasterInit(Config->instanceNum, Config->userMasterConfigPtr, &Config->flexIOSPIState))
                        {
                                Config->open =true;
                                res = WRES_OK;
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
        }else if (S32K_FLEX_SPI_SLAVE ==Config->SpiMode ){
                if( STATUS_SUCCESS  == FLEXIO_DRV_InitDevice(Config->instanceNum, &Config->flexIODeviceState))
                {
                        if( STATUS_SUCCESS  == FLEXIO_SPI_DRV_SlaveInit(Config->instanceNum, Config->userSlaveConfigPtr, &Config->flexIOSPIState))
                        {
                                Config->open =true;
                                res = WRES_OK;
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
        }
        else
        {
                res = WRES_ERROR_INVALID_PARAMETER;
        }
  }
  else
  {
	res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;
}

wres_t S32K_FlexSpi_TxRxMaster(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *txBuffer, s_FlexSpi_msg_t *rxBuffer )
{
    wres_t res;
    s_FlexSpi *Config;
    
    if (S32K_FLEX_SPI_OBJ_NUM > (uint8_t)ObjId)
    {
        Config = FlexSpi_Obj_Config[ObjId].FlexSpiPtr;
        if ((true == Config->open) && (S32K_FLEX_SPI_MASTER == Config->SpiMode ))
        {
            FlexSpiStatus = FLEXIO_SPI_DRV_MasterTransferBlocking(&Config->flexIOSPIState, (uint8_t *)txBuffer, (uint8_t *)rxBuffer, FLEX_SPI_TRANSFER_SIZE,2);
            if (STATUS_SUCCESS != FlexSpiStatus)
            {
	        FLEXIO_SPI_DRV_MasterTransferAbort(&Config->flexIOSPIState);
	        res = WRES_ERROR_NOT_READY;
            }
            else
            {
                res = WRES_OK;
            }
        }
        else
        {
            res = WRES_ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    return res;
}

wres_t S32K_FlexSpi_TxRxSlave(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *txBuffer, s_FlexSpi_msg_t *rxBuffer )
{
    wres_t res;
    s_FlexSpi *Config;
    
    if (S32K_FLEX_SPI_OBJ_NUM > (uint8_t)ObjId)
    {
        Config = FlexSpi_Obj_Config[ObjId].FlexSpiPtr;
        if ((true == Config->open)&& (S32K_FLEX_SPI_SLAVE == Config->SpiMode ))
        {
            FLEXIO_SPI_DRV_MasterTransferAbort(&Config->flexIOSPIState);
            FlexSpiStatus = FLEXIO_SPI_DRV_SlaveTransferBlocking(&Config->flexIOSPIState, (uint8_t *)txBuffer, (uint8_t *)rxBuffer, FLEX_SPI_TRANSFER_SIZE,3);
            if (STATUS_SUCCESS != FlexSpiStatus)
            {
	        FLEXIO_SPI_DRV_MasterTransferAbort(&Config->flexIOSPIState);
	        res = WRES_ERROR_NOT_READY;
            }
            else
            {
                res = WRES_OK;
            }
        }
        else
        {
            res = WRES_ERROR_INVALID_PARAMETER;
        }
    }
    else
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    return res;
}
/**************************************************************************************************
 * EOF                                                                                            *
 **************************************************************************************************/
