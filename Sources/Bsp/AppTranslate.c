/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_flexio.c
 * History:
 *  - 29 gen 2020 1.0 Davoli - First release
 * S32K_can.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "wtypes.h"
#include "string.h"
#include "Cpu.h"
#include "S32K_ObjectConfig.h"
#include "S32K_flexSpi.h"
#include "S32K_LpSpi.h"
#include "S32K_CanDma.h"
#include "S32K_Adc.h"
#include "S32K_Timer.h"
#include "S32K_SysPin.h"
#include "AppTranslate.h"
#include "application.h"

/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/
static s_storeRxCANMsg_t CANMsgRx_Stored[CAN_MSG_BUFFER_NUM][CAN_RX_BUFFER_NUM_MAX];
static uint8_t CANMsgRx_storedSize[CAN_MSG_BUFFER_NUM];
static bool addressCheck[CAN_MSG_BUFFER_NUM];
static w_error_entry_t ledError[LED_OUT_NUM];
static bool ledErrorIsPresent[LED_OUT_NUM];
/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
**************************************************************************************************/
#define LED_ERROR_STATUS  ( 0x01 )
#define LED_ERROR_CNT_TH  ( 10U )
#define LED_CHECK_ERROR_ON_COUNTER_TH   ( 100U )
#define LED_CHECK_ERROR_OFF_COUNTER_TH  (   15U )

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
static uint8_t ledDutyEn[LED_OUT_NUM];

 /**************************************************************************************************
  *  9. PUBLIC VARIABLES                                                                           *
  **************************************************************************************************/

 /**************************************************************************************************
  * 10. FUNCTION DEFINITIONS                                                                       *
  **************************************************************************************************/
extern void CAN_StoredMsgUpdate( uint8_t index, uint8_t msgIndex, uint32_t newID, bool sourceAddressCheck )
{
    CANMsgRx_Stored[index][msgIndex].ID = newID;
    addressCheck[index] = sourceAddressCheck;
    return;
}

extern void CAN_ModuleEnable( s_S32K_can_id moduleIndex, s_bspcan_bitRate baudRate_bps )
{
    S32K_Can_SetBitRate( moduleIndex, baudRate_bps);
    return;
}

void CAN_SetInitBit( s_S32K_can_id moduleIndex )
{
    (void)moduleIndex;
    return;
}

void CAN_ResetInitBit( s_S32K_can_id moduleIndex )
{
    (void)moduleIndex;
    return;
}

wres_t CAN_MsgTx( s_TxRxCANMsg *msgCan, bool checkBuffIndex, uint8_t buffIndex )
{
    wres_t res;
    s_S32K_can_msg_t TxMessage;
    if ( msgCan->TimeOut > 0U )
    {
        msgCan->TimeCnt = msgCan->TimeCnt + 1U;
        if ( msgCan->TimeCnt >= msgCan->TimeOut )
        {
            msgCan->NewMsg = CAN_NEW_MSG;
            msgCan->TimeCnt = 0U;
        }
    }
    else
    {
        msgCan->TimeCnt = 0U;
    }

    if ( CAN_NEW_MSG == msgCan->NewMsg ) // @suppress("Symbol is not resolved")
    {
        TxMessage.id = msgCan->ID;
        TxMessage.dlc = msgCan->DLC;
        TxMessage.TypeID = (flexcan_msgbuff_id_type_t)msgCan->TypeID;
        TxMessage.isRemote = (msgCan->IsRTR)!=0u;
        TxMessage.data[0]= msgCan->Data[0];
        TxMessage.data[1]= msgCan->Data[1];
        TxMessage.data[2]= msgCan->Data[2];
        TxMessage.data[3]= msgCan->Data[3];
        TxMessage.data[4]= msgCan->Data[4];
        TxMessage.data[5]= msgCan->Data[5];
        TxMessage.data[6]= msgCan->Data[6];
        TxMessage.data[7]= msgCan->Data[7];

        res = S32K_Can_MsgTx(CAN_JOY_OUT, &TxMessage, checkBuffIndex, buffIndex );
        //if (WRES_OK ==S32K_Can_MsgTx(CAN_JOY_OUT, &TxMessage))
        if ( WRES_OK == res )
        {
            msgCan->NewMsg = CAN_NO_MSG; // @suppress("Symbol is not resolved")
        }
    }    
    else
    {
        res = WRES_OK;
    }

    return res;
}

void CAN_StoredMsgClear( uint8_t index, uint8_t msgIndex )
{
    uint8_t i;

    for ( i=msgIndex; i<( CAN_RX_BUFFER_NUM_MAX - 1U ); i++ )
    {
        memcpy( CANMsgRx_Stored[index][i].Data, CANMsgRx_Stored[index][i+1U].Data, 8U );
        CANMsgRx_Stored[index][i].DA = CANMsgRx_Stored[index][i+1U].DA;
        CANMsgRx_Stored[index][i].SA = CANMsgRx_Stored[index][i+1U].SA;
        CANMsgRx_Stored[index][i].DLC = CANMsgRx_Stored[index][i+1U].DLC;
        CANMsgRx_Stored[index][i].NewMsg = CANMsgRx_Stored[index][i+1U].NewMsg;
    }
    if ( CANMsgRx_storedSize[index] > 0U )
    {
        CANMsgRx_storedSize[index]--;
    }
    for ( i= CANMsgRx_storedSize[index]; i < CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CANMsgRx_Stored[index][i].NewMsg = CAN_NO_MSG;
        memset( CANMsgRx_Stored[index][i].Data, 0x00L, 8U );
    }
    return;
}
void CAN_MsgUpdateData( s_TxRxCANMsg *msgCan, uint8_t *bufferData, uint8_t txEn )
{
    uint8_t i;


    for ( i=0U; i<8U; i++ )
    {
        msgCan->Data[i] = *( bufferData + i );
    }
    if ( CAN_MSG_TX_YES == txEn )
    {
        msgCan->NewMsg = CAN_NEW_MSG; // @suppress("Symbol is not resolved")
    }

    return;
}
//bool CanTxRequestGet()
//{
//  return CanTxRequest;
//}
//void CanTxRequestClear()
//{
//  CanTxRequest =false;
//}
void CAN_MsgUpdateRxFilter ( void )
{
  S32K_Can_UpdateHwRxFilter(CAN_MODULE_0);
}
void CAN_MsgUpdateID( s_TxRxCANMsg *msgCan, uint32_t id, flexcan_msgbuff_id_type_t typeID, bool sourceAddressCheck )
{
    uint8_t msgIndex;
    msgIndex = msgCan->Index;
    msgCan->TypeID = typeID;
    msgCan->ID = id;
    if (msgCan->RxTx == CAN_MSG_RX)
    { // RX Msg
		if (true == sourceAddressCheck)
		{
			S32K_Can_SetSwRxFilter(CAN_MODULE_0, msgIndex,id,S32K_CAN_MASK_SA_CHECK,MY_CAN_MSG_ID_EXT );
		}
		else
		{
			S32K_Can_SetSwRxFilter(CAN_MODULE_0, msgIndex,id,S32K_CAN_MASK_SA_NOT_CHECK,MY_CAN_MSG_ID_EXT );
		}
    }
    return;
}
//uint8_t BroadCastmsgIndex = 1;
void CAN_MsgUpdateIDGeneral( uint32_t id, flexcan_msgbuff_id_type_t typeID )
{
    (void)typeID;
    uint8_t MySAmsgIndex = 0U;
    S32K_Can_SetSwRxFilter(CAN_MODULE_0, MySAmsgIndex,id,S32K_CAN_MASK_DA_YES_SA_NOT_CHECK,MY_CAN_MSG_ID_EXT );
    return;
}

static void CAN_MsgGeneralEnable( s_TxRxCANMsg *msgCan, s_S32K_can_id moduleIndex, flexcan_msgbuff_id_type_t typeID, uint8_t msgIndex, uint32_t filterMatch, s_S32K_can_FilterMask maskMatch )
{
    msgCan->TypeID = typeID;
    msgCan->ID = filterMatch;
    msgCan->DA = 0xFFU;
    msgCan->SA = 0xFFU;
    msgCan->DLC = 8U;
    msgCan->TimeOut = 0U;
    msgCan->TimeCnt = 0U;
    msgCan->Error = 0U;
    msgCan->RxTx = CAN_MSG_RX;
    msgCan->IsRTR = CAN_MSG_IS_NOT_RTR;
    msgCan->NewMsg = CAN_NO_MSG;
    msgCan->Index =  msgIndex;

    S32K_Can_SetSwRxFilter(moduleIndex, msgIndex, msgCan->ID, maskMatch, MY_CAN_MSG_ID_EXT );
    return;
}

void CAN_MsgUpdateTimeOut( s_TxRxCANMsg *msgCan, uint16_t timeOut )
{
    msgCan->TimeOut = timeOut;

    return;
}

uint32_t CAN_StoredMsgGetID( uint8_t index, uint8_t msgIndex )
{
    return CANMsgRx_Stored[index][msgIndex].ID;
}

s_TxRxCANMsg CAN_StoredMsgGet( uint8_t index, uint8_t msgIndex )
{
    s_TxRxCANMsg msg;

    msg.SA = CANMsgRx_Stored[index][msgIndex].SA;
    msg.DA = CANMsgRx_Stored[index][msgIndex].DA;
    memcpy( msg.Data, CANMsgRx_Stored[index][msgIndex].Data, CANMsgRx_Stored[index][msgIndex].DLC );
    msg.DLC = CANMsgRx_Stored[index][msgIndex].DLC;

    return msg;
}

uint8_t CAN_StoredMsgGetData( uint8_t index, uint8_t msgIndex, uint8_t dataNum )
{
    return CANMsgRx_Stored[index][msgIndex].Data[dataNum];
}

uint8_t CAN_StoredMsgRx( uint8_t index, uint8_t *msgIndex )
{
    uint8_t retVal;

    *msgIndex = 0U;
    retVal = CAN_NO_MSG;

        if (  CANMsgRx_storedSize[index] > 0U )
        {
            if ( CANMsgRx_Stored[index][0U].NewMsg == CAN_NEW_MSG )
            {
                *msgIndex = 0U;
                retVal = CAN_NEW_MSG;
            }
            else
            {
                CAN_StoredMsgClear( index, 0U );
            }
        }
    return retVal;
}
extern void CAN_MsgEnable( s_TxRxCANMsg *msgCan, s_S32K_can_id moduleIndex, uint8_t msgIndex, uint8_t typeRxTx, flexcan_msgbuff_id_type_t typeID, uint32_t msgID, bool sourceAddressCheck, uint8_t length, uint16_t timeOut, uint8_t remoteReqEn )
{

    msgCan->TypeID = typeID;
    msgCan->ID = msgID;
    msgCan->DA = (uint8_t)( msgID >> 8U );
    msgCan->SA = (uint8_t)( msgID );
    msgCan->DLC = length;
    msgCan->TimeOut = timeOut;
    msgCan->TimeCnt = 0U;
    msgCan->Error = 0U;
    msgCan->RxTx = typeRxTx;
    msgCan->IsRTR = remoteReqEn;
    msgCan->NewMsg = CAN_NO_MSG;

    if (typeRxTx == CAN_MSG_RX){ // RX Msg
    	if (true == sourceAddressCheck){
    		S32K_Can_SetSwRxFilter(moduleIndex, msgIndex, msgCan->ID,S32K_CAN_MASK_SA_CHECK,MY_CAN_MSG_ID_EXT );
    	}
    	else
    	{
    		S32K_Can_SetSwRxFilter(moduleIndex, msgIndex, msgCan->ID,S32K_CAN_MASK_SA_NOT_CHECK,MY_CAN_MSG_ID_EXT );
    	}
    }
    return;
}

void CAN_Init_K144( uint8_t source_addr )
{
    s_TxRxCANMsg CANMsgRx_GeneralMsg;
    s_TxRxCANMsg CANMsgRx_GeneralMsgAll;
    uint8_t j;  
  
    CAN_MsgGeneralEnable( &CANMsgRx_GeneralMsg   , CAN_MODULE_0, CAN_ID_29BIT, 0U, ( ( (uint32_t)source_addr << 8U ) & 0x0000FF00UL ), S32K_CAN_MASK_DA_YES_SA_NOT_CHECK );
    CAN_MsgGeneralEnable( &CANMsgRx_GeneralMsgAll, CAN_MODULE_0, CAN_ID_29BIT, 1U, 0x0000FF00UL, S32K_CAN_MASK_DA_YES_SA_NOT_CHECK );
    //CAN_MsgEnable( &RxMLX0Message, CAN_MODULE_0, 2U, CAN_MSG_RX, CAN_ID_29BIT, 0x18DA22FAUL, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    //CAN_MsgEnable( &RxMLX1Message, CAN_MODULE_0, 3U, CAN_MSG_RX, CAN_ID_29BIT, 0x18DA23FAUL, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    //CAN_MsgEnable( &CANMsgRx_MLX_SPICmd_X, CAN_MODULE_0, 6, CAN_MSG_RX, CAN_ID_29BIT, 0x18DA22F9UL, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    
    for ( j=0U; j<LED_OUT_NUM; j++ )
    {
        ledError[j].value = LED_ERROR_STATUS;
        ledError[j].cntTh = LED_ERROR_CNT_TH;
        ledError[j].active = false;
        ledError[j].occurrence = 0U;
        ledError[j].cnt = 0U;
        ledError[j].en = true;
        ledErrorIsPresent[j] = false;
    }
    
    return;

}
static void CAN_MsgRxDequeue( void )
{

    uint8_t j;
    uint32_t auxIDIn;
    uint32_t auxIDStored;
    s_S32K_can_msg_t TempCanMsg;
    wres_t Result = WRES_OK;
	
    while (Result != WRES_BUFFER_EMPTY)
    {
	Result = S32K_Can_RecvMsg(CAN_JOY_OUT,&TempCanMsg);
	if (WRES_OK == Result)
	{
            for ( j=0U; j<CAN_MSG_BUFFER_NUM; j++ )
            {
	        if ( CANMsgRx_storedSize[j] < CAN_RX_BUFFER_NUM_MAX )
		{
		    if ( true == addressCheck[j] )
		    {
			auxIDStored = CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].ID & 0x00FFFFFFUL;
			auxIDIn = TempCanMsg.id & 0x00FFFFFFUL;
		    }
		    else
		    {
			auxIDStored = CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].ID & 0x00FFFF00UL;
			auxIDIn = TempCanMsg.id & 0x00FFFF00UL;
		    }
		    if ( ( 0x0000FF00UL & auxIDIn ) == 0x0000FF00UL )
		    {
			auxIDStored = auxIDStored & 0xFFFF00FFUL;
			auxIDIn = auxIDIn & 0xFFFF00FFUL;
		    }
		    if ( auxIDStored == auxIDIn )
		    {
			memcpy( CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].Data, TempCanMsg.data, TempCanMsg.dlc );
			CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].DA = (uint8_t)((TempCanMsg.id & 0xFF00u)>>8u);
			CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].SA = (uint8_t)(TempCanMsg.id & 0xFFu);
			CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].DLC = TempCanMsg.dlc;
			CANMsgRx_Stored[j][CANMsgRx_storedSize[j]].NewMsg = CAN_NEW_MSG;
			CANMsgRx_storedSize[j]++;
			if ( CANMsgRx_storedSize[j] >= CAN_RX_BUFFER_NUM_MAX )
			{
			    CANMsgRx_storedSize[j]--;
			}
		    }
	        }
	    }
	}
    }
    return;
}
void CAN_Main_K144( void )
{
  CAN_MsgRxDequeue();
    return;
}


void BSP_OutLedUpdate( uint8_t ledCmd, uint8_t ledDuty )
{
    uint8_t i;
    
    for ( i=0U; i<LED_OUT_NUM; i++ )
    {
        ledDutyEn[i] = LED_DUTY_OFF;
    }
  
    if ( ( ledCmd & 0x03U ) == 0x01U )
    {
        if ( false == ledErrorIsPresent[0U] )
        {
            S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN0);
        }
        else
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
        }
    }
    else if ( ( ledCmd & 0x03U ) == 0x02U )
    {
        if ( ( ledDuty & 0x03U ) == 0x00U )
        {
            ledDutyEn[LED_OUT_0] = LED_DUTY_25;
        }
        else if ( ( ledDuty & 0x03U ) == 0x01U )
        {
            ledDutyEn[LED_OUT_0] = LED_DUTY_50;
        }
        else if ( ( ledDuty & 0x03U ) == 0x02U )
        {
            ledDutyEn[LED_OUT_0] = LED_DUTY_75;
        }
        else
        {
          // Do Nothing
        }
    }
    else
    {
        S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
    }
    if ( ( ( ledCmd & 0x0CU ) >> 2U ) == 0x01U )
    {
        if ( false == ledErrorIsPresent[1U] )
        {
            S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN1);
        }
        else
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
        }
    }
    else if ( ( ( ledCmd & 0x0CU ) >> 2U ) == 0x02U )
    {
        if ( ( ( ledDuty & 0x0CU ) >> 2U ) == 0x00U )
        {
            ledDutyEn[LED_OUT_1] = LED_DUTY_25;
        }
        else if ( ( ( ledDuty & 0x0CU ) >> 2U ) == 0x01U )
        {
            ledDutyEn[LED_OUT_1] = LED_DUTY_50;
        }
        else if ( ( ( ledDuty & 0x0CU ) >> 2U ) == 0x02U )
        {
            ledDutyEn[LED_OUT_1] = LED_DUTY_75;
        }
        else
        {
          // Do Nothing
        }
    }    
    else
    {
        S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
    }
    if ( ( ( ledCmd & 0x30U ) >> 4U ) == 0x01U )
    {
        if ( false == ledErrorIsPresent[2U] )
        {
            S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN2);
        }
        else
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
        }
    }
    else if ( ( ( ledCmd & 0x30U ) >> 4U ) == 0x02U )
    {
        if ( ( ( ledDuty & 0x30U ) >> 4U ) == 0x00U )
        {
            ledDutyEn[LED_OUT_2] = LED_DUTY_25;
        }
        else if ( ( ( ledDuty & 0x30U ) >> 4U ) == 0x01U )
        {
            ledDutyEn[LED_OUT_2] = LED_DUTY_50;
        }
        else if ( ( ( ledDuty & 0x30U ) >> 4U ) == 0x02U )
        {
            ledDutyEn[LED_OUT_2] = LED_DUTY_75;
        }
        else
        {
          // Do Nothing
        }
    }
    else
    {
        S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
    }
    if ( ( ( ledCmd & 0xC0U ) >> 6U ) == 0x01U )
    {
        if ( false == ledErrorIsPresent[3U] )
        {
            S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN3);
        }
        else
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
        }
    }
    else if ( ( ( ledCmd & 0xC0U ) >> 6U ) == 0x02U )
    {
        if ( ( ( ledDuty & 0xC0U ) >> 6U ) == 0x00U )
        {
            ledDutyEn[LED_OUT_3] = LED_DUTY_25;
        }
        else if ( ( ( ledDuty & 0xC0U ) >> 6U ) == 0x01U )
        {
            ledDutyEn[LED_OUT_3] = LED_DUTY_50;
        }
        else if ( ( ( ledDuty & 0xC0U ) >> 6U ) == 0x02U )
        {
            ledDutyEn[LED_OUT_3] = LED_DUTY_75;
        }
        else
        {
          // Do Nothing
        }
    }    
    else
    {
        S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
    }    

    return;
}

// ------------------------------------------------------------
void BSP_OutLedCheck( uint8_t ledIndex, uint8_t ledCmd, bool ledEn, bool ledInternalEn, uint8_t ledErrorIndex )
{
    uint16_t ledFeedBack;
    static uint16_t ledCheckErrorCounter[LED_OUT_NUM];
    
    if ( ( true == ledEn ) || ( true == ledInternalEn ) )
    {
        if ( ( true == ledInternalEn ) && ( ledIndex == ledErrorIndex ) )
        {
            Error_Reset( &ledError[ledIndex] );
            ledErrorIsPresent[ledIndex] = false;
            ledCheckErrorCounter[ledIndex] = LED_CHECK_ERROR_OFF_COUNTER_TH;
        }
        else
        {
            if ( ( ( ( ledCmd & ( 0x03U << ( ledIndex * 2U ) ) ) >> ( 2U * ledIndex ) ) == 0x01U ) || ( ( ( ( ledCmd & ( 0x03U << ( ledIndex * 2U ) ) ) >> ( 2U * ledIndex ) ) == 0x02U ) && ( ledDutyEn[ledIndex] != LED_DUTY_OFF ) ) )
            {                          
                if ( false == ledErrorIsPresent[ledIndex] )
                {
                    ledFeedBack = ADC_GetValue_mV( (uint8_t)( ADC_AN_CH10 ) + ledIndex );
                    Error_Update( &ledError[ledIndex], ( ledFeedBack < 600U ) || ( ledFeedBack > 3500U ) , ERROR_RESET_WITH_CNT ); 
                    if ( ledCheckErrorCounter[ledIndex] >= LED_CHECK_ERROR_OFF_COUNTER_TH )
                    {
                        ledCheckErrorCounter[ledIndex] = LED_CHECK_ERROR_OFF_COUNTER_TH;
                    }
                    else
                    {
                      ledCheckErrorCounter[ledIndex]++;
                    }
                    if ( ( true == ledError[ledIndex].active ) && ( ledCheckErrorCounter[ledIndex] >= LED_CHECK_ERROR_OFF_COUNTER_TH ) )
                    {
                        ledErrorIsPresent[ledIndex] = true;
                        ledCheckErrorCounter[ledIndex] = 0U;
                    }
                }
                else
                {
                    if ( ledCheckErrorCounter[ledIndex] >= LED_CHECK_ERROR_ON_COUNTER_TH )
                    {
                        ledErrorIsPresent[ledIndex] = false;
                        ledCheckErrorCounter[ledIndex] = 0U;
                    }
                    else
                    {
                      ledCheckErrorCounter[ledIndex]++;
                    }
                }
                        
            }
            else
            {
                Error_Reset( &ledError[ledIndex] );
                ledErrorIsPresent[ledIndex] = false;
                ledCheckErrorCounter[ledIndex] = LED_CHECK_ERROR_OFF_COUNTER_TH;
            }
        }
    }
    else
    {
        Error_Reset( &ledError[ledIndex] );
        ledErrorIsPresent[ledIndex] = false;
        ledCheckErrorCounter[ledIndex] = LED_CHECK_ERROR_OFF_COUNTER_TH;
    }
      
    return;
}

// ------------------------------------------------------------
uint16_t ADC_GetValue_mV( uint8_t channel )
{
    uint16_t AuxAdcValue;
    S32K_Adc_GetValue_mV( (s_S32K_SysAdc_AdcID)channel, &AuxAdcValue);
    return (AuxAdcValue);
}

// ------------------------------------------------------------
bool LED_ErrorIsActive( uint8_t ledCh )
{
    bool retVal;
    
    retVal = false;
    if ( true == Error_IsActive( ledError[ledCh] ) )
    {
        retVal = true;
    }
    
    return retVal;
}

// ------------------------------------------------------------
uint8_t LED_DutyGetValue( uint8_t ledCh )
{
    uint8_t aux;
    
    aux = LED_DUTY_OFF;
    
    if ( ledCh < LED_OUT_NUM )
    {
        aux = ledDutyEn[ledCh];
    }
    
    return ( aux );
}

// ------------------------------------------------------------
void LED_DutyUpdate( void )
{
    static uint8_t counter = 0U;
    
    if ( counter < LED_COUNTER_25 )
    {
        if ( LED_DUTY_OFF != ledDutyEn[LED_OUT_0] )
        {
            if ( true == ledErrorIsPresent[0U] )
            {
                S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
            }
            else
            {
                S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN0);
            }
        }
        if ( LED_DUTY_OFF != ledDutyEn[LED_OUT_1] )
        {
            if ( true == ledErrorIsPresent[1U] )
            {          
                S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
            }
            else
            {
                S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN1);
            }
        }        
        if ( LED_DUTY_OFF != ledDutyEn[LED_OUT_2] )
        {
            if ( true == ledErrorIsPresent[2U] )
            {          
                S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
            }
            else
            {          
                S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN2);
            }
        }        
        if ( LED_DUTY_OFF != ledDutyEn[LED_OUT_3] )
        {
            if ( true == ledErrorIsPresent[3U] )
            {          
                S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
            }
            else
            {          
                S32K_SysPin_SetOutputPins(GPIO_OUT_LED_EN3);
            }
        }        
    }
    else if ( counter < 5U )
    {
        if ( ( LED_DUTY_25 == ledDutyEn[LED_OUT_0] ) || ( true == ledErrorIsPresent[0U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
        }
        if ( ( LED_DUTY_25 == ledDutyEn[LED_OUT_1] ) || ( true == ledErrorIsPresent[1U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
        }        
        if ( ( LED_DUTY_25 == ledDutyEn[LED_OUT_2] ) || ( true == ledErrorIsPresent[2U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
        }        
        if ( ( LED_DUTY_25 == ledDutyEn[LED_OUT_3] ) || ( true == ledErrorIsPresent[3U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
        }      
    }
    else if ( counter < 7U )
    {
        if ( ( LED_DUTY_50 == ledDutyEn[LED_OUT_0] ) || ( true == ledErrorIsPresent[0U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
        }
        if ( ( LED_DUTY_50 == ledDutyEn[LED_OUT_1] ) || ( true == ledErrorIsPresent[1U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
        }        
        if ( ( LED_DUTY_50 == ledDutyEn[LED_OUT_2] ) || ( true == ledErrorIsPresent[2U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
        }        
        if ( ( LED_DUTY_50 == ledDutyEn[LED_OUT_3] ) || ( true == ledErrorIsPresent[3U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
        }      
    }
    else 
    {
        if ( ( LED_DUTY_OFF != ledDutyEn[LED_OUT_0] ) || ( true == ledErrorIsPresent[0U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN0);
        }
        if ( ( LED_DUTY_OFF != ledDutyEn[LED_OUT_1] ) || ( true == ledErrorIsPresent[1U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN1);
        }        
        if ( ( LED_DUTY_OFF != ledDutyEn[LED_OUT_2] ) || ( true == ledErrorIsPresent[2U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN2);
        }        
        if ( ( LED_DUTY_OFF != ledDutyEn[LED_OUT_3] ) || ( true == ledErrorIsPresent[3U] ) )
        {
            S32K_SysPin_ClearOutputPins(GPIO_OUT_LED_EN3);
        }      
    }

    counter = counter + 1U;
    if ( counter >= 10U )
    {
        counter = 0U;
    }
    
    return;
}

// ------------------------------------------------------------
void LED_DutyInit( void )
{
    uint8_t i;
    
    for ( i=0U; i<LED_OUT_NUM; i++ )
    {
        ledDutyEn[i] = LED_DUTY_OFF;
    }
    
    return;
}
