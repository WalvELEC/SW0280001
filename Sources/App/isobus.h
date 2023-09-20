/*
 * isobus.h
 *
 *  Created on: 06 sep 2018
 *      Author: PoliD
 */

#ifndef _ISOBUS_H
#define _ISOBUS_H

#ifdef ISOBUS_DJW

#include "string.h"
#include "pool.h"

#define ISOBUS_VT_TO_ECU_MSG_ID    ( 0x1CE6FF00UL )
#define ISOBUS_ECU_TO_VT_MSG_ID    ( 0x1CE7FF00UL )
#define ISOBUS_WORKING_SET_MSG_ID  ( 0x1CFE0D00UL )
#define ISOBUS_CERT_INFO_ID        ( 0x18FD4200UL )

#define ISOBUS_VT_TO_ECU_PGN       ( 0x0000E600UL )
#define ISOBUS_ECU_TO_VT_PGN       ( 0x0000E700UL )

#define ISOBUS_VT_VERSION_3        ( 3U )

// COMM_VT_COMMAND
#define ISOBUS_GET_SOFTKEY_NUM     ( 0xC2U )  //Get Number of Softkeys
#define ISOBUS_GET_TEXT_FONT       ( 0xC3U )  //Get Text Font
#define ISOBUS_GET_HARDWARE        ( 0xC7U )  //Get Hardware
#define ISOBUS_GET_MEMORY          ( 0xC0U )  //Get Memory
#define ISOBUS_STORE_OP_VER        ( 0xD0U )  //Store OP Version (on non-vol VT mem)
#define ISOBUS_LOAD_OP_VER         ( 0xD1U )  //Load OP Version (from non-vol VT mem)
#define ISOBUS_DELETE_OP_VER       ( 0xD2U )  //Delete OP Version (from non-vol VT mem)
#define ISOBUS_GET_VERSION         ( 0xDFU )  //Get Version
#define ISOBUS_VT_STATUS           ( 0xFEU )  //VT Status Message
#define ISOBUS_WS_MAINTEN          ( 0xFFU )  //Working Set Maintenance Message
#define ISOBUS_OP_TRANSFER         ( 0x11U )  //Object Pool Transfer
#define ISOBUS_OP_END              ( 0x12U )  //End of Object Pool
#define ISOBUS_OP_DEL              ( 0xB2U )  //Delete Object Pool
#define ISOBUS_CHILD_LOC_CHANGE    ( 0xA5U )  //Change Child Location
#define ISOBUS_CHILD_POS_CHANGE    ( 0xB4U )  //Change Child Position
#define ISOBUS_BACK_COLOR_CHANGE   ( 0xA7U )  //Change Background Color
#define ISOBUS_ATTRIB_CHANGE       ( 0xAFU )  //Change Attribute
#define ISOBUS_AUX_2_ASSIGN        ( 0x24U )  //Auxiliary Assignment Type 2
#define ISOBUS_PREF_ASSIGN         ( 0x22U )  //Preferred Assignment
#define ISOBUS_AUX_IN_2_STATUS_MSG ( 0x26U )  //Auxiliary Input Type 2 Status Message
#define ISOBUS_AUX_IN_2_STATUS_EN  ( 0x25U )  //Auxiliary Input Status Type 2 Enable
#define ISOBUS_HIDE_SHOW_OBJECT    ( 0xA0U )  //Hide/Show Object
#define ISOBUS_CHANGE_NUM_VALUE    ( 0xA8U )  //Change Numeric Value

//Net status
#define ISOBUS_NET_OFF_STATUS           (  0U )
#define ISOBUS_NET_STARTUP_STATUS       (  1U )
#define ISOBUS_NET_WS_INIT_STATUS       (  2U )
#define ISOBUS_NET_WS_START_STATUS      (  3U )
#define ISOBUS_NET_OK_STATUS            (  4U )
#define ISOBUS_NET_ERROR_STATUS         (  5U )
#define ISOBUS_NET_CLAIM_ANSWER_STATUS  ( 11U )

//VT status
#define ISOBUS_VT_STATUS_OFF              ( 1U )
#define ISOBUS_VT_STATUS_INIT             ( 2U )
#define ISOBUS_VT_STATUS_GET_SOFTKEY_NUM  ( 3U )
#define ISOBUS_VT_STATUS_GET_HARDWARE     ( 4U )
#define ISOBUS_VT_STATUS_GET_TEXT_FONT    ( 5U )
#define ISOBUS_VT_STATUS_GET_MEMORY       ( 6U )
#define ISOBUS_VT_STATUS_LOAD_OP_VER      ( 7U )
#define ISOBUS_VT_STATUS_OP_UPLOAD        ( 8U )
#define ISOBUS_VT_STATUS_OP_END           ( 9U )
#define ISOBUS_VT_STATUS_STORE_OP_VER     ( 10U )
#define ISOBUS_VT_STATUS_ON               ( 11U )
#define ISOBUS_VT_STATUS_ERROR            ( 255U )

//---
#define VT_STATUS_LEARN_MODE_ACTIVE     ( 0x40 )


#define FONT_8_8_EN     ( 0x01U )
#define FONT_8_12_EN    ( 0x02U )
#define FONT_12_16_EN   ( 0x04U )
#define FONT_16_16_EN   ( 0x08U )
#define FONT_16_24_EN   ( 0x10U )
#define FONT_24_32_EN   ( 0x20U )
#define FONT_32_32_EN   ( 0x40U )

#define FONT_32_48_EN   ( 0x01U )
#define FONT_48_64_EN   ( 0x02U )
#define FONT_64_64_EN   ( 0x04U )
#define FONT_64_96_EN   ( 0x08U )
#define FONT_96_128_EN  ( 0x10U )
#define FONT_128_128_EN ( 0x20U )
#define FONT_128_192_EN ( 0x40U )      

#define FONT_6_8_SIZE     ( 0U )
#define FONT_8_8_SIZE     ( 1U )
#define FONT_8_12_SIZE    ( 2U )
#define FONT_12_16_SIZE   ( 3U )
#define FONT_16_16_SIZE   ( 4U )
#define FONT_16_24_SIZE   ( 5U )
#define FONT_24_32_SIZE   ( 6U )
#define FONT_32_32_SIZE   ( 7U )
#define FONT_32_48_SIZE   ( 8U )
#define FONT_48_64_SIZE   ( 9U )
#define FONT_64_64_SIZE   ( 10U )
#define FONT_64_96_SIZE   ( 11U )
#define FONT_96_128_SIZE  ( 12U )
#define FONT_128_128_SIZE ( 13U )
#define FONT_128_192_SIZE ( 14U )

#define ISOBUS_NEUTRAL       ( 0x7D7FU )
#define ISOBUS_MIN           ( 0x0000U )
#define ISOBUS_MAX           ( 0xFAFFU )
#define ISOBUS_ERROR         ( 0xFEB0U )
#define ISOBUS_LATCH_FWD     ( 0xFB00U )
#define ISOBUS_LATCH_BWD     ( 0xFB01U )

#define ISOBUS_MSG_RX_COUNTER_TH     (  5U )
#define ISOBUS_MSG_LED_ON_COUNTER_TH ( 20U ) 

//------------------------------------------------------------------------------
extern const uint8_t Isobus_FunctionalitiesBuff[13U];
extern uint16_t Isobus_FunctionalitiesGet( void );
extern void Isobus_WorkingSetInitMsgTx( void );
extern void Isobus_UpdateMsgSourceAddress( uint8_t sourceAddr );
extern void Isobus_UpdateMsgVtAddress( void );
extern uint8_t Isobus_CertificationGet( uint8_t byteNum ); 
extern void Isobus_MsgRxCounterUpdate();
extern uint8_t IsobusTestEnableSet( uint32_t value );
extern bool IsobusTestEnableGet( void );
extern uint8_t IsobusVTSymEnableSet( uint32_t value );
extern uint16_t IsobusVTSymEnableGet( void );
extern void Isobus_Init( void );
extern void Isobus_Main( void );

#endif

#endif
