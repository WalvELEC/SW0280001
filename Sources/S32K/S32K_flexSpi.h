/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_flexSpi.h
 * Brief description
 */
#ifndef S32K_FLEXSPI_H_
#define S32K_FLEXSPI_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include "queue.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/

/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/
typedef enum
{
    S32K_FLEX_SPI_MASTER        = 0x00U,
    S32K_FLEX_SPI_SLAVE         = 0x01U,
} S32K_flexSpi_spimode_t;

typedef enum
{
    S32K_FLEXSPI_ID_0        = 0x00U,
    S32K_FLEXSPI_ID_1        = 0x01U,
} s_FlexSpi_SpiId;

/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
    uint8_t data[FLEX_SPI_TRANSFER_SIZE];
} s_FlexSpi_msg_t;

/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/

	/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/

extern wres_t S32K_FlexSpi_Init( 	s_FlexSpi_SpiId ObjId,
								S32K_flexSpi_spimode_t SpiMode,
								uint32_t instanceNum,
								const flexio_spi_master_user_config_t *userMasterConfigPtr,
								const flexio_spi_slave_user_config_t *userSlaveConfigPtr
		);
extern wres_t S32K_FlexSpi_Task( s_FlexSpi_SpiId ObjId );
extern wres_t S32K_FlexSpi_SendMsg(s_FlexSpi_SpiId ObjId, const s_FlexSpi_msg_t *msg);
extern wres_t S32K_FlexSpi_RecvMsg(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *msg);
extern wres_t S32K_FlexSpi_ReadLastMsg(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *msg);
extern wres_t S32K_FlexSpi_TxRxMaster(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *txBuffer, s_FlexSpi_msg_t *rxBuffer );
extern wres_t S32K_FlexSpi_TxRxSlave(s_FlexSpi_SpiId ObjId, s_FlexSpi_msg_t *txBuffer, s_FlexSpi_msg_t *rxBuffer );


/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/
//S32K_Flexio_Init(FLEXIO_SPI_MASTER, S32K_FLEXIO_MASTER, INST_FLEXIO_TE, &flexio_TE_MasterConfig0, NULL);
//S32K_Flexio_Init(FLEXIO_SPI_SLAVE, S32K_FLEXIO_SLAVE , INST_FLEXIO_M4, NULL , &flexio_M4_SlaveConfig0);
//S32K_Flexio_SendMsg(FLEXIO_SPI_MASTER,&masterBuffer.txBuffer);
//S32K_Flexio_SendMsg(FLEXIO_SPI_SLAVE,&slaveBuffer.txBuffer);
//S32K_Flexio_Task(FLEXIO_SPI_SLAVE);
//S32K_Flexio_Task( FLEXIO_SPI_MASTER);
//S32K_Flexio_RecvMsg( FLEXIO_SPI_SLAVE,&RxSlaveMsg);
//S32K_Flexio_RecvMsg( FLEXIO_SPI_MASTER,&RxMasterMsg);

#endif /* S32K_FLEXSPI_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
