/*
 * roller.h
 *
 *  Created on: 22 jun 2018
 *      Author: PoliD
 */

#ifndef _ROLLER_H
#define _ROLLER_H

#define ROLLER_NUM_MAX   ( 5U )

#define ROLLER_W         ( 0U )
#define ROLLER_Z         ( 1U )
#define ROLLER_J         ( 2U )
#define ROLLER_Q         ( 3U )
#define ROLLER_V         ( 4U )

#define ROLLER_INPUT_NOT_CONNECTED    ( 0xFFU )

#define ROLLER_ERROR_INDEX_VALUE_HIGH         ( 0U )
#define ROLLER_ERROR_INDEX_VALUE_LOW          ( 1U )
#define ROLLER_ERROR_INDEX_REDUNDANCE         ( 2U )
#define ROLLER_ERROR_INDEX_VBB                ( 3U )
#define ROLLER_ERROR_INDEX_VCC                ( 4U )
#define ROLLER_ERROR_INDEX_VCC_EXT            ( 5U )
#define ROLLER_ERROR_INDEX_MEMORY             ( 6U )
#define ROLLER_ERROR_NUM                      ( 7U ) 

#define ROLLER_ERROR_VALUE_VALUE_HIGH         ( 0x000001UL )
#define ROLLER_ERROR_VALUE_VALUE_LOW          ( 0x000002UL )
#define ROLLER_ERROR_VALUE_REDUNDANCE         ( 0x000004UL )
#define ROLLER_ERROR_VALUE_VBB                ( 0x000008UL )
#define ROLLER_ERROR_VALUE_VCC                ( 0x000010UL )
#define ROLLER_ERROR_VALUE_VCC_EXT            ( 0x000020UL )
#define ROLLER_ERROR_VALUE_MEMORY             ( 0x000080UL )

#define ROLLER_ERROR_CNT_TH_OUT_OF_RANGE_US ( 100000UL )
#define ROLLER_ERROR_CNT_TH_REDUNDANCE_US   ( 100000UL )
#define ROLLER_ERROR_CNT_TH_VBB_US          ( 10000UL )       //questo errore lo faccio intervenire subito perché già nel modulo supply ho il counter di innesco
#define ROLLER_ERROR_CNT_TH_VCC_US          ( 10000UL )       //questo errore lo faccio intervenire subito perché già nel modulo supply ho il counter di innesco
#define ROLLER_ERROR_CNT_TH_VCC_EXT_US      ( 10000UL )       //questo errore lo faccio intervenire subito perché già nel modulo supply ho il counter di innesco
#define ROLLER_ERROR_CNT_TH_MEMORY_US       ( 10000UL )       //questo errore lo faccio intervenire subito

//#define ROLLER_SGN_MIN_A      ( 2250U ) 
//#define ROLLER_SGN_MAX_A      ( 750U )
//#define ROLLER_SGN_ERR_TH_A   ( 250U )
//#define ROLLER_SGN_MIN_B      ( 2750U )
//#define ROLLER_SGN_MAX_B      ( 4250U )
//#define ROLLER_SGN_ERR_TH_B   ( 4750U )
//#define ROLLER_RED_MIN_A      ( 2250U ) 
//#define ROLLER_RED_MAX_A      ( 750U )
//#define ROLLER_RED_ERR_TH_A   ( 250U )
//#define ROLLER_RED_MIN_B      ( 2750U )
//#define ROLLER_RED_MAX_B      ( 4250U )
//#define ROLLER_RED_ERR_TH_B   ( 4750U )

#define ROLLER_ALL_DISABLED                  ( 0x0000U )
#define ROLLER_W_SGN_EN                      ( 0x0001U )   
#define ROLLER_W_RED_EN                      ( 0x0002U )      
#define ROLLER_W_NOT_WAIT_NEUTRAL_AT_STARTUP ( 0x0004U )   
#define ROLLER_Z_SGN_EN                      ( 0x0008U )   
#define ROLLER_Z_RED_EN                      ( 0x0010U )   
#define ROLLER_Z_NOT_WAIT_NEUTRAL_AT_STARTUP ( 0x0020U )      
#define ROLLER_J_SGN_EN                      ( 0x0040U )      
#define ROLLER_J_RED_EN                      ( 0x0080U )      
#define ROLLER_J_NOT_WAIT_NEUTRAL_AT_STARTUP ( 0x0100U )         
#define ROLLER_Q_SGN_EN                      ( 0x0200U )      
#define ROLLER_Q_RED_EN                      ( 0x0400U )        
#define ROLLER_Q_NOT_WAIT_NEUTRAL_AT_STARTUP ( 0x0800U )         
#define ROLLER_V_SGN_EN                      ( 0x1000U )         
#define ROLLER_V_RED_EN                      ( 0x2000U )         
#define ROLLER_V_NOT_WAIT_NEUTRAL_AT_STARTUP ( 0x4000U )         
//#define ROLLER_CFG_DEFAULT                   ( ROLLER_Z_SGN_EN )

#define ROLLER_ALL_ERROR_SHOW_DISABLED        ( 0x00U )
#define ROLLER_W_ERROR_SHOW_EN                ( 0x01U )
#define ROLLER_Z_ERROR_SHOW_EN                ( 0x02U )
#define ROLLER_J_ERROR_SHOW_EN                ( 0x04U )
#define ROLLER_Q_ERROR_SHOW_EN                ( 0x08U )
#define ROLLER_V_ERROR_SHOW_EN                ( 0x10U )
   
extern uint16_t Roller_GetOutput( uint8_t rollerIndex );
extern bool Roller_ErrorGet( uint8_t rollerIndex, uint8_t errorIndex );
extern void Roller_Init( void );
extern void Roller_Main( void );

extern uint8_t Roller_signalMainIndex[ROLLER_NUM_MAX];
extern uint32_t Saej_cfgAxes;
extern uint16_t  Roller_MinAGet (uint8_t Roller);
extern uint16_t  Roller_MinBGet (uint8_t Roller);
extern uint16_t  Roller_MaxAGet (uint8_t Roller);
extern uint16_t  Roller_MaxBGet (uint8_t Roller);

#endif