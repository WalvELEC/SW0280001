/*
 * eemap.c
 *
 *  Created on: 12 jun 2018
 *      Author: PoliD
 */

#include "application.h"

wres_t Get_CRC_EEPromSector( uint32_t StartOffset, uint32_t SectorSize, uint32_t * SectorCrc ) ;
#define MEM_DATA_RESET_NUM  ( 5U )
#define MEM_DATA_CALIB_NUM  ( 10U )
#ifdef ISOBUS_DJW
#define MEM_DATA_TRACE_NUM  ( 11U )
#else
#define MEM_DATA_TRACE_NUM  ( 9U )
#endif

//#define MEM_FIXED_CRC_VALUE ( 0xC959209F )
//#define MEM_FIXED_CRC_VALUE ( 0xFB041680 )
#define MEM_FIXED_CRC_VALUE ( 0x23EC0D9B )

typedef struct 
{
    uint32_t address;
    MemData_paramSize_t size;
    bool     writable;
    bool     chksumeval;
    uint32_t min;
    uint32_t max;
} sMemData_Table;

//------------------------------------------------------------------------------
static uint32_t MemData_paramVal[MEM_SIZE];
static uint8_t MemData_error;
#ifdef ISOBUS_DJW
static uint32_t IsobusChecksumVal;
#endif

//------------------------------------------------------------------------------
static const sMemData_Table MEM_DATA_TABLE[MEM_SIZE] = 
{
    //indirizzo, dimensioni (0x01 = 8bit, 0x02 = 16bit, 0x04 = 32 bit), parametro scrivibile (true, false), parametro sotto checksum (true, false), valore minimo ammesso, valore massimo ammesso
    {ADDR_MLX_ALPHA_MIN_A,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_ALPHA_MAX_A,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_ALPHA_MIN_B,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_ALPHA_MAX_B,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_MIN_A,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_MAX_A,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_MIN_B,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_MAX_B,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_SAEJ_DETENT_AXIS_X,                      MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_Y,                      MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_W,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_Z,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_J,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_Q,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_DETENT_AXIS_V,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_MLX_ALPHA_SAFE_DB,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_ALPHA_ERR_TH_A,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_ALPHA_ERR_TH_B,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_SAFE_DB,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_BETA_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_SAEJ_CFG_PUSHBUTTON,                     MEM_SIZE_4_BYTE /*MEM_SIZE_2_BYTE*/, true, true,  0, MEM_VALUE_MAX_2_BYTE},  //ok
    {ADDR_SAEJ_CFG_AXES,                           MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_SAEJ_CFG_DETENT,                         MEM_SIZE_4_BYTE /*MEM_SIZE_2_BYTE*/, true, true,  0, MEM_VALUE_MAX_2_BYTE},  //ok
    {ADDR_SAEJ_JOY_MSG_CFG,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_MLX_CFG,                                 MEM_SIZE_4_BYTE /*MEM_SIZE_1_BYTE*/, true, true,  0, MEM_VALUE_MAX_1_BYTE},  //ok
    {ADDR_ROLLER_CFG,                              MEM_SIZE_4_BYTE /*MEM_SIZE_2_BYTE*/, true, true,  0, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_SAEJ_DEFAULT_ADDRESS,                    MEM_SIZE_4_BYTE /*MEM_SIZE_2_BYTE*/, true, true,  0, MEM_VALUE_MAX_2_BYTE},  //ok
    {ADDR_APPLICATION_BAUD_RATE,                   MEM_SIZE_4_BYTE /*MEM_SIZE_1_BYTE*/, true, true,  0, MEM_VALUE_MAX_1_BYTE},  //ok
    {ADDR_SAEJ_MSG_JOY_FREE_TIME_OUT,              MEM_SIZE_4_BYTE, true, true,  20000UL, MEM_VALUE_MAX_4_BYTE},  //ok
    {ADDR_SAEJ_MSG_JOY_ON_CHANGE_BASE_TIME_OUT,    MEM_SIZE_4_BYTE, true, true,  100000UL, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_MSG_JOY_ON_CHANGE_REFRESH_TIME_OUT, MEM_SIZE_4_BYTE, true, true,  20000UL, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_PUSH_BUTTON_INDEX_1_4,              MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_PUSH_BUTTON_INDEX_5_8,              MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_PUSH_BUTTON_INDEX_9_12,             MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_PUSH_BUTTON_INDEX_13_16,            MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ROLLER_SGN_INDEX_1_4,                    MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ROLLER_SGN_INDEX_5_8,                    MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ROLLER_RED_INDEX_1_4,                    MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ROLLER_RED_INDEX_5_8,                    MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_INPUT_TYPE,                  MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_WAIT_NEED,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_WAIT_VALUE,                  MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_ROLLER_INDEX,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_FNR,                                MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_WST_BUTTON_INDEX_1_8,                    MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_WST_BUTTON_INDEX_9_15,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_1_2,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_3_4,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_5_6,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_7_8,                   MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_9_10,                  MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_11_12,                 MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_CFG_SETUP_13_14,                 MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ROLLER_W_MIN_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_W_MAX_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_W_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_W_MIN_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},    
    {ADDR_ROLLER_W_MAX_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_W_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_MIN_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_MAX_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_MIN_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_MAX_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Z_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_MIN_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_MAX_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_MIN_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_MAX_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_J_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_MIN_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_MAX_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_MIN_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_MAX_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_Q_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_MIN_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_MAX_A,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_ERR_TH_A,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_MIN_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_MAX_B,                          MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_ROLLER_V_ERR_TH_B,                       MEM_SIZE_4_BYTE, true, true,  0, 0x13881388UL},
    {ADDR_SERIAL_NUM_0_3,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SERIAL_NUM_4_7,                          MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ASCII_DUT_CODE_0_3,                      MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ASCII_DUT_CODE_4_7,                      MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ASCII_DUT_CODE_8_11,                     MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_HW_ID_0_3,                               MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_HW_ID_4_7,                               MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ASCII_MANUFACTURER_CODE_0_3,             MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ASCII_MANUFACTURER_CODE_4_7,             MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},    
    {ADDR_VBB_PROT_VALUE,                          MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_VCC_VALUE,                               MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_VCC_EXT_VALUE,                           MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_W_SGN,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_Z_SGN,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_J_SGN,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_Q_SGN,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_V_SGN,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_W_RED,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_Z_RED,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_J_RED,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_Q_RED,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_ROLLER_V_RED,                            MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_MLX_0_ALPHA_SGN,                         MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_MLX_0_BETA_SGN,                          MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_MLX_1_ALPHA_SGN,                         MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_MLX_1_BETA_SGN,                          MEM_SIZE_2_BYTE, false, false, 0, MEM_VALUE_MAX_2_BYTE},
    {ADDR_DIGITAL_INPUT_MONITOR_1_16,              MEM_SIZE_4_BYTE, false, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_DIGITAL_INPUT_MONITOR_17_32,             MEM_SIZE_4_BYTE, false, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_SAEJ_ISOBUS_EN,                          MEM_SIZE_1_BYTE, false, false, 0, MEM_VALUE_MAX_1_BYTE},
#ifdef ISOBUS_DJW
    {ADDR_ISOBUS_CERT_0_3,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ISOBUS_CERT_4_7,                         MEM_SIZE_4_BYTE, true, false, 0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_0_1,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_2_3,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_4_5,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_6_7,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_8_9,                        MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_10_11,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_12_13,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_14_15,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_16_17,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_18_19,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_20_21,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_22_23,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_24_25,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_26_27,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_BOOL_28_29,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_AN_0_1,                          MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_AN_2_3,                          MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_AN_4_5,                          MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_AUX_AN_6_7,                          MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_KEY_INDEX_0_3,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_KEY_INDEX_4_7,                       MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_KEY_INDEX_8_11,                      MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_KEY_INDEX_12_15,                     MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_CFG_KEY_INDEX_16_19,                     MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
    {ADDR_ISOBUS_KEY_CFG,                          MEM_SIZE_4_BYTE, true, true,  0, MEM_VALUE_MAX_4_BYTE},
#endif    
};

static void MemData_EepromClearReset( void );
static uint32_t MemData_CalibChecksumEval( void );
static wres_t MemData_CalibEepromToRAMCopyAll( void );
static uint32_t MemData_EepromChecksumEval( void );
//static uint8_t MemData_EepromChecksumUpdate( uint32_t address, uint32_t newValue );
static wres_t MemData_RAMToEepromCopyAll( void );
static wres_t MemData_EepromToRAMCopyAll( void );
//static wres_t MemData_EepromToRAMCopySourceAddressAndBaudRate( void );
static wres_t MemData_TraceDataEepromToRAMCopyAll( void );
static bool MemData_EepromDoReset( void );

////------------------------------------------------------------------------------
//uint32_t MemData_AssignRAMItem( uint32_t address, MemData_paramSize_t size, uint32_t *data )
//{
//    uint8_t res;
//    uint8_t i;
//    uint32_t retVal;
//    
//    res = 1U; 
//    
//    for ( i=0U; i<MEM_SIZE; i++ )
//    {
//        if ( MEM_DATA_TABLE[i].address == address )
//        {
//            res = 0U;
//            break;
//        }
//    }
//    
//    if ( 0U == res )
//    {
//        if ( MEM_SIZE_4_BYTE == size )
//        {
//            retVal = MemData_paramVal[i];
//        }
//        else if ( MEM_SIZE_2_BYTE == size )
//        {
//            retVal = ( MemData_paramVal[i] & 0x0000FFFF );
//        }
//        else
//        {
//            retVal = ( MemData_paramVal[i] & 0x000000FF );
//        }
//        *data = retVal;
//    }
//    
//    return res;
//}
//------------------------------------------------------------------------------
static void MemData_EepromClearReset( void )
{
    uint8_t i;
    
    for ( i=0U; i<MEM_DATA_RESET_NUM; i++ )
    {
        Flash_ParameterUpdate( ADDR_RESET_BASE + i, 0x00UL );
    }
    
    return;
}

//------------------------------------------------------------------------------
static uint32_t MemData_CalibChecksumEval( void ) 
{
    uint32_t checksum;
    
    Get_CRC_EEProm( MEM_CALIB_START_ADDR << 2U, ( MEM_CALIB_END_ADDR - MEM_CALIB_START_ADDR + 1U ) << 2U, &checksum ); // Size in Byte
    
    return (uint32_t)( checksum );
}

//------------------------------------------------------------------------------
void Get_CRC_EEProm( uint32_t StartOffset, uint32_t SectorSize, uint32_t * SectorCrc )  
{
    uint32_t address;
    uint8_t *data = (uint8_t*)0x00;
    
    CRC_DRV_Deinit(INST_CRC1);
    CRC_DRV_Init(INST_CRC1, &crc1_InitConfig0);
    address = FEATURE_FLS_FLEX_RAM_START_ADDRESS + StartOffset; //0x14000000 + StartOffset
    data = (uint8_t*)address;
    CRC_DRV_WriteData(INST_CRC1, data ,SectorSize);
    *SectorCrc = CRC_DRV_GetCrcResult(INST_CRC1);
    CRC_DRV_Deinit(INST_CRC1);

    return;
}

//------------------------------------------------------------------------------
static uint32_t MemData_EepromChecksumEval( void ) 
{
    //uint8_t i;
    uint32_t checksum;
    
 
 /*   
    checksum = 0u;
           S32K_SysPin_SetOutputPins(GPIO_OUT_TP1);
   
    for ( i=0u; i<MEM_SIZE; i++ ) 
    {
        if ( true == MEM_DATA_TABLE[i].chksumeval ) 
        {
            checksum = checksum + (uint16_t)( Flash_ParameterGet( i ) );
        }
    }
    
    checksum = ~checksum;
   
          S32K_SysPin_ClearOutputPins(GPIO_OUT_TP1);
  */ 
    Get_CRC_EEProm( MEM_FIXED_CHK_START_ADDR << 2U,( MEM_FIXED_CHK_END_ADDR - MEM_FIXED_CHK_START_ADDR + 1U ) << 2U, &checksum ); // Size in Byte
    return (uint32_t)( checksum );
}

//------------------------------------------------------------------------------
#ifdef ISOBUS_DJW
static uint32_t MemData_IsobusChecksumEval( void ) 
{
    uint32_t checksum;
    
    Get_CRC_EEProm( MEM_ISOBUS_CHK_START_ADDR << 2U, ( MEM_ISOBUS_CHK_END_ADDR - MEM_ISOBUS_CHK_START_ADDR + 1U ) << 2U, &checksum ); // Size in Byte
    return (uint32_t)( checksum );
}
#endif
//------------------------------------------------------------------------------
/*static uint8_t MemData_EepromChecksumUpdate( uint32_t address, uint32_t newValue ) 
{

    uint8_t res;
    uint8_t i;
    uint16_t checksum;
        
    res = 1U;
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        if ( MEM_DATA_TABLE[i].address == address )
        {
            res = 0U;
            break;
        }
    }

    if ( 0U == res) 
    {
        if ( true == MEM_DATA_TABLE[i].chksumeval ) 
        {
            checksum = Flash_ParameterGet( ADDR_CHECKSUM );
            checksum = ~checksum;
            checksum = checksum - Flash_ParameterGet( MEM_DATA_TABLE[i].address );
            checksum = checksum + (uint16_t)( newValue );
            checksum = ~checksum;
            Flash_ParameterUpdate( ADDR_CHECKSUM, (uint32_t)( checksum ) );
        }
    }
    
    return res;
 
  return 0U;  
}*/

//------------------------------------------------------------------------------
static wres_t MemData_RAMToEepromCopyAll( void )
{
    uint8_t i;
    uint8_t aux;
    //uint32_t checksum;
    wres_t retVal;
    
    retVal = WRES_OK;
    aux = 0U;
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        if ( true == MEM_DATA_TABLE[i].chksumeval )
        {
            if ( Flash_ParameterUpdate( MEM_DATA_TABLE[i].address, MemData_paramVal[i] )!= 0u )
            {
                aux = aux + 1U;
            }
        }
    }
    
    MemData_EepromClearReset();
    if ( aux > 0U )
    {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if ( Flash_ParameterUpdate( ADDR_CHECKSUM, MEM_FIXED_CRC_VALUE /*MemData_EepromChecksumEval()*/ ) != 0u )
        {
            retVal = WRES_ERROR_INVALID_PARAMETER;
        }
#ifdef ISOBUS_DJW  
#ifndef ISOBUS_MEM_TEST_DEBUG        
        if ( Flash_ParameterUpdate( ADDR_ISOBUS_CHECKSUM, MemData_IsobusChecksumEval() ) )
        {
            retVal = WRES_ERROR_INVALID_PARAMETER;
        }        
#endif
#endif
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t MemData_EepromToRAMCopyAll( void )
{
    uint8_t i;
    uint8_t aux;
    
    wres_t retVal;
    
    retVal = WRES_OK;
    aux = 0U;
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        if ( true == MEM_DATA_TABLE[i].chksumeval )
        {      
            if ( Flash_ParameterRead( MEM_DATA_TABLE[i].address, &MemData_paramVal[i] ) != 0u )
            {
                aux = aux + 1U;
            }
        }
    }
    
    if ( aux > 0U ) {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
//static wres_t MemData_EepromToRAMCopySourceAddressAndBaudRate( void )
//{
//    uint8_t aux;
//    
//    wres_t retVal;
//    
//    retVal = WRES_OK;
//    aux = 0U;
//
//    if ( Flash_ParameterRead( MEM_DATA_TABLE[ADDR_SAEJ_DEFAULT_ADDRESS].address, &MemData_paramVal[ADDR_SAEJ_DEFAULT_ADDRESS] ) != 0u )
//    {
//        aux = aux + 1U;
//    }
//    if ( Flash_ParameterRead( MEM_DATA_TABLE[ADDR_APPLICATION_BAUD_RATE].address, &MemData_paramVal[ADDR_APPLICATION_BAUD_RATE] ) != 0u )
//    {
//        aux = aux + 1U;
//    }    
//    
//    if ( aux > 0U ) {
//        retVal = WRES_ERROR_INVALID_PARAMETER;
//    }
//    
//    return retVal;
//}

//------------------------------------------------------------------------------
static wres_t MemData_CalibEepromToRAMCopyAll( void )
{
  static const uint8_t MEM_DATA_CALIB_ADDR[MEM_DATA_CALIB_NUM] =
  {
    ADDR_MLX_ALPHA_MIN_A,
    ADDR_MLX_ALPHA_MAX_A,
    ADDR_MLX_ALPHA_MIN_B,
    ADDR_MLX_ALPHA_MAX_B,
    ADDR_MLX_BETA_MIN_A,
    ADDR_MLX_BETA_MAX_A,
    ADDR_MLX_BETA_MIN_B,
    ADDR_MLX_BETA_MAX_B,
    ADDR_SAEJ_DETENT_AXIS_X,
    ADDR_SAEJ_DETENT_AXIS_Y,
  };
    uint8_t i;
    uint8_t j;
    uint8_t aux;
    
    wres_t retVal;
    
    retVal = WRES_OK;
    aux = 0U;
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        for ( j=0U; j<MEM_DATA_CALIB_NUM; j++ )
        {
            if ( MEM_DATA_TABLE[i].address == MEM_DATA_CALIB_ADDR[j] )
            {
                if ( Flash_ParameterRead( MEM_DATA_TABLE[i].address, &MemData_paramVal[i] ) != 0u )
                {
                    aux = aux + 1U;
                }
            }
        }
    }

    if ( aux > 0U ) {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static wres_t MemData_TraceDataEepromToRAMCopyAll( void )
{
  static const uint8_t MEM_DATA_TRACE_ADDR[MEM_DATA_TRACE_NUM] = 
  {
    ADDR_SERIAL_NUM_0_3,
    ADDR_SERIAL_NUM_4_7,
    ADDR_ASCII_DUT_CODE_0_3,
    ADDR_ASCII_DUT_CODE_4_7,
    ADDR_ASCII_DUT_CODE_8_11,
    ADDR_HW_ID_0_3,
    ADDR_HW_ID_4_7,
    ADDR_ASCII_MANUFACTURER_CODE_0_3,
    ADDR_ASCII_MANUFACTURER_CODE_4_7,
#ifdef ISOBUS_DJW
    ADDR_ISOBUS_CERT_0_3,
    ADDR_ISOBUS_CERT_4_7,
#endif
  };
    uint8_t i;
    uint8_t j;
    uint8_t aux;
    
    wres_t retVal;
    
    retVal = WRES_OK;
    aux = 0U;
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        for ( j=0U; j<MEM_DATA_TRACE_NUM; j++ )
        {
            if ( MEM_DATA_TABLE[i].address == MEM_DATA_TRACE_ADDR[j] )
            {
                if ( Flash_ParameterRead( MEM_DATA_TABLE[i].address, &MemData_paramVal[i] ) != 0u )
                {
                    aux = aux + 1U;
                }
            }
        }
    }

    if ( aux > 0U ) {
        retVal = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static bool MemData_EepromDoReset( void ) 
{
    static const uint32_t MEM_DATA_RESET[MEM_DATA_RESET_NUM] =  { 0x52UL, 0x45UL, 0x53UL, 0x45UL, 0x54UL, };
    
    uint8_t i;
    bool retVal;
    uint32_t data;
    uint8_t aux;

    retVal = true;
    aux = 0U;
    for ( i = 0u; i<MEM_DATA_RESET_NUM; i++ ) 
    {
        if ( Flash_ParameterRead( ADDR_RESET_BASE + i, &data ) != 0u )
        {
            aux = aux + 1U;
        }
        if ( MEM_DATA_RESET[i] != data ) 
        {
            retVal = false;
        }
        if ( aux > 0U )
        {
            retVal = false;
        }
    }
    return retVal;
}   
   
//------------------------------------------------------------------------------
uint8_t MemData_SetRAMItem( uint32_t address, uint32_t value )
{
    uint8_t res;
    uint8_t i;
  
    res = 1U; 
    
    for ( i=0U; i<MEM_SIZE; i++ )
    {
        if ( MEM_DATA_TABLE[i].address == address )
        {
            res = 0U;
            break;
        }
    }
    
    if ( 0U == res )
    {
        if ( ( value >= MEM_DATA_TABLE[i].min ) && ( value <= MEM_DATA_TABLE[i].max )  )
        {
            MemData_paramVal[i] = value;
        }
        else
        {
            res = 2U;
        }
    }
    
#ifdef ISOBUS_DJW
    if ( ADDR_ISOBUS_TEST_ENABLE == address )
    {
        res = IsobusTestEnableSet( value );
    }
    
    if ( ADDR_ISOBUS_VT_SYM_ENABLE == address )
    {
        res = IsobusVTSymEnableSet( value );
    }
#endif

    return res;
}

//------------------------------------------------------------------------------
uint8_t MemData_GetRAMItem( uint32_t address, uint32_t *data )
{
    uint8_t res;
    uint8_t i;
  
    res = 1U; 

    for ( i=0U; i<MEM_SIZE; i++ )
    {
        if ( MEM_DATA_TABLE[i].address == address )
        {
            res = 0U;
            break;
        }
    }
    
    if ( 0U == res )
    {
        *data =  MemData_paramVal[i];
    }


#ifdef ISOBUS_DJW
    if ( ADDR_ISOBUS_TEST_ENABLE == address )
    {
        if ( true == IsobusTestEnableGet() )
        {
            *data = 0xA55A55AAUL;
        }
        else
        {
            *data = 0UL;
        }
        res = 0U;
    }
    if ( ADDR_ISOBUS_VT_SYM_ENABLE == address )
    {
        if ( 0x01u == ( IsobusVTSymEnableGet() & 0x0001u ) )
        {
            *data = 0xA55A5500UL + (uint8_t)( IsobusVTSymEnableGet() >> 8u );
        }
        else
        {
            *data = 0UL;
        }
        res = 0;
    }
#endif
    
    return res;
}

//------------------------------------------------------------------------------
uint8_t MemData_SetEepromItem( uint32_t address, uint32_t value )
{
    uint8_t res;
    uint8_t i;
  
    res = 1U; 
    
    if ( ( ( address >= ADDR_OUT_OF_CHK_MEM_START ) && ( address <= ADDR_OUT_OF_CHK_MEM_END ) ) || ( address >= ADDR_CRC_OFFSET ) )
    {
        res = Flash_ParameterUpdate( address, value );
    }
    else
    {
        for ( i=0U; i<MEM_SIZE; i++ )
        {
            if ( MEM_DATA_TABLE[i].address == address )
            {
                res = 0U;
                break;
            }
        }
    
        if ( 0U == res )
        {
            if ( true == MEM_DATA_TABLE[i].writable )
            {
                if ( ( value >= MEM_DATA_TABLE[i].min ) && ( value <= MEM_DATA_TABLE[i].max ) )
                {
                    res = Flash_ParameterUpdate( MEM_DATA_TABLE[i].address, value );                  
                    if ( 0U == res )
                    {
                        res = MemData_SetRAMItem( address, value );
                    }
                }
                else
                {
                    res = 2U;
                }
            }
            else
            {
                res = 1U;
            }
        }
    }
    
    return res;
}

//------------------------------------------------------------------------------
uint8_t MemData_GetEepromItem( uint32_t address, uint32_t *data )
{
    uint8_t res;
    
    res = Flash_ParameterRead( address, data );
    
    return res;
}

//------------------------------------------------------------------------------
uint8_t MemData_ErrorGet( void )
{
    return (uint8_t)MemData_error;
}

//------------------------------------------------------------------------------
void MemData_ErrorSet( uint8_t errVal )
{
    MemData_error = MemData_error | errVal;
    
    return;
}
#ifdef ISOBUS_DJW
//------------------------------------------------------------------------------
uint32_t Isobus_ChecksumGet( void )
{
    return IsobusChecksumVal;
}

#endif
//------------------------------------------------------------------------------
void MemData_Init( void ) 
{
    uint8_t retVal;
    uint32_t tmpCheckSum;
    
    retVal = MEM_DATA_NO_ERROR;
    if ( true == MemData_EepromDoReset() ) 
    {
        if ( WRES_OK != MemData_RAMToEepromCopyAll() )
        {
            retVal |= MEM_DATA_ERROR_WRITE;
        }
    } 
    else 
    {
        if ( Flash_ParameterRead( ADDR_CHECKSUM, &tmpCheckSum ) != 0u )
        {
            retVal |= MEM_DATA_ERROR_CHECKSUM;
        }
        else
        {
            if ( tmpCheckSum == MemData_EepromChecksumEval() ) 
            {
#ifdef ISOBUS_DJW
                IsobusChecksumVal = 0xFFFFFFFFUL;
#ifndef ISOBUS_MEM_TEST_DEBUG              
                if ( Flash_ParameterRead( ADDR_ISOBUS_CHECKSUM, &tmpCheckSum ) )
                {
                    retVal |= MEM_DATA_ERROR_ISOBUS_CHECKSUM;
                }
                else
                {  
                    if ( tmpCheckSum != MemData_IsobusChecksumEval() ) 
                    {
                        retVal |= MEM_DATA_ERROR_ISOBUS_CHECKSUM;
                    }
                    else
                    {
                        IsobusChecksumVal = tmpCheckSum;
                    }
                }
#endif                
#endif             
            }
            else 
            {
                retVal |= MEM_DATA_ERROR_CHECKSUM;
            }                  

            if ( MEM_DATA_NO_ERROR == retVal )
            {
              
              if ( WRES_OK != MemData_EepromToRAMCopyAll() )
                {
                    retVal |= MEM_DATA_ERROR_READ;
                }
            } 
//            else
//            {
//                if ( WRES_OK != MemData_EepromToRAMCopySourceAddressAndBaudRate() )
//                {
//                    retVal |= MEM_DATA_ERROR_READ;
//                }
//            }
        }

    }
    
    if ( Flash_ParameterRead( ADDR_CALIB_CHECKSUM, &tmpCheckSum ) != 0u )
    {
        retVal |= MEM_DATA_ERROR_CALIB_CHECKSUM;
    }
    else
    {
        if ( tmpCheckSum == MemData_CalibChecksumEval() ) 
        {
            if ( WRES_OK != MemData_CalibEepromToRAMCopyAll() )
            {
                retVal |= MEM_DATA_ERROR_READ;
            }
        }
        else
        {
            retVal |= MEM_DATA_ERROR_CALIB_CHECKSUM;  
        }
    }
    
    if ( WRES_OK != MemData_TraceDataEepromToRAMCopyAll() )
    {
        //retVal |= MEM_DATA_ERROR_READ;
    }
    
    MemData_error = retVal;
    
    return;
}
    
