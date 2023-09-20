/*
 * mlx.c
 *
 *  Created on: 18 apr 2018
 *      Author: PoliD
 */

#include "string.h"
#include "stdlib.h"
#include "S32K_LpSpi.h"
#include "S32K_CanDma.h"
#include "S32K_Driver.h"
#include "mlx.h"
#include "error.h"
#include "djwConfig.h"
#include "application.h"

#define TASK_MLX_PERIOD TASK_DRIVER_PERIOD

#define MLX_SPI_STATUS_WAIT                           ( 0U )
#define MLX_SPI_STATUS_INIT                           ( 1U )
#define MLX_SPI_STATUS_OFF                            ( 2U )
#define MLX_SPI_STATUS_TX                             ( 3U )
#define MLX_SPI_STATUS_EXIT                           ( 4U )
#define MLX_SPI_STATUS_EE_READ                        ( 5U )
#define MLX_SPI_STATUS_EE_READ_RX                     ( 6U )
#define MLX_SPI_STATUS_EE_WRITE                       ( 7U )
#define MLX_SPI_STATUS_EE_READ_CHALLENGE              ( 8U )
#define MLX_SPI_STATUS_EE_CHALLENGE_ANSWER            ( 9U )
#define MLX_SPI_STATUS_EE_WRITE_STATUS                ( 10U )
#define MLX_SPI_STATUS_EE_ACCESS_EXIT                 ( 11U )

#define MLX_ENABLE_SIGNAL_NONE                        ( 0x00U )
#define MLX_ENABLE_SIGNAL_MAIN                        ( 0x01U )
#define MLX_ENABLE_SIGNAL_REDUNDANCE                  ( 0x02U )

#define MLX_STATUS_NORMAL                             ( 0x00U )
#define MLX_STATUS_ERROR                              ( 0xFFU )

static uint8_t MLX_SPIStatus[MLX_SENSOR_NUM];

static uint8_t MLX_cmd[MLX_SENSOR_NUM];
static uint8_t MLX_cmdPrev[MLX_SENSOR_NUM];

static uint8_t MLX_SPIMsgTransmitted[MLX_SENSOR_NUM];
static uint8_t MLX_SPIMsgReceived[MLX_SENSOR_NUM];
static uint16_t MLX_EEAddr[MLX_SENSOR_NUM];
static uint16_t MLX_EEData[MLX_SENSOR_NUM];
static uint16_t MLX_EEKeyEcho[MLX_SENSOR_NUM];
static uint8_t MLX_EEAccessRequest[MLX_SENSOR_NUM];
static uint8_t MLX_EEAnswer[MLX_SENSOR_NUM];
static uint16_t MLX_EECommand[MLX_SENSOR_NUM];
static uint8_t MLX_angleMsgEn[MLX_SENSOR_NUM];
static uint8_t MLX_initSPI[MLX_SENSOR_NUM];
//static uint8_t MLX_SPITxRxStatus[MLX_SENSOR_NUM];
static uint8_t MLX_waitCounter[MLX_SENSOR_NUM];
//static uint8_t MLX_signalNum[MLX_SENSOR_NUM];
uint16_t MLX_alphaData[MLX_SENSOR_NUM];
uint16_t MLX_alphaValue[MLX_SENSOR_NUM];
uint16_t MLX_betaData[MLX_SENSOR_NUM];
uint16_t MLX_betaValue[MLX_SENSOR_NUM];
static uint16_t MLX_LinearAlpha[MLX_SENSOR_NUM];
static uint16_t MLX_LinearBeta[MLX_SENSOR_NUM];
static uint8_t MLX_SPIDiagnoseEn[MLX_SENSOR_NUM];
static uint8_t MLX_SPIAutoEn[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaMinA[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaMaxA[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaMinB[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaMaxB[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaErrThA[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaErrThB[MLX_SENSOR_NUM];
static uint16_t MLX_AlphaSafeDb[MLX_SENSOR_NUM];
static uint16_t MLX_BetaMinA[MLX_SENSOR_NUM];
static uint16_t MLX_BetaMaxA[MLX_SENSOR_NUM];
static uint16_t MLX_BetaMinB[MLX_SENSOR_NUM];
static uint16_t MLX_BetaMaxB[MLX_SENSOR_NUM];
static uint16_t MLX_BetaErrThA[MLX_SENSOR_NUM];
static uint16_t MLX_BetaErrThB[MLX_SENSOR_NUM];
static uint16_t MLX_BetaSafeDb[MLX_SENSOR_NUM];   
//static uint8_t MLX_Error[MLX_SENSOR_NUM];
//static uint16_t MLX_ErrorCounter[MLX_SENSOR_NUM][MLX_ERROR_NUM];
static w_error_entry_t MLX_error[MLX_SENSOR_NUM][MLX_ERROR_NUM];
static w_status_t MLX_statusAlpha[MLX_SENSOR_NUM];
static w_status_t MLX_statusBeta[MLX_SENSOR_NUM];
static uint8_t MLX_cfg;
   
static s_LpSpi_msg_t MLX_send[MLX_SENSOR_NUM];
static s_LpSpi_msg_t MLX_recv[MLX_SENSOR_NUM];

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static uint8_t MLX_computeCRC( uint8_t Byte0, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4, uint8_t Byte5, uint8_t Byte6);
static uint16_t MLX_EEKeyEval( uint8_t address );
static uint16_t MLX_EEKeyEchoEval( uint16_t key );
static uint8_t MLX_SPIMsgTxRx( uint8_t index );
static uint8_t MLX_SPIWrite( uint8_t cmd, uint8_t index );
static wres_t MLX_alphaSignalEval( uint8_t index );
static wres_t MLX_betaSignalEval( uint8_t index );
static void MLX_paramFromRAMUpdate(void);
static void MLX_paramToRAMInit( void );
static void MLX_paramToRAMUpdate( uint8_t index );
static void MLX_paramFromRAMUpdate(void);
uint16_t MLX_AlphaLinearEval( uint8_t index, uint16_t scaledMaxValue );
uint16_t MLX_BetaLinearEval( uint8_t index, uint16_t scaledMaxValue );
static wres_t MLX_AlphaRedundanceCheck( uint8_t index );
static wres_t MLX_BetaRedundanceCheck( uint8_t index );
static uint8_t MLX_computeCRC( uint8_t Byte0, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4, uint8_t Byte5, uint8_t Byte6)
{
  static const uint8_t MLX_crcArray[] = 
  {
    0x00, 0x2F, 0x5E, 0x71, 0xBC, 0x93, 0xE2, 0xCD, 0x57, 0x78, 0x09, 0x26,
    0xEB, 0xC4, 0xB5, 0x9A, 0xAE, 0x81, 0xF0, 0xDF, 0x12, 0x3D, 0x4C, 0x63,
    0xF9, 0xD6, 0xA7, 0x88, 0x45, 0x6A, 0x1B, 0x34, 0x73, 0x5C, 0x2D, 0x02,
    0xCF, 0xE0, 0x91, 0xBE, 0x24, 0x0B, 0x7A, 0x55, 0x98, 0xB7, 0xC6, 0xE9,
    0xDD, 0xF2, 0x83, 0xAC, 0x61, 0x4E, 0x3F, 0x10, 0x8A, 0xA5, 0xD4, 0xFB,
    0x36, 0x19, 0x68, 0x47, 0xE6, 0xC9, 0xB8, 0x97, 0x5A, 0x75, 0x04, 0x2B,
    0xB1, 0x9E, 0xEF, 0xC0, 0x0D, 0x22, 0x53, 0x7C, 0x48, 0x67, 0x16, 0x39,
    0xF4, 0xDB, 0xAA, 0x85, 0x1F, 0x30, 0x41, 0x6E, 0xA3, 0x8C, 0xFD, 0xD2,
    0x95, 0xBA, 0xCB, 0xE4, 0x29, 0x06, 0x77, 0x58, 0xC2, 0xED, 0x9C, 0xB3,
    0x7E, 0x51, 0x20, 0x0F, 0x3B, 0x14, 0x65, 0x4A, 0x87, 0xA8, 0xD9, 0xF6,
    0x6C, 0x43, 0x32, 0x1D, 0xD0, 0xFF, 0x8E, 0xA1, 0xE3, 0xCC, 0xBD, 0x92,
    0x5F, 0x70, 0x01, 0x2E, 0xB4, 0x9B, 0xEA, 0xC5, 0x08, 0x27, 0x56, 0x79,
    0x4D, 0x62, 0x13, 0x3C, 0xF1, 0xDE, 0xAF, 0x80, 0x1A, 0x35, 0x44, 0x6B,
    0xA6, 0x89, 0xF8, 0xD7, 0x90, 0xBF, 0xCE, 0xE1, 0x2C, 0x03, 0x72, 0x5D,
    0xC7, 0xE8, 0x99, 0xB6, 0x7B, 0x54, 0x25, 0x0A, 0x3E, 0x11, 0x60, 0x4F,
    0x82, 0xAD, 0xDC, 0xF3, 0x69, 0x46, 0x37, 0x18, 0xD5, 0xFA, 0x8B, 0xA4,
    0x05, 0x2A, 0x5B, 0x74, 0xB9, 0x96, 0xE7, 0xC8, 0x52, 0x7D, 0x0C, 0x23,
    0xEE, 0xC1, 0xB0, 0x9F, 0xAB, 0x84, 0xF5, 0xDA, 0x17, 0x38, 0x49, 0x66,
    0xFC, 0xD3, 0xA2, 0x8D, 0x40, 0x6F, 0x1E, 0x31, 0x76, 0x59, 0x28, 0x07,
    0xCA, 0xE5, 0x94, 0xBB, 0x21, 0x0E, 0x7F, 0x50, 0x9D, 0xB2, 0xC3, 0xEC,
    0xD8, 0xF7, 0x86, 0xA9, 0x64, 0x4B, 0x3A, 0x15, 0x8F, 0xA0, 0xD1, 0xFE,
    0x33, 0x1C, 0x6D, 0x42 
  };

    uint8_t crc = 0xFFU;
    crc = MLX_crcArray[crc ^ Byte0];
    crc = MLX_crcArray[crc ^ Byte1];
    crc = MLX_crcArray[crc ^ Byte2];
    crc = MLX_crcArray[crc ^ Byte3];
    crc = MLX_crcArray[crc ^ Byte4];
    crc = MLX_crcArray[crc ^ Byte5];
    crc = MLX_crcArray[crc ^ Byte6];
    crc = ~crc;
    return ( crc );
}

//------------------------------------------------------------------------------
static uint16_t MLX_EEKeyEval( uint8_t address )
{
  static const uint16_t MLX_EEWriteKeyArray[4U][8U] = 
  {
    { 17485, 31053, 57190, 57724,  7899, 53543, 26763, 12528 },
    { 38105, 51302, 16209, 24847, 13134, 52339, 14530, 18350 },
    { 55636, 64477, 40905, 45498, 24411, 36677,  4213, 48843 },
    {  6368,  5907, 31384, 63325,  3562, 19816,  6995,  3147 },
  };

    uint8_t rowindex;
    uint8_t columnindex;

    rowindex = ( address & 0x3FU ) >> 4U;
    columnindex = ( address & 0x0EU ) >> 1U;

    return ( MLX_EEWriteKeyArray[rowindex][columnindex] );
}

//------------------------------------------------------------------------------
static uint16_t MLX_EEKeyEchoEval( uint16_t key )
{
    return ( key ^ 0x1234U );
}

//------------------------------------------------------------------------------
static uint8_t MLX_SPIMsgTxRx( uint8_t index )
{
    uint8_t retValue;
    s_LpSpi_SpiId mlxSpiID;
    
    retValue = MLX_SPI_TX_RX_FAIL;
    
    mlxSpiID = SPI_MLX_1;
    if ( MLX_1 == index )
    {
        mlxSpiID = SPI_MLX_2;
    }
    
    if ( WRES_OK == S32K_LpSpi_TxRxMaster( mlxSpiID, &MLX_send[index].data[0U], &MLX_recv[index].data[0U], GPIO_OUT_MLX_CP ))
    {
        retValue = MLX_SPI_TX_OK | MLX_SPI_RX_OK;
    }

    

    return retValue;
    
}

//------------------------------------------------------------------------------
//static uint8_t MLX_SPIMsgTx( uint8_t index )
//{
//    uint8_t retValue;
//    s_LpSpi_SpiId mlxSpiID;
//    
//    retValue = MLX_SPI_TX_RX_FAIL;
//    
//    mlxSpiID = SPI_MLX_1;
//    if ( MLX_1 == index )
//    {
//        mlxSpiID = SPI_MLX_2;
//    }
//    
//    if ( WRES_OK == S32K_LpSpi_SendMsg( mlxSpiID, &MLX_send[index] ) )
//    {
//        retValue |= MLX_SPI_TX_OK;
//    }
//
//    return retValue;
//}
//
////------------------------------------------------------------------------------
//static uint8_t MLX_SPIMsgRx( uint8_t index )
//{
//    uint8_t retValue;
////    wres_t res;
//    s_LpSpi_SpiId mlxSpiID;
//    
//    retValue = MLX_SPI_TX_RX_FAIL;
////    res = WRES_ERROR;
//    
//    mlxSpiID = SPI_MLX_1;
//    if ( MLX_1 == index )
//    {
//        mlxSpiID = SPI_MLX_2;
//    }
//    
//    memset(&MLX_recv[index], 0x00U, 8U);
//
//    
//    if ( WRES_OK == S32K_LpSpi_RecvMsg( mlxSpiID, &MLX_recv[index] ) )
//    {
//        retValue |= MLX_SPI_RX_OK;
//    }
//
////    while ( WRES_BUFFER_EMPTY !=  res )
////    {
////        res = S32K_LpSpi_RecvMsg( mlxSpiID, &MLX_recv[index] );
////        if ( WRES_OK == res )
////        {
////            retValue |= MLX_SPI_RX_OK;
////        }
////    }
//    
//    return retValue;
//}

//------------------------------------------------------------------------------
static uint8_t MLX_SPIWrite( uint8_t cmd, uint8_t index ) 
{
    uint8_t txBuffer[8];
    uint8_t i;
    uint8_t msgTxStatus;
    uint16_t EEKey;

    switch ( cmd ) {
    case MLX_CMD_NOP:
        txBuffer[0] = 0U;
        txBuffer[1] = 0U;
        txBuffer[2] = 0xCCU;
        txBuffer[3] = 0xEEU;
        txBuffer[4] = 0U;
        txBuffer[5] = 0U;
        txBuffer[6] = 0xD0U;
        break;
    case MLX_CMD_GET1:
        txBuffer[0] = 0U;
        txBuffer[1] = 0U;
        txBuffer[2] = 0xFFU;
        txBuffer[3] = 0xFFU;
        txBuffer[4] = 0U;
        txBuffer[5] = 0U;
        if ( MLX_MARKER_0_ON == MLX_angleMsgEn[index] ) 
        {
            txBuffer[6] = 0x13U; //Marker = 0 - Alpha + Diagnostic
        } 
        else if ( MLX_MARKER_1_ON == MLX_angleMsgEn[index] ) 
        {
	    txBuffer[6] = 0x53U; //Marker = 1 - Alpha + Beta + Diagnostic
        } 
        else if ( MLX_MARKER_2_ON == MLX_angleMsgEn[index] ) 
        {
	    txBuffer[6] = 0x93; //Marker = 2 - X + Y + Z + Diagnostic
        } 
        else 
        {
	    txBuffer[6] = 0x13U;
        }
        break;
    case MLX_CMD_EE_READ:
        txBuffer[0] = (uint8_t)( MLX_EEAddr[index] );
        txBuffer[1] = (uint8_t)( MLX_EEAddr[index] >> 8U );
        txBuffer[2] = (uint8_t)( MLX_EEAddr[index] );
        txBuffer[3] = (uint8_t)( MLX_EEAddr[index] >> 8U );
        txBuffer[4] = 0U;
        txBuffer[5] = 0U;
        txBuffer[6] = 0xC1U;
        break;
    case MLX_CMD_EE_WRITE:
        EEKey = MLX_EEKeyEval( (uint8_t)( MLX_EEAddr[index] ) );
        txBuffer[1] = (uint8_t)MLX_EEAddr[index] & 0x3FU;
        txBuffer[4] = (uint8_t)( MLX_EEData[index] );
        txBuffer[5] = (uint8_t)( MLX_EEData[index] >> 8U );
        txBuffer[0] = 0U;
        txBuffer[2] = (uint8_t)( EEKey );
        txBuffer[3] = (uint8_t)( EEKey >> 8U );
        txBuffer[6] = 0xC3U;
        break;
    case MLX_CMD_EE_READ_CHALLENGE:
        txBuffer[0] = 0U;
        txBuffer[1] = 0U;
        txBuffer[2] = 0U;
        txBuffer[3] = 0U;
        txBuffer[4] = 0U;
        txBuffer[5] = 0U;
        txBuffer[6] = 0xCFU;
        break;
    case MLX_CMD_EE_CHALLENGE_ANSWER:
        EEKey = MLX_EEKeyEchoEval( MLX_EEKeyEcho[index] );
        txBuffer[0] = 0U;
        txBuffer[1] = 0U;
        txBuffer[2] = (uint8_t)( EEKey );
        txBuffer[3] = (uint8_t)( EEKey >> 8U );
        txBuffer[4] = (uint8_t)( ~EEKey );
        txBuffer[5] = (uint8_t)( (~EEKey) >> 8U );
        txBuffer[6] = 0xC5U;
        break;
    default:
        txBuffer[0] = 0U;
        txBuffer[1] = 0U;
        txBuffer[2] = 0U;
        txBuffer[3] = 0U;
        txBuffer[4] = 0U;
        txBuffer[5] = 0U;
        txBuffer[6] = 0U;
        break;
    }
    txBuffer[7] = MLX_computeCRC( txBuffer[0], txBuffer[1], txBuffer[2], txBuffer[3], txBuffer[4], txBuffer[5], txBuffer[6] );

    for ( i=0U; i<MLX_MSG_BYTE_NUM; i++ ) 
    { 
        MLX_send[index].data[i] = txBuffer[i]; 
    }

    msgTxStatus = MLX_SPIMsgTxRx( index );

    return msgTxStatus;
}

//------------------------------------------------------------------------------
static wres_t MLX_alphaSignalEval( uint8_t index )
{
     wres_t retVal;
     uint16_t aux;
     
     retVal = WRES_ERROR_INVALID_PARAMETER;
     if ( 0x8000U == ( MLX_alphaData[index] & 0x8000U ) )  //il byte 15 mi dice se il dato è valido oppure se ho avuto errore
     {
         aux = MLX_alphaData[index] & 0x3FFFU;
         MLX_alphaValue[index] = aux;
         retVal = WRES_OK;
     }
     
     return retVal;
}

//------------------------------------------------------------------------------
static wres_t MLX_betaSignalEval( uint8_t index )
{
     wres_t retVal;
     uint16_t aux;
     
     retVal = WRES_ERROR_INVALID_PARAMETER;
     if ( 0x8000U == ( MLX_alphaData[index] & 0x8000U ) )  //il byte 15 mi dice se il dato è valido oppure se ho avuto errore
     {
         aux = MLX_betaData[index] & 0x3FFFU;
         MLX_betaValue[index] = aux;
         retVal = WRES_OK;
     }
       
     return retVal;
}

//------------------------------------------------------------------------------
static void MLX_paramFromRAMUpdate(void)
{
#ifndef DJW_MEM_DISABLED

    uint32_t datoRead;
    
    //Alpha
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_MIN_A, &datoRead ) )
    {
        MLX_AlphaMinA[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaMinA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_MAX_A, &datoRead ) )
    {
        MLX_AlphaMaxA[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaMaxA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_MIN_B, &datoRead ) )
    {
        MLX_AlphaMinB[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaMinB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_MAX_B, &datoRead ) )
    {
      MLX_AlphaMaxB[MLX_0] = (uint16_t)( datoRead ); 
      MLX_AlphaMaxB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_ERR_TH_A, &datoRead ) )
    {
        MLX_AlphaErrThA[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaErrThA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_ERR_TH_B, &datoRead ) )
    {
        MLX_AlphaErrThB[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaErrThB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_ALPHA_SAFE_DB, &datoRead ) )
    {
        MLX_AlphaSafeDb[MLX_0] = (uint16_t)( datoRead );
        MLX_AlphaSafeDb[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    
    //Beta
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_MIN_A, &datoRead ) )
    {
        MLX_BetaMinA[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaMinA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_MAX_A, &datoRead ) )
    {
        MLX_BetaMaxA[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaMaxA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_MIN_B, &datoRead ) )
    {
        MLX_BetaMinB[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaMinB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_MAX_B, &datoRead ) )
    {
        MLX_BetaMaxB[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaMaxB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_ERR_TH_A, &datoRead ) )
    {
        MLX_BetaErrThA[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaErrThA[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_ERR_TH_B, &datoRead ) )
    {
        MLX_BetaErrThB[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaErrThB[MLX_1] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_BETA_SAFE_DB, &datoRead ) )
    {
        MLX_BetaSafeDb[MLX_0] = (uint16_t)( datoRead );
        MLX_BetaSafeDb[MLX_1] = (uint16_t)( datoRead >> 16U );
    }   
    
    if ( 0U == MemData_GetRAMItem( ADDR_MLX_CFG, &datoRead ) )
    {
        MLX_cfg = (uint8_t)( datoRead );
        if ( MLX_REDUNDANCE_OFF == MLX_cfg )
        {
            MLX_error[MLX_0][MLX_ERROR_INDEX_ALPHA_REDUNDANCE].en = false;
            MLX_error[MLX_0][MLX_ERROR_INDEX_BETA_REDUNDANCE].en = false;
        }
        else
        {
            MLX_error[MLX_1][MLX_ERROR_INDEX_ALPHA_REDUNDANCE].en = true;
            MLX_error[MLX_1][MLX_ERROR_INDEX_BETA_REDUNDANCE].en = true;          
        }
    }
    
#endif    
    return;
  
}

//------------------------------------------------------------------------------
static void MLX_paramToRAMInit( void )
{
#ifndef DJW_MEM_DISABLED
    MemData_SetRAMItem( ADDR_MLX_ALPHA_MIN_A,    ( MLX_AlphaMinA[MLX_0] )   | ( (uint32_t)MLX_AlphaMinA[MLX_1] << 16U ) );
    MemData_SetRAMItem( ADDR_MLX_ALPHA_MAX_A,    ( MLX_AlphaMaxA[MLX_0] )   | ( (uint32_t)MLX_AlphaMaxA[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_ALPHA_MIN_B,    ( MLX_AlphaMinB[MLX_0] )   | ( (uint32_t)MLX_AlphaMinB[MLX_1] << 16U ) );
    MemData_SetRAMItem( ADDR_MLX_ALPHA_MAX_B,    ( MLX_AlphaMaxB[MLX_0] )   | ( (uint32_t)MLX_AlphaMaxB[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_ALPHA_ERR_TH_A, ( MLX_AlphaErrThA[MLX_0] ) | ( (uint32_t)MLX_AlphaErrThA[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_ALPHA_ERR_TH_B, ( MLX_AlphaErrThB[MLX_0] ) | ( (uint32_t)MLX_AlphaErrThB[MLX_1] << 16U ) );        
    MemData_SetRAMItem( ADDR_MLX_ALPHA_SAFE_DB,  ( MLX_AlphaSafeDb[MLX_0] ) | ( (uint32_t)MLX_AlphaSafeDb[MLX_1] << 16U ) );
    MemData_SetRAMItem( ADDR_MLX_BETA_MIN_A,     ( MLX_BetaMinA[MLX_0] )    | ( (uint32_t)MLX_BetaMinA[MLX_1] << 16U ) );
    MemData_SetRAMItem( ADDR_MLX_BETA_MAX_A,     ( MLX_BetaMaxA[MLX_0] )    | ( (uint32_t)MLX_BetaMaxA[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_BETA_MIN_B,     ( MLX_BetaMinB[MLX_0] )    | ( (uint32_t)MLX_BetaMinB[MLX_1] << 16U ) );
    MemData_SetRAMItem( ADDR_MLX_BETA_MAX_B,     ( MLX_BetaMaxB[MLX_0] )    | ( (uint32_t)MLX_BetaMaxB[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_BETA_ERR_TH_A,  ( MLX_BetaErrThA[MLX_0] )  | ( (uint32_t)MLX_BetaErrThA[MLX_1] << 16U ) );    
    MemData_SetRAMItem( ADDR_MLX_BETA_ERR_TH_B,  ( MLX_BetaErrThB[MLX_0] )  | ( (uint32_t)MLX_BetaErrThB[MLX_1] << 16U ) );        
    MemData_SetRAMItem( ADDR_MLX_BETA_SAFE_DB,   ( MLX_BetaSafeDb[MLX_0] )  | ( (uint32_t)MLX_BetaSafeDb[MLX_1] << 16U ) );  
    MemData_SetRAMItem( ADDR_MLX_CFG,            (uint32_t)MLX_cfg );
#endif
    return; 
}

//------------------------------------------------------------------------------
static void MLX_paramToRAMUpdate( uint8_t index )
{
#ifndef DJW_MEM_DISABLED
    MemData_SetRAMItem( (uint32_t)ADDR_MLX_0_ALPHA_SGN + ( 0x02u * index ), (uint32_t)MLX_alphaValue[index] );
    MemData_SetRAMItem( (uint32_t)ADDR_MLX_0_BETA_SGN  + ( 0x02u * index ), (uint32_t)MLX_betaValue[index] );
#endif
    return;
}

//------------------------------------------------------------------------------
uint8_t *MLX_GetTxData( uint8_t byteNum, uint8_t index )
{
    uint8_t *buffer;

    buffer = (uint8_t *)malloc( byteNum * sizeof( uint8_t ) );
    buffer = MLX_send[index].data;

    return buffer;
}

//------------------------------------------------------------------------------
uint8_t *MLX_GetRxData( uint8_t byteNum, uint8_t index )
{
    uint8_t *buffer;

    buffer = (uint8_t *)malloc( byteNum * sizeof( uint8_t ) );
    buffer = MLX_recv[index].data;

    return buffer;
}

//------------------------------------------------------------------------------
uint8_t MLX_GetMsgRxStatus( uint8_t index )
{
    return MLX_SPIMsgReceived[index];
}

//------------------------------------------------------------------------------
uint8_t MLX_GetMsgTxStatus( uint8_t index )
{
    return MLX_SPIMsgTransmitted[index];
}

//------------------------------------------------------------------------------
void MLX_ClearMsgRxStatus( uint8_t index )
{
    MLX_SPIMsgReceived[index] = MLX_SPI_NO_MSG;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_ClearMsgTxStatus( uint8_t index )
{
    MLX_SPIMsgTransmitted[index] = MLX_SPI_NO_MSG;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_AngleMsgSet( uint8_t index, uint8_t value )
{
    MLX_angleMsgEn[index] = value;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_InitSPI( uint8_t index )
{
    MLX_initSPI[index] = 1U;
    
    return;
}

//------------------------------------------------------------------------------
uint8_t MLX_EEAccessStatusGet( uint8_t index )
{
    return MLX_EEAccessRequest[index];
}

//------------------------------------------------------------------------------
void MLX_EEAccessStatusSet( uint8_t index )
{
    MLX_EEAccessRequest[index] = MLX_EE_ACCESS_BUSY;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_EEAccessStatusClear( uint8_t index )
{
    MLX_EEAccessRequest[index] = MLX_EE_ACCESS_READY;
    
    return;
}

//------------------------------------------------------------------------------
uint8_t MLX_EEAnswerGet( uint8_t index )
{
    return MLX_EEAnswer[index];
}

//------------------------------------------------------------------------------
void MLX_EEAnswerSet( uint8_t index )
{
    MLX_EEAnswer[index] = MLX_EE_ANSWER_READY;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_EEAnswerClear( uint8_t index )
{
    MLX_EEAnswer[index] = MLX_EE_ANSWER_NOT_READY;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_EEParameterSet( uint8_t index, uint16_t address, uint16_t data, uint8_t command )
{
    MLX_EEAddr[index] = address;
    MLX_EEData[index] = data;
    MLX_EECommand[index] = command;
    
    return;
}

//------------------------------------------------------------------------------
void MLX_SPIDiagnoseSet( uint8_t index, uint8_t value )
{
    MLX_SPIDiagnoseEn[index] = value;
    
    return;
}

//------------------------------------------------------------------------------
uint8_t MLX_SPIDiagnoseGet( uint8_t index )
{
    return MLX_SPIDiagnoseEn[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaDataGet( uint8_t index )
{
    return MLX_alphaData[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaDataGet( uint8_t index )
{
    return MLX_betaData[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaValueGet( uint8_t index )
{
    return MLX_alphaValue[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaValueGet( uint8_t index )
{
    return MLX_betaValue[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaMinAGet( uint8_t index )
{
    return MLX_AlphaMinA[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaMinAGet( uint8_t index )
{
    return MLX_BetaMinA[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaMaxAGet( uint8_t index )
{
    return MLX_AlphaMaxA[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaMaxAGet( uint8_t index )
{
    return MLX_BetaMaxA[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaMinBGet( uint8_t index )
{
    return MLX_AlphaMinB[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaMinBGet( uint8_t index )
{
    return MLX_BetaMinB[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaMaxBGet( uint8_t index )
{
    return MLX_AlphaMaxB[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaMaxBGet( uint8_t index )
{
    return MLX_BetaMaxB[index];
}

//------------------------------------------------------------------------------
void MLX_AlphaMinASet( uint8_t index, uint16_t value )
{
    MLX_AlphaMinA[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaMinASet( uint8_t index, uint16_t value )
{
    MLX_AlphaMinA[index] = value;
}

//------------------------------------------------------------------------------
void MLX_AlphaMaxASet( uint8_t index, uint16_t value )
{
    MLX_AlphaMaxA[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaMaxASet( uint8_t index, uint16_t value )
{
    MLX_BetaMaxA[index] = value;
}

//------------------------------------------------------------------------------
void MLX_AlphaMinBSet( uint8_t index, uint16_t value )
{
    MLX_AlphaMinB[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaMinBSet( uint8_t index, uint16_t value )
{
    MLX_BetaMinB[index] = value;
}

//------------------------------------------------------------------------------
void MLX_AlphaMaxBSet( uint8_t index, uint16_t value )
{
    MLX_AlphaMaxB[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaMaxBSet( uint8_t index, uint16_t value )
{
    MLX_BetaMaxB[index] = value;
}

//------------------------------------------------------------------------------
void MLX_AlphaErrThASet( uint8_t index, uint16_t value )
{
    MLX_AlphaErrThA[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaErrThASet( uint8_t index, uint16_t value )
{
    MLX_BetaErrThA[index] = value;
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaErrThAGet( uint8_t index )
{
    return MLX_AlphaErrThA[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaErrThAGet( uint8_t index )
{
    return MLX_BetaErrThA[index];
}

//------------------------------------------------------------------------------
void MLX_AlphaErrThBSet( uint8_t index, uint16_t value )
{
    MLX_AlphaErrThB[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaErrThBSet( uint8_t index, uint16_t value )
{
    MLX_BetaErrThB[index] = value;
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaErrThBGet( uint8_t index )
{
    return MLX_AlphaErrThB[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaErrThBGet( uint8_t index )
{
    return MLX_BetaErrThB[index];
}

//------------------------------------------------------------------------------
void MLX_AlphaSafeDbSet( uint8_t index, uint16_t value )
{
    MLX_AlphaSafeDb[index] = value;
}

//------------------------------------------------------------------------------
void MLX_BetaSafeDbSet( uint8_t index, uint16_t value )
{
    MLX_BetaSafeDb[index] = value;
}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaSafeDbGet( uint8_t index )
{
    return MLX_AlphaSafeDb[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaSafeDbGet( uint8_t index )
{
    return MLX_BetaSafeDb[index];
}

//------------------------------------------------------------------------------
uint16_t MLX_LinearAlphaGet( void )
{
    uint16_t aux;  
  
    aux = 0xFFFEU; 
    if ( MLX_REDUNDANCE_OFF == MLX_cfg )
    {
        aux = MLX_LinearAlpha[MLX_1];
    }
    else
    {
        if ( ( MLX_STATUS_NORMAL == MLX_statusAlpha[MLX_0].value ) && ( MLX_STATUS_NORMAL == MLX_statusAlpha[MLX_1].value ) )
        {
            aux = MLX_LinearAlpha[MLX_1];
        }
    }
    
    return aux;
}

//------------------------------------------------------------------------------
uint16_t MLX_LinearBetaGet( void )
{
    uint16_t aux;  
  
    aux = 0xFFFEU;
    
    if ( MLX_REDUNDANCE_OFF == MLX_cfg )
    {    
        aux = MLX_LinearBeta[MLX_1];
    }
    else
    {
        if ( ( MLX_STATUS_NORMAL == MLX_statusBeta[MLX_0].value ) && ( MLX_STATUS_NORMAL == MLX_statusBeta[MLX_1].value ) )
        {
            aux = MLX_LinearBeta[MLX_1];
        }
    }
    
    return aux;
}

////------------------------------------------------------------------------------
//uint8_t MLX_ErrorGet( uint8_t index, uint8_t errorIndex )
//{
//    //return MLX_Error[index];
//}
//
////------------------------------------------------------------------------------
//void MLX_ErrorSet( uint8_t index, uint8_t value )
//{
//    //MLX_Error[index] = value;
//}

//------------------------------------------------------------------------------
uint16_t MLX_AlphaLinearEval( uint8_t index, uint16_t scaledMaxValue )
{
    uint32_t auxVal;
    
    if ( 0xFFFFU == MLX_alphaValue[index] )
    {
        auxVal = 0xFFFFUL;
    }
    else if ( ( MLX_alphaValue[index] < MLX_AlphaErrThA[index] ) || ( MLX_alphaValue[index] > MLX_AlphaErrThB[index] ) )
    {
        auxVal = 0xFFFEUL;
    }
    else if ( ( MLX_alphaValue[index] < ( MLX_AlphaMinB[index] + MLX_AlphaSafeDb[index] ) ) && ( MLX_alphaValue[index] > ( MLX_AlphaMinA[index] - MLX_AlphaSafeDb[index] ) ) )
    {
        auxVal = scaledMaxValue;
    }
    else if ( MLX_alphaValue[index] > ( MLX_AlphaMaxB[index] - MLX_AlphaSafeDb[index] ) )
    {
        auxVal = ((uint32_t)scaledMaxValue + scaledMaxValue);
    }    
    else if ( MLX_alphaValue[index] >= ( MLX_AlphaMinB[index] + MLX_AlphaSafeDb[index] ) )
    {
        auxVal = (uint32_t)( MLX_alphaValue[index] ) - ( (uint32_t)MLX_AlphaMinB[index] + MLX_AlphaSafeDb[index] );
        auxVal = auxVal * (uint32_t)( scaledMaxValue );
        auxVal = auxVal / ( ( (uint32_t)MLX_AlphaMaxB[index] - MLX_AlphaSafeDb[index] ) - ( (uint32_t)MLX_AlphaMinB[index] + MLX_AlphaSafeDb[index] ) );
        auxVal = auxVal + scaledMaxValue;         
    }
    else if ( MLX_alphaValue[index] < ( MLX_AlphaMaxA[index] + MLX_AlphaSafeDb[index] ) )
    {
        auxVal = 0UL;
    }    
    else if ( MLX_alphaValue[index] <= ( MLX_AlphaMinA[index] - MLX_AlphaSafeDb[index] ) )
    {
        auxVal = ( (uint32_t)MLX_AlphaMinA[index] - MLX_AlphaSafeDb[index] ) - ( (uint32_t)MLX_alphaValue[index] );
        auxVal = auxVal * (uint32_t)( scaledMaxValue );
        auxVal = auxVal / ( ( (uint32_t)MLX_AlphaMinA[index] - MLX_AlphaSafeDb[index] ) - ( (uint32_t)MLX_AlphaMaxA[index] + MLX_AlphaSafeDb[index] ) );      
        auxVal = scaledMaxValue - auxVal;        
    }
    else
    {
        auxVal = 0xFFFFUL;
    }
    
    return (uint16_t)( auxVal ); 
}

//------------------------------------------------------------------------------
uint16_t MLX_BetaLinearEval( uint8_t index, uint16_t scaledMaxValue )
{
    uint32_t auxVal;
    
    if ( 0xFFFFU == MLX_betaValue[index] )
    {
        auxVal = 0xFFFFUL;
    }
    else if ( ( MLX_betaValue[index] < MLX_BetaErrThA[index] ) || ( MLX_betaValue[index] > MLX_BetaErrThB[index] ) )
    {
        auxVal = 0xFFFEUL;
    }
    else if ( ( MLX_betaValue[index] < ( MLX_BetaMinB[index] + MLX_BetaSafeDb[index] ) ) && ( MLX_betaValue[index] > ( MLX_BetaMinA[index] - MLX_BetaSafeDb[index] ) ) )
    {
        auxVal = scaledMaxValue;
    }
    else if ( MLX_betaValue[index] > ( MLX_BetaMaxB[index] - MLX_BetaSafeDb[index] ) )
    {
        auxVal = (uint32_t)scaledMaxValue + scaledMaxValue;
    }    
    else if ( MLX_betaValue[index] >= ( MLX_BetaMinB[index] + MLX_BetaSafeDb[index] ) )
    {
        auxVal = ( (uint32_t)MLX_betaValue[index] ) - ( (uint32_t)MLX_BetaMinB[index] + MLX_BetaSafeDb[index] );
        auxVal = auxVal * (uint32_t)( scaledMaxValue );
        auxVal = auxVal / ( ( (uint32_t)MLX_BetaMaxB[index] - MLX_BetaSafeDb[index] ) - ( (uint32_t)MLX_BetaMinB[index] + MLX_BetaSafeDb[index] ) );
        auxVal = auxVal + scaledMaxValue;         
    }
    else if ( MLX_betaValue[index] < ( MLX_BetaMaxA[index] + MLX_BetaSafeDb[index] ) )
    {
        auxVal = 0UL;
    }    
    else if ( MLX_betaValue[index] <= ( MLX_BetaMinA[index] - MLX_BetaSafeDb[index] ) )
    {
        auxVal = ( (uint32_t)MLX_BetaMinA[index] - MLX_BetaSafeDb[index] ) - ( (uint32_t)MLX_betaValue[index] );
        auxVal = auxVal * (uint32_t)( scaledMaxValue );
        auxVal = auxVal / ( ( (uint32_t)MLX_BetaMinA[index] - MLX_BetaSafeDb[index] ) - ( (uint32_t)MLX_BetaMaxA[index] + MLX_BetaSafeDb[index] ) );      
        auxVal = scaledMaxValue - auxVal;        
    }
    else
    {
        auxVal = 0xFFFFUL;
    }
    
    return (uint16_t)( auxVal ); 
}

//------------------------------------------------------------------------------
//static wres_t MLX_RedundanceCheck( void )
//{
//    wres_t retVal;
//    
////    uint32_t aux1;
////    uint32_t aux2;
////    uint32_t redundanceSumCheck;
////    uint32_t redundanceSum;
//    
//    retVal = WRES_OK;
//    
////    if ( 3U == MLX_signalNum[index] )
////    {
//////        aux1 = ( (uint32_t)( MLX_AlphaMinA[index][MLX_SIGNAL_MAIN] ) + (uint32_t)( MLX_AlphaMinB[index][MLX_SIGNAL_MAIN] ) ) >> 1U;
//////        aux2 = ( (uint32_t)( MLX_AlphaMinA[index][MLX_SIGNAL_REDUNDANCE] ) + (uint32_t)( MLX_AlphaMinB[index][MLX_SIGNAL_REDUNDANCE] ) ) >> 1U;
////        aux1 = ( (uint32_t)( MLX_AlphaMaxA[index][MLX_SIGNAL_MAIN] ) + (uint32_t)( MLX_AlphaMaxB[index][MLX_SIGNAL_MAIN] ) ) >> 1U;
////        aux2 = ( (uint32_t)( MLX_AlphaMaxA[index][MLX_SIGNAL_REDUNDANCE] ) + (uint32_t)( MLX_AlphaMaxB[index][MLX_SIGNAL_REDUNDANCE] ) ) >> 1U;      
////        redundanceSumCheck = aux1 + aux2;
////        redundanceSum = (uint32_t)( MLX_alphaValue[index][MLX_SIGNAL_MAIN] ) + (uint32_t)( MLX_alphaValue[index][MLX_SIGNAL_REDUNDANCE] );
////        if ( ( redundanceSum > ( redundanceSumCheck + MLX_REDUNDANCE_DB ) ) || ( redundanceSum < ( redundanceSumCheck - MLX_REDUNDANCE_DB ) ) )
////        {
////            retVal = WRES_ERROR_INVALID_PARAMETER;
////        }
////    }
//    
//    return retVal;
//}

//------------------------------------------------------------------------------
static wres_t MLX_AlphaRedundanceCheck( uint8_t index )
{
    (void)( index );
    wres_t retVal;
    
    uint32_t aux1;
    uint32_t aux2;
    uint32_t redundanceSumCheck;
    uint32_t redundanceSum;
    
    retVal = WRES_OK;
    
    aux1 = ( (uint32_t)( MLX_AlphaMaxA[MLX_0] ) + (uint32_t)( MLX_AlphaMaxB[MLX_0] ) ) >> 1U;
    aux2 = ( (uint32_t)( MLX_AlphaMaxA[MLX_1] ) + (uint32_t)( MLX_AlphaMaxB[MLX_1] ) ) >> 1U;      
    redundanceSumCheck = aux1 + aux2;
    redundanceSum = (uint32_t)( MLX_alphaValue[MLX_0] ) + (uint32_t)( MLX_alphaValue[MLX_1] );
    if ( ( redundanceSum > ( redundanceSumCheck + MLX_REDUNDANCE_DB ) ) || ( redundanceSum < ( redundanceSumCheck - MLX_REDUNDANCE_DB ) ) )
    {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t MLX_BetaRedundanceCheck( uint8_t index )
{
    (void)( index );
    wres_t retVal;
    
    uint32_t aux1;
    uint32_t aux2;
    uint32_t redundanceSumCheck;
    uint32_t redundanceSum;
    
    retVal = WRES_OK;
    
    aux1 = ( (uint32_t)( MLX_BetaMaxA[MLX_0] ) + (uint32_t)( MLX_BetaMaxB[MLX_0] ) ) >> 1U;
    aux2 = ( (uint32_t)( MLX_BetaMaxA[MLX_1] ) + (uint32_t)( MLX_BetaMaxB[MLX_1] ) ) >> 1U;      
    redundanceSumCheck = aux1 + aux2;
    redundanceSum = (uint32_t)( MLX_betaValue[MLX_0] ) + (uint32_t)( MLX_betaValue[MLX_1] );
    if ( ( redundanceSum > ( redundanceSumCheck + MLX_REDUNDANCE_DB ) ) || ( redundanceSum < ( redundanceSumCheck - MLX_REDUNDANCE_DB ) ) )
    {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
bool MLX_ErrorGet( uint8_t index, uint8_t errorIndex )
{
    return Error_IsActive( MLX_error[index][errorIndex] );
}

//------------------------------------------------------------------------------
uint8_t MLX_ErrorOccurrenceGet( uint8_t index, uint8_t errorIndex )
{
    return Error_OccurrenceGet( MLX_error[index][errorIndex] );
}

//------------------------------------------------------------------------------
void MLX_Init( void )
{
  static const uint16_t MLX_ERROR_CNT_TH[MLX_ERROR_NUM] = 
  {
    MLX_ERROR_OUT_OF_RANGE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_OUT_OF_RANGE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_OUT_OF_RANGE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_OUT_OF_RANGE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_TIME_OUT_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_INVALID_DATA_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_INVALID_DATA_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_REDUNDANCE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_REDUNDANCE_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_VBB_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_VCC_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_ERROR_VCC_EXT_CNT_TH_US / TASK_MLX_PERIOD,
//    MLX_ERROR_SAFETY_CNT_TH_US / TASK_MLX_PERIOD,
    MLX_MEMORY_ERROR_VALUE_CNT_TH_US / TASK_MLX_PERIOD, 
  };
  static const uint16_t MLX_ERROR_VALUE[MLX_ERROR_NUM] = 
  {
    MLX_ERROR_ALPHA_VALUE_HIGH_VALUE,     
    MLX_ERROR_ALPHA_VALUE_LOW_VALUE,      
    MLX_ERROR_BETA_VALUE_HIGH_VALUE,      
    MLX_ERROR_BETA_VALUE_LOW_VALUE,      
    MLX_ERROR_TIME_OUT_VALUE,             
    MLX_ERROR_INVALID_DATA_ALPHA,         
    MLX_ERROR_INVALID_DATA_BETA,          
    MLX_ERROR_ALPHA_REDUNDANCE_VALUE,
    MLX_ERROR_BETA_REDUNDANCE_VALUE,
    MLX_ERROR_VBB_VALUE,                  
    MLX_ERROR_VCC_VALUE,                  
    MLX_ERROR_VCC_EXT_VALUE,   
//    MLX_ERROR_SAFETY_VALUE,
    MLX_MEMORY_ERROR_VALUE,
  };

    uint8_t i;
    uint8_t j;
    
    for ( i=0U; i<MLX_SENSOR_NUM; i++ ) 
    {
        MLX_SPIStatus[i] = MLX_SPI_STATUS_WAIT;
        MLX_angleMsgEn[i] = 0U;
        MLX_initSPI[i] = 0U;
        MLX_cmd[i] = 0U;
        MLX_cmdPrev[i] = 0xFFU;
        //MLX_SPITxRxStatus[i] = MLX_SPI_NO_MSG;
        MLX_waitCounter[i] = 0U;
        MLX_SPIDiagnoseEn[i] = MLX_SPI_DIAGNOSE_OFF;
        MLX_SPIAutoEn[i] = MLX_SPI_AUTO_ON;
        MLX_LinearAlpha[i] = 0xFFFFU;
        MLX_LinearBeta[i] = 0xFFFFU;
        memset( MLX_recv[i].data, 0xFF, MLX_MSG_BYTE_NUM );
        memset( MLX_send[i].data, 0x00, MLX_MSG_BYTE_NUM );
        MLX_statusAlpha[i].value = MLX_STATUS_NORMAL;
        MLX_statusBeta[i].value = MLX_STATUS_NORMAL;
        MLX_SPIMsgReceived[i] = 0U;
        MLX_SPIMsgTransmitted[i] = 0U;
        MLX_EEAddr[i] = 0U;
        MLX_EEData[i] = 0U;
        MLX_EEAccessRequest[i] = MLX_EE_ACCESS_READY;
        MLX_EEAnswer[i] = MLX_EE_ANSWER_NOT_READY;
        MLX_EECommand[i] = 0U;
        MLX_alphaData[i] = 0x8000U;
        MLX_alphaValue[i] = 0xFFFFU;
        MLX_betaData[i] = 0x8000U;
        MLX_betaValue[i] = 0xFFFFU;        
        for ( j=0U; j<MLX_ERROR_NUM; j++ )
        {
            MLX_error[i][j].cnt = 0U;
            MLX_error[i][j].cntTh = MLX_ERROR_CNT_TH[j];
            MLX_error[i][j].en = true;
            MLX_error[i][j].active = false;
            MLX_error[i][j].occurrence = 0U;
            MLX_error[i][j].value = MLX_ERROR_VALUE[j];
        }
    }

//    MLX_cfg = MLX_CFG_DEFAULT;
//    if ( ( MLX_CFG_AXIS_X_NOT_WAIT_NEUTRAL_AT_STARTUP & MLX_cfg ) ==  MLX_CFG_AXIS_X_NOT_WAIT_NEUTRAL_AT_STARTUP )
//    {
//        MLX_status[MLX_0].value = MLX_STATUS_NORMAL;
//        MLX_alphaValue[MLX_0][MLX_SIGNAL_MAIN] = 0U;
//        MLX_alphaValue[MLX_0][MLX_SIGNAL_REDUNDANCE] = 0U;
//    }
//    if ( ( MLX_CFG_AXIS_Y_NOT_WAIT_NEUTRAL_AT_STARTUP & MLX_cfg ) ==  MLX_CFG_AXIS_Y_NOT_WAIT_NEUTRAL_AT_STARTUP )
//    {
//        MLX_status[MLX_1].value = MLX_STATUS_NORMAL;
//        MLX_alphaValue[MLX_1][MLX_SIGNAL_MAIN] = 0U;
//        MLX_alphaValue[MLX_1][MLX_SIGNAL_REDUNDANCE] = 0U;        
//    }
//    
//    if ( ( MLX_CFG_AXIS_X_MAIN_SIGNAL_EN & MLX_cfg ) == MLX_CFG_AXIS_X_MAIN_SIGNAL_EN )
//    {
//        MLX_signalNum[MLX_0] = 1U;
//    }
//    if ( ( MLX_CFG_AXIS_Y_MAIN_SIGNAL_EN & MLX_cfg ) == MLX_CFG_AXIS_Y_MAIN_SIGNAL_EN )
//    {
//        MLX_signalNum[MLX_1] = 1U;
//    }        
//    if ( ( MLX_CFG_AXIS_X_REDUNDANT_SIGNAL_EN & MLX_cfg ) == MLX_CFG_AXIS_X_REDUNDANT_SIGNAL_EN )
//    {
//        MLX_signalNum[MLX_0] = 2U;
//    }
//    if ( ( MLX_CFG_AXIS_Y_REDUNDANT_SIGNAL_EN & MLX_cfg ) == MLX_CFG_AXIS_Y_REDUNDANT_SIGNAL_EN )
//    {
//        MLX_signalNum[MLX_1] = 2U;
//    }
    
    MLX_AlphaMinA[MLX_0] = MLX_0_ALPHA_MIN_A;
    MLX_AlphaMaxA[MLX_0] = MLX_0_ALPHA_MAX_A;   
    MLX_AlphaMinB[MLX_0] = MLX_0_ALPHA_MIN_B;   
    MLX_AlphaMaxB[MLX_0] = MLX_0_ALPHA_MAX_B;   
    MLX_AlphaErrThA[MLX_0] = MLX_0_ALPHA_ERR_TH_A;
    MLX_AlphaErrThB[MLX_0] = MLX_0_ALPHA_ERR_TH_B;
    MLX_AlphaSafeDb[MLX_0] = MLX_0_ALPHA_SAFE_DB;
    MLX_BetaMinA[MLX_0] = MLX_0_BETA_MIN_A;
    MLX_BetaMaxA[MLX_0] = MLX_0_BETA_MAX_A;   
    MLX_BetaMinB[MLX_0] = MLX_0_BETA_MIN_B;   
    MLX_BetaMaxB[MLX_0] = MLX_0_BETA_MAX_B;   
    MLX_BetaErrThA[MLX_0] = MLX_0_BETA_ERR_TH_A;
    MLX_BetaErrThB[MLX_0] = MLX_0_BETA_ERR_TH_B;
    MLX_BetaSafeDb[MLX_0] = MLX_0_BETA_SAFE_DB;    

    MLX_AlphaMinA[MLX_1] = MLX_1_ALPHA_MIN_A;
    MLX_AlphaMaxA[MLX_1] = MLX_1_ALPHA_MAX_A;   
    MLX_AlphaMinB[MLX_1] = MLX_1_ALPHA_MIN_B;   
    MLX_AlphaMaxB[MLX_1] = MLX_1_ALPHA_MAX_B;   
    MLX_AlphaErrThA[MLX_1] = MLX_1_ALPHA_ERR_TH_A;
    MLX_AlphaErrThB[MLX_1] = MLX_1_ALPHA_ERR_TH_B;
    MLX_AlphaSafeDb[MLX_1] = MLX_1_ALPHA_SAFE_DB;
    MLX_BetaMinA[MLX_1] = MLX_1_BETA_MIN_A;
    MLX_BetaMaxA[MLX_1] = MLX_1_BETA_MAX_A;   
    MLX_BetaMinB[MLX_1] = MLX_1_BETA_MIN_B;   
    MLX_BetaMaxB[MLX_1] = MLX_1_BETA_MAX_B;   
    MLX_BetaErrThA[MLX_1] = MLX_1_BETA_ERR_TH_A;
    MLX_BetaErrThB[MLX_1] = MLX_1_BETA_ERR_TH_B;
    MLX_BetaSafeDb[MLX_1] = MLX_1_BETA_SAFE_DB;    
    
    MLX_cfg = MLX_REDUNDANCE_TYPE;
    
    MLX_paramToRAMInit();
    
    return;
}

//------------------------------------------------------------------------------
void MLX_StartupMain( void )
{
    static bool first_entry = true;
    
    if ( true == first_entry )
    {
        MLX_paramFromRAMUpdate();       
        first_entry = false;
    }
    return;
}
//------------------------------------------------------------------------------
void MLX_Main( uint8_t index )
{
    uint16_t auxLinearAlpha = 0xFFFFU;
    uint16_t auxLinearBeta = 0xFFFFU;
    
    //static uint8_t counter = 0U;
    //static bool onEnter[MLX_SENSOR_NUM] = { true, true };
    static uint8_t retValue[MLX_SENSOR_NUM] = { 0, 0 };
    //static uint8_t retryCnt[MLX_SENSOR_NUM] = { 0, 0 };
    //static uint8_t initCnt[MLX_SENSOR_NUM]  = { 0, 0 };

//    if ( 0x01U == ( MLX_signalNum[index] & 0x01U ) )
//    {
//        enableSignal |= MLX_ENABLE_SIGNAL_MAIN;
//        retValue |= MLX_SPI_TX_RX_OK_MAIN;
//    }
//    if ( 0x02U == ( MLX_signalNum[index] & 0x02U ) )
//    {
//        enableSignal |= MLX_ENABLE_SIGNAL_REDUNDANCE;
//        retValue |= MLX_SPI_TX_RX_OK_REDUNDANCE;
//    }    
      
//    if ( MLX_signalNum[index] > 0U )
//    {
//        enableSignal = MLX_ENABLE_SIGNAL_MAIN;
//        retValue = MLX_SPI_TX_RX_OK_MAIN;
//        if ( MLX_signalNum[index] > 1U )
//        {
//            enableSignal |= MLX_ENABLE_SIGNAL_REDUNDANCE; 
//            retValue |= MLX_SPI_TX_RX_OK_REDUNDANCE;
//        } 
//    }
  
    switch ( MLX_SPIStatus[index] ) 
    {
    case MLX_SPI_STATUS_WAIT:
        if ( MLX_SPI_AUTO_ON == MLX_SPIAutoEn[index] )
        {
            MLX_SPIStatus[index] = MLX_SPI_STATUS_INIT;
        }
        else
        {
            if ( MLX_initSPI[index] != 0u ) 
            { 
                MLX_SPIStatus[index] = MLX_SPI_STATUS_INIT; 
                MLX_initSPI[index] = 0U; 
            }
        }
	break;
    case MLX_SPI_STATUS_INIT:
        retValue[index] = MLX_SPIWrite( MLX_CMD_NOP, index );
        if ( MLX_SPI_TX_RX_OK == retValue[index] )
	{
            MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
            MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
        }
        MLX_SPIMsgReceived[index] = MLX_SPI_NO_MSG;
        MLX_SPIStatus[index] = MLX_SPI_STATUS_OFF;
        retValue[index] = 0U;                
	break;
    case MLX_SPI_STATUS_OFF:
        if ( MLX_EE_ACCESS_BUSY == MLX_EEAccessStatusGet( index ) ) 
        {
            if ( MLX_CMD_EE_READ == MLX_EECommand[index] ) 
            { 
                MLX_cmd[index] = MLX_CMD_EE_READ;
                MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_READ;
            } 
            else if ( MLX_CMD_EE_WRITE == MLX_EECommand[index] ) 
            { 
                MLX_cmd[index] = MLX_CMD_EE_WRITE; 
                MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_WRITE; 
            }
            else
            {
              // Do Nothing
            }
        } 
        else 
        {
	    if ( MLX_SPI_AUTO_ON == MLX_SPIAutoEn[index] )
            {
                MLX_angleMsgEn[index] = MLX_MARKER_1_ON;
                MLX_cmd[index] = MLX_CMD_GET1;
                MLX_SPIStatus[index] = MLX_SPI_STATUS_TX;
            }
            else
            {
                if ( MLX_angleMsgEn[index] != 0u )  //trigger del regular message in modalità 1
                {  
                    MLX_cmd[index] = MLX_CMD_GET1; 
                    MLX_SPIStatus[index] = MLX_SPI_STATUS_TX; 
                }
            } 
	}
	break;
    case MLX_SPI_STATUS_TX:
        retValue[index] = MLX_SPIWrite( MLX_cmd[index], index );
        if ( MLX_cmdPrev[index] == MLX_cmd[index] ) 
        { 
            if ( MLX_SPI_TX_RX_OK == retValue[index] )
            {
                MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
                MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
                MLX_alphaData[index] = ( MLX_recv[index].data[0U] & 0x00FFu ) + ( ( MLX_recv[index].data[1U] << 8U ) & 0xFF00u );
                MLX_betaData[index] =  ( MLX_recv[index].data[2U] & 0x00FFu ) + ( ( MLX_recv[index].data[3U] << 8U ) & 0xFF00u );
                Error_Reset( &MLX_error[index][MLX_ERROR_INDEX_TIME_OUT] );
            }
            else
            {
                Error_Update( &MLX_error[index][MLX_ERROR_INDEX_TIME_OUT], ( retValue[index] != ( MLX_SPI_TX_OK | MLX_SPI_RX_OK ) ), ERROR_RESET_WITH_CNT );
            }
            
            if ( ( MLX_MARKER_OFF == MLX_angleMsgEn[index] ) || (MLX_EEAccessStatusGet( index ) == MLX_EE_ACCESS_BUSY) ) 
            { 
                MLX_SPIStatus[index] = MLX_SPI_STATUS_EXIT; 
                MLX_angleMsgEn[index] = 0U; 
            }          
        }
        retValue[index] = 0U;
        MLX_cmdPrev[index] = MLX_cmd[index];
	break;
    case MLX_SPI_STATUS_EXIT:
        MLX_SPIStatus[index] = MLX_SPI_STATUS_INIT;
        MLX_SPIAutoEn[index] = MLX_SPI_AUTO_OFF;
	break;
    case MLX_SPI_STATUS_EE_READ:
        retValue[index] = MLX_SPIWrite( MLX_cmd[index], index );
        MLX_cmdPrev[index] = MLX_cmd[index];
        retValue[index] = 0U;
        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_READ_RX;
	break;
    case MLX_SPI_STATUS_EE_READ_RX:
        retValue[index] = MLX_SPIWrite( MLX_cmd[index], index );
        if ( MLX_SPI_TX_RX_OK == retValue[index] )
        {
            MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
            MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
        }
        MLX_EEAnswerSet( index );
        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_ACCESS_EXIT;
        retValue[index] = 0U;
//        MLX_SPITxRxStatus[index] = MLX_SPIWrite( MLX_cmd[index], index, MLX_ENABLE_SIGNAL_MAIN );
//        if ( MLX_SPI_TX_RX_OK_MAIN == MLX_SPITxRxStatus[index] )
//        {
//            MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
//            MLX_SPIMsgReceived[index][MLX_SIGNAL_MAIN] = MLX_SPI_NEW_MSG;
//        }
//        MLX_EEAnswerSet( index, MLX_SIGNAL_MAIN );
//        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_ACCESS_EXIT;

        
        break;
    case MLX_SPI_STATUS_EE_WRITE:
        retValue[index] = MLX_SPIWrite( MLX_cmd[index], index );
        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_READ_CHALLENGE;
        retValue[index] = 0U;
	break;
    case MLX_SPI_STATUS_EE_READ_CHALLENGE:
        retValue[index] = MLX_SPIWrite( MLX_CMD_EE_READ_CHALLENGE, index );
        MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
        if ( MLX_SPI_RX_OK == ( retValue[index] & MLX_SPI_RX_OK ) )
        {
            MLX_EEKeyEcho[index] = (uint16_t)MLX_recv[index].data[2U] + ( MLX_recv[index].data[3U] << 8U );
            MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;              
        }
        retValue[index] = 0U;
        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_CHALLENGE_ANSWER;
//        MLX_SPITxRxStatus[index] = MLX_SPIWrite( MLX_CMD_EE_READ_CHALLENGE, index );
//        if ( ( MLX_SPI_TX_OK | MLX_SPI_RX_OK ) == MLX_SPITxRxStatus[index] )
//        {
//            MLX_EEKeyEcho[index] = MLX_recv[index].data[2U] + ( MLX_recv[index].data[3U] << 8U );
//            MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;            
//            MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
//        }
//	MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_CHALLENGE_ANSWER;
	break;
    case MLX_SPI_STATUS_EE_CHALLENGE_ANSWER:
        retValue[index] = MLX_SPIWrite( MLX_CMD_EE_CHALLENGE_ANSWER, index );
        if ( MLX_SPI_TX_RX_OK == retValue[index] )
        {
            MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
            MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
        }
        MLX_waitCounter[index] = 0U;
        retValue[index] = 0U;
        MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_WRITE_STATUS;
 	break;
    case MLX_SPI_STATUS_EE_WRITE_STATUS:
        MLX_waitCounter[index]++;
        if ( MLX_waitCounter[index] > 10U )
        {
            MLX_waitCounter[index] = 10U;
            retValue[index] = MLX_SPIWrite( MLX_CMD_NOP, index );
            if ( MLX_SPI_TX_RX_OK == retValue[index] )
            {
                MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
                MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
            }
            MLX_EEAnswerSet( index );
            MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_ACCESS_EXIT;
            retValue[index] = 0U;
//            MLX_SPITxRxStatus[index] = MLX_SPIWrite( MLX_CMD_NOP, index );
//            if ( ( MLX_SPI_TX_OK | MLX_SPI_RX_OK ) == MLX_SPITxRxStatus[index] )
//            {
//                MLX_SPIMsgTransmitted[index] = MLX_SPI_NEW_MSG;
//                MLX_SPIMsgReceived[index] = MLX_SPI_NEW_MSG;
//            }
//	    MLX_EEAnswerSet( index );
//	    MLX_SPIStatus[index] = MLX_SPI_STATUS_EE_ACCESS_EXIT;
        }
        break;
    case MLX_SPI_STATUS_EE_ACCESS_EXIT:
	if ( MLX_EE_ACCESS_READY == MLX_EEAccessStatusGet( index ) ) 
        {
            MLX_SPIStatus[index] = MLX_SPI_STATUS_OFF; 
        }
	break;
    default:
        break;
    }

    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_INVALID_DATA_ALPHA], ( WRES_OK != MLX_alphaSignalEval( index ) ), ERROR_RESET_WITH_CNT );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_INVALID_DATA_BETA],  ( WRES_OK != MLX_betaSignalEval( index ) ),  ERROR_RESET_WITH_CNT );               
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_VBB], ( SUPPLY_VBB_PROT_ERR_NO != Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) ), ERROR_RESET_IMMEDIATE );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_VCC], ( SUPPLY_VCC_5VDC_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_ERROR ) ), ERROR_RESET_IMMEDIATE );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_VCC_EXT], ( SUPPLY_VCC_5VDC_EXT_ERR_NO != Supply_ErrorGet( SUPPLY_VCC_5VDC_EXT_ERROR ) ), ERROR_RESET_IMMEDIATE );
    
    auxLinearAlpha = MLX_AlphaLinearEval( index, 1000U );
    auxLinearBeta  = MLX_BetaLinearEval( index, 1000U );

    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_ALPHA_VALUE_HIGH], ( MLX_alphaValue[index] > MLX_AlphaErrThB[index] ), ERROR_RESET_WITH_CNT );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_ALPHA_VALUE_LOW],  ( MLX_alphaValue[index] < MLX_AlphaErrThA[index] ), ERROR_RESET_WITH_CNT );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_BETA_VALUE_HIGH],  ( MLX_betaValue[index]  > MLX_BetaErrThB[index] ),  ERROR_RESET_WITH_CNT );
    Error_Update( &MLX_error[index][MLX_ERROR_INDEX_BETA_VALUE_LOW],   ( MLX_betaValue[index]  < MLX_BetaErrThA[index] ),  ERROR_RESET_WITH_CNT );
    
    if ( MLX_REDUNDANCE_OFF == MLX_cfg )
    {
        Error_Reset( &MLX_error[index][MLX_ERROR_INDEX_ALPHA_REDUNDANCE] );
        Error_Reset( &MLX_error[index][MLX_ERROR_INDEX_BETA_REDUNDANCE] );
    }
    else
    {
        Error_Update( &MLX_error[index][MLX_ERROR_INDEX_ALPHA_REDUNDANCE], ( WRES_OK != MLX_AlphaRedundanceCheck( index ) ), ERROR_RESET_WITH_CNT );
        Error_Update( &MLX_error[index][MLX_ERROR_INDEX_BETA_REDUNDANCE],  ( WRES_OK != MLX_BetaRedundanceCheck( index ) ),  ERROR_RESET_WITH_CNT );
    }
    
    switch ( MLX_statusAlpha[index].value )
    {
    case MLX_STATUS_ERROR:
        if ( !( ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_TIME_OUT ) )           || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_ALPHA ) ) || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_BETA ) )  || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_VALUE_HIGH ) )   || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_VALUE_LOW ) )    || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_REDUNDANCE ) )   || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VBB ) )                || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC ) )                || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC_EXT ) )            || 
                ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_MEMORY ) )              ) )
        {
            if ( 1000U == auxLinearAlpha )
            {
                MLX_statusAlpha[index].value = MLX_STATUS_NORMAL;
            }
        }     
        MLX_LinearAlpha[index] = 0xFFFEU;
        break;
    case MLX_STATUS_NORMAL:
        if ( ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_TIME_OUT ) )           || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_ALPHA ) ) ||
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_BETA ) )  || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_VALUE_HIGH ) )   || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_VALUE_LOW ) )    || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_ALPHA_REDUNDANCE ) )   || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VBB ) )                || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC ) )                || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC_EXT ) )            || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_MEMORY ) )              )
        {
            MLX_statusAlpha[index].value = MLX_STATUS_ERROR;
            //MLX_Linear[index] = 0xFFFE;
        }
        if ( 0xFFFEU != auxLinearAlpha )
        {
            MLX_LinearAlpha[index] = auxLinearAlpha;
        }
        break;
    default:
        break;
    }

    switch ( MLX_statusBeta[index].value )
    {
    case MLX_STATUS_ERROR:
        if ( !( ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_TIME_OUT ) )            || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_BETA ) ) || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_VALUE_HIGH ) )   || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_VALUE_LOW ) )    || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_REDUNDANCE ) )   || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VBB ) )               || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC ) )               || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC_EXT ) )           || 
                  ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_MEMORY ) )             ) )
        {
            if ( 1000U == auxLinearBeta )
            {
                MLX_statusBeta[index].value = MLX_STATUS_NORMAL;
            }
        }     
        MLX_LinearBeta[index] = 0xFFFEU;
        break;
    case MLX_STATUS_NORMAL:
        if ( ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_TIME_OUT ) )          || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_INVALID_DATA_BETA ) ) ||
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_VALUE_HIGH ) )   || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_VALUE_LOW ) )    || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_BETA_REDUNDANCE ) )   || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VBB ) )               || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC ) )               || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_VCC_EXT ) )           || 
             ( true == MLX_ErrorGet( index, MLX_ERROR_INDEX_MEMORY ) )             )
        {
            MLX_statusBeta[index].value = MLX_STATUS_ERROR;
            //MLX_Linear[index] = 0xFFFE;
        }
        if ( 0xFFFEU != auxLinearBeta )
        {
            MLX_LinearBeta[index] = auxLinearBeta;
        }
        break;
    default:
        break;
    }    
    
    MLX_paramToRAMUpdate( index );

    return;
}

