/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_LpSpi.c
 * History:
 *  - 29 gen 2020 1.0 Davoli - First release
 * S32K_can.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_LpSpi.h"
#include "S32K_Timer.h"
#include "string.h"
#include "S32K_SysPin.h"

/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#define  LPSPI_DATA_OUT_TRISTATE 1U
#define LPSPI_SDI_IN_SDO_OUT 0U

#define LP_SPI_MAX_TRANSFER_SIZE 16U
//static uint8_t tmpMsg[LP_SPI_MAX_TRANSFER_SIZE];

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
  //------------------ Local -----------------------------------
  lpspi_state_t LpSPIState; //Master/Slave
  uint32_t MastertDelayBetwenTransfers;
  uint32_t MastertDelaySCKtoPCS;
  uint32_t MastertDelayPCStoSCK;
  bool open;
  //------------------ Config ----------------------------------
  s_S32K_LpSpi_spimode_t SpiMode;
  uint32_t instanceNum;
  const lpspi_master_config_t * userMasterConfigPtr;
  const lpspi_slave_config_t * userSlaveConfigPtr;
  //------------------ Output ----------------------------------
  uint8_t *ptxBuffer;
  uint8_t *prxBuffer;
  uint8_t TransferSize;
} s_LpSpi;

typedef struct {
	s_LpSpi * LpSpiPtr;
	uint8_t * pTxBuffer;
	uint8_t * pRxBuffer;
	uint8_t TransferSize;
}s_LpSpi_Config;

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
static status_t LpSpiStatus;

#ifndef LP_SPI_NUM
#define LP_SPI_OBJ_NUM 0L
#else
#define LP_SPI_OBJ_NUM LP_SPI_NUM
#endif
#if LP_SPI_OBJ_NUM > 4u
  #error Gli Oggetti LpSpi possono essere al massimo 4
#endif

#if LP_SPI_TRANSFER_SIZE_0 > 0u
#define S32K_LP_SPI_TRANSFER_SIZE_0 LP_SPI_TRANSFER_SIZE_0
#else
#define S32K_LP_SPI_TRANSFER_SIZE_0 8UL
#endif

#ifdef LP_SPI_TRANSFER_SIZE_1
#if LP_SPI_TRANSFER_SIZE_1 > 0u
#define S32K_LP_SPI_TRANSFER_SIZE_1 LP_SPI_TRANSFER_SIZE_1
#else
#define S32K_LP_SPI_TRANSFER_SIZE_1 8UL
#endif
#endif

#ifdef LP_SPI_TRANSFER_SIZE_2
#if LP_SPI_TRANSFER_SIZE_2 > 0u
#define S32K_LP_SPI_TRANSFER_SIZE_2 LP_SPI_TRANSFER_SIZE_2
#else
#define S32K_LP_SPI_TRANSFER_SIZE_2 8UL
#endif
#endif

#ifdef LP_SPI_TRANSFER_SIZE_3
#if LP_SPI_TRANSFER_SIZE_3 > 0u
#define S32K_LP_SPI_TRANSFER_SIZE_3 LP_SPI_TRANSFER_SIZE_3
#else
#define S32K_LP_SPI_TRANSFER_SIZE_3 8UL
#endif
#endif

#if LP_SPI_OBJ_NUM > 0u
static s_LpSpi LpSpi_0;

typedef struct
{
  uint8_t data[S32K_LP_SPI_TRANSFER_SIZE_0];
} s_LpSpi_msg_0t;

static uint8_t txBuffer_0[S32K_LP_SPI_TRANSFER_SIZE_0];
static uint8_t rxBuffer_0[S32K_LP_SPI_TRANSFER_SIZE_0];
#endif
#if LP_SPI_OBJ_NUM > 1u
static s_LpSpi LpSpi_1;

typedef struct
{
  uint8_t data[S32K_LP_SPI_TRANSFER_SIZE_1];
} s_LpSpi_msg_1t;

static uint8_t txBuffer_1[S32K_LP_SPI_TRANSFER_SIZE_1];
static uint8_t rxBuffer_1[S32K_LP_SPI_TRANSFER_SIZE_1];
#endif

#if LP_SPI_OBJ_NUM > 2u
static s_LpSpi LpSpi_2;

typedef struct
{
  uint8_t data[S32K_LP_SPI_TRANSFER_SIZE_2];
} s_LpSpi_msg_2t;

static uint8_t txBuffer_2[S32K_LP_SPI_TRANSFER_SIZE_2];
static uint8_t rxBuffer_2[S32K_LP_SPI_TRANSFER_SIZE_2];
#endif
#if LP_SPI_OBJ_NUM > 3u
static s_LpSpi LpSpi_3;

typedef struct
{
  uint8_t data[S32K_LP_SPI_TRANSFER_SIZE_3];
} s_LpSpi_msg_3t;
static uint8_t txBuffer_3[S32K_LP_SPI_TRANSFER_SIZE_3];
static uint8_t rxBuffer_3[S32K_LP_SPI_TRANSFER_SIZE_3];

#endif


#if LP_SPI_OBJ_NUM == 0u
static s_LpSpi_Config LpSpi_Obj_Config[1U]={
  {NULL,NULL,NULL,0}
};
#endif
#if LP_SPI_OBJ_NUM == 1u
static s_LpSpi_Config LpSpi_Obj_Config[LP_SPI_OBJ_NUM]={
  {&LpSpi_0,&txBuffer_0[0],&rxBuffer_0[0], S32K_LP_SPI_TRANSFER_SIZE_0},
};
#endif
#if LP_SPI_OBJ_NUM == 2u
static s_LpSpi_Config LpSpi_Obj_Config[LP_SPI_OBJ_NUM]={
  {&LpSpi_0,&txBuffer_0[0],&rxBuffer_0[0], S32K_LP_SPI_TRANSFER_SIZE_0},
  {&LpSpi_1,&txBuffer_1[0],&rxBuffer_1[0], S32K_LP_SPI_TRANSFER_SIZE_1},
};
#endif
#if LP_SPI_OBJ_NUM == 3u
static s_LpSpi_Config LpSpi_Obj_Config[LP_SPI_OBJ_NUM]={
  {&LpSpi_0,&txBuffer_0[0],&rxBuffer_0[0], S32K_LP_SPI_TRANSFER_SIZE_0},
  {&LpSpi_1,&txBuffer_1[0],&rxBuffer_1[0], S32K_LP_SPI_TRANSFER_SIZE_1},
  {&LpSpi_2,&txBuffer_2[0],&rxBuffer_2[0], S32K_LP_SPI_TRANSFER_SIZE_2},
};
#endif
#if LP_SPI_OBJ_NUM == 4u
static s_LpSpi_Config LpSpi_Obj_Config[LP_SPI_OBJ_NUM]={
  {&LpSpi_0,&txBuffer_0[0],&rxBuffer_0[0],S32K_LP_SPI_TRANSFER_SIZE_0},
  {&LpSpi_1,&txBuffer_1[0],&rxBuffer_1[0],S32K_LP_SPI_TRANSFER_SIZE_1},
  {&LpSpi_2,&txBuffer_2[0],&rxBuffer_2[0],S32K_LP_SPI_TRANSFER_SIZE_2},
  {&LpSpi_3,&txBuffer_3[0],&rxBuffer_3[0],S32K_LP_SPI_TRANSFER_SIZE_3},
};
#endif


/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/
static status_t MY_LPSPI_SetPinConfigMode(LPSPI_Type * base,
                                          uint32_t pinCfg,
                                          uint32_t dataOutConfig,
                                          bool pcs3and2Enable)
{
    uint32_t cfgr1Value = 0u;

	cfgr1Value = base->CFGR1 &
				 ~(LPSPI_CFGR1_PINCFG_MASK|LPSPI_CFGR1_OUTCFG_MASK|LPSPI_CFGR1_PCSCFG_MASK);

	cfgr1Value |= ((uint32_t)(pinCfg) << LPSPI_CFGR1_PINCFG_SHIFT) |
                      ((uint32_t)(dataOutConfig) << LPSPI_CFGR1_OUTCFG_SHIFT) |
                      ((uint32_t)(pcs3and2Enable? 0u : 1u) << LPSPI_CFGR1_PCSCFG_SHIFT);  /* enable = 0 */

	base->CFGR1 = cfgr1Value;

	return STATUS_SUCCESS;
}
static status_t MY_LPSPI_Disable(LPSPI_Type * base)
{
    status_t res;
    uint32_t lpspi_tmp = base->SR;
    lpspi_tmp = (lpspi_tmp & LPSPI_SR_MBF_MASK) >> LPSPI_SR_MBF_SHIFT;

    if (lpspi_tmp == (uint32_t)1u)
    {
        res = STATUS_BUSY;
    }
    else
    {
        base->CR = base->CR & (~(LPSPI_CR_MEN_MASK));
        res = STATUS_SUCCESS;
    }
    
    return res;
}
static inline void MY_LPSPI_Enable(LPSPI_Type * base)
{
    (base->CR) |= (uint32_t)1U << LPSPI_CR_MEN_SHIFT;
}

static wres_t Set_SlaveOutasTreeState(uint32_t instance)
{
#if LPSPI_INSTANCE_COUNT == 3u
static uint32_t SPI_BASE_ARR[LPSPI_INSTANCE_COUNT] ={LPSPI0_BASE,LPSPI1_BASE,LPSPI2_BASE};
#endif
#if LPSPI_INSTANCE_COUNT == 2u
static uint32_t SPI_BASE_ARR[LPSPI_INSTANCE_COUNT] ={LPSPI0_BASE,LPSPI1_BASE};
#endif
#if LPSPI_INSTANCE_COUNT == 1u
static uint32_t SPI_BASE_ARR[LPSPI_INSTANCE_COUNT] ={LPSPI0_BASE};
#endif

	wres_t res;
	LPSPI_Type * base = (LPSPI_Type *)SPI_BASE_ARR[instance];
	status_t errorCode = MY_LPSPI_Disable(base);
	if (errorCode != STATUS_SUCCESS)
	{
		res = WRES_ERROR;
	}
	else
	{
		MY_LPSPI_SetPinConfigMode(base, LPSPI_SDI_IN_SDO_OUT, LPSPI_DATA_OUT_TRISTATE, true);
		MY_LPSPI_Enable(base);
		res = WRES_OK;
	}
	return res;
}
extern wres_t S32K_LpSpi_MasterSetDelay(s_LpSpi_SpiId ObjId,
										uint32_t delayBetwenTransfers,
										uint32_t delaySCKtoPCS,
										uint32_t delayPCStoSCK
		)

{
  wres_t res;
  if (LP_SPI_OBJ_NUM > (uint8_t)ObjId)
  {
		s_LpSpi *Config;
		Config = LpSpi_Obj_Config[ObjId].LpSpiPtr;
		if (true == Config->open)
		{
			Config->MastertDelayBetwenTransfers =delayBetwenTransfers;
			Config->MastertDelaySCKtoPCS = delaySCKtoPCS;
			Config->MastertDelayPCStoSCK = delayPCStoSCK;
			res = WRES_OK;
		}
		else
		{
			res = WRES_ERROR_INVALID_MODE;
		}
  }
  else
  {
	res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;
}
extern wres_t S32K_LpSpi_Init( 	s_LpSpi_SpiId ObjId,
								s_S32K_LpSpi_spimode_t SpiMode,
								uint32_t instanceNum,
								const lpspi_master_config_t *userMasterConfigPtr,
								const lpspi_slave_config_t *userSlaveConfigPtr
		)

{
  wres_t res = WRES_ERROR;
  if (LP_SPI_OBJ_NUM > (uint8_t)ObjId)
  {

	s_LpSpi *Config;
	Config = LpSpi_Obj_Config[ObjId].LpSpiPtr;

	Config->MastertDelayBetwenTransfers =0;
	Config->MastertDelaySCKtoPCS=0;
	Config->MastertDelayPCStoSCK=0;
	Config->SpiMode =SpiMode;
	Config->instanceNum = instanceNum;
	Config->userMasterConfigPtr = userMasterConfigPtr;
	Config->userSlaveConfigPtr = userSlaveConfigPtr;
	Config->ptxBuffer = LpSpi_Obj_Config[ObjId].pTxBuffer;
	Config->prxBuffer = LpSpi_Obj_Config[ObjId].pRxBuffer;
	Config->TransferSize  = LpSpi_Obj_Config[ObjId].TransferSize;
	Config->open =false;
        if (S32K_LP_SPI_MASTER == Config->SpiMode ){
                if (STATUS_SUCCESS == LPSPI_DRV_MasterInit(Config->instanceNum, &Config->LpSPIState, Config->userMasterConfigPtr))
                {
                        res = WRES_OK;
                        Config->open =true;
                }
                else
                {
                        res = WRES_ERROR_INVALID_MODE;
                        Config->open =false;
                }
        }
        else if (S32K_LP_SPI_SLAVE ==Config->SpiMode )
        {
                if (STATUS_SUCCESS == LPSPI_DRV_SlaveInit(Config->instanceNum, &Config->LpSPIState,Config->userSlaveConfigPtr ))
                {
                        if (WRES_OK == Set_SlaveOutasTreeState(Config->instanceNum))
                        {
                                if (STATUS_SUCCESS == LPSPI_DRV_SlaveTransfer(Config->instanceNum, Config->ptxBuffer, Config->prxBuffer, Config->TransferSize))
                                {
                                        res = WRES_OK;
                                        Config->open =true;
                                }
                                else
                                {
                                        res = WRES_ERROR_INVALID_MODE;
                                }
                        }
                        else
                        {
                                res = WRES_ERROR_INVALID_PARAMETER;
                        }
                }
                else
                {
                        res = WRES_ERROR_UNINITIALIZED;
                        Config->open =false;
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
/*------------ prova nuovo basso livello con Tx-Rx punto-punto ---------------*/
wres_t S32K_LpSpi_TxRxMaster(s_LpSpi_SpiId ObjId, uint8_t *txBuffer, uint8_t *rxBuffer, s_S32K_SysPin_GPioID csID )
{
    wres_t res;
     s_LpSpi *Config;
    
    if (LP_SPI_OBJ_NUM > (uint8_t)ObjId)
    {
        Config = LpSpi_Obj_Config[ObjId].LpSpiPtr;
        if ((true == Config->open)&& (S32K_LP_SPI_MASTER == Config->SpiMode))
        {
            LPSPI_DRV_MasterConfigureBus(Config->instanceNum,Config->userMasterConfigPtr,NULL);
            if ((0x00u != Config->MastertDelayBetwenTransfers) ||
                (0x00u != Config->MastertDelaySCKtoPCS)        ||
                (0x00u != Config->MastertDelayPCStoSCK)         )
            {
	        LPSPI_DRV_MasterSetDelay(Config->instanceNum, Config->MastertDelayBetwenTransfers,Config->MastertDelaySCKtoPCS,Config->MastertDelayPCStoSCK);
            }
            if (GPIO_NUM > csID)
	    {
              S32K_SysPin_ClearOutputPins( csID );
            }
            
            LpSpiStatus = LPSPI_DRV_MasterTransferBlocking(Config->instanceNum, txBuffer, rxBuffer,Config->TransferSize,20);
            if (STATUS_SUCCESS != LpSpiStatus)
            {
	        LPSPI_DRV_MasterAbortTransfer(Config->instanceNum);
	        res = WRES_ERROR_NOT_READY;
            }
            else
            {
                res = WRES_OK;
            }
            if (GPIO_NUM > csID)
	    {
              S32K_SysPin_SetOutputPins( csID );
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

wres_t S32K_LpSpi_TxRxSlave(s_LpSpi_SpiId ObjId)
{
    wres_t res;
    s_LpSpi *Config;
    
    if (LP_SPI_OBJ_NUM > (uint8_t)ObjId)
    {
        Config = LpSpi_Obj_Config[ObjId].LpSpiPtr;
        if ((true == Config->open)||(S32K_LP_SPI_SLAVE != Config->SpiMode))
        {
#ifdef TEST_DEBUG_CODE_SPY
            CodeSpy_DEBUG_Set(GPIO_OUT_TPTASK);
#endif          
           memset(Config->ptxBuffer, 0x00, Config->TransferSize); 
           LpSpiStatus = LPSPI_DRV_SlaveTransfer(Config->instanceNum, Config->ptxBuffer, Config->prxBuffer, Config->TransferSize);
                                                  
            if (STATUS_SUCCESS != LpSpiStatus)
            {
	        LPSPI_DRV_SlaveAbortTransfer(Config->instanceNum);
	        res = WRES_ERROR_NOT_READY;
            }
            else
            {
                res = WRES_OK;
#ifdef TEST_DEBUG_CODE_SPY
                CodeSpy_DEBUG_Clear(GPIO_OUT_TPTASK);
#endif                
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


extern wres_t S32K_LpSpi_GetRxMsgSlave(s_LpSpi_SpiId ObjId, void *msg )
{
    wres_t res;
    s_LpSpi *Config;
    uint32_t ByteRemaining;

    if (LP_SPI_OBJ_NUM > (uint8_t)ObjId)
    {
      Config = LpSpi_Obj_Config[ObjId].LpSpiPtr;
      if ((true != Config->open)||(NULL == msg)||(S32K_LP_SPI_SLAVE != Config->SpiMode))
      {
        memset(Config->ptxBuffer, 0x00, Config->TransferSize);   
        res = WRES_ERROR_INVALID_PARAMETER;
      }
      else
      {
        if ( STATUS_SUCCESS == LPSPI_DRV_SlaveGetTransferStatus(Config->instanceNum,&ByteRemaining))
        {
          (void)memcpy(msg,Config->prxBuffer,Config->TransferSize);
          res = WRES_OK; 
        }
        else
        {
          memset(Config->ptxBuffer, 0x00, Config->TransferSize);   
          res = WRES_ERROR_NOT_READY;
        }
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