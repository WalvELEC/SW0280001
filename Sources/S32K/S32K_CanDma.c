/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Can.c
 * History:
 *  - 30 gen 2020 1.0 Davoli - First release
 * S32K_can.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_CanDma.h"
#include "flexcan_hw_access.h"
#include "string.h"
#include "S32K_SysPin.h"
static flexcan_state_t s_flexcanState[CAN_INSTANCE_COUNT];
extern wres_t S32K_Can_GetRxStatus( s_S32K_can_id ObjId, wres_t *Status);
static uint32_t S32K_Can_SetInitBitRate( s_bspcan_bitRate BitRate );
//extern wres_t S32K_Can_GetBitRate( s_S32K_can_id ObjId, s_bspcan_bitRate *BitRate);
//extern wres_t S32K_Can_MsgTx( s_S32K_can_id ObjId,s_S32K_can_msg_t *msgCan );
/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/

//can_buff_config_t TxbuffCfg =  {
//    .enableFD = false,
//    .enableBRS = false,
//    .fdPadding = 0U,
//    .idType = CAN_MSG_ID_STD,
//    .isRemote = false
//};
//
//can_buff_config_t buffCfgSTD =  {
//    .enableFD = false,
//    .enableBRS = false,
//    .fdPadding = 0U,
//    .idType = CAN_MSG_ID_STD,
//    .isRemote = false
//};
//
//can_buff_config_t buffCfgEXT =  {
//    .enableFD = false,
//    .enableBRS = false,
//    .fdPadding = 0U,
//    .idType = CAN_MSG_ID_EXT,
//    .isRemote = false
//};

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/

#define RX_MAILBOX_STD   0UL  //Buffer 0 Receive Standard Id Messages
#define RX_MAILBOX_EXT   1UL  //Buffer 1 Receive Extended Id Messages

#define FIRST_RX_MAILBOX 0UL  //From Buffer 2 to All buffer defined transmit Standard and Extended Id Messages
#define FIRST_TX_MAILBOX 10UL  //From Buffer 2 to All buffer defined transmit Standard and Extended Id Messages
#define MAILBOX_NUM      16UL  //From Buffer 2 to All buffer defined transmit Standard and Extended Id Messages


#define CAN_MSG_ALL_ID_PASS 0u        //Receive Filter : Pass all ID
#define CAN_MSG_ALL_ID_PASS_TYPE 3u   //Receive Mask   : Pass all ID

#ifndef CAN_OBJ_NUM
#define S32K_CAN_OBJ_NUM 0L
#else
#define S32K_CAN_OBJ_NUM CAN_OBJ_NUM
#endif

#if CAN_OBJ_NUM > 3u
    #error Gli Oggetti Can possono essere al massimo 3
#endif

#ifdef CAN_NUM_RX_FILTER_0
#if CAN_NUM_RX_FILTER_0 > 0u
#define S32K_CAN_NUM_RX_FILTER_0 CAN_NUM_RX_FILTER_0
#else
#define S32K_CAN_NUM_RX_FILTER_0 0UL
#endif
#if CAN_RX_QUEUE_SIZE_0 > 0u
#define S32K_CAN_RX_QUEUE_SIZE_0 CAN_RX_QUEUE_SIZE_0
#else
#define S32K_CAN_RX_QUEUE_SIZE_0 1UL
#endif
#endif

#ifdef CAN_NUM_RX_FILTER_1
#if CAN_NUM_RX_FILTER_1 > 0u
#define S32K_CAN_NUM_RX_FILTER_1 CAN_NUM_RX_FILTER_1
#else
#define S32K_CAN_NUM_RX_FILTER_1 0UL
#endif
#if CAN_RX_QUEUE_SIZE_1 > 0u
#define S32K_CAN_RX_QUEUE_SIZE_1 CAN_RX_QUEUE_SIZE_1
#else
#define S32K_CAN_RX_QUEUE_SIZE_1 1UL
#endif
#endif

#ifdef CAN_NUM_RX_FILTER_2
#if CAN_NUM_RX_FILTER_2 > 0u
#define S32K_CAN_NUM_RX_FILTER_2 CAN_NUM_RX_FILTER_2
#else
#define S32K_CAN_NUM_RX_FILTER_2 0UL
#endif
#if CAN_RX_QUEUE_SIZE_2 > 0u
#define S32K_CAN_RX_QUEUE_SIZE_2 CAN_RX_QUEUE_SIZE_2
#else
#define S32K_CAN_RX_QUEUE_SIZE_2 1UL
#endif
#endif

#define S32K_CAN_RX_FILTER_NUM_0 S32K_CAN_NUM_RX_FILTER_0 + 1u
#define S32K_CAN_RX_FILTER_NUM_1 S32K_CAN_NUM_RX_FILTER_1 + 1u
#define S32K_CAN_RX_FILTER_NUM_2 S32K_CAN_NUM_RX_FILTER_2 + 1u

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/


/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct {
    uint32_t RxId;
    uint32_t RxMask;
    s_can_msg_id_type_t IdType;
}s_S32K_can_RxFilter;

typedef struct {
    //------------------ Local -----------------------------------
    bool open;
    //------------------ Config ----------------------------------
    uint8_t instanceNum;
    //------------------ Input -----------------------------------
//    uint32_t maxBuffNum;
    Queue_t rxQueue;
    flexcan_user_config_t UserConfig;
    flexcan_state_t *State;
    //------------------ Output ----------------------------------
    flexcan_operation_modes_t Mode;
    s_bspcan_bitRate ActualbitRate;
    wres_t Rxres;
    //flexcan_msgbuff_t *p_RxMsg;
    //can_message_t *p_RxMsgEXT;
    flexcan_msgbuff_t TxMessage;
    flexcan_msgbuff_t RxMessage;
    s_S32K_can_RxFilter *p_RxFilter;
    flexcan_id_table_t  *p_Id_table;
//    uint8_t RxFilerNum;
//    bool CmdSampleTxMsg;
//    bool CmdSampleRxMsg;
//    s_SafetySample_Status StatusSampleTxMsg;
//    s_SafetySample_Status StatusSampleRxMsg;
//    s_S32K_can_msg_t *pRxSampleMsg;
//    s_S32K_can_msg_t *pTxSampleMsg;
} s_S32K_canObj;

typedef struct {
    s_S32K_canObj * CanObjPtr;
    s_S32K_can_msg_t * p_Q_rxBuffer;
    uint8_t QrxBuffsize;
    //flexcan_msgbuff_t *p_RxMsg;
    //can_message_t *p_RxMsgEXT;
    s_S32K_can_RxFilter *p_RxFilter;
    flexcan_id_table_t  *p_Id_table;
//    uint8_t RxFilerNum;
//    s_S32K_can_msg_t * pRxSampleMsg;
//    s_S32K_can_msg_t * pTxSampleMsg;
}s_S32K_can_Config;

#define RX_MAILBOX_NUM FIRST_TX_MAILBOX  //From Buffer 2 to All buffer defined transmit Standard and Extended Id Messages

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/
//static can_callback_t CanCallback(uint8_t instance, flexcan_event_type_t eventType, flexcan_state_t *flexcanState);
static void CanCallback(uint8_t instance, flexcan_event_type_t eventType, uint32_t buffIdx,flexcan_state_t *flexcanState);
//static uint8_t FindFreeTxBuffer(uint8_t instance );
static uint8_t FindFreeTxBuffer(uint8_t instance, uint32_t id, flexcan_state_t *state, bool checkBuffIndex, uint8_t buffIndex );

//static bool CAN_MsgRxCheckID(uint8_t ObjId, const can_message_t *msgCan, s_can_msg_id_type_t IdType );
/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
static flexcan_time_segment_t ActlBitrate;
#if CAN_INSTANCE_COUNT == 1u
  static uint8_t Instance_To_CanObj[CAN_INSTANCE_COUNT]={0xFFu};
#endif
#if CAN_INSTANCE_COUNT == 2u
  static uint8_t Instance_To_CanObj[CAN_INSTANCE_COUNT]={0xFFu,0xFFu};
#endif
#if CAN_INSTANCE_COUNT == 3u
  static uint8_t Instance_To_CanObj[CAN_INSTANCE_COUNT]={0xFFu,0xFFu,0xFFu};
#endif
#if S32K_CAN_OBJ_NUM == 0u
s_S32K_can_Config Can_Obj_Config[1]={
    {NULL, NULL, 0u,NULL, NULL,NULL,0u, NULL,NULL},
};
#endif

#if S32K_CAN_OBJ_NUM == 1u
can_message_t recvMsgSTD0;
can_message_t recvMsgEXT0;

s_S32K_canObj CanObj_0;
s_S32K_can_msg_t Q_CanRxBuff_0[S32K_CAN_RX_QUEUE_SIZE_0];
s_S32K_can_RxFilter RxFilter_0[S32K_CAN_RX_FILTER_NUM_0];

s_S32K_can_msg_t CanSampleRxBuffer_0;
s_S32K_can_msg_t CanSampleTxBuffer_0;

s_S32K_can_Config Can_Obj_Config[S32K_CAN_OBJ_NUM]={
    {&CanObj_0, &Q_CanRxBuff_0[0], S32K_CAN_RX_QUEUE_SIZE_0,&recvMsgSTD0,&recvMsgEXT0, &RxFilter_0[0],S32K_CAN_RX_FILTER_NUM_0,&CanSampleRxBuffer_0,&CanSampleTxBuffer_0},
};
#endif

#if S32K_CAN_OBJ_NUM == 2u
//flexcan_msgbuff_t recvMsg0;
//flexcan_msgbuff_t recvMsg1;

static s_S32K_canObj CanObj_0;
static s_S32K_canObj CanObj_1;
static s_S32K_can_msg_t Q_CanRxBuff_0[S32K_CAN_RX_QUEUE_SIZE_0];
static s_S32K_can_msg_t Q_CanRxBuff_1[S32K_CAN_RX_QUEUE_SIZE_1];

static s_S32K_can_RxFilter RxFilter_0[RX_MAILBOX_NUM];
static s_S32K_can_RxFilter RxFilter_1[RX_MAILBOX_NUM];
static flexcan_id_table_t IdTable_0[RX_MAILBOX_NUM];
static flexcan_id_table_t IdTable_1[RX_MAILBOX_NUM];

//s_S32K_can_msg_t CanSampleRxBuffer_0;
//s_S32K_can_msg_t CanSampleTxBuffer_0;
//s_S32K_can_msg_t CanSampleRxBuffer_1;
//s_S32K_can_msg_t CanSampleTxBuffer_1;

static s_S32K_can_Config Can_Obj_Config[S32K_CAN_OBJ_NUM]={
    {&CanObj_0, &Q_CanRxBuff_0[0], S32K_CAN_RX_QUEUE_SIZE_0, &RxFilter_0[0], &IdTable_0[0]},
    {&CanObj_1, &Q_CanRxBuff_1[0], S32K_CAN_RX_QUEUE_SIZE_1, &RxFilter_1[0], &IdTable_1[0]},
};
#endif

#if S32K_CAN_OBJ_NUM == 3u
can_message_t recvMsgSTD0;
can_message_t recvMsgEXT0;
can_message_t recvMsgSTD1;
can_message_t recvMsgEXT1;
can_message_t recvMsgSTD2;
can_message_t recvMsgEXT2;

s_S32K_canObj CanObj_0;
s_S32K_canObj CanObj_1;
s_S32K_canObj CanObj_2;
s_S32K_can_msg_t Q_CanRxBuff_0[S32K_CAN_RX_QUEUE_SIZE_0];
s_S32K_can_msg_t Q_CanRxBuff_1[S32K_CAN_RX_QUEUE_SIZE_1];
s_S32K_can_msg_t Q_CanRxBuff_2[S32K_CAN_RX_QUEUE_SIZE_2];

s_S32K_can_RxFilter RxFilter_0[S32K_CAN_RX_FILTER_NUM_0];
s_S32K_can_RxFilter RxFilter_1[S32K_CAN_RX_FILTER_NUM_1];
s_S32K_can_RxFilter RxFilter_2[S32K_CAN_RX_FILTER_NUM_2];

s_S32K_can_msg_t CanSampleRxBuffer_0;
s_S32K_can_msg_t CanSampleTxBuffer_0;
s_S32K_can_msg_t CanSampleRxBuffer_1;
s_S32K_can_msg_t CanSampleTxBuffer_1;
s_S32K_can_msg_t CanSampleRxBuffer_2;
s_S32K_can_msg_t CanSampleTxBuffer_2;

s_S32K_can_Config Can_Obj_Config[S32K_CAN_OBJ_NUM]={
    {&CanObj_0, &Q_CanRxBuff_0[0], S32K_CAN_RX_QUEUE_SIZE_0,&recvMsgSTD0,&recvMsgEXT0, &RxFilter_0[0],S32K_CAN_RX_FILTER_NUM_0,&CanSampleRxBuffer_0,&CanSampleTxBuffer_0},
    {&CanObj_1, &Q_CanRxBuff_1[0], S32K_CAN_RX_QUEUE_SIZE_1,&recvMsgSTD1,&recvMsgEXT1, &RxFilter_1[0],S32K_CAN_RX_FILTER_NUM_1,&CanSampleRxBuffer_1,&CanSampleTxBuffer_1},
    {&CanObj_2, &Q_CanRxBuff_2[0], S32K_CAN_RX_QUEUE_SIZE_2,&recvMsgSTD2,&recvMsgEXT2, &RxFilter_2[0],S32K_CAN_RX_FILTER_NUM_2,&CanSampleRxBuffer_2,&CanSampleTxBuffer_2},
};

#endif

#if S32K_CAN_OBJ_NUM > 3u
    #error Gli Oggetti Can possono essere al massimo 3
  #endif
/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/

// -----------------------------------------------------------------------------------------------------
static uint8_t FindFreeTxBuffer(uint8_t instance, uint32_t id, flexcan_state_t *state, bool checkBuffIndex, uint8_t buffIndex )
{
    uint8_t n;
    uint8_t Res;
    uint8_t startIndex;
    status_t status[MAILBOX_NUM-FIRST_TX_MAILBOX];
    
    Res = (uint8_t)MAILBOX_NUM;
    startIndex = FIRST_TX_MAILBOX;
    for( n = (uint8_t)FIRST_TX_MAILBOX; n <MAILBOX_NUM; n++ )
    {
        status[n-FIRST_TX_MAILBOX] = FLEXCAN_DRV_GetTransferStatus(instance,n);
        flexcan_msgbuff_t *mb_m = state->mbs[n].mb_message;
        if ( id == mb_m->msgId )
        {
            startIndex = n+1U;
        }    
    }
    if ( true == checkBuffIndex )
    {
        Res = buffIndex+FIRST_TX_MAILBOX;
        if ( STATUS_SUCCESS != status[buffIndex] )
        {
            Res = (uint8_t)MAILBOX_NUM;
        }
    }
    else
    {
        for( n=startIndex; n<MAILBOX_NUM; n++ )
        {
            if ( STATUS_SUCCESS == status[n-FIRST_TX_MAILBOX] )
            {
                Res = n;
                break;
            }
        }
    }
    
//    
//    for( n = (uint8_t)FIRST_TX_MAILBOX; n <MAILBOX_NUM; n ++ )
//    //for( n = (uint8_t)( MAILBOX_NUM - 1U ); n >=FIRST_TX_MAILBOX; n -- )
//    {
//        status = FLEXCAN_DRV_GetTransferStatus(instance,n);
//        if ( STATUS_SUCCESS != status )
//        {
//            flexcan_msgbuff_t *mb_m = state->mbs[n].mb_message;
//            if ( id == mb_m->msgId )
//            {
//                Res = n+1U;
//            }
//        }
//        else
//        {
//	    Res = n;
//	}
//    }
    return Res;
}

//// -----------------------------------------------------------------------------------------------------
//static bool CAN_MsgRxCheckID(uint8_t ObjId, const can_message_t *msgCan, s_can_msg_id_type_t IdType )
//{   
//    // Find if RxId is une of My RxId
//    uint8_t i;
//    s_S32K_canObj *Config;
//    s_can_msg_id_type_t FilterIdType;
//    Config = Can_Obj_Config[ObjId].CanObjPtr;
//
//    for( i = 0; i < RX_MAILBOX_NUM; i ++ )
//    {
//	FilterIdType = Config->p_RxFilter[i].IdType ;
//	if ( MY_CAN_MSG_ID_ALL == Config->p_RxFilter[i].IdType )
//	{
//	    FilterIdType = IdType;
//	}
//	if (( (msgCan->id & Config->p_RxFilter[i].RxMask) == Config->p_RxFilter[i].RxId )&&(IdType == FilterIdType ))
//	{
//	    return true;
//	}
//    }
//    return false;
//}

// -----------------------------------------------------------------------------------------------------
static void CanCallback(uint8_t instance, flexcan_event_type_t eventType, uint32_t buffIdx,flexcan_state_t *flexcanState)
{
//    (void)flexcanState;
//    (void)instance;
    uint8_t ObjId;
    switch(eventType)
    {
   case FLEXCAN_EVENT_RXFIFO_COMPLETE:
    ObjId = Instance_To_CanObj[instance];
    if (S32K_CAN_OBJ_NUM > ObjId)
    {
	    s_S32K_canObj *Config;
	    Config = Can_Obj_Config[ObjId].CanObjPtr;
            s_S32K_can_msg_t msgCan;
            msgCan.id  = Config->RxMessage.msgId;
            msgCan.dlc = Config->RxMessage.dataLen;
            msgCan.isRemote =false;
            msgCan.TypeID =(flexcan_msgbuff_id_type_t)((Config->RxMessage.cs & CAN_CS_IDE_MASK)>>CAN_CS_IDE_SHIFT);
            memcpy( &msgCan.data, Config->RxMessage.data, 8U );
            if (WRES_OK == Queue_Enqueue(&Config->rxQueue,&msgCan))
            {
                Config->Rxres = WRES_OK;
            }
            else
            {
                Config->Rxres = WRES_BUFFER_FULL;
            }
            FLEXCAN_DRV_RxFifo(INST_CANCOM1,&Config->RxMessage);
    }
     	break;
   case FLEXCAN_EVENT_RX_COMPLETE:
	break;
    case FLEXCAN_EVENT_TX_COMPLETE:
	break;
    default:
	break;
    }
    
 
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_SetSwRxFilter(s_S32K_can_id ObjId, uint8_t FilterIndex, uint32_t PassId, s_S32K_can_FilterMask Mask, s_can_msg_id_type_t IdType )
{
    wres_t res;
    uint32_t auxMask;

    if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
    {
        s_S32K_canObj *Config;
	Config = Can_Obj_Config[ObjId].CanObjPtr;
	if (FilterIndex < RX_MAILBOX_NUM)
	{
#ifdef RX_FIFO_FORMAT_A   
                auxMask = (uint32_t)( Mask );              
                if ( true == Config->p_Id_table[FilterIndex].isExtendedFrame )
                {
                    auxMask = ( auxMask << 1U ) &  0x3FFFFFFFUL;                  
                    auxMask = auxMask | 0x40000000UL;
                }
                else
                {
                    auxMask = ( auxMask << 19U ) &  0x3FF80000UL;
                }
                if ( true == Config->p_Id_table[FilterIndex].isRemoteFrame )
                {
                    auxMask = auxMask | 0x80000000UL;
                }
#endif
#ifdef RX_FIFO_FORMAT_B
                uint32_t val1 = 0u;
                uint32_t val2 = 0u;
                if ( true == Config->p_Id_table[FilterIndex].isRemoteFrame )
                {
                    val1 = 0x80000000UL;
                    val2 = 0x00008000UL;
                }
                if ( true == Config->p_Id_table[FilterIndex].isExtendedFrame )
                {
                    val1 = val1 | 0x40000000UL;
                    val2 = val2 | 0x00004000UL;
                    val1 = val1 + ( ( ( Mask & 0x0000FFFFUL ) & 0x3FFFU ) << 16U );
                    val2 = val2 + ( ( ( ( Mask >> 16U ) & 0x0000FFFFUL ) & 0x3FFFU ) << 16U );
                }
                else
                {
                    val1 = val1 + ( ( Mask & 0x0000FFFFUL ) & 0x3FFFU );
                    val2 = val2 + ( ( ( Mask >> 16U ) & 0x0000FFFFUL ) & 0x3FFFU );
                }
                auxMask = val1 | val2;
#endif
#ifdef RX_FIFO_FORMAT_C                
                auxMask = (uint32_t)( Mask ); //mask scritta con un id per ogni byte
#endif
	    Config->p_RxFilter[FilterIndex].RxId=PassId & (uint32_t)Mask;
	    Config->p_RxFilter[FilterIndex].RxMask = (uint32_t)auxMask;
	    Config->p_RxFilter[FilterIndex].IdType=(s_can_msg_id_type_t)IdType;
	    if (0x00u == (uint32_t)Mask)
            {
              Config->p_RxFilter[FilterIndex].RxId=0u;
            } //All Pass
	    res = WRES_OK;
	}
	else
	{
	    res = WRES_ERROR_INVALID_PARAMETER;
	}
    }    
    else
    {
	res = WRES_ERROR_INVALID_MODE;
    }
    return res;
}
extern wres_t S32K_Can_UpdateHwRxFilter(s_S32K_can_id ObjId)
{
  wres_t res;
  uint8_t n;
  if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
  {
    s_S32K_canObj *Config;
    Config = Can_Obj_Config[ObjId].CanObjPtr;

    //for(n=(uint8_t)FIRST_RX_MAILBOX;n<RX_MAILBOX_NUM;n++)
    for(n=(uint8_t)FIRST_RX_MAILBOX;n<MAILBOX_NUM;n++)
    {
//    Config->p_RxFilter[n].RxId = 0;
//    Config->p_RxFilter[n].RxMask = 0;
//    Config->p_RxFilter[n].IdType = MY_CAN_MSG_ID_STD;
    //Config->p_Id_table[n].isExtendedFrame = ((uint8_t)Config->p_RxFilter[n].IdType!=0u); //originale
      Config->p_Id_table[n].isExtendedFrame = true;
    //Config->p_Id_table[n].idFilter = &Config->p_RxFilter[n].RxId;
    Config->p_Id_table[n].isRemoteFrame=false;
    }
     
    // Configure RX FIFO ID filter table elements based on filter table defined above
    MY_FLEXCAN_DRV_ConfigRxFifo(Config->instanceNum, FLEXCAN_RX_FIFO_ID_FORMAT_A, Config->p_Id_table);

    //for(n=(uint8_t)FIRST_RX_MAILBOX;n<RX_MAILBOX_NUM;n++)
    for(n=(uint8_t)FIRST_RX_MAILBOX;n<MAILBOX_NUM;n++)
    {
      FLEXCAN_DRV_SetRxIndividualMask(Config->instanceNum, FLEXCAN_MSG_ID_EXT, n, Config->p_RxFilter[n].RxMask);
    }

    
        
    res = WRES_OK;  
  }
  else
  {
      res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;
}


extern wres_t S32K_CanDma_StopRx( s_S32K_can_id ObjId)
{
  wres_t res = WRES_ERROR;
  if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
  {
    s_S32K_canObj *Config;
    Config = Can_Obj_Config[ObjId].CanObjPtr;
    res = WRES_ERROR;
    if (0x02u == Config->instanceNum) 
    {
      INT_SYS_DisableIRQ(CAN2_ORed_0_15_MB_IRQn);
      res = WRES_OK;
    }  
    if (0x01u == Config->instanceNum) 
    {
      INT_SYS_DisableIRQ(CAN1_ORed_0_15_MB_IRQn);
      res = WRES_OK;
    }  
    if (0x00u == Config->instanceNum) 
    {
      INT_SYS_DisableIRQ(CAN0_ORed_0_15_MB_IRQn);
      res = WRES_OK;
    }  
  }
  else
  {
    res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;
}
extern wres_t S32K_CanDma_StartRx( s_S32K_can_id ObjId)
{
  wres_t res = WRES_ERROR;
  if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
  {
    s_S32K_canObj *Config;
    Config = Can_Obj_Config[ObjId].CanObjPtr;

    if ( STATUS_SUCCESS == FLEXCAN_DRV_RxFifo(Config->instanceNum ,&Config->RxMessage))
    {
      if (0x02u == Config->instanceNum) 
      {
        INT_SYS_EnableIRQ(CAN2_ORed_0_15_MB_IRQn);
      }  
      if (0x01u == Config->instanceNum) 
      {
        INT_SYS_EnableIRQ(CAN1_ORed_0_15_MB_IRQn);
      }  
      if (0x00u == Config->instanceNum) 
      {
        INT_SYS_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
      }  
      res = WRES_OK;
    }
    else
    {
      res = WRES_ERROR;
    }
  }
  else
  {
    res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;
}
// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_CanDma_Init( s_S32K_can_id ObjId, uint8_t instanceNum, const flexcan_user_config_t *UsrConfig, s_bspcan_bitRate UsrBaudRate )
{
    wres_t res;
    uint8_t n;
   
  if ((S32K_CAN_OBJ_NUM > (uint8_t)ObjId) && ( instanceNum < CAN_INSTANCE_COUNT))
  {
    s_S32K_canObj *Config;
    Config = Can_Obj_Config[ObjId].CanObjPtr;
    Config->UserConfig = *UsrConfig;
    Config->instanceNum = instanceNum;
    //Config->p_RxMsg = Can_Obj_Config[ObjId].p_RxMsg;
    Config->p_RxFilter = Can_Obj_Config[ObjId].p_RxFilter;
    Config->p_Id_table = Can_Obj_Config[ObjId].p_Id_table;
    Config->State = &canCom1_State;
    Config->Mode= (flexcan_operation_modes_t)UsrConfig->flexcanMode;
    if ( WRES_OK == Queue_Init(&Config->rxQueue,Can_Obj_Config[ObjId].p_Q_rxBuffer,sizeof(s_S32K_can_msg_t),(uint32_t)Can_Obj_Config[ObjId].QrxBuffsize))
    {
      //if ( STATUS_SUCCESS == FLEXCAN_DRV_Init(instanceNum, Config->State, UsrConfig))
      if ( STATUS_SUCCESS == MyFlexCanDrvInit(instanceNum, Config->State, &Config->UserConfig, S32K_Can_SetInitBitRate(UsrBaudRate) ) )
      {
        Config->ActualbitRate = UsrBaudRate;
        Instance_To_CanObj[instanceNum]=(uint8_t)ObjId;
        FLEXCAN_DRV_InstallEventCallback(instanceNum, (flexcan_callback_t)&CanCallback, NULL);
        for(n=(uint8_t)FIRST_RX_MAILBOX;n<RX_MAILBOX_NUM;n++)
        {
        Config->p_RxFilter[n].RxId = 0x1FFFFFFFu;
        Config->p_RxFilter[n].RxMask = 0xFFFFFFFFu;
        Config->p_RxFilter[n].IdType = MY_CAN_MSG_ID_EXT;
        Config->p_Id_table[n].isExtendedFrame = true;
        Config->p_Id_table[n].idFilter = &Config->p_RxFilter[n].RxId;
        }
        //n=1;
        //Config->p_Id_table[0].isExtendedFrame = false;
        
        // Configure RX FIFO ID filter table elements based on filter table defined above
        MY_FLEXCAN_DRV_ConfigRxFifo(instanceNum, FLEXCAN_RX_FIFO_ID_FORMAT_A, Config->p_Id_table);
        // set individual masking type 
        FLEXCAN_DRV_SetRxMaskType(instanceNum, FLEXCAN_RX_MASK_INDIVIDUAL);
        // first 10 filter items are masked with RXIMR0-RXIMR9 
        //for(n=(uint8_t)FIRST_RX_MAILBOX;n<RX_MAILBOX_NUM;n++)
        for(n=(uint8_t)FIRST_RX_MAILBOX;n<MAILBOX_NUM;n++)
        {
          FLEXCAN_DRV_SetRxIndividualMask(instanceNum, FLEXCAN_MSG_ID_EXT, n, 0xFFFFFFFFu);
        }
        // rest of filter items are masked with RXFGMASK 
        FLEXCAN_DRV_SetRxFifoGlobalMask(instanceNum, FLEXCAN_MSG_ID_EXT, 0xFFFFFFFFu);
        
        S32K_CanDma_StartRx(ObjId);

        res = WRES_OK;
        
//        s_S32K_can_msg_t aTxMessage;
//        aTxMessage.id = 0xDA;
//        aTxMessage.dlc = 8;
//        aTxMessage.TypeID = FLEXCAN_MSG_ID_STD;
//        aTxMessage.isRemote = false;
//        aTxMessage.data[0]= 0;
//        aTxMessage.data[1]= 1;
//        aTxMessage.data[2]= 2;
//        aTxMessage.data[3]= 3;
//        aTxMessage.data[4]= 4;
//        aTxMessage.data[5]= 5;
//        aTxMessage.data[6]= 6;
//        aTxMessage.data[7]= 7;
//
//        res =S32K_Can_MsgTx(ObjId, &aTxMessage);

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
    res = WRES_ERROR_INVALID_PARAMETER;
  }
  return res;         
        
//	// Configure TX buffer
//	for( n = FIRST_TX_MAILBOX; n < Config->maxBuffNum; n ++ )
//        {
//	    CAN_ConfigTxBuff((can_instance_t)instanceNum, n, &buffCfgSTD);
//	}
//	// Configure RX buffer
//	CAN_ConfigRxBuff((can_instance_t)instanceNum, RX_MAILBOX_STD, &buffCfgSTD,0x0UL);
//	CAN_ConfigRxBuff((can_instance_t)instanceNum, RX_MAILBOX_EXT, &buffCfgEXT,0x0UL);
//	CAN_SetRxFilter((can_instance_t)instanceNum,CAN_MSG_ID_STD, RX_MAILBOX_STD,0x0);
//	CAN_SetRxFilter((can_instance_t)instanceNum,CAN_MSG_ID_EXT, RX_MAILBOX_EXT,0x0);
//
//	Queue_Init(&Config->rxQueue,Can_Obj_Config[ObjId].p_Q_rxBuffer,sizeof(s_S32K_can_msg_t),Can_Obj_Config[ObjId].QrxBuffsize);
//	Config->open =true;
//	//CAN_InstallEventCallback((can_instance_t)instanceNum, (can_callback_t)&flexcan_Callback, NULL);
//
//	for( n = 0; n < RX_MAILBOX_NUM; n ++ )
//        {
//	    Config->p_RxFilter[n].RxId=0xFFFFFFFF;
//	    Config->p_RxFilter[n].RxMask=0xFFFFFFFF;
//	}
//	S32K_Can_SetSwRxFilter(ObjId, 0x00,CAN_MSG_ALL_ID_PASS,S32K_CAN_MASK_PASS_ALL,MY_CAN_MSG_ID_ALL );
//	CAN_Receive((can_instance_t)instanceNum, RX_MAILBOX_STD,Config->p_RxMsgSTD);
//	CAN_Receive((can_instance_t)instanceNum, RX_MAILBOX_EXT,Config->p_RxMsgEXT);
//	S32K_Can_GetBitRate(ObjId, &Config->ActualbitRate);
//	res = WRES_OK;
//    }
//    else
//    {
//	res = WRES_ERROR_INVALID_PARAMETER;
//    }
//    return res;
}

//// -----------------------------------------------------------------------------------------------------
//extern wres_t S32K_Can_DeInit( s_S32K_can_id ObjId)
//{
//    wres_t res;
//
//    s_S32K_canObj *Config;
//    if (S32K_CAN_OBJ_NUM > ObjId)
//    {
//	Config = Can_Obj_Config[ObjId].CanObjPtr;
//	if(STATUS_SUCCESS == CAN_Deinit((can_instance_t)Config->instanceNum))
//	{
//	    memset(Config, 0x00, sizeof(s_S32K_canObj));
//	    res = WRES_OK;
//	}
//	else
//	{
//	    res = WRES_ERROR;
//	}
//    }
//    else
//    {
//	res = WRES_ERROR_INVALID_PARAMETER;
//    }
//
//    return res;
//}
// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_MsgTx( s_S32K_can_id ObjId,s_S32K_can_msg_t *msgCan, bool checkBuffIndex, uint8_t buffIndex )
{
    wres_t res;
    s_S32K_canObj *Config;
    uint8_t BufferFree;
    uint8_t canStatus;

    if ( WRES_OK == S32K_SysPin_GetOutputPins(GPIO_OUT_CAN_STB, &canStatus) )
    {
        if ( 0x00u == canStatus )
        {
            if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
            {
	        Config = Can_Obj_Config[ObjId].CanObjPtr;
                if (false == Config->open)
                {
                    res = WRES_ERROR_INVALID_PARAMETER;
                }
                else
                {
	            Config->TxMessage.msgId = msgCan->id;
	            Config->TxMessage.dataLen = msgCan->dlc;
	            memcpy(&Config->TxMessage.data[0],&msgCan->data[0],8u);

                    flexcan_data_info_t dataInfo = 
                    {
                        .msg_id_type = (flexcan_msgbuff_id_type_t) msgCan->TypeID,
                        .data_length = Config->TxMessage.dataLen,
                        .fd_enable = false,
                        .fd_padding = 0u,
                        .enable_brs = false,
                        .is_remote = false,
                    };

                    BufferFree = FindFreeTxBuffer(Config->instanceNum, Config->TxMessage.msgId, Config->State, checkBuffIndex, buffIndex);
	            if( MAILBOX_NUM > BufferFree)
                    {
	                if (STATUS_SUCCESS != FLEXCAN_DRV_Send(Config->instanceNum,BufferFree,&dataInfo,Config->TxMessage.msgId, &Config->TxMessage.data[0]))
                        {
		            res = WRES_BUFFER_FULL;
	                }
                        else
                        {
		            res = WRES_OK;
	                }
	            }
                    else
                    {
	                res = WRES_BUFFER_FULL;
                    }
	        }
            }
            else
            {
	        res = WRES_ERROR_INVALID_PARAMETER;
            }
        }
        else
        {
            res = WRES_ERROR_NOT_READY;
        }
    }
    else
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
        
    
    return res;
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_RecvMsg(s_S32K_can_id ObjId, s_S32K_can_msg_t *msgCan)
{
    wres_t res;
    s_S32K_canObj *Config;
    
    if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
    {
	Config = Can_Obj_Config[ObjId].CanObjPtr;
        if ( true == Config->open )
        {
            if (WRES_OK == Queue_Dequeue(&Config->rxQueue,msgCan))
            {
                res = WRES_OK;
            }
            else
            {
                res = WRES_BUFFER_EMPTY;
            }
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

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_GetRxStatus( s_S32K_can_id ObjId, wres_t *Status)
{
    wres_t res;
    s_S32K_canObj *Config;
    if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
    {
	Config = Can_Obj_Config[ObjId].CanObjPtr;
	*Status = Config->Rxres;
        res = WRES_OK;
    }
    else
    {
	res = WRES_ERROR_INVALID_PARAMETER;
    }
    return res;
}


// -----------------------------------------------------------------------------------------------------
static uint32_t S32K_Can_SetInitBitRate( s_bspcan_bitRate BitRate )
{
    uint32_t res;
    
    switch ( BitRate )
    {
    case CAN_BIT_RATE_20K:
        res = 20000UL;
	break;
    case CAN_BIT_RATE_50K:
        res = 50000UL;
	break;
    case CAN_BIT_RATE_125K:
        res = 125000UL;
	break;
    case CAN_BIT_RATE_250K:
        res = 250000UL;
	break;
    case CAN_BIT_RATE_500K:
        res = 500000UL;
	break;
    case CAN_BIT_RATE_1M:
        res = 1000000UL;
	break;
    default:
        res = 250000UL;
	break;
    }  
    
    return res;
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_SetBitRate( s_S32K_can_id ObjId, s_bspcan_bitRate BitRate)
{
    wres_t res;
    s_S32K_canObj *Config;
    
    if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
    {
	Config = Can_Obj_Config[ObjId].CanObjPtr;
	switch ( BitRate )
	{
	case CAN_BIT_RATE_20K:
	    ActlBitrate.propSeg = 7u;
	    ActlBitrate.phaseSeg1 = 5u;
	    ActlBitrate.phaseSeg2 = 4u;
	    ActlBitrate.preDivider = 19u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	case CAN_BIT_RATE_50K:
	    ActlBitrate.propSeg = 7u;
	    ActlBitrate.phaseSeg1 = 5u;
	    ActlBitrate.phaseSeg2 = 4u;
	    ActlBitrate.preDivider = 7u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	case CAN_BIT_RATE_125K:
	    ActlBitrate.propSeg = 6u;
	    ActlBitrate.phaseSeg1 = 3u;
	    ActlBitrate.phaseSeg2 = 3u;
	    ActlBitrate.preDivider = 3u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	case CAN_BIT_RATE_250K:
	    ActlBitrate.propSeg = 7u;
	    ActlBitrate.phaseSeg1 = 4u;
	    ActlBitrate.phaseSeg2 = 1u;
	    ActlBitrate.preDivider = 1u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
        case CAN_BIT_RATE_500K:
   	    ActlBitrate.propSeg = 7u;
	    ActlBitrate.phaseSeg1 = 4u;
	    ActlBitrate.phaseSeg2 = 1u;
	    ActlBitrate.preDivider = 0u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	case CAN_BIT_RATE_1M:
	    ActlBitrate.propSeg = 2u;
	    ActlBitrate.phaseSeg1 = 1u;
	    ActlBitrate.phaseSeg2 = 1u;
	    ActlBitrate.preDivider = 0u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	default:
	    ActlBitrate.propSeg = 7u;
	    ActlBitrate.phaseSeg1 = 4u;
	    ActlBitrate.phaseSeg2 = 1u;
	    ActlBitrate.preDivider = 1u;
	    ActlBitrate.rJumpwidth = 1u;
	    break;
	}
	FLEXCAN_DRV_SetBitrate(Config->instanceNum, &ActlBitrate);
        Config->UserConfig.bitrate = ActlBitrate;
        S32K_Can_GetBitRate(ObjId, &Config->ActualbitRate);
        res = WRES_OK;
    }
    else
    {
	res = WRES_ERROR_INVALID_PARAMETER;
    }
    
    return res;
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Can_GetBitRate( s_S32K_can_id ObjId, s_bspcan_bitRate *BitRate)
{
    wres_t res;
    s_S32K_canObj *Config;
    uint32_t Aux;
    
    if (S32K_CAN_OBJ_NUM > (uint8_t)ObjId)
    {
	Config = Can_Obj_Config[ObjId].CanObjPtr;

	FLEXCAN_DRV_GetBitrate(Config->instanceNum, &ActlBitrate);
        Aux = ActlBitrate.propSeg + ActlBitrate.phaseSeg1 + ActlBitrate.phaseSeg2 + ActlBitrate.preDivider + ActlBitrate.rJumpwidth;
        switch ( Aux )
        {
        case 36U:
            *BitRate = CAN_BIT_RATE_20K;
            res = WRES_OK;
            break;
        case 24U:
            *BitRate = CAN_BIT_RATE_50K;
            res = WRES_OK;
            break;
        case 16U:
            *BitRate = CAN_BIT_RATE_125K;
            res = WRES_OK;
            break;
        case 14U:
            *BitRate = CAN_BIT_RATE_250K;
            res = WRES_OK;
            break;
        case 13U:
            *BitRate = CAN_BIT_RATE_500K;
            res = WRES_OK;
            break;
        case 5U:
            *BitRate = CAN_BIT_RATE_1M;
            res = WRES_OK;
            break;
        default:
            res = WRES_ERROR_INVALID_MODE;
            break;
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
