/*
 * mlx.h
 *
 *  Created on: 08 may 2018
 *      Author: PoliD
 */

#ifndef _MLX_H
#define _MLX_H

//---- nota mappatura sensori
////--- macro per calibrare i sensori
////#define MLX_SIGNAL_X_ALPHA_MIN_A    ( 7150U ) 
////#define MLX_SIGNAL_X_ALPHA_MAX_A    ( 2327U )
////#define MLX_SIGNAL_X_ALPHA_MIN_B    ( 8604U )
////#define MLX_SIGNAL_X_ALPHA_MAX_B    ( 11035U )
////#define MLX_SIGNAL_X_ALPHA_ERR_TH_A ( 1000U )
////#define MLX_SIGNAL_X_ALPHA_ERR_TH_B ( 12000U ) 
////#define MLX_SIGNAL_X_ALPHA_SAFE_DB  ( 150U )
//
//#define MLX_SIGNAL_X_ALPHA_MIN_A    ( 7400U ) 
//#define MLX_SIGNAL_X_ALPHA_MAX_A    ( 2500U )
//#define MLX_SIGNAL_X_ALPHA_MIN_B    ( 8200U )
//#define MLX_SIGNAL_X_ALPHA_MAX_B    ( 11800U )
//#define MLX_SIGNAL_X_ALPHA_ERR_TH_A ( 1000U )
//#define MLX_SIGNAL_X_ALPHA_ERR_TH_B ( 13000U ) 
//#define MLX_SIGNAL_X_ALPHA_SAFE_DB  ( 150U )
//
////#define MLX_SIGNAL_Y_ALPHA_MIN_A    ( 7150U ) 
////#define MLX_SIGNAL_Y_ALPHA_MAX_A    ( 2327U )
////#define MLX_SIGNAL_Y_ALPHA_MIN_B    ( 8604U )
////#define MLX_SIGNAL_Y_ALPHA_MAX_B    ( 11035U )
////#define MLX_SIGNAL_Y_ALPHA_ERR_TH_A ( 1000U )
////#define MLX_SIGNAL_Y_ALPHA_ERR_TH_B ( 12000U ) 
////#define MLX_SIGNAL_Y_ALPHA_SAFE_DB  ( 150U )
//
//#define MLX_SIGNAL_Y_ALPHA_MIN_A    ( 7400U ) 
//#define MLX_SIGNAL_Y_ALPHA_MAX_A    ( 2500U )
//#define MLX_SIGNAL_Y_ALPHA_MIN_B    ( 8200U )
//#define MLX_SIGNAL_Y_ALPHA_MAX_B    ( 11800U )
//#define MLX_SIGNAL_Y_ALPHA_ERR_TH_A ( 1000U )
//#define MLX_SIGNAL_Y_ALPHA_ERR_TH_B ( 13000U ) 
//#define MLX_SIGNAL_Y_ALPHA_SAFE_DB  ( 150U )

//#define MLX_REDUNDANCE_DB           ( 5000UL )
#define MLX_REDUNDANCE_DB           ( 3000UL )

#define MLX_REDUNDANCE_ON           ( 0xAAu )
#define MLX_REDUNDANCE_OFF          ( 0x85u )

//--- fine macro

#define MLX_CMD_NOP                 ( 0U )
#define MLX_CMD_GET1                ( 1U )
#define MLX_CMD_EE_READ             ( 2U )
#define MLX_CMD_EE_WRITE            ( 3U )
#define MLX_CMD_EE_READ_CHALLENGE   ( 4U )
#define MLX_CMD_EE_CHALLENGE_ANSWER ( 5U )

#define MLX_SPI_NO_MSG              ( 0U )
#define MLX_SPI_NEW_MSG             ( 1U )

#define MLX_SPI_TX_RX_FAIL          ( 0U )
#define MLX_SPI_TX_RX_OK_MAIN       ( 1U )
#define MLX_SPI_TX_RX_OK_REDUNDANCE ( 2U )
#define MLX_SPI_TX_OK               ( 1U )
#define MLX_SPI_RX_OK               ( 2U )
#define MLX_SPI_TX_RX_OK            ( 3U )


#define MLX_MSG_BYTE_NUM            ( 8U )

#define MLX_CAN_COMMAND_MK_0_ON     ( 0x100U )
#define MLX_CAN_COMMAND_MK_1_ON     ( 0x200U )
#define MLX_CAN_COMMAND_MK_2_ON     ( 0x400U )
#define MLX_CAN_COMMAND_MK_OFF      ( 0x800U )
#define MLX_CAN_COMMAND_INIT        ( 0x1000U )

#define MLX_MARKER_OFF              ( 0U )
#define MLX_MARKER_0_ON             ( 1U )
#define MLX_MARKER_1_ON             ( 2U )
#define MLX_MARKER_2_ON             ( 3U )

#define MLX_EE_ACCESS_READY         ( 0U )
#define MLX_EE_ACCESS_BUSY          ( 1U )

#define MLX_EE_ANSWER_NOT_READY     ( 0U )
#define MLX_EE_ANSWER_READY         ( 1U )

#define MLX_SPI_DIAGNOSE_OFF        ( 0U )
#define MLX_SPI_DIAGNOSE_ON         ( 1U )

#define MLX_SPI_AUTO_OFF            ( 0U )
#define MLX_SPI_AUTO_ON             ( 1U )

#define MLX_SENSOR_NUM              ( 2U )
#define MLX_0                       ( 0U )
#define MLX_1                       ( 1U )
//#define MLX_AXIS_X                  ( MLX_0 )
//#define MLX_AXIS_Y                  ( MLX_1 )


#define MLX_SIGNAL_MAIN             ( 0U )
#define MLX_SIGNAL_REDUNDANCE       ( 1U )

#define MLX_SIGNAL_NUM              ( 2U )
//#define MLX_SIGNAL_X_NUM            ( 1U )
//#define MLX_SIGNAL_Y_NUM            ( 1U )

#define MLX_ERROR_COUNTER_TH        ( 5U )

#define MLX_DELAY_TIME ( 30U )


#define MLX_ERROR_INDEX_ALPHA_VALUE_HIGH             (  0U )
#define MLX_ERROR_INDEX_ALPHA_VALUE_LOW              (  1U )
#define MLX_ERROR_INDEX_BETA_VALUE_HIGH              (  2U )
#define MLX_ERROR_INDEX_BETA_VALUE_LOW               (  3U )   
#define MLX_ERROR_INDEX_TIME_OUT                     (  4U )
#define MLX_ERROR_INDEX_INVALID_DATA_ALPHA           (  5U )
#define MLX_ERROR_INDEX_INVALID_DATA_BETA            (  6U )
#define MLX_ERROR_INDEX_ALPHA_REDUNDANCE             (  7U )
#define MLX_ERROR_INDEX_BETA_REDUNDANCE              (  8U )
#define MLX_ERROR_INDEX_VBB                          (  9U )
#define MLX_ERROR_INDEX_VCC                          ( 10U )
#define MLX_ERROR_INDEX_VCC_EXT                      ( 11U )
#define MLX_ERROR_INDEX_MEMORY                       ( 12U )
#define MLX_ERROR_NUM                                ( 13U )

#define MLX_ERROR_ALPHA_VALUE_HIGH_VALUE       ( 0x00000001UL )
#define MLX_ERROR_ALPHA_VALUE_LOW_VALUE        ( 0x00000002UL )
#define MLX_ERROR_BETA_VALUE_HIGH_VALUE        ( 0x00000200UL )
#define MLX_ERROR_BETA_VALUE_LOW_VALUE         ( 0x00000400UL )   
#define MLX_ERROR_TIME_OUT_VALUE               ( 0x00000004UL )
#define MLX_ERROR_INVALID_DATA_ALPHA           ( 0x00000008UL )
#define MLX_ERROR_INVALID_DATA_BETA            ( 0x00000010UL )
#define MLX_ERROR_ALPHA_REDUNDANCE_VALUE       ( 0x00000020UL )
#define MLX_ERROR_BETA_REDUNDANCE_VALUE        ( 0x00000040UL )
#define MLX_ERROR_VBB_VALUE                    ( 0x00000080UL )
#define MLX_ERROR_VCC_VALUE                    ( 0x00000100UL )
#define MLX_ERROR_VCC_EXT_VALUE                ( 0x00000200UL )
#define MLX_MEMORY_ERROR_VALUE                 ( 0x00000800UL )

#define MLX_ERROR_OUT_OF_RANGE_CNT_TH_US       ( 100000UL )
#define MLX_ERROR_TIME_OUT_CNT_TH_US           ( 100000UL )
#define MLX_ERROR_INVALID_DATA_CNT_TH_US       ( 100000UL )
#define MLX_ERROR_REDUNDANCE_CNT_TH_US         ( 100000UL )
#define MLX_ERROR_VBB_CNT_TH_US                ( 10000UL )
#define MLX_ERROR_VCC_CNT_TH_US                ( 10000UL )
#define MLX_ERROR_VCC_EXT_CNT_TH_US            ( 10000UL )
#define MLX_MEMORY_ERROR_VALUE_CNT_TH_US       ( 10000UL )
   
#define MLX_CFG_WAIT_NEUTRAL_AT_STARTUP_ALL_AXIS    ( 0x00U )
#define MLX_CFG_NOT_WAIT_NEUTRAL_AT_STARTUP_AXIS_X  ( 0x01U )
#define MLX_CFG_NOT_WAIT_NEUTRAL_AT_STARTUP_AXIS_Y  ( 0x02U )   
//#define MLX_CFG_DEFAULT                             MLX_CFG_WAIT_NEUTRAL_AT_STARTUP_ALL_AXIS    

//#define MLX_ALL_DISABLED                              ( 0x00U )
//#define MLX_CFG_AXIS_X_MAIN_SIGNAL_EN                 ( 0x01U )
//#define MLX_CFG_AXIS_X_REDUNDANT_SIGNAL_EN            ( 0x02U ) 
//#define MLX_CFG_AXIS_X_NOT_WAIT_NEUTRAL_AT_STARTUP    ( 0x04U )
//#define MLX_CFG_AXIS_Y_MAIN_SIGNAL_EN                 ( 0x08U )
//#define MLX_CFG_AXIS_Y_REDUNDANT_SIGNAL_EN            ( 0x10U ) 
//#define MLX_CFG_AXIS_Y_NOT_WAIT_NEUTRAL_AT_STARTUP    ( 0x20U )
//#define MLX_CFG_DEFAULT                               ( MLX_CFG_AXIS_X_MAIN_SIGNAL_EN | MLX_CFG_AXIS_X_REDUNDANT_SIGNAL_EN | MLX_CFG_AXIS_Y_MAIN_SIGNAL_EN | MLX_CFG_AXIS_Y_REDUNDANT_SIGNAL_EN )
//#define MLX_CFG_DEFAULT                               ( MLX_CFG_AXIS_X_MAIN_SIGNAL_EN | MLX_CFG_AXIS_Y_MAIN_SIGNAL_EN )

//#define MLX_DEFAULT_WAIT_NEUTRAL_AT_STARTUP_AXIS_X  true
//#define MLX_DEFAULT_WAIT_NEUTRAL_AT_STARTUP_AXIS_Y  true

extern uint8_t *MLX_GetTxData( uint8_t byteNum, uint8_t index );
extern uint8_t *MLX_GetRxData( uint8_t byteNum, uint8_t index );
extern uint8_t MLX_GetMsgRxStatus( uint8_t index );
extern uint8_t MLX_GetMsgTxStatus( uint8_t index );
extern void MLX_ClearMsgRxStatus( uint8_t index );
extern void MLX_ClearMsgTxStatus( uint8_t index );
extern void MLX_AngleMsgSet( uint8_t index, uint8_t value );
extern void MLX_InitSPI( uint8_t index );
extern uint8_t MLX_EEAccessStatusGet( uint8_t index );
extern void MLX_EEAccessStatusSet( uint8_t index );
extern void MLX_EEAccessStatusClear( uint8_t index );
extern uint8_t MLX_EEAnswerGet( uint8_t index );
extern void MLX_EEAnswerSet( uint8_t index );
extern void MLX_EEAnswerClear( uint8_t index );
extern void MLX_EEParameterSet( uint8_t index, uint16_t address, uint16_t data, uint8_t command );
extern uint8_t MLX_signalNumGet( uint8_t index );
extern void MLX_SPIDiagnoseSet( uint8_t index, uint8_t value );
extern uint8_t MLX_SPIDiagnoseGet( uint8_t index );
extern uint16_t MLX_AlphaDataGet( uint8_t index );
extern uint16_t MLX_AlphaValueGet( uint8_t index );
extern uint16_t MLX_BetaDataGet( uint8_t index );
extern uint16_t MLX_BetaValueGet( uint8_t index );
extern uint16_t MLX_LinearAlphaGet( void );
extern uint16_t MLX_LinearBetaGet( void );
extern uint16_t MLX_AlphaMinAGet( uint8_t index );
extern uint16_t MLX_AlphaMaxAGet( uint8_t index );
extern uint16_t MLX_AlphaMinBGet( uint8_t index );
extern uint16_t MLX_AlphaMaxBGet( uint8_t index );
extern uint16_t MLX_BetaMinAGet( uint8_t index );
extern uint16_t MLX_BetaMaxAGet( uint8_t index );
extern uint16_t MLX_BetaMinBGet( uint8_t index );
extern uint16_t MLX_BetaMaxBGet( uint8_t index );
extern void MLX_AlphaMinASet( uint8_t index, uint16_t value );
extern void MLX_AlphaMaxASet( uint8_t index, uint16_t value );
extern void MLX_AlphaMinBSet( uint8_t index, uint16_t value );
extern void MLX_AlphaMaxBSet( uint8_t index, uint16_t value );
extern void MLX_BetaMinASet( uint8_t index, uint16_t value );
extern void MLX_BetaMaxASet( uint8_t index, uint16_t value );
extern void MLX_BetaMinBSet( uint8_t index, uint16_t value );
extern void MLX_BetaMaxBSet( uint8_t index, uint16_t value );
extern void MLX_AlphaErrThASet( uint8_t index, uint16_t value );
extern uint16_t MLX_AlphaErrThAGet( uint8_t index );
extern void MLX_AlphaErrThBSet( uint8_t index, uint16_t value );
extern uint16_t MLX_AlphaErrThBGet( uint8_t index );
extern void MLX_AlphaSafeDbSet( uint8_t index, uint16_t value );
extern uint16_t MLX_AlphaSafeDbGet( uint8_t index );
extern void MLX_BetaErrThASet( uint8_t index, uint16_t value );
extern uint16_t MLX_BetaErrThAGet( uint8_t index );
extern void MLX_BetaErrThBSet( uint8_t index, uint16_t value );
extern uint16_t MLX_BetaErrThBGet( uint8_t index );
extern void MLX_BetaSafeDbSet( uint8_t index, uint16_t value );
extern uint16_t MLX_BetaSafeDbGet( uint8_t index );
//extern uint8_t MLX_ErrorGet( uint8_t index );
//extern void MLX_ErrorSet( uint8_t index, uint8_t value );
extern uint16_t MLX_LinearAlphaEval( uint8_t index, uint16_t scaledMaxValue );
extern uint16_t MLX_LinearBetaEval( uint8_t index, uint16_t scaledMaxValue );
//extern void MLX_paramFromRAMUpdate( uint8_t index );
//extern void MLX_paramToRAMUpdate( uint8_t index );
extern bool MLX_ErrorGet( uint8_t index, uint8_t errorIndex );
extern uint8_t MLX_ErrorOccurrenceGet( uint8_t index, uint8_t errorIndex );
extern void MLX_StartupMain( void );
extern void MLX_Init( void );
extern void MLX_Main( uint8_t index );

extern uint16_t MLX_alphaData[MLX_SENSOR_NUM];
extern uint16_t MLX_alphaValue[MLX_SENSOR_NUM];
extern uint16_t MLX_betaData[MLX_SENSOR_NUM];
extern uint16_t MLX_betaValue[MLX_SENSOR_NUM];

#endif
