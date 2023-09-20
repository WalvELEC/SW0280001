/*
 * eemap.h
 *
 *  Created on: 12 jun 2018
 *      Author: PoliD
 */

#ifndef _MEM_DATA_H
#define _MEM_DATA_H

//EEPROM + RAM
#define ADDR_MLX_ALPHA_MIN_A                          (  0U )
#define ADDR_MLX_ALPHA_MAX_A                          (  1U )
#define ADDR_MLX_ALPHA_MIN_B                          (  2U )
#define ADDR_MLX_ALPHA_MAX_B                          (  3U )
#define ADDR_MLX_BETA_MIN_A                           (  4U )
#define ADDR_MLX_BETA_MAX_A                           (  5U )
#define ADDR_MLX_BETA_MIN_B                           (  6U )
#define ADDR_MLX_BETA_MAX_B                           (  7U )
#define ADDR_SAEJ_DETENT_AXIS_X                       (  8UL ) 
#define ADDR_SAEJ_DETENT_AXIS_Y                       (  9U )
#define ADDR_SAEJ_DETENT_AXIS_W                       ( 10U )
#define ADDR_SAEJ_DETENT_AXIS_Z                       ( 11U )
#define ADDR_SAEJ_DETENT_AXIS_J                       ( 12U )
#define ADDR_SAEJ_DETENT_AXIS_Q                       ( 13U ) 
#define ADDR_SAEJ_DETENT_AXIS_V                       ( 14U ) 
#define ADDR_MLX_ALPHA_SAFE_DB                        ( 15U )
#define ADDR_MLX_ALPHA_ERR_TH_A                       ( 16u )
#define ADDR_MLX_ALPHA_ERR_TH_B                       ( 17u )
#define ADDR_MLX_BETA_SAFE_DB                         ( 18U )
#define ADDR_MLX_BETA_ERR_TH_A                        ( 19U )
#define ADDR_MLX_BETA_ERR_TH_B                        ( 20U )
#define ADDR_SAEJ_CFG_PUSHBUTTON                      ( 21U )
#define ADDR_SAEJ_CFG_AXES                            ( 22U )
#define ADDR_SAEJ_CFG_DETENT                          ( 23U )
#define ADDR_SAEJ_JOY_MSG_CFG                         ( 24U )
#define ADDR_MLX_CFG                                  ( 25U )
#define ADDR_ROLLER_CFG                               ( 26U )
#define ADDR_SAEJ_DEFAULT_ADDRESS                     ( 27U )
#define ADDR_APPLICATION_BAUD_RATE                    ( 28U )
#define ADDR_SAEJ_MSG_JOY_FREE_TIME_OUT               ( 29U )
#define ADDR_SAEJ_MSG_JOY_ON_CHANGE_BASE_TIME_OUT     ( 30U )
#define ADDR_SAEJ_MSG_JOY_ON_CHANGE_REFRESH_TIME_OUT  ( 31U )
#define ADDR_SAEJ_PUSH_BUTTON_INDEX_1_4               ( 32U )
#define ADDR_SAEJ_PUSH_BUTTON_INDEX_5_8               ( 33U )
#define ADDR_SAEJ_PUSH_BUTTON_INDEX_9_12              ( 34U )
#define ADDR_SAEJ_PUSH_BUTTON_INDEX_13_16             ( 35U )
#define ADDR_ROLLER_SGN_INDEX_1_4                     ( 36U )
#define ADDR_ROLLER_SGN_INDEX_5_8                     ( 37U )
#define ADDR_ROLLER_RED_INDEX_1_4                     ( 38U )
#define ADDR_ROLLER_RED_INDEX_5_8                     ( 39U )
#define ADDR_DIGITAL_CFG_INPUT_TYPE                   ( 40U )
#define ADDR_DIGITAL_CFG_WAIT_NEED                    ( 41U )
#define ADDR_DIGITAL_CFG_WAIT_VALUE                   ( 42U )
#define ADDR_SAEJ_ROLLER_INDEX                        ( 43U )
#define ADDR_SAEJ_FNR                                 ( 44U )
#define ADDR_WST_BUTTON_INDEX_1_8                     ( 45U ) 
#define ADDR_WST_BUTTON_INDEX_9_15                    ( 46U )
#define ADDR_DIGITAL_CFG_SETUP_1_2                    ( 47U )
#define ADDR_DIGITAL_CFG_SETUP_3_4                    ( 48U )
#define ADDR_DIGITAL_CFG_SETUP_5_6                    ( 49U )
#define ADDR_DIGITAL_CFG_SETUP_7_8                    ( 50U )
#define ADDR_DIGITAL_CFG_SETUP_9_10                   ( 51U )
#define ADDR_DIGITAL_CFG_SETUP_11_12                  ( 52U )
#define ADDR_DIGITAL_CFG_SETUP_13_14                  ( 53U )
#define ADDR_ROLLER_W_MIN_A                           ( 54UL )
#define ADDR_ROLLER_W_MAX_A                           ( 55UL )
#define ADDR_ROLLER_W_ERR_TH_A                        ( 56UL )
#define ADDR_ROLLER_W_MIN_B                           ( 57UL )
#define ADDR_ROLLER_W_MAX_B                           ( 58UL )
#define ADDR_ROLLER_W_ERR_TH_B                        ( 59UL )
#define ADDR_ROLLER_Z_MIN_A                           ( 60U )
#define ADDR_ROLLER_Z_MAX_A                           ( 61U )
#define ADDR_ROLLER_Z_ERR_TH_A                        ( 62U )
#define ADDR_ROLLER_Z_MIN_B                           ( 63U )
#define ADDR_ROLLER_Z_MAX_B                           ( 64U )
#define ADDR_ROLLER_Z_ERR_TH_B                        ( 65U )
#define ADDR_ROLLER_J_MIN_A                           ( 66U )
#define ADDR_ROLLER_J_MAX_A                           ( 67U )
#define ADDR_ROLLER_J_ERR_TH_A                        ( 68U )
#define ADDR_ROLLER_J_MIN_B                           ( 69U )
#define ADDR_ROLLER_J_MAX_B                           ( 70U )
#define ADDR_ROLLER_J_ERR_TH_B                        ( 71U )
#define ADDR_ROLLER_Q_MIN_A                           ( 72U )
#define ADDR_ROLLER_Q_MAX_A                           ( 73U )
#define ADDR_ROLLER_Q_ERR_TH_A                        ( 74U )
#define ADDR_ROLLER_Q_MIN_B                           ( 75U )
#define ADDR_ROLLER_Q_MAX_B                           ( 76U )
#define ADDR_ROLLER_Q_ERR_TH_B                        ( 77U )
#define ADDR_ROLLER_V_MIN_A                           ( 78U )
#define ADDR_ROLLER_V_MAX_A                           ( 79U )
#define ADDR_ROLLER_V_ERR_TH_A                        ( 80U )
#define ADDR_ROLLER_V_MIN_B                           ( 81U )
#define ADDR_ROLLER_V_MAX_B                           ( 82U )
#define ADDR_ROLLER_V_ERR_TH_B                        ( 83U )
//fuori checksum
#define ADDR_SERIAL_NUM_0_3                           ( 84U )
#define ADDR_SERIAL_NUM_4_7                           ( 85U )
#define ADDR_ASCII_DUT_CODE_0_3                       ( 86U )
#define ADDR_ASCII_DUT_CODE_4_7                       ( 87U )
#define ADDR_ASCII_DUT_CODE_8_11                      ( 88U )
#define ADDR_HW_ID_0_3                                ( 89U )  
#define ADDR_HW_ID_4_7                                ( 90U ) 
#define ADDR_ASCII_MANUFACTURER_CODE_0_3              ( 91U )
#define ADDR_ASCII_MANUFACTURER_CODE_4_7              ( 92U )
//RAM
#define ADDR_VBB_PROT_VALUE                           ( 93U )
#define ADDR_VCC_VALUE                                ( 94U )
#define ADDR_VCC_EXT_VALUE                            ( 95U )
#define ADDR_ROLLER_W_SGN                             ( 96U )
#define ADDR_ROLLER_Z_SGN                             ( 97U )
#define ADDR_ROLLER_J_SGN                             ( 98U )
#define ADDR_ROLLER_Q_SGN                             ( 99U )
#define ADDR_ROLLER_V_SGN                             ( 100U )
#define ADDR_ROLLER_W_RED                             ( 101U )
#define ADDR_ROLLER_Z_RED                             ( 102U )
#define ADDR_ROLLER_J_RED                             ( 103U )
#define ADDR_ROLLER_Q_RED                             ( 104U )
#define ADDR_ROLLER_V_RED                             ( 105U )
#define ADDR_MLX_0_ALPHA_SGN                          ( 106U )
#define ADDR_MLX_0_BETA_SGN                           ( 107U )
#define ADDR_MLX_1_ALPHA_SGN                          ( 108U )
#define ADDR_MLX_1_BETA_SGN                           ( 109U )
#define ADDR_DIGITAL_INPUT_MONITOR_1_16               ( 110U )
#define ADDR_DIGITAL_INPUT_MONITOR_17_32              ( 111U )
#define ADDR_SAEJ_ISOBUS_EN                           ( 112U )
#define MEM_SIZE_FIXED                                ( 113U )
#define MEM_FIXED_CHK_START_ADDR                      ( ADDR_SAEJ_DETENT_AXIS_W )
#define MEM_FIXED_CHK_END_ADDR                        ( ADDR_ROLLER_V_ERR_TH_B )
#define MEM_CALIB_START_ADDR                          ( ADDR_MLX_ALPHA_MIN_A )
#define MEM_CALIB_END_ADDR                            ( ADDR_SAEJ_DETENT_AXIS_Y )
#ifndef ISOBUS_DJW
#define MEM_SIZE                                      ( MEM_SIZE_FIXED )
#endif
//ISOBUS
#ifdef ISOBUS_DJW
#define ADDR_ISOBUS_CERT_0_3                          ( 130U )
#define ADDR_ISOBUS_CERT_4_7                          ( 131U )
#define ADDR_CFG_AUX_BOOL_0_1                         ( 132U )
#define ADDR_CFG_AUX_BOOL_2_3                         ( 133U )
#define ADDR_CFG_AUX_BOOL_4_5                         ( 134U )
#define ADDR_CFG_AUX_BOOL_6_7                         ( 135U )
#define ADDR_CFG_AUX_BOOL_8_9                         ( 136U )
#define ADDR_CFG_AUX_BOOL_10_11                       ( 137U )
#define ADDR_CFG_AUX_BOOL_12_13                       ( 138U )
#define ADDR_CFG_AUX_BOOL_14_15                       ( 139U )
#define ADDR_CFG_AUX_BOOL_16_17                       ( 140U )
#define ADDR_CFG_AUX_BOOL_18_19                       ( 141U )
#define ADDR_CFG_AUX_BOOL_20_21                       ( 142U )
#define ADDR_CFG_AUX_BOOL_22_23                       ( 143U )
#define ADDR_CFG_AUX_BOOL_24_25                       ( 144U )
#define ADDR_CFG_AUX_BOOL_26_27                       ( 145U )
#define ADDR_CFG_AUX_BOOL_28_29                       ( 146U )
#define ADDR_CFG_AUX_AN_0_1                           ( 147U )
#define ADDR_CFG_AUX_AN_2_3                           ( 148U )
#define ADDR_CFG_AUX_AN_4_5                           ( 149U )
#define ADDR_CFG_AUX_AN_6_7                           ( 150U )
#define ADDR_CFG_KEY_INDEX_0_3                        ( 151U )
#define ADDR_CFG_KEY_INDEX_4_7                        ( 152U )
#define ADDR_CFG_KEY_INDEX_8_11                       ( 153U )
#define ADDR_CFG_KEY_INDEX_12_15                      ( 154U )
#define ADDR_CFG_KEY_INDEX_16_19                      ( 155U )
#define ADDR_ISOBUS_KEY_CFG                           ( 156U )
#define MEM_ISOBUS_START_ADDR                         ( ADDR_ISOBUS_CERT_0_3 )
#define MEM_ISOBUS_END_ADDR                           ( ADDR_ISOBUS_KEY_CFG )
#define MEM_ISOBUS_CHK_START_ADDR                     ( ADDR_CFG_AUX_BOOL_0_1 )
#define MEM_ISOBUS_CHK_END_ADDR                       ( ADDR_ISOBUS_KEY_CFG )
#define MEM_SIZE                                      ( MEM_SIZE_FIXED + MEM_ISOBUS_END_ADDR - MEM_ISOBUS_START_ADDR + 1U )
#endif
    
//#define MEM_FIRST_ADDRESS           ( E2_KP )
//#define MEM_LAST_ADDRESS            ( E2_LIN_ADDR_OFFSET )
//#define MEM_SIZE                    ( ( ( MEM_LAST_ADDRESS -  MEM_FIRST_ADDRESS )/2u )+ 1u )
    
//fuori da MEM_SIZE    
#define ADDR_OUT_OF_PARAM_OFFSET      ( 120UL )
//#define ADDR_CHECKSUM                 ( ADDR_OUT_OF_PARAM_OFFSET + 2U )
#define ADDR_RESET_BASE               ( ADDR_OUT_OF_PARAM_OFFSET + 3U )
#define ADDR_RESET_00                 ( ADDR_RESET_BASE + 0u )
#define ADDR_RESET_01                 ( ADDR_RESET_BASE + 1u )
#define ADDR_RESET_02                 ( ADDR_RESET_BASE + 2u )
#define ADDR_RESET_03                 ( ADDR_RESET_BASE + 3u )
#define ADDR_RESET_04                 ( ADDR_RESET_BASE + 4u )
#define ADDR_OUT_OF_CHK_MEM_START     ( 120U )
#define ADDR_OUT_OF_CHK_MEM_END       ( 129U )
#define ADDR_CRC_OFFSET               ( 0x3F0U )
#define ADDR_BOOT_SET_DATA_01         ( 0x3F8U )
#define ADDR_BOOT_SET_DATA_02         ( 0x3FCU )
#define ADDR_CALIB_CHECKSUM           ( 0x3FDU )
#ifdef ISOBUS_DJW
#define ADDR_ISOBUS_CHECKSUM          ( 0x3FEU )
#define ADDR_ISOBUS_TEST_ENABLE       ( 0x500U )
#define ADDR_ISOBUS_VT_SYM_ENABLE     ( 0x502U )
#endif
#define ADDR_CHECKSUM                 ( 0x3FFU )
// Indirizzamento a dWord (32bit)

//extern void *EE_Param[MEM_SIZE];
   
#define MEM_DATA_NO_ERROR              (  0U )
#define MEM_DATA_ERROR_CHECKSUM        (  1U )
#define MEM_DATA_ERROR_CALIB_CHECKSUM  (  2U )
#define MEM_DATA_ERROR_WRITE           (  4U )
#define MEM_DATA_ERROR_READ            (  8U )
#ifdef ISOBUS_DJW
#define MEM_DATA_ERROR_ISOBUS_CHECKSUM ( 16U )
#endif
#define MEM_DATA_ERROR_INVALID_SA        (  32U )
#define MEM_DATA_ERROR_INVALID_BAUD      (  64U )
#define MEM_DATA_ERROR_INVALID_APP_PARAM ( 128U )

typedef enum MemData_paramSize_e
{
    MEM_SIZE_1_BYTE = 0x01,
    MEM_SIZE_2_BYTE = 0x02,
    MEM_SIZE_4_BYTE = 0x04,
} MemData_paramSize_t;

#define MEM_VALUE_MAX_1_BYTE    ( 0x000000FFU )
#define MEM_VALUE_MAX_2_BYTE    ( 0x0000FFFFU )
#define MEM_VALUE_MAX_4_BYTE    ( 0xFFFFFFFFU )

//extern uint32_t MemData_AssignRAMItem( uint32_t address, MemData_paramSize_t size, uint32_t *data );
extern uint8_t MemData_SetRAMItem( uint32_t address, uint32_t value );
extern uint8_t MemData_GetRAMItem( uint32_t address, uint32_t *data );
extern uint8_t MemData_SetEepromItem( uint32_t address, uint32_t value );
extern uint8_t MemData_GetEepromItem( uint32_t address, uint32_t *data );
extern uint8_t MemData_ErrorGet( void );
extern void MemData_ErrorSet( uint8_t errVal );
extern void MemData_Init( void );
extern void Get_CRC_EEProm( uint32_t StartOffset, uint32_t SectorSize, uint32_t * SectorCrc )  ;
#ifdef ISOBUS_DJW
extern uint32_t Isobus_ChecksumGet( void );
#endif

#endif