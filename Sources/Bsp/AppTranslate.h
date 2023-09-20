/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Can.h
 * Brief description
 */
#ifndef APP_TRANSLATE_H_
#define APP_TRANSLATE_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
//#include "Cpu.h"
//#include "wtypes.h"
//#include "SafetyCheckList.h"
//#include "S32K_ObjectConfig.h"
//
//#include "S32K_flexSpi.h"
//#include "S32K_LpSpi.h"
//#include "S32K_SysPin.h"
//#include "S32K_Timer.h"
//#include "stdbool.h"
#include "S32K_Driver.h"
#include "djwConfig.h"
/**************************************************************************************************
 *    CAN.H
 **************************************************************************************************/

#define CAN_ID_11BIT   ( FLEXCAN_MSG_ID_STD )
#define CAN_ID_29BIT   ( FLEXCAN_MSG_ID_EXT )

#define CAN_MSG_RX     ( 0U )
#define CAN_MSG_TX     ( 1U )

#define CAN_NO_MSG     ( 0U )
#define CAN_NEW_MSG    ( 1U )

#define CAN_MODULE_0       CAN_JOY_OUT

#define CAN_TX_ALWAYS    ( 0U )
#define CAN_TX_ON_CHANGE ( 1U )

#define CAN_MSG_RX_NO    ( 0U )
#define CAN_MSG_RX_YES   ( 1U )

#define CAN_MSG_IS_NOT_RTR  ( 0U )
#define CAN_MSG_IS_RTR      ( 1U )

#define CAN_MSG_TX_NO    ( 0U )
#define CAN_MSG_TX_YES   ( 1U )

#define CAN_SOURCE_ADDRESS_CHECK_YES  true
#define CAN_SOURCE_ADDRESS_CHECK_NO   false

#define CAN_EVENT_RX     ( 0U )
#define CAN_EVENT_TX     ( 1U )

#define CAN_MSG_BUFFER_NUM     ( 24U )
#define CAN_RX_BUFFER_NUM_MAX  ( 24U )  //48
#define CAN_BUFFER_NUM_MAX     ( 48U )

typedef struct S_TX_RX_CAN_MSG
{
    uint8_t NewMsg;       //indica se un nuovo messaggio è stato ricevuto (se Rx) o se è pronto per essere trasmesso (se Tx)
    flexcan_msgbuff_id_type_t TypeID;       //specifica identificativo a 11 o 29 bit
    uint8_t Error;
    uint32_t ID;
    uint8_t SA;
    uint8_t DA;
    uint8_t DLC;
    uint8_t IsRTR;        //indica se remote frame
    uint8_t RxTx;         //indica se messaggio in ricezione o trasmissione
    uint16_t TimeOut;
    //se Rx: se TimeOut = 0U, non c'è limite di tempo alla ricezione del msg, altrimenti si ha Errore se tra un msg e l'altro passa un tempo maggiore di TimeOut
    //se Tx: se TimeOut = 0U, il messaggio viene Trasmesso solo a eventi definiti dall'utente, altrimenti viene trasmesso con periodo TimeOut
    uint16_t TimeCnt;
    uint8_t Index;
    uint8_t Data[8U];
} s_TxRxCANMsg;

typedef struct s_storeRxCANMsg_e
{
    uint8_t NewMsg;       //indica se un nuovo messaggio è stato ricevuto (se Rx) o se è pronto per essere trasmesso (se Tx)
    uint32_t ID;
    uint8_t SA;
    uint8_t DA;
    uint8_t DLC;
    uint8_t Data[8U];
} s_storeRxCANMsg_t;
 //  8.  FUNCTION PROTOTYPES
extern void CAN_StoredMsgUpdate( uint8_t index, uint8_t msgIndex, uint32_t newID, bool sourceAddressCheck );
extern uint8_t CAN_StoredMsgRx( uint8_t index, uint8_t *msgIndex );
extern void CAN_StoredMsgClear( uint8_t index, uint8_t msgIndex );
extern uint32_t CAN_StoredMsgGetID( uint8_t index, uint8_t msgIndex );
extern void CAN_MsgUpdateTimeOut( s_TxRxCANMsg *msgCan, uint16_t timeOut );

extern uint8_t CAN_StoredMsgGetData( uint8_t index, uint8_t msgIndex, uint8_t dataNum );
extern s_TxRxCANMsg CAN_StoredMsgGet( uint8_t index, uint8_t msgIndex );

// ------------------ DONE -------------------------------------------
extern void CAN_ModuleEnable( s_S32K_can_id moduleIndex, s_bspcan_bitRate baudRate_bps ); // Change bitrate OK

// ---------------------------------- RX --------------------------------------------------------
// ATTIVA QUEL BUFFER , se TX o RX, se RX specifica tutti ID che puo ricevere ID+ SA/DA  - OK1/2
extern void CAN_MsgEnable( s_TxRxCANMsg *msgCan, s_S32K_can_id moduleIndex, uint8_t msgIndex, uint8_t typeRxTx, flexcan_msgbuff_id_type_t typeID, uint32_t msgID, bool sourceAddressCheck, uint8_t length, uint16_t timeOut, uint8_t remoteReqEn );
// Aggiorna ID HW con il SA buffer da 32 ID 0xFFFFFFFF

// se sourceAddressCheck = true aggiorno ID e Mask in ricezione -OK
extern void CAN_MsgUpdateID( s_TxRxCANMsg *msgCan, uint32_t id, flexcan_msgbuff_id_type_t typeID, bool sourceAddressCheck );
extern void CAN_MsgUpdateIDGeneral( uint32_t id, flexcan_msgbuff_id_type_t typeID );

// ----------------------------------------------------------------------------------------------

// ---------------------------------- TX --------------------------------------------------------
// Tx tutto il messaggio con ID
extern wres_t CAN_MsgTx( s_TxRxCANMsg *msgCan, bool checkBuffIndex, uint8_t buffIndex );
extern void CAN_MsgUpdateData( s_TxRxCANMsg *msgCan, uint8_t *bufferData, uint8_t txEn ); // Aggiorna i dati
extern void CAN_MsgUpdateRxFilter ( void );

// ----------------------------------------------------------------------------------------------

// ---------------------------------- EMPTY -----------------------------------------------------
extern void CAN_SetInitBit( s_S32K_can_id moduleIndex );
extern void CAN_ResetInitBit( s_S32K_can_id moduleIndex );
extern void CAN_Main_K144( void );
extern void CAN_Init_K144( uint8_t source_addr );
/**************************************************************************************************
 *  8.  FLASH.H
 **************************************************************************************************/
//#define FLASH_ACCESS_REQUEST_OK     ( 1U )
//#define FLASH_ACCESS_REQUEST_FAIL   ( 0U )
//
//#define FLASH_MSG_TX_DM16_STATUS_OK ( 2U )
//#define FLASH_MSG_TX_DM15_STATUS_OK ( 1U )
//#define FLASH_MSG_TX_STATUS_NO      ( 0U )
//
//#define FLASH_MSG_BYTE_NUM          ( 8U )
//
//#define FLASH_INTERNAL_ACCESS_REQUEST_OFF   ( 0U )
//#define FLASH_INTERNAL_ACCESS_REQUEST_WRITE ( 1U )
//#define FLASH_INTERNAL_ACCESS_REQUEST_READ  ( 2U )
//
//#define FLASH_ERROR_OK       ( 0U )
//#define FLASH_ERROR_WRITE    ( 1U )
//#define FLASH_ERROR_READ     ( 2U )
//#define FLASH_ERROR_READ     ( 2U )

/**************************************************************************************************
 *  8.  BSP.H
 **************************************************************************************************/
#define LED_OUT_0      0U
#define LED_OUT_1      1U
#define LED_OUT_2      2U
#define LED_OUT_3      3U
#define LED_OUT_NUM    4U

#define LED_DUTY_OFF   ( 0xFFU )
#define LED_DUTY_25    ( 0x00U )
#define LED_DUTY_50    ( 0x01U )
#define LED_DUTY_75    ( 0x02U )   
   
extern void BSP_OutLedUpdate( uint8_t ledCmd, uint8_t ledDuty );
extern void BSP_OutLedCheck( uint8_t ledIndex, uint8_t ledCmd, bool ledEn, bool ledInternalEn, uint8_t ledErrorIndex );

extern uint16_t ADC_GetValue_mV( uint8_t channel );
extern uint8_t LED_DutyGetValue( uint8_t ledCh );
extern bool LED_ErrorIsActive( uint8_t ledCh );
extern void LED_DutyUpdate( void );
extern void LED_DutyInit( void );

extern bool CanTxRequestGet(void);
extern void CanTxRequestClear(void);
#endif /* APP_TRANSLATE_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
