/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Can.h
 * Brief description
 */
#ifndef S32K_CAN_H_
#define S32K_CAN_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
//#include "S32K_can.h"
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
typedef enum {
    MY_CAN_MSG_ID_STD=0,         // Standard ID
    MY_CAN_MSG_ID_EXT=1,         // Extended ID
    MY_CAN_MSG_ID_ALL=3          // Standard and Extended ID
} s_can_msg_id_type_t;

typedef enum {
    CAN_BIT_RATE_10K  =10000U,
    CAN_BIT_RATE_20K  =20000U,
    CAN_BIT_RATE_50K  =50000U,
    CAN_BIT_RATE_125K =125000U,
    CAN_BIT_RATE_250K =250000U,
    CAN_BIT_RATE_500K =500000U,
    CAN_BIT_RATE_800K =800000U,
    CAN_BIT_RATE_1M   =1000000U,
} s_bspcan_bitRate;

typedef enum
{
    S32K_CAN_ID_0        = 0x00U,
    S32K_CAN_ID_1        = 0x01U,
    S32K_CAN_ID_2        = 0x02U,
} s_S32K_can_id;

typedef enum
{
    S32K_CAN_MASK_PASS_ALL      = 0x00000000U,          // Check nothing, pass all
    S32K_CAN_MASK_SA_CHECK     = 0x00FFFFFFUL,    //Check PGN and source Address
    S32K_CAN_MASK_SA_NOT_CHECK  = 0x00FFFF00UL,   //Check only  PGN ,not source Address
    S32K_CAN_MASK_DA_YES_SA_NOT_CHECK = 0x0000FF00UL, //Check only destination Address, used for receiving all messagges transmitted to a specific address (or to all), independently from PGN
    S32K_CAN_MASK_CHECK_ALL     = 0xFFFFFFFFUL,       // Check ALL    
} s_S32K_can_FilterMask;

typedef enum
{
    S32K_CAN_SAMPLE_RX_MSG        = 0x00U,
    S32K_CAN_SAMPLE_TX_MSG        = 0x01U,
} s_Can_Sample;

/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
    uint32_t id;
    uint32_t Cnt;
    uint8_t dlc;
    bool isRemote;
    flexcan_msgbuff_id_type_t TypeID;
    uint8_t data[8];
} s_S32K_can_msg_t;

/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/
#define RX_ID_FILTER_NUM 16u //canCom1_InitConfig0.max_num_mb

#define RX_FIFO_FORMAT_A
//define RX_FIFO_FORMAT_B
//define RX_FIFO_FORMAT_C
//define RX_FIFO_FORMAT_D
  
//    FLEXCAN_RX_FIFO_ID_FORMAT_A, /*!< One full ID (standard and extended) per ID Filter Table element.*/
//    FLEXCAN_RX_FIFO_ID_FORMAT_B, /*!< Two full standard IDs or two partial 14-bit (standard and
//                                     extended) IDs per ID Filter Table element.*/
//    FLEXCAN_RX_FIFO_ID_FORMAT_C, /*!< Four partial 8-bit Standard IDs per ID Filter Table element.*/
//    FLEXCAN_RX_FIFO_ID_FORMAT_D  /*!< All frames rejected.*/   

/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/

//extern wres_t S32K_CanDma_Init( s_S32K_can_id ObjId, uint32_t instanceNum, flexcan_user_config_t *userMasterConfigPtr);
extern wres_t S32K_CanDma_Init( s_S32K_can_id ObjId, uint8_t instanceNum, const flexcan_user_config_t *UsrConfig, s_bspcan_bitRate UsrBaudRate );
extern wres_t S32K_CanDma_StopRx( s_S32K_can_id ObjId);
extern wres_t S32K_CanDma_StartRx( s_S32K_can_id ObjId);
extern wres_t S32K_Can_SetBitRate( s_S32K_can_id ObjId, s_bspcan_bitRate BitRate);
extern wres_t S32K_Can_GetBitRate( s_S32K_can_id ObjId, s_bspcan_bitRate *BitRate);

extern wres_t S32K_Can_MsgTx( s_S32K_can_id ObjId,s_S32K_can_msg_t *msgCan, bool checkBuffIndex, uint8_t buffIndex );
extern wres_t S32K_Can_SetSwRxFilter(s_S32K_can_id ObjId, uint8_t FilterIndex, uint32_t PassId, s_S32K_can_FilterMask Mask, s_can_msg_id_type_t IdType );
extern wres_t S32K_Can_UpdateHwRxFilter(s_S32K_can_id ObjId);
extern wres_t S32K_Can_RecvMsg(s_S32K_can_id ObjId, s_S32K_can_msg_t *msgCan);


//extern wres_t S32K_CanDma_Init( void );
extern void S32K_CanDma_InitFilter( void );
extern wres_t S32K_CanDma_SetFilter( uint8_t index, uint32_t ID, bool isExtendedFrame );

/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/

#endif /* S32K_CAN_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
