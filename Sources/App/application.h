/*
 * application.h
 *
 *  Created on: 15 may 2018
 *      Author: PoliD
 */
#ifndef _APPLICATION_H
#define _APPLICATION_H

//#include "string.h"
#include "wtypes.h"
#include "S32K_ObjectConfig.h"
#include "AppTranslate.h"
//#include "bsp.h"
#include "digital.h"
#include "supply.h"
#include "djwConfig.h"
#include "memdata.h"
#include "flash.h"
#include "error.h"
#include "mlx.h"
//#include "mlx_Spi.h"
#include "roller.h"
#include "saej1939.h"
#include "crc.h"

#ifdef ISOBUS_DJW
#include "isobus.h"
#endif
#define TASK_APP_INDEX  (s_S32K_TimerTaskId)TASK_4_ID
//#define TASK_APP_PERIOD 4000U
//#define TASK_APP_DELAY  4000U
#define TASK_APP_PERIOD 4000U
#define TASK_APP_DELAY  1000U

#define SCHEDULE_COUNTER_TH_IO           ( TASK_APP_PERIOD )
#define SCHEDULE_COUNTER_TH_SAEJ         ( TASK_APP_PERIOD )

//#define DP_DEBUG_EN //per sospendere il WDT quando voglio fare il debug
//#define ISOBUS_DJW  //da commentare se non è joystick ISOBUS

#ifdef ISOBUS_DJW
#endif
   
#define CAN_RES_TERM_NOT_ENABLED       ( 0x50U )
#define CAN_RES_TERM_ENABLED           ( 0xA0U )   

//#include <xmc_flash.h>
//#include <stdlib.h>
//#include <tle.h>

//#define PRG_APP_ID           ( 190070U ) //SW[0190070]
//#define PRG_VERSION          ( 0x0001U )   
//#define PRG_CHR_APP_ID_D1    ( 0x37U )   //Asc(7)  SW01900[7]0
//#define PRG_CHR_APP_ID_D2    ( 0x30U )   //Asc(0)  SW019007[0]
//#define PRG_CHR_APP_ID_D3    ( 0x2EU )   //Asc(.)
//#define PRG_CHR_APP_ID_D4    ( 0x31U )   //Asc(1)  PRG_VERSION = [1]
//   
//#define CAN0_BAUD_RATE_BPS  250000UL
#define SPI0_BAUD_RATE_HZ   500000UL
#define SPI1_BAUD_RATE_HZ   500000UL
#define SPI2_BAUD_RATE_HZ   500000UL
//#define TICKS_PER_SECOND    5000UL  //1 tick ogni 200us
//#define TICK_PERIOD_US      200UL
#define TICK_PERIOD_US      1000UL
//#define COUNTER_OFF                ( 0U )

#define CAN_MSG_MLX_0  ( 11U )
#define CAN_MSG_MLX_1  ( 12U )

#define CAN_MSG_TX_MLX_0_SPI_TX_TIME_OUT_US   ( 20000U )
#define CAN_MSG_TX_MLX_0_SPI_RX_TIME_OUT_US   ( 20000U )

#define CAN_MSG_TX_ALPHA_X_TX_TIME_OUT_US   ( 20000U )
#define CAN_MSG_TX_ALPHA_Y_TX_TIME_OUT_US   ( 20000U )

#define OUT_TE_OFF                  ( 0x2000U )
#define OUT_TE_ON                   ( 0x4000U )

//#define CAN_MSG_TLE_SPI_RX          ( 0U )
//#define CAN_MSG_MLX_SPI_X_CMD_RX    ( 1U )
//#define CAN_MSG_MLX_SPI_Y_CMD_RX    ( 2U )
//#define CAN_MSG_FLASH_ACCESS        ( 3U )

extern uint8_t Application_sourceAddressGet( void );
extern uint32_t Application_ParamGet( void );
extern wres_t Application_Init( void );
extern void Application_Main( void );

#endif
