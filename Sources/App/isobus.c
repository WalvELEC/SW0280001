/*
 *  isobus.c
 *
 *  Created on: 06 sep 2018
 *      Author: PoliD
 */

//#include <wtypes.h>
//#include <hjwConfig.h>
//#include <can.h>
//#include <saej1939.h>
//#include <application.h>
//#include <digital.h>
//#include <memdata.h>
//#include <isobus\isobus.h>
//#include <isobus\pool.h>
//

#include "application.h"
#include "S32K_SysPin.h"

#ifdef ISOBUS_DJW

//------------------------------------------------------------------------------
#define CAN_MSG_SAEJ_ADDRESS_CLAIM             ( 11U )
#define CAN_MSG_VT_TO_ECU                      ( 12U )

#define ISOBUS_VT_TO_ECU_MSG_RX_NO_MSG             ( 0U )
#define ISOBUS_VT_TO_ECU_MSG_RX_VT_STATUS          ( 1U )
#define ISOBUS_VT_TO_ECU_MSG_RX_GET_SOFTKEY_NUM    ( 2U )
#define ISOBUS_VT_TO_ECU_MSG_RX_GET_HARDWARE       ( 3U )
#define ISOBUS_VT_TO_ECU_MSG_RX_GET_TEXT_FONT      ( 4U )
#define ISOBUS_VT_TO_ECU_MSG_RX_GET_MEMORY         ( 5U )
#define ISOBUS_VT_TO_ECU_MSG_RX_LOAD_OP_VER        ( 6U )
#define ISOBUS_VT_TO_ECU_MSG_RX_STORE_OP_VER       ( 7U )
#define ISOBUS_VT_TO_ECU_MSG_RX_OP_END             ( 8U )
#define ISOBUS_VT_TO_ECU_MSG_RX_ATTRIB_CHANGE      ( 9U )
#define ISOBUS_VT_TO_ECU_MSG_RX_HIDE_SHOW_OBJECT   ( 10U )
#define ISOBUS_VT_TO_ECU_MSG_RX_AUX_IN_2_STATUS_EN ( 11U )
#define ISOBUS_VT_TO_ECU_MSG_RX_CHANGE_NUM_VALUE   ( 12U )

#define ISOBUS_VT_STARTUP_RETRY_CNT_TH    ( 3U )  //se ho un errore di inizializzazione tra WS e VT, riprovo questo numero di volte e poi rinuncio andando in errore
#define ISOBUS_VT_MSG_TX_RETRY_CNT_TH     ( 3U )  //se non ricevo un messaggio durante l'inizializzazione del VT, riprovo a rimandarlo questo numero di volte e poi rinuncio andando in errore

#define ISOBUS_VT_STATUS_MSG_RX_TIMEOUT_US           ( 3000000UL )
#define ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US           ( 1300000UL )
#define ISOBUS_WS_MAINTENANCE_MSG_TX_TIMEOUT_US      ( 1000000UL )
#define ISOBUS_AUX_IN_STATUS_MSG_TX_TIMEOUT_US       ( 50000UL )
#define ISOBUS_AUX_IN_MAINTENANCE_MSG_TX_TIMEOUT_US  ( 100000UL )

#define ISOBUS_ROLLER_NUM      ( 3U )
#define ISOBUS_AXIS_NUM        ( 5U )
#define ISOBUS_AUX_BUTTON_NUM_MAX  ( 23U )
#define ISOBUS_PUSH_BUTTON_NUM ( 14U )
#define ISOBUS_PHYSICAL_BUTTON_NUM ( 10U )
#define ISOBUS_LOGIC_DIRECTION_NUM ( 4U )

#define BOOL_DISABLED                          ( 0x00U )
#define BOOL_KEY_MODE                          ( 0x01U )
#define BOOL_UP                                ( 0x02U )
#define BOOL_ISO_LATCH                         ( 0x04U )
#define BOOL_ISO_NOT_LATCH                     ( 0x08U )

#define BOOL_SHIFTED_01                        ( 0x01U )
#define BOOL_SHIFTED_02                        ( 0x02U )

#define ANALOG_DISABLED                        ( 0x00U )
#define ANALOG_MAINTAIN_POSITION               ( 0x01U )
#define ANALOG_RETURN_TO_50                    ( 0x02U )
#define ANALOG_RETURN_TO_00                    ( 0x04U )
#define ANALOG_WITH_LATCH                      ( 0x08U )

#define BOOL_INPUT_UNUSED                      ( 0x0FU )
#define BOOL_INPUT_OFF                         ( 0x00U )
#define BOOL_INPUT_ON                          ( 0x01u )
#define BOOL_INPUT_AS_ROLLER                   ( 0x0AU )
#define BOOL_INPUT_TEMP_CHANGE_MODE            ( 0x0EU )

#define KEY_1          ( 0U )
#define KEY_2          ( 1U )
#define KEY_3          ( 2U )
#define KEY_4          ( 3U )
#define KEY_5          ( 4U )
#define KEY_6          ( 5U )
#define KEY_7          ( 6U )
#define KEY_8          ( 7U )
#define KEY_9          ( 8U )
#define KEY_1_M2       ( 9U )
#define KEY_2_M2       ( 10U )
#define KEY_3_M2       ( 11U )
#define KEY_4_M2       ( 12U )
#define KEY_5_M2       ( 13U )
#define KEY_6_M2       ( 14U )
#define KEY_7_M2       ( 15U )
#define KEY_8_M2       ( 16U )
#define KEY_9_M2       ( 17U )
#define KEY_MODE       ( 18U )
#define KEY_DIR_X_SX   ( 19U )
#define KEY_DIR_X_DX   ( 20U )
#define KEY_DIR_Y_BW   ( 21U )
#define KEY_DIR_Y_FW   ( 22U )

#define PHYS_KEY_00       ( 0U )
#define PHYS_KEY_01       ( 1U )
#define PHYS_KEY_02       ( 2U )
#define PHYS_KEY_03       ( 3U )
#define PHYS_KEY_04       ( 4U )
#define PHYS_KEY_05       ( 5U )
#define PHYS_KEY_06       ( 6U )
#define PHYS_KEY_07       ( 7U )
#define PHYS_KEY_08       ( 8U )
#define PHYS_KEY_09       ( 9U )
#define PHYS_KEY_10       ( 10U )
#define PHYS_KEY_11       ( 11U )
#define PHYS_KEY_12       ( 12U )
#define PHYS_KEY_13       ( 13U )

#define ISOBUS_BUTTON_00    ( PHYS_KEY_00 )
#define ISOBUS_BUTTON_01    ( PHYS_KEY_01 )
#define ISOBUS_BUTTON_02    ( PHYS_KEY_02 )
#define ISOBUS_BUTTON_03    ( PHYS_KEY_03 )
#define ISOBUS_BUTTON_04    ( PHYS_KEY_04 )
#define ISOBUS_BUTTON_05    ( PHYS_KEY_05 )
#define ISOBUS_BUTTON_06    ( PHYS_KEY_06 )
#define ISOBUS_BUTTON_07    ( PHYS_KEY_07 )
#define ISOBUS_BUTTON_08    ( PHYS_KEY_08 )
#define ISOBUS_BUTTON_09    ( PHYS_KEY_09 )
#define ISOBUS_X_SX_KEY_INPUT  ( PHYS_KEY_10 )
#define ISOBUS_X_DX_KEY_INPUT  ( PHYS_KEY_11 )
#define ISOBUS_Y_BWD_KEY_INPUT ( PHYS_KEY_12 )
#define ISOBUS_Y_FWD_KEY_INPUT ( PHYS_KEY_13 )

#define ISOBUS_KEY_STATUS_WAIT    ( 0U )
#define ISOBUS_KEY_STATUS_UP      ( 1U )
#define ISOBUS_KEY_STATUS_CLICK   ( 2U )
#define ISOBUS_KEY_STATUS_DOWN    ( 3U )
#define ISOBUS_KEY_STATUS_HOLD    ( 4U )
#define ISOBUS_KEY_STATUS_RELEASE ( 5U )

#define ISOBUS_KEY_MODE_STATUS_01 ( 0U )
#define ISOBUS_KEY_MODE_STATUS_02 ( 1U )

#define ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1 ( 0x00 )
#define ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_1 ( 0x01 )
#define ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_2 ( 0x02 )
#define ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_2 ( 0x03 )
#define ISOBUS_STATUS_CONTAINER_AXIS_CHECK          ( 0x04 )
#define ISOBUS_STATUS_CONTAINER_AXIS_CHECK_RX       ( 0x05 )
#define ISOBUS_STATUS_KEY_START                     ( 0x06 )
#define ISOBUS_STATUS_KEY_PRESS_CHECK               ( 0x07 )
#define ISOBUS_STATUS_KEY_PRESS_CHECK_RX            ( 0x08 )
//#define ISOBUS_STATUS_KEY_HEADER_START              ( 0x07 )
#define ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK        ( 0x09 )
#define ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK_RX     ( 0x0A )
#define ISOBUS_STATUS_KEY_PRESS_0                   ( 0x0B )   
#define ISOBUS_STATUS_KEY_PRESS_0_RX                ( 0x0C )  
#define ISOBUS_STATUS_KEY_MODE_OFF                  ( 0x0D )
#define ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_POINTER ( 0x0E )
#define ISOBUS_STATUS_KEY_MODE_ON_RX_POINTER        ( 0x0F )
#define ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_RECT    ( 0x10 )
#define ISOBUS_STATUS_KEY_MODE_ON_RX_RECT           ( 0x11 )
#define ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE        ( 0x12 )
#define ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE_RX     ( 0x13 )
#define ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE        ( 0x14 )
#define ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE_RX     ( 0x15 )
#define ISOBUS_STATUS_KEY_MODE_UP_CHECK             ( 0x16 )
#define ISOBUS_STATUS_KEY_MODE_UP_CHECK_RX          ( 0x17 )
#define ISOBUS_STATUS_KEY_MODE_LATCH_CHECK          ( 0x18 )
#define ISOBUS_STATUS_KEY_MODE_LATCH_CHECK_RX       ( 0x19 )
#define ISOBUS_STATUS_LINEBAR_START                 ( 0x1A )
#define ISOBUS_STATUS_LINEBAR_CHECK                 ( 0x1B )
#define ISOBUS_STATUS_LINEBAR_CHECK_RX              ( 0x1C )  

//------------------------------------------------------------------------------
#define ISOBUS_LED_OFF   ( SAEJ_LED_OFF     )
#define ISOBUS_LED_ON    ( SAEJ_LED_0_ON    )   
#define ISOBUS_LED_BLINK ( SAEJ_LED_0_BLINK ) 

#define ISOBUS_LED_KEY_MODE  ( SAEJ_LED_3_INDEX )   //rosso   ISOBUS_LED_SUPPLY_ON
#define ISOBUS_LED_OP        ( SAEJ_LED_2_INDEX )   //orange  ISOBUS_LED_OP 
#define ISOBUS_LED_SUPPLY_ON ( SAEJ_LED_1_INDEX )   //green   ISOBUS_LED_KEY_MODE

//------------------------------------------------------------------------------
#define ISOBUS_P_ENABLE  ( 0x01U )
#define ISOBUS_P_DISABLE ( 0x00U )

#define ISBOUS_IN_ON     ( 0x01U )
#define ISOBUS_IN_OFF    ( 0x00U )

//------------------------------------------------------------------------------
#define ISOBUS_AUX_IN_ENABLE    ( 1U )
#define ISOBUS_AUX_IN_DISABLE   ( 0U )

#define ISOBUS_VT_LEARN_MODE_ON ( 0x40U )

//------------------------------------------------------------------------------
typedef struct isobus_instance_e
{
    w_status_t status;  
    uint32_t startupCnt;
} isobus_instance_t;

//------------------------------------------------------------------------------
typedef struct isobus_vt_e
{
    w_status_t status;  
    wres_t error;        
    uint8_t busy;  
    uint8_t learnModeStatus;
    uint8_t sourceAddr;
    uint8_t preferredAddr;
    uint8_t virtualKeysNum;
    uint8_t physicalKeysNum;
    uint8_t keysXPixel;
    uint8_t keysYPixel;
    uint8_t bootTime;
    uint8_t graphic;
    uint8_t hardware;
    uint16_t xPixel;
    uint16_t yPixel;
    uint8_t isoVersion;
    uint8_t memStatus;
    uint8_t ownerSA;
    uint32_t ownerID;
    uint8_t function;
    uint8_t fontSmall;
    uint8_t fontLarge;
    uint8_t typeAttribute;
    uint8_t objectPoolReady;
    uint8_t startupRetryCntTh;
    uint8_t startupRetryCnt;
    uint8_t msgRetryCntTh;
    uint8_t msgRetryCnt;    
} isobus_vt_t;

//------------------------------------------------------------------------------
typedef struct isobus_auxInput_e
{
    uint16_t ID;
    uint8_t type;
    uint16_t value1;
    uint16_t value2;
    uint16_t value1Pre;
    uint16_t value2Pre;
    uint8_t status;
    uint8_t enabled;
    uint8_t learnMode;
    uint8_t indexInput;
    uint8_t indexInput2;
    uint16_t objPointer;
    uint32_t cntTh;
    uint32_t cnt;
} isobus_auxInput_t;

//------------------------------------------------------------------------------
typedef struct isobus_key_e
{
    uint8_t input;
    uint8_t output;
    uint8_t auxValue;
    uint16_t transitionCnt;
    uint32_t cnt;
    uint32_t cntTh;
    uint8_t pressed;
    uint8_t pressedPrevious;
    uint8_t pressChangeValue;
} isobus_key_t;

//------------------------------------------------------------------------------
typedef struct isobus_analog_e
{
    uint16_t value;
    uint8_t latchEn;
    uint8_t latchOtherDir;
    uint8_t latchShow;
    uint8_t latchShowPrevious;
} isobus_analog_t;

//------------------------------------------------------------------------------
static s_TxRxCANMsg CANMsgTx_Isobus_workingSetInit;
static s_TxRxCANMsg CANMsgTx_Isobus_workingSetMaintenance;
static s_TxRxCANMsg CANMsgTx_Isobus_auxInMaintenance;
static s_TxRxCANMsg CANMsgTx_Isobus_EcuToVt;
static s_TxRxCANMsg CANMsgTx_Isobus_auxInStatusMsg;
static s_TxRxCANMsg CANMsgTx_Isobus_auxInStatusEnMsg;

static isobus_instance_t isobus_Net;
static isobus_vt_t isobus_Vt;

static uint16_t Isobus_manufacturerIdCode;

static uint8_t Isobus_tmpPool[SAEJ_TP_BYTE_NUM_MAX];

//provvisori
static uint16_t ObjectID;
static uint16_t AIDValue;
static uint8_t Isobus_AttributeId;

static uint8_t FontStyle;
static uint8_t FontSize;

static uint8_t OPStepNumMax;
static uint8_t NumPictureObject;
static uint8_t StepGraphAttributesValue;
static uint8_t StepAuxInputValue;
static uint8_t StepObjectPointerValue;
static uint8_t StepContainer;
static uint8_t StepWSDM;
static uint32_t Isobus_opByteNum;
static uint32_t Isobus_opPacketNum;
static uint16_t stringWidthScaled;
static uint32_t ContainerShow;
static uint8_t ContainerP0Show;
static uint8_t ContainerAnShow;
static const uint8_t *poolBufferIndex;
static uint16_t Isobus_functionalities;

static uint8_t AuxInAnNum;
static uint8_t AuxInBoolNum;
static uint8_t AuxInNum;
static uint16_t CfgAuxAn[AUX_IN_AN_NUM_MAX];
static uint16_t CfgAuxBool[ISOBUS_AUX_BUTTON_NUM_MAX];
static uint8_t CfgkeyIndex[ISOBUS_PUSH_BUTTON_NUM];
static uint8_t Isobus_keyModeIndex;
static uint8_t Isobus_keyDeadManIndex;
//static uint8_t Isobus_keyLatchIndex;
static bool Isobus_deadManEn;
static uint8_t Isobus_deadManShow;
static uint8_t Isobus_deadManShowPrevious;
static uint8_t Isobus_keyModeStatus;
static uint8_t Isobus_keyModeStatusPre;
static uint8_t Isobus_keyModeStatusChange;
static uint8_t KeyChangeCounter;
static uint8_t KeyChangeValue;
static uint8_t ChangeAttribOk;
static uint8_t ChangeAttribRequest;
static uint8_t ChangeAttribNumTx;
static uint8_t HideShowRequest;
static uint8_t LineBarChangeCounter;
static uint8_t ChangeNumValueRequest;

static uint16_t Isobus_cfgAxes;
static uint16_t Isobus_cfgPushButton;
//static uint16_t Isobus_cfgDetent;
static uint8_t Isobus_pushButtonIndex[ISOBUS_PUSH_BUTTON_NUM];
static uint8_t Isobus_pushButtonEn[ISOBUS_PUSH_BUTTON_NUM];
//static uint8_t Isobus_rollerIndex[ISOBUS_ROLLER_NUM];
static isobus_auxInput_t AuxInput[AUX_INPUT_NUM_MAX];
static isobus_key_t Isobus_key[ISOBUS_PUSH_BUTTON_NUM];
static isobus_key_t Isobus_keyAux[ISOBUS_AUX_BUTTON_NUM_MAX];
static isobus_analog_t Isobus_analogAux[ISOBUS_AXIS_NUM];

static uint8_t Isobus_certification[8U];

static uint16_t LineBarSetPoint[4U];
static uint8_t ContainerAxisShow[4U];

static uint8_t isobus_LedOut[LED_OUT_NUM];

static uint8_t isobus_MsgRxCounter;
static bool isobus_LedMsgDrive;
static bool isobus_KeyModeFirstEntry;

static bool IsobusTestEnable;
static bool IsobusVTSymEnable;

static uint8_t isobus_Label[7u];

//------------------------------------------------------------------------------
//const uint8_t Isobus_FunctionalitiesBuff[13U] = 
//{
//    0xFFU, 0x03U, 0x00U, 0x01U, 0x00U, 0x01U, 0x02U,
//    0x00U, 0x05U, 0x01U, 0x02U, 0x00U, 0x00U,
//};
const uint8_t Isobus_FunctionalitiesBuff[13U] = 
{
    0xFFU, 
    0x03U, //n° di functionalities
    0x00U, //minimum control function
    0x02U, //mimimum control function generation 2 (2.0)
    0x00U, //minimum control function byte option n° 
    0x02U, //universal terminal working set (client)
    0x02U, //universal terminal generation (2.0)
    0x00U, //universal terminal byte option n°
    0x05U, //Aux-N input
    0x01U, //Aux-N input generation (1.0)
    0x02U, //Aux-N input byte opton n°
    0x00U, //Aux-N input option byte 1
    0x00U, //Aux-N input option byte 2
};

////------------------------------------------------------------------------------
//void CAN0_7_IRQHandler( void )
//{
//    //quando ricevo il messaggio associato a questo IRQ eseguo questo codice
//
//    /* per test
//    if ( CAN_NEW_MSG == CAN_MsgRx( &CANMsgRx_Isobus_Test ) )
//    {
//        CANMsgTx_Isobus_Test.Data[0U]++;
//        CAN_MsgUpdateData( &CANMsgTx_Isobus_Test, &CANMsgTx_Isobus_Test.Data[0U], CAN_MSG_TX_YES );
//        CAN_MsgTx( &CANMsgTx_Isobus_Test );
//        CANMsgRx_Isobus_Test.NewMsg = CAN_NO_MSG;
//    }
//    */
//    return;
//}
//
////------------------------------------------------------------------------------
//void CAN0_6_IRQHandler( void )
//{
//    //quando trasmetto il messaggio associato a questo IRQ eseguo questo codice
//  
//    /* per test
//    static uint32_t i;
//    
//    i++;
//    */
//    
//    return;
//}

//------------------------------------------------------------------------------
void Isobus_WorkingSetInitMsgTx( void )
{
    CANMsgTx_Isobus_workingSetInit.Data[0U] = 0x01U;
    CAN_MsgUpdateData( &CANMsgTx_Isobus_workingSetInit, &CANMsgTx_Isobus_workingSetInit.Data[0U], CAN_MSG_TX_YES );
    CAN_MsgTx( &CANMsgTx_Isobus_workingSetInit, false, 0xFF );
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_WorkingSetMaintenanceMsgTx( void )
{
    uint8_t txEn;  
  
    if ( ISOBUS_NET_WS_START_STATUS == isobus_Net.status.value )
    {
        CANMsgTx_Isobus_workingSetMaintenance.Data[1U] = 0x01U;
        txEn = CAN_MSG_TX_YES;
    }
    else
    {
        CANMsgTx_Isobus_workingSetMaintenance.Data[1U] = 0x00U;
        txEn = CAN_MSG_TX_NO;
    }
    CANMsgTx_Isobus_workingSetMaintenance.Data[2U] = 0x03U;
    CAN_MsgUpdateData( &CANMsgTx_Isobus_workingSetMaintenance, &CANMsgTx_Isobus_workingSetMaintenance.Data[0U], txEn );
    CAN_MsgTx( &CANMsgTx_Isobus_workingSetMaintenance, false, 0xFF );
    if ( ISOBUS_NET_WS_START_STATUS == isobus_Net.status.value )
    {
        CANMsgTx_Isobus_workingSetMaintenance.TimeCnt = 0u;
    }
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AuxInMaintenanceMsgTx( void )
{
    CANMsgTx_Isobus_auxInMaintenance.Data[0U] = 0x23U;
    CANMsgTx_Isobus_auxInMaintenance.Data[1U] = (uint8_t)( Isobus_manufacturerIdCode );
    CANMsgTx_Isobus_auxInMaintenance.Data[2U] = (uint8_t)( Isobus_manufacturerIdCode >> 8U );
    CANMsgTx_Isobus_auxInMaintenance.Data[3U] = isobus_Vt.objectPoolReady;
    CAN_MsgUpdateData( &CANMsgTx_Isobus_auxInMaintenance, &CANMsgTx_Isobus_auxInMaintenance.Data[0U], CAN_MSG_TX_NO );
    CAN_MsgTx( &CANMsgTx_Isobus_auxInMaintenance, false, 0xFF );

    return;    
}

//------------------------------------------------------------------------------
static void Isobus_AuxInStatusMsgTx( uint8_t auxIndex )
{
    if ( AuxInput[auxIndex].learnMode == 1U )
    {
        CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInStatusMsg, ( ( ISOBUS_ECU_TO_VT_MSG_ID & 0xFFFF0000UL ) + (uint32_t)( Saej_SourceAddressGet() ) + (uint32_t)( isobus_Vt.sourceAddr << 8U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
	if ( AuxInput[auxIndex].value1 != AuxInput[auxIndex].value1Pre ) 
        {
	    AuxInput[auxIndex].status = 3U;
        } 
        else 
        {
	    AuxInput[auxIndex].status = 1U;
        }        
    }
    else
    {
        CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInStatusMsg, ( ( ISOBUS_VT_TO_ECU_MSG_ID & 0xFFFF0000UL ) + (uint32_t)( Saej_SourceAddressGet() ) + 0x0000FF00UL ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );      
        AuxInput[auxIndex].status = 0U;
    }        
    CANMsgTx_Isobus_auxInStatusMsg.Data[0U] = ISOBUS_AUX_IN_2_STATUS_MSG;
    CANMsgTx_Isobus_auxInStatusMsg.Data[1U] = (uint8_t)( AuxInput[auxIndex].ID );
    CANMsgTx_Isobus_auxInStatusMsg.Data[2U] = (uint8_t)( AuxInput[auxIndex].ID >> 8U );
    CANMsgTx_Isobus_auxInStatusMsg.Data[3U] = (uint8_t)( AuxInput[auxIndex].value1 );
    CANMsgTx_Isobus_auxInStatusMsg.Data[4U] = (uint8_t)( AuxInput[auxIndex].value1 >> 8U );
    CANMsgTx_Isobus_auxInStatusMsg.Data[5U] = (uint8_t)( AuxInput[auxIndex].value2 );
    CANMsgTx_Isobus_auxInStatusMsg.Data[6U] = (uint8_t)( AuxInput[auxIndex].value2 >> 8U );    
    CANMsgTx_Isobus_auxInStatusMsg.Data[7U] = (uint8_t)( AuxInput[auxIndex].status );        
    CAN_MsgUpdateData( &CANMsgTx_Isobus_auxInStatusMsg, &CANMsgTx_Isobus_auxInStatusMsg.Data[0U], CAN_MSG_TX_YES );
    CAN_MsgTx( &CANMsgTx_Isobus_auxInStatusMsg, false, 0xFF );    

    return;
}

//------------------------------------------------------------------------------
static wres_t Isobus_AuxInStatusUpdate( uint16_t auxInID, uint8_t auxInEn )
{
    wres_t retVal;
    uint8_t i;
    uint8_t j;
    
    retVal = WRES_ERROR;
    
    if ( 2U < isobus_Vt.isoVersion )
    {
        if ( 0xFFFFU == auxInID )
        {
            if ( 0U == auxInEn )
            {
                for ( i=0U; i<AuxInNum; i++ )
                {
                    AuxInput[i].enabled = 0U;
                }
                retVal = WRES_OK;
            }
        }
        else
        {
            j=0xFFU;
            for ( i=0U; i<AuxInNum; i++ )
            {
                if ( auxInID == AuxInput[i].ID )
                {
                    j=i;
                    break;
                }
            }
            if ( 0xFFU != j )
            {
                AuxInput[j].enabled = auxInEn;
                retVal = WRES_OK;
            }
        }
    }

    return retVal;
}

//------------------------------------------------------------------------------
static uint8_t Isobus_EcutToVtMsgTx( uint8_t command, uint16_t optArg1, uint16_t optArg2 ) 
{
    uint8_t retVal;
    uint32_t aux;
    uint8_t i;
    uint8_t j;

    retVal = 0U;    
    if ( CAN_NO_MSG == CANMsgTx_Isobus_EcuToVt.NewMsg )
    {
        memset( CANMsgTx_Isobus_EcuToVt.Data, 0xFFU, 8U );      
        CANMsgTx_Isobus_EcuToVt.Data[0U] = command;
        switch ( command )
        {
        case ISOBUS_GET_SOFTKEY_NUM:
        case ISOBUS_GET_TEXT_FONT:
        case ISOBUS_GET_HARDWARE:
        case ISOBUS_OP_END:
        case ISOBUS_OP_DEL:
            retVal = 1U;
            break;
        case ISOBUS_GET_MEMORY:
            aux = SIZE_OF_WS_DM + SIZE_OF_CONTAINER + SIZE_OF_OUT_STRINGS + SIZE_OF_OUT_RECTS + SIZE_OF_PATTERNS + SIZE_OF_GRAPH_ATTRIBUTES + SIZE_OF_AUX_INPUT + SIZE_OF_OBJ_POINTER + SIZE_OF_PICTURE_256;
            CANMsgTx_Isobus_EcuToVt.Data[2U] = (uint8_t)( aux );
            CANMsgTx_Isobus_EcuToVt.Data[3U] = (uint8_t)( aux >> 8U );
            CANMsgTx_Isobus_EcuToVt.Data[4U] = (uint8_t)( aux >> 16U );
            CANMsgTx_Isobus_EcuToVt.Data[5U] = (uint8_t)( aux >> 24U );
            retVal = 1U;
            break;
        case ISOBUS_STORE_OP_VER:
        case ISOBUS_LOAD_OP_VER:
            aux = Isobus_ChecksumGet();
            CANMsgTx_Isobus_EcuToVt.Data[1U] = isobus_Label[0u];
            CANMsgTx_Isobus_EcuToVt.Data[2U] = isobus_Label[1u];
            CANMsgTx_Isobus_EcuToVt.Data[3U] = isobus_Label[2u];
            CANMsgTx_Isobus_EcuToVt.Data[4U] = isobus_Label[3u];
            CANMsgTx_Isobus_EcuToVt.Data[5U] = isobus_Label[4u];
            CANMsgTx_Isobus_EcuToVt.Data[6U] = isobus_Label[5u];
            CANMsgTx_Isobus_EcuToVt.Data[7U] = isobus_Label[6u]; 
            retVal = 1U;
            break;
        case ISOBUS_DELETE_OP_VER:
        case ISOBUS_GET_VERSION:
        case ISOBUS_OP_TRANSFER:
        case ISOBUS_CHILD_LOC_CHANGE:
        case ISOBUS_CHILD_POS_CHANGE:
        case ISOBUS_BACK_COLOR_CHANGE:
            retVal = 2U;
            break;
        case ISOBUS_ATTRIB_CHANGE:
            CANMsgTx_Isobus_EcuToVt.Data[1U] = (uint8_t)( ObjectID );
            CANMsgTx_Isobus_EcuToVt.Data[2U] = (uint8_t)( ObjectID >> 8U ); 
            CANMsgTx_Isobus_EcuToVt.Data[3U] = (uint8_t)( Isobus_AttributeId );
            CANMsgTx_Isobus_EcuToVt.Data[4U] = (uint8_t)( AIDValue ); 
            CANMsgTx_Isobus_EcuToVt.Data[5U] = (uint8_t)( AIDValue >> 8U );
            CANMsgTx_Isobus_EcuToVt.Data[6U] = 0U; 
            CANMsgTx_Isobus_EcuToVt.Data[7U] = 0U; 
            retVal = 1U;
            break;
        case ISOBUS_HIDE_SHOW_OBJECT:
            CANMsgTx_Isobus_EcuToVt.Data[1U] = (uint8_t)( ObjectID );
            CANMsgTx_Isobus_EcuToVt.Data[2U] = (uint8_t)( ObjectID >> 8U ); 
            CANMsgTx_Isobus_EcuToVt.Data[3U] = (uint8_t)( AIDValue );
            retVal = 1U;          
            break;
        case ISOBUS_AUX_IN_2_STATUS_EN:
            CANMsgTx_Isobus_EcuToVt.Data[1U] = (uint8_t)( optArg1 );
            CANMsgTx_Isobus_EcuToVt.Data[2U] = (uint8_t)( optArg1 >> 8U );
            if ( 0xFFFFU == optArg1 )
            {
                if ( 0U == (uint8_t)( optArg2 ) )
                {
                    CANMsgTx_Isobus_EcuToVt.Data[3U] = 0U;
                    CANMsgTx_Isobus_EcuToVt.Data[4U] = 0U;
                }
                else
                {
                    CANMsgTx_Isobus_EcuToVt.Data[3U] = (uint8_t)( optArg2 );
                    CANMsgTx_Isobus_EcuToVt.Data[4U] = 1U;                  
                }
            }
            else
            {
                j=0xFFU;
                for ( i=0U; i<AuxInNum; i++ )
                {
                    if ( optArg1 == AuxInput[i].ID )
                    {
                        j=i;
                        break;
                    }
                }
                if ( 0xFFU != j )
                {
                    CANMsgTx_Isobus_EcuToVt.Data[3U] = AuxInput[j].enabled;
                    CANMsgTx_Isobus_EcuToVt.Data[4U] = 0U;
                }
                else
                {
                    CANMsgTx_Isobus_EcuToVt.Data[3U] = 0U;
                    CANMsgTx_Isobus_EcuToVt.Data[4U] = 1U;
                }
            }
            retVal = 1U;
            break;
        case ISOBUS_CHANGE_NUM_VALUE:
            CANMsgTx_Isobus_EcuToVt.Data[1U] = (uint8_t)( ObjectID );
            CANMsgTx_Isobus_EcuToVt.Data[2U] = (uint8_t)( ObjectID >> 8U ); 
            CANMsgTx_Isobus_EcuToVt.Data[3U] = 0xFF;
            CANMsgTx_Isobus_EcuToVt.Data[4U] = (uint8_t)( AIDValue );
            CANMsgTx_Isobus_EcuToVt.Data[5U] = (uint8_t)( AIDValue >> 8U);
            CANMsgTx_Isobus_EcuToVt.Data[6U] = 0x00U;
            CANMsgTx_Isobus_EcuToVt.Data[7U] = 0x00U;
            retVal = 1U;          
            break;
        default:
            break;
        }
        if ( 1U == retVal )
        {
            CAN_MsgUpdateData( &CANMsgTx_Isobus_EcuToVt, &CANMsgTx_Isobus_EcuToVt.Data[0U], CAN_MSG_TX_YES );
            CAN_MsgTx( &CANMsgTx_Isobus_EcuToVt, false, 0xFF );
        }
    }
    return retVal;
}

//------------------------------------------------------------------------------
static uint8_t Isobus_AuxInStatusEnMsgTx( uint16_t auxInId, uint16_t auxInEn ) 
{
    uint8_t retVal;
    uint8_t i;
    uint8_t j;

    retVal = 0U;    
    if ( CAN_NO_MSG == CANMsgTx_Isobus_auxInStatusEnMsg.NewMsg )
    {
        memset( CANMsgTx_Isobus_auxInStatusEnMsg.Data, 0xFFU, 8U );      
        CANMsgTx_Isobus_auxInStatusEnMsg.Data[0U] = ISOBUS_AUX_IN_2_STATUS_EN;
        CANMsgTx_Isobus_auxInStatusEnMsg.Data[1U] = (uint8_t)( auxInId );
        CANMsgTx_Isobus_auxInStatusEnMsg.Data[2U] = (uint8_t)( auxInId >> 8U );
        if ( 0xFFFFU == auxInId )
        {
            if ( 0U == (uint8_t)( auxInEn ) )
            {
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[3U] = 0U;
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[4U] = 0U;
            }
            else
            {
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[3U] = (uint8_t)( auxInEn );
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[4U] = 1U;                  
            }
        }
        else
        {
            j=0xFFU;
            for ( i=0U; i<AuxInNum; i++ )
            {
                if ( auxInId == AuxInput[i].ID )
                {
                    j=i;
                    break;
                }
            }
            if ( 0xFFU != j )
            {
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[3U] = AuxInput[j].enabled;
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[4U] = 0U;
            }
            else
            {
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[3U] = 0U;
                CANMsgTx_Isobus_auxInStatusEnMsg.Data[4U] = 1U;
            }
        }
        CAN_MsgUpdateData( &CANMsgTx_Isobus_auxInStatusEnMsg, &CANMsgTx_Isobus_auxInStatusEnMsg.Data[0U], CAN_MSG_TX_YES );
        CAN_MsgTx( &CANMsgTx_Isobus_auxInStatusEnMsg, false, 0xFF );
        retVal = 1U;
    }
    return retVal;
}

//------------------------------------------------------------------------------
static uint8_t Isobus_VtToEcuMsgRxCheck( uint8_t msgIndex, wres_t *retValOut )
{
    uint8_t retVal;
    uint16_t auxInputID;
    uint8_t auxInputEn;
    
    retVal = ISOBUS_VT_TO_ECU_MSG_RX_NO_MSG;
    *retValOut = WRES_ERROR;

    if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_VT_TO_ECU, msgIndex ).DA )
    {
        if ( 0xFEU == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) ) 
        {
            //Rx di VT status message
            isobus_Vt.ownerSA = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U );
            isobus_Vt.ownerID = (uint32_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) ) + (uint32_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U ) << 8U )
                              + (uint32_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 4U ) << 16U ) + (uint32_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U ) << 24U );
            isobus_Vt.busy = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 6U );
            isobus_Vt.learnModeStatus = ( isobus_Vt.busy & ISOBUS_VT_LEARN_MODE_ON ) >> 6U;
            isobus_Vt.function = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 7U ); 
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_VT_STATUS;
            *retValOut = WRES_OK;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif
        }
    }
    else if ( Saej_SourceAddressGet() == CAN_StoredMsgGet( CAN_MSG_VT_TO_ECU, msgIndex ).DA )
    {
        if ( ISOBUS_GET_SOFTKEY_NUM == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            isobus_Vt.keysXPixel = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 4U );
            isobus_Vt.keysYPixel = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U );
            //isobus_Vt.keysXPixel = 120U;
            //isobus_Vt.keysYPixel = 80U;
            isobus_Vt.virtualKeysNum = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 6U );
            isobus_Vt.physicalKeysNum = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 6U );      
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_GET_SOFTKEY_NUM;
            *retValOut = WRES_OK;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_GET_HARDWARE == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            isobus_Vt.bootTime = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U );
	    isobus_Vt.graphic = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U );
            // per debug pool colori
            //isobus_Vt.graphic = 0U;
            //isobus_Vt.graphic = 1U;
            //
            isobus_Vt.hardware = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U );
            isobus_Vt.xPixel = (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 4U ) ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U ) << 8U );
            isobus_Vt.yPixel = (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 6U ) ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 7U ) << 8U );
            /* per test scalatura DP   
            //isobus_Vt.xPixel = 200U;
            //isobus_Vt.yPixel = 200U;
            isobus_Vt.xPixel = 600U;
            isobus_Vt.yPixel = 600U;            
            */
            
            if ( 0x00U == isobus_Vt.graphic ) {
                NumPictureObject = NumPictureObject + 2U;
            }
            StepGraphAttributesValue = ISOBUS_OP_LAST_STEP_BEFORE_PICS + NumPictureObject + 1U;
            StepAuxInputValue = StepGraphAttributesValue + 1U;
            StepObjectPointerValue = StepAuxInputValue + 1U;
            StepContainer = StepObjectPointerValue + 1U;
            StepWSDM = StepContainer + 1U;
            OPStepNumMax = StepWSDM + 1U;
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_GET_HARDWARE;
            *retValOut = WRES_OK;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_GET_TEXT_FONT == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
	    isobus_Vt.fontSmall = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U );
  	    isobus_Vt.fontLarge = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 6U );
  	    isobus_Vt.typeAttribute = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 7U );
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_GET_TEXT_FONT;
            *retValOut = WRES_OK;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_GET_MEMORY == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( 0U == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) )
            {
                *retValOut = WRES_OK;
            }
            isobus_Vt.isoVersion = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U );
            isobus_Vt.memStatus = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U );          
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_GET_MEMORY;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_LOAD_OP_VER == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( 0U == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U ) )
            {
                *retValOut = WRES_OK;
            }
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_LOAD_OP_VER;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_STORE_OP_VER == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( 0U == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 5U ) )
            {
                *retValOut = WRES_OK;
            }          
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_STORE_OP_VER;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_OP_END == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( 0U == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U ) )
            {
                *retValOut = WRES_OK;
            }          
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_OP_END;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }  
        else if ( ISOBUS_ATTRIB_CHANGE == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( ( (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U ) & 0x00FFU ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) << 8U ) ) == ObjectID ) 
            {
                if ( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U ) == Isobus_AttributeId ) 
                {
                    *retValOut = WRES_OK;
                }
            }
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_ATTRIB_CHANGE;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_HIDE_SHOW_OBJECT == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( ( (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U ) & 0x00FFU ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) << 8U ) ) == ObjectID ) 
            {
                if ( ( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U ) == AIDValue ) && ( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 4U ) == 0x00 ) ) 
                {
                    *retValOut = WRES_OK;  
                }
            } 
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_HIDE_SHOW_OBJECT;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else if ( ISOBUS_CHANGE_NUM_VALUE == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            if ( ( (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U ) & 0x00FFU ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) << 8U ) ) == ObjectID ) 
            {
                if ( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U ) ==  0x00 ) 
                {
                    *retValOut = WRES_OK;  
                }
            } 
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_CHANGE_NUM_VALUE;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }        
        else if ( ISOBUS_AUX_IN_2_STATUS_EN == CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 0U ) )
        {
            auxInputID = (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 1U ) & 0x00FFU ) + (uint16_t)( CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 2U ) << 8U );
            auxInputEn = CAN_StoredMsgGetData( CAN_MSG_VT_TO_ECU, msgIndex, 3U );
            *retValOut = Isobus_AuxInStatusUpdate( auxInputID, auxInputEn );
            Isobus_AuxInStatusEnMsgTx( auxInputID, (uint16_t)( auxInputEn & 0x00FFU ) );
            //Isobus_EcutToVtMsgTx( ISOBUS_AUX_IN_2_STATUS_EN, auxInputID, (uint16_t)( auxInputEn & 0x00FFU ) );
            retVal = ISOBUS_VT_TO_ECU_MSG_RX_AUX_IN_2_STATUS_EN;
#ifdef ISOBUS_CAN_RX_COUNTER
            Isobus_MsgRxCounterUpdate();
#endif            
        }
        else
        {
        }
    }
    else
    {
    }
  
    return retVal;
}

//------------------------------------------------------------------------------            
void Isobus_PoolObjectFieldScale( uint16_t ref_x, uint16_t ref_y, uint16_t SizeX, uint16_t SizeY,  uint16_t DefaultSize, uint8_t checkX, uint8_t checkY)
{
    uint32_t aux;
    uint32_t aux1;
    uint32_t aux2;

    uint16_t UsedSize;

    UsedSize = SizeY;
    if ( SizeY > SizeX ) 
    { 
        UsedSize = SizeX; 
    }

    aux = (uint32_t)( Isobus_tmpPool[ref_x] );
    aux = aux + (uint32_t)( Isobus_tmpPool[ref_x+1U] << 8U );

    if ( checkX ) 
    {
        aux1 = (uint32_t)( DefaultSize / 2U );
        aux2 = (uint32_t)( SizeX / 2U );
        aux1 = aux + aux1;
        if ( aux1 >= aux2 )
        {
            aux1 = aux1 - aux2;
        }
        else
        {
            aux1 = aux2 - aux1;
            aux1 = aux1 & 0x0000FFFFU;
            aux1 = 65536U - aux1;
        }
    } 
    else 
    {
        aux = aux * (uint32_t)( UsedSize );
        aux1 = aux / (uint32_t)( DefaultSize );
        aux2 = aux % (uint32_t)( DefaultSize );
        if ( aux2 > ( (uint32_t)( DefaultSize / 2U ) ) )
        { 
            aux1++; 
        }
    }

    Isobus_tmpPool[ref_x] = (uint8_t)( aux1 );
    Isobus_tmpPool[ref_x+1U] = (uint8_t)( ( aux1 & 0xFF00 ) >> 8U );

    aux = (uint32_t)( Isobus_tmpPool[ref_y] );
    aux = aux + (uint32_t)( Isobus_tmpPool[ref_y+1U] << 8U );

    if ( checkY ) 
    {
        aux1 = (uint32_t)( DefaultSize / 2U );
        aux2 = (uint32_t)( SizeY / 2U );
        aux1 = aux + aux1;
        if ( aux1 >= aux2 )
        {
            aux1 = aux1 - aux2;
        }
        else
        {
            aux1 = aux2 - aux1;
            aux1 = aux1 & 0x0000FFFFU;
            aux1 = 65536U - aux1;
        }
    } 
    else 
    {
        aux = aux * (uint32_t)( UsedSize );
        aux1 = aux / (uint32_t)( DefaultSize );
        aux2 = aux % (uint32_t)( DefaultSize );
        if ( aux2 > ( DefaultSize / 2U ) ) 
        { 
            aux1++; 
        }
    }

    Isobus_tmpPool[ref_y] = (uint8_t)( aux1 );
    Isobus_tmpPool[ref_y+1U] = (uint8_t)( ( aux1 >> 8U ) & 0x000000FFU );

    return; 
}

//------------------------------------------------------------------------------            
void Isobus_PoolObjectSingleScale( uint16_t ref, uint16_t Size, uint16_t DefaultSize )
{
    uint32_t aux;
    uint32_t aux1;
    uint32_t aux2;

    aux = (uint32_t)( Isobus_tmpPool[ref] );
    aux = aux + (uint32_t)( Isobus_tmpPool[ref+1] << 8U );
    aux = aux * (uint32_t)( Size );
    aux1 = aux / (uint32_t)( DefaultSize );
    aux2 = aux % (uint32_t)( DefaultSize );
    if ( aux2 > (uint32_t)( DefaultSize / 2U ) ) 
    { 
        aux1++; 
    }

    Isobus_tmpPool[ref] = (uint8_t)( aux1 );
    Isobus_tmpPool[ref+1U] = (uint8_t)( ( aux1 & 0xFF00 ) >> 8U );
    
    return;

}

//------------------------------------------------------------------------------            
void Isobus_PoolObjectFieldChange(uint16_t ref, uint16_t value)
{
    Isobus_tmpPool[ref] = (uint8_t)( value );
    Isobus_tmpPool[ref+1U] = (uint8_t)( (uint16_t)(value & 0xFF00U ) >> 8U );
    
    return;
}

//------------------------------------------------------------------------------            
void Isobus_PoolMeasureScale( void )
{
    uint8_t fontaux;

    fontaux = stringWidthScaled / 8U;

    FontSize = FONT_6_8_SIZE;
    FontStyle = 0U;

    //-- scalatura dimensione del font
    if ( fontaux >= 8U ) 
    {
        if ( fontaux < 12U ) 
        {
            if ( FONT_8_8_EN == ( isobus_Vt.fontSmall & FONT_8_8_EN ) ) 
            {
                FontSize = FONT_8_8_SIZE;
            }
            if ( isobus_Vt.keysYPixel > 12U ) 
            {
                if ( FONT_8_12_EN == ( isobus_Vt.fontSmall & FONT_8_12_EN ) ) 
                {
                    FontSize = FONT_8_12_SIZE;
                }
            }
        } 
        else if ( fontaux < 16U ) 
        {
            if ( FONT_12_16_EN == ( isobus_Vt.fontSmall & FONT_12_16_EN ) ) 
            {
                FontSize = FONT_12_16_SIZE;
            }
        } 
        else if ( fontaux < 24U ) 
        {
            if ( FONT_16_16_EN == ( isobus_Vt.fontSmall & FONT_16_16_EN ) ) 
            {
                FontSize = FONT_16_16_SIZE;
            }
            if ( isobus_Vt.keysYPixel > 24U ) 
            {
                if ( FONT_16_24_EN == ( isobus_Vt.fontSmall & FONT_16_24_EN ) ) 
                {
                    FontSize = FONT_16_24_SIZE;
                }
            }
        } 
        else if ( fontaux < 32U ) 
        {
            if ( FONT_24_32_EN == ( isobus_Vt.fontSmall & FONT_24_32_EN ) ) 
            {
                FontSize = FONT_24_32_SIZE;
            }
        } 
        else if ( fontaux < 48U ) 
        {
            if ( FONT_32_32_EN == ( isobus_Vt.fontSmall & FONT_32_32_EN ) ) 
            {
                FontSize = FONT_32_32_SIZE;
            }
            if ( isobus_Vt.keysYPixel > 48U ) 
            {
                if ( FONT_32_48_EN == ( isobus_Vt.fontLarge & FONT_32_48_EN ) ) 
                {
                    FontSize = FONT_32_48_SIZE;
                }
            }
        } 
        else if ( fontaux < 64U ) 
        {
            if ( FONT_48_64_EN == ( isobus_Vt.fontLarge & FONT_48_64_EN ) ) 
            {
                FontSize = FONT_48_64_SIZE;
            }
        } 
        else if ( fontaux < 96U ) 
        {
            if ( FONT_64_64_EN == ( isobus_Vt.fontSmall & FONT_64_64_EN ) ) 
            {
                FontSize = FONT_64_64_SIZE;
            }
            if ( isobus_Vt.keysYPixel > 96U ) 
            {
                if ( FONT_64_96_EN == ( isobus_Vt.fontLarge & FONT_64_96_EN ) ) 
                {
                    FontSize = FONT_64_96_SIZE;
                }
            }
        } 
        else if ( fontaux < 128U ) 
        {
            if ( FONT_96_128_EN == ( isobus_Vt.fontSmall & FONT_96_128_EN ) ) 
            {
                FontSize = FONT_96_128_SIZE;
            }
        } 
        else 
        {
            if ( FONT_128_128_EN == ( isobus_Vt.fontSmall & FONT_128_128_EN ) ) 
            {
                FontSize = FONT_128_128_SIZE;
            }
            if ( isobus_Vt.keysYPixel > 192U ) 
            {
                if ( FONT_128_192_EN == ( isobus_Vt.fontLarge & FONT_128_192_EN ) ) 
                {
                    FontSize = FONT_128_192_SIZE;
                }
            }
        }
    }
    
    return;
}

//------------------------------------------------------------------------------            
static void Isobus_PoolBufferBuild( uint8_t step )
{
    uint16_t j;
    uint16_t i;
    uint16_t k;
    uint16_t z;
    uint8_t objectnum;
    uint16_t offset;
    uint16_t offsetindex;
    uint16_t ref_x;
    uint16_t ref_y;
    uint16_t ref_y1;
    uint16_t auxNumByte;
    uint16_t scale_ref;
    uint16_t ScaleDimTarget;
    uint16_t ScaleDimOriginal;
    uint16_t auxID;
    uint8_t auxBytes;
    uint8_t coloraux;
    uint16_t auxStringScale;
    static const uint8_t *picAddressOffset;

    Isobus_tmpPool[0U] = 17U;
    if ( ISOBUS_OP_STEP_FIRST == step ) 
    {
        //VT.Graphic = 0u;
        /* qui dovrò gestire il caricamento delle varie immagini a seconda delle risorse del VT
        picAddressOffset = picMonoBase;
        if ( ( VT.Graphic & 0x01 ) == 0x01 ) { picAddressOffset = pic16Base; }
        if ( ( VT.Graphic & 0x02 ) == 0x02 ) { picAddressOffset = E2_PIC_BASE; } //ricordati i primi 3 byte che indicano se è da scalare o no e le dimensioni di riferimento, quindi offset =  +3
        picAddressOffset = E2_PIC_BASE;
        */
        Isobus_opByteNum = SIZE_OF_OUT_STRINGS + 1U;
        if ( 1U <Isobus_opByteNum ) 
        {
            if ( Isobus_poolString[0U] > 11U )
            {
                objectnum = Isobus_poolString[0U]-11U;
            }
            else
            {
                objectnum = Isobus_poolString[0U];
            }   
            poolBufferIndex = &Isobus_poolString[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolString[j];
            }
            Isobus_opPacketNum = Isobus_opByteNum / 7UL;
            if ( 0U < ( Isobus_opByteNum % 7UL ) )
            {
                Isobus_opPacketNum++;
            }

            /* scalatura*/
            offset = 0U;
            for ( j=0U; j<objectnum; j++ ) 
            {
                ref_x = j * (uint16_t)( 17U ) + offset;
                ref_x = ref_x + (uint16_t)( 4U );
                ref_y = ref_x + (uint16_t)( 2U );
                if ( 0U == isobus_Vt.graphic ) 
                { 
                    Isobus_tmpPool[ref_x+4U] = 1U; 
                }
                offsetindex = ref_x + (uint16_t)( 11U );
                offset = offset + (uint16_t)( Isobus_tmpPool[offsetindex] );
                offset = offset + (uint16_t)( Isobus_tmpPool[offsetindex+1U] << 8U );
                Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.yPixel, DM_SIZE, 0U, 0U );
                auxStringScale = (uint16_t)( Isobus_tmpPool[ref_x] ) + ( (uint16_t)( Isobus_tmpPool[ref_x+1U] ) << 8U );
                if ( auxStringScale > stringWidthScaled )
                {
                    stringWidthScaled = auxStringScale;
                }
                //stringWidthScaled = (uint16_t)( Isobus_tmpPool[ref_x] ) + ( (uint16_t)( Isobus_tmpPool[ref_x+1U] ) << 8U );
            }
            /**/
        } else {
          Isobus_opByteNum = 0U;
          Isobus_opPacketNum = 0U;
        }
    } 
    else if ( step == StepWSDM ) 
    {
//        uint8_t headerObjectNum;
      
        Isobus_opByteNum = SIZE_OF_WS_DM;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        objectnum = Isobus_poolWorkingSetDataMask[31U];
//        headerObjectNum = Isobus_poolWorkingSetDataMask[(uint16_t)( 6U * objectnum ) + 39U];
        poolBufferIndex = &Isobus_poolWorkingSetDataMask[0U];
        for ( j=0U; j<Isobus_opByteNum; j++ ) 
        {
            Isobus_tmpPool[j] = Isobus_poolWorkingSetDataMask[j];
        }
        if ( isobus_Vt.graphic < 2U )
        {
            Isobus_tmpPool[35U] = (uint8_t)( REF_RECT_P1_X_BN_16 ); 
            Isobus_tmpPool[36U] = (uint8_t)( REF_RECT_P1_X_BN_16 >> 8U );
            Isobus_tmpPool[41U] = (uint8_t)( REF_RECT_P2_X_BN_16 ); 
            Isobus_tmpPool[42U] = (uint8_t)( REF_RECT_P2_X_BN_16 >> 8U );
            Isobus_tmpPool[47U] = (uint8_t)( REF_RECT_P3_X_BN_16 ); 
            Isobus_tmpPool[48U] = (uint8_t)( REF_RECT_P3_X_BN_16 >> 8U );
            Isobus_tmpPool[53U] = (uint8_t)( REF_RECT_P4_X_BN_16 ); 
            Isobus_tmpPool[54U] = (uint8_t)( REF_RECT_P4_X_BN_16 >> 8U );
            Isobus_tmpPool[59U] = (uint8_t)( REF_RECT_P5_X_BN_16 ); 
            Isobus_tmpPool[60U] = (uint8_t)( REF_RECT_P5_X_BN_16 >> 8U );
            Isobus_tmpPool[65U] = (uint8_t)( REF_RECT_P6_X_BN_16 ); 
            Isobus_tmpPool[66U] = (uint8_t)( REF_RECT_P6_X_BN_16 >> 8U );
            Isobus_tmpPool[71U] = (uint8_t)( REF_RECT_P7_X_BN_16 ); 
            Isobus_tmpPool[72U] = (uint8_t)( REF_RECT_P7_X_BN_16 >> 8U );
            Isobus_tmpPool[77U] = (uint8_t)( REF_RECT_P8_X_BN_16 ); 
            Isobus_tmpPool[78U] = (uint8_t)( REF_RECT_P8_X_BN_16 >> 8U );            
            Isobus_tmpPool[83U] = (uint8_t)( REF_RECT_P9_X_BN_16 ); 
            Isobus_tmpPool[84U] = (uint8_t)( REF_RECT_P9_X_BN_16 >> 8U );            
        }
        if ( SAEJ_AXIS_Z_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_Z_ENABLED ) )
        { 
            if ( isobus_Vt.graphic < 2U )
            {
                Isobus_tmpPool[71U] = (uint8_t)( REF_RECT_P7_ROLLER_X_BN_16 ); 
                Isobus_tmpPool[72U] = (uint8_t)( REF_RECT_P7_ROLLER_X_BN_16 >> 8U );
            }
            else
            {
                Isobus_tmpPool[71U] = (uint8_t)( REF_RECT_P7_ROLLER_X ); 
                Isobus_tmpPool[72U] = (uint8_t)( REF_RECT_P7_ROLLER_X >> 8U );
            }
            Isobus_tmpPool[73U] = (uint8_t)( REF_RECT_P7_ROLLER_Y ); 
            Isobus_tmpPool[74U] = (uint8_t)( REF_RECT_P7_ROLLER_Y >> 8U ); 
        }
        if ( SAEJ_AXIS_W_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_W_ENABLED ) )
        {    
            if ( isobus_Vt.graphic < 2U )
            {
                Isobus_tmpPool[59U] = (uint8_t)( REF_RECT_P5_ROLLER_X_BN_16 ); 
                Isobus_tmpPool[60U] = (uint8_t)( REF_RECT_P5_ROLLER_X_BN_16 >> 8U );              
            }
            else
            {
                Isobus_tmpPool[59U] = (uint8_t)( REF_RECT_P5_ROLLER_X ); 
                Isobus_tmpPool[60U] = (uint8_t)( REF_RECT_P5_ROLLER_X >> 8U );
            }
            Isobus_tmpPool[61U] = (uint8_t)( REF_RECT_P5_ROLLER_Y ); 
            Isobus_tmpPool[62U] = (uint8_t)( REF_RECT_P5_ROLLER_Y >> 8U ); 
        }
        if ( SAEJ_AXIS_J_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_J_ENABLED ) )
        {    
            if ( isobus_Vt.graphic < 2U )
            {
                Isobus_tmpPool[83U] = (uint8_t)( REF_RECT_P9_ROLLER_X_BN_16 );
                Isobus_tmpPool[84U] = (uint8_t)( REF_RECT_P9_ROLLER_X_BN_16 >> 8U );               
            }
            else
            {
                Isobus_tmpPool[83U] = (uint8_t)( REF_RECT_P9_ROLLER_X );
                Isobus_tmpPool[84U] = (uint8_t)( REF_RECT_P9_ROLLER_X >> 8U ); 
            }
            Isobus_tmpPool[85U] = (uint8_t)( REF_RECT_P9_ROLLER_Y ); 
            Isobus_tmpPool[86U] = (uint8_t)( REF_RECT_P9_ROLLER_Y >> 8U ); 
        }

        /*scalatura*/
        for ( j=0U; j<objectnum; j++ ) 
        {
            ref_x = j * (uint16_t)( 6U ) + (uint16_t)( 35U );
            ref_y = ref_x + (uint16_t)( 2U );
            Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.yPixel, DM_SIZE, 0U, 0U );
        }
        if ( 400U > isobus_Vt.xPixel )
        {
//            uint16_t picHeaderDMIndex;
//            picHeaderDMIndex = (uint16_t)( 41U ) + (uint16_t)( 6U * objectnum ) + (uint16_t)( 6U * (headerObjectNum - 3U) ); 
//            Isobus_tmpPool[picHeaderDMIndex] = Isobus_tmpPool[picHeaderDMIndex] + 1U; //carico il disegno per la data mask a 200, che ha ID + 1 rispetto a quello standard
//            uint16_t auxHeaderObj;
//            for ( j=0U; j<headerObjectNum; j++ )
//            {
//                ref_x = (uint16_t)( 6U*objectnum ) + (uint16_t)( 43U ) + ( j*6U );
//                ref_y = ref_x + (uint16_t)( 2U );
//                auxHeaderObj = (uint16_t)( Isobus_tmpPool[ref_x] ) + (uint16_t)( Isobus_tmpPool[ref_x+1U] << 8U );
//                auxHeaderObj = auxHeaderObj / 2U;
//                if ( j < 9U )
//                {
//                    auxHeaderObj = auxHeaderObj - 1U;
//                }
//                if ( ( j == ( headerObjectNum - 2U ) ) || ( j == ( headerObjectNum - 1U ) ) ) //|| ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) ) //|| ( j == ( headerObjectNum - 4U ) ) || ( j == ( headerObjectNum - 5U ) ) || ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )
//                {
//                    auxHeaderObj = auxHeaderObj - 2U;  
//                }
//                if ( ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) ) //|| ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) ) //|| ( j == ( headerObjectNum - 4U ) ) || ( j == ( headerObjectNum - 5U ) ) || ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )
//                {
//                    auxHeaderObj = auxHeaderObj - 3U;  
//                }                
//                Isobus_tmpPool[ref_x] = (uint8_t)( auxHeaderObj );
//                Isobus_tmpPool[ref_x+1] = (uint8_t)( auxHeaderObj >> 8U);
//                auxHeaderObj = (uint16_t)( Isobus_tmpPool[ref_y] ) + (uint16_t)( Isobus_tmpPool[ref_y+1U] << 8U );
//                auxHeaderObj = auxHeaderObj / 2U;
//                if ( ( j == ( headerObjectNum - 2U ) ) || ( j == ( headerObjectNum - 1U ) ) ) //|| ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )//|| ( j == ( headerObjectNum - 4U ) ) || ( j == ( headerObjectNum - 5U ) ) || ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )
//                {
//                    auxHeaderObj = auxHeaderObj - 1U;  
//                }
//                if ( ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) ) //|| ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )//|| ( j == ( headerObjectNum - 4U ) ) || ( j == ( headerObjectNum - 5U ) ) || ( j == ( headerObjectNum - 6U ) ) || ( j == ( headerObjectNum - 7U ) ) )
//                {
//                    auxHeaderObj = auxHeaderObj - 3U;  
//                }                
//                Isobus_tmpPool[ref_y] = (uint8_t)( auxHeaderObj );
//                Isobus_tmpPool[ref_y+1] = (uint8_t)( auxHeaderObj >> 8U);
//            } 
            uint16_t indexHeaderDM;
            indexHeaderDM = (uint16_t)( 41U ) + (uint16_t)( 6U*objectnum );
            for ( j=0U; j<120U; j++ )
            {
                Isobus_tmpPool[indexHeaderDM + j] = Isobus_poolHeaderDataMask[j];
            }
        }
        if ( 0U == isobus_Vt.graphic )
        {
            Isobus_tmpPool[(uint16_t)( 36U ) + (uint16_t)( 6U*objectnum )] = 0x01;  //cambio il background della DM in bianco se sono in B/N
        }
        /**/
    } 
    else if ( step == StepContainer ) 
    {
        Isobus_opByteNum = SIZE_OF_CONTAINER + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolContainer[0U];
            poolBufferIndex = &Isobus_poolContainer[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolContainer[j];
            }
    
            /*scalatura*/
            offset = 0U;
            for ( j=0U; j<objectnum-18U; j++ ) 
            {
                ref_x = (j*22U)+4U;
                ref_y = ref_x+2U;
                Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.yPixel, DM_SIZE, 0U, 0U );
//                ref_y1 = (uint16_t)( Isobus_tmpPool[ref_y] ) + ((uint16_t)( Isobus_tmpPool[ref_y+1U] ) * 256U );
//                ref_x = (j*22U)+13U;
//                ref_y = ref_x+2U;
//                Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.keysXPixel, isobus_Vt.keysYPixel, ref_y1, 1U, 1U );
                if ( ( 1U << j ) == ( ContainerShow & ( 1U << j ) ) ) 
                { 
                    Isobus_tmpPool[(j*22U)+8U] = 0x00U; 
                }
            }
            for ( j=0U; j<12U; j++ ) 
            {
                ref_x = ((objectnum-18U)*22U) + (16U*j) + 4U;
                ref_y = ref_x + 2U; 
                Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.yPixel, DM_SIZE, 0U, 0U );
                if ( 0U == j ) 
                { 
                    if ( 0U == isobus_Vt.graphic ) 
                    { 
                        Isobus_tmpPool[ref_y+2U] = 0x00U; 
                        Isobus_tmpPool[ref_y+5U] = 0x35U; 
                        Isobus_tmpPool[ref_y+6U] = 0x4EU; 
                    } 
                }
                else if ( 1U == j ) 
                { 
                    if ( 0U ==isobus_Vt.graphic ) 
                    { 
                        Isobus_tmpPool[ref_y+5U] = 0x36U; 
                        Isobus_tmpPool[ref_y+6U] = 0x4EU; 
                    } 
                }
                else if ( 2U == j ) 
                { 
                    Isobus_tmpPool[ref_y+2U] = (uint8_t)( ContainerP0Show ); 
                }
                else if ( 3U == j ) 
                { 
                    Isobus_tmpPool[ref_y+2U] = (uint8_t)( ~ContainerP0Show & 0x01 ); 
                }
                else if ( 4U == j ) 
                { 
                    Isobus_tmpPool[ref_y+2U] = ContainerAnShow; 
                }
                else if ( 5U == j ) 
                { 
                    Isobus_tmpPool[ref_y+2U] = ContainerAnShow; 
                }                
            }
            /* scalatura per Header Data Mask */
           
            if ( 400U > isobus_Vt.xPixel )
            {
                for ( j=0U; j<6U; j++ )
                {
                    ref_x = ((objectnum-18U)*22U) + (16U*9) + (j* 16U) + 4U;
                    ref_y = ref_x + 2U;
                    Isobus_tmpPool[ref_x] = Isobus_tmpPool[ref_x]/2U; 
                    Isobus_tmpPool[ref_y] = Isobus_tmpPool[ref_y]/2U + 1U; 
                }
            }
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    }     
    else if ( step == ISOBUS_OP_STEP_OUT_RECTS ) 
    {
        Isobus_opByteNum = SIZE_OF_OUT_RECTS + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolRectangle[0U];
            poolBufferIndex = &Isobus_poolRectangle[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolRectangle[j];
            }
            /* scalatura*/
            for ( j=0u; j<objectnum-2U; j++ ) 
            {
                ref_x = j * (uint16_t)( 13U ) + (uint16_t)( 6U );
                ref_y = ref_x + (uint16_t)( 2U );
                if ( j < (objectnum-3U) ) 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.yPixel, DM_SIZE, 0U, 0U );
                } 
                else 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.keysXPixel, isobus_Vt.keysYPixel, DESIGNATOR_WIDTH, 0U, 0U );
                }
            }
            if ( 400 > isobus_Vt.xPixel )
            {
                for ( j=objectnum-2U; j<objectnum; j++ ) 
                {
                    uint16_t auxRect;
                    ref_x = j * (uint16_t)( 13U ) + (uint16_t)( 6U );
                    ref_y = ref_x + (uint16_t)( 2U );
                    auxRect = (uint16_t)( Isobus_tmpPool[ref_x] ) + (uint16_t)(  Isobus_tmpPool[ref_x+1U] << 8U );
                    auxRect = auxRect / 2U + 1;
                    Isobus_tmpPool[ref_x] = (uint8_t)( auxRect );
                    Isobus_tmpPool[ref_x + 1U] = (uint8_t)( auxRect >> 8U);
                    auxRect = (uint16_t)( Isobus_tmpPool[ref_y] ) + (uint16_t)(  Isobus_tmpPool[ref_y+1U] << 8U );
                    auxRect = auxRect / 2U + 1;
                    Isobus_tmpPool[ref_y] = (uint8_t)( auxRect );
                    Isobus_tmpPool[ref_y + 1U] = (uint8_t)( auxRect >> 8U);                    
                    
                }
            }            
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    } 
    else if ( step == ISOBUS_OP_STEP_OUT_ELLIPSE ) 
    {
        Isobus_opByteNum = SIZE_OF_OUT_ELLIPSE + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolEllipse[0U];
            poolBufferIndex = &Isobus_poolEllipse[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolEllipse[j];
            }
            /* scalatura*/
            for ( j=0U; j<objectnum; j++ ) 
            {
                ref_x = j * (uint16_t)( 15U ) + (uint16_t)( 6U );
                ref_y = ref_x + (uint16_t)( 2U );
                if ( objectnum < 2U ) 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.keysXPixel, isobus_Vt.keysYPixel, DESIGNATOR_WIDTH, 0U, 0U );
                } 
                else 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.xPixel, DM_SIZE, 0U, 0U );
                }
            }
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    }
    else if ( step == ISOBUS_OP_STEP_OUT_HEAD_ELLIPSE ) 
    {
        Isobus_opByteNum = SIZE_OF_OUT_HEAD_ELLIPSE + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolHeadEllipse[0U];
            poolBufferIndex = &Isobus_poolHeadEllipse[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolHeadEllipse[j];
            }
            /* scalatura*/
            if ( 400 > isobus_Vt.xPixel )
            {
                for ( j=0U; j<objectnum; j++ ) 
                {
                    ref_x = j * (uint16_t)( 13U ) + (uint16_t)( 6U );
                    ref_y = ref_x + (uint16_t)( 2U );
                    Isobus_tmpPool[ref_x] = Isobus_tmpPool[ref_x]/2U + 1U;
                    Isobus_tmpPool[ref_y] = Isobus_tmpPool[ref_y]/2U + 1U;
                }
            }
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    }    
    else if ( step == ISOBUS_OP_STEP_OUT_NUM ) 
    {
        Isobus_opByteNum = SIZE_OF_OUT_NUM + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolOutputNumber[0U];
            poolBufferIndex = &Isobus_poolOutputNumber[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolOutputNumber[j];
            }
            /* scalatura
            for ( j=0U; j<objectnum; j++ ) 
            {
                ref_x = j * (uint16_t)( 15U ) + (uint16_t)( 6U );
                ref_y = ref_x + (uint16_t)( 2U );
                if ( objectnum < 2U ) 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.keysXPixel, isobus_Vt.keysYPixel, DESIGNATOR_WIDTH, 0U, 0U );
                } 
                else 
                {
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.xPixel, isobus_Vt.xPixel, DM_SIZE, 0U, 0U );
                }
            }
            */
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    }
    else if ( step == ISOBUS_OP_STEP_OUT_LINEBAR ) 
    {
        Isobus_opByteNum = SIZE_OF_OUT_LINEBAR + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            objectnum = Isobus_poolLineBar[0U];
            poolBufferIndex = &Isobus_poolLineBar[0U];
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolLineBar[j];
            }
            /* scalatura*/
            for ( j=0u; j<objectnum; j++ ) 
            {
                ref_x = j * (uint16_t)( 24U ) + (uint16_t)( 4U );
                ref_y = ref_x + (uint16_t)( 2U );                
                if ( 400 > isobus_Vt.xPixel )
                {
                    uint16_t auxLineBar;
                    auxLineBar = (uint16_t)( Isobus_tmpPool[ref_x] ) + (uint16_t)(  Isobus_tmpPool[ref_x+1U] << 8U );
                    auxLineBar = auxLineBar / 2U;
                    Isobus_tmpPool[ref_x] = (uint8_t)( auxLineBar );
                    Isobus_tmpPool[ref_x + 1U] = (uint8_t)( auxLineBar >> 8U);
                    auxLineBar = (uint16_t)( Isobus_tmpPool[ref_y] ) + (uint16_t)(  Isobus_tmpPool[ref_y+1U] << 8U );
                    auxLineBar = auxLineBar / 2U;
                    Isobus_tmpPool[ref_y] = (uint8_t)( auxLineBar );
                    Isobus_tmpPool[ref_y + 1U] = (uint8_t)( auxLineBar >> 8U);                    
                }
                if ( 0U == isobus_Vt.graphic ) 
                {
                    Isobus_tmpPool[ref_x + 4U] = 0x00;  //metto il colore nero se ho dislpay monocromatico
                }
                
            }
//            if ( 400 > isobus_Vt.xPixel )
//            {
//                for ( j=objectnum-5U; j<objectnum; j++ ) 
//                {
//                    uint16_t auxRect;
//                    ref_x = j * (uint16_t)( 13U ) + (uint16_t)( 6U );
//                    ref_y = ref_x + (uint16_t)( 2U );
//                    auxRect = (uint16_t)( Isobus_tmpPool[ref_x] ) + (uint16_t)(  Isobus_tmpPool[ref_x+1U] << 8U );
//                    auxRect = auxRect / 2U;
//                    Isobus_tmpPool[ref_x] = (uint8_t)( auxRect );
//                    Isobus_tmpPool[ref_x + 1U] = (uint8_t)( auxRect >> 8U);
//                    auxRect = (uint16_t)( Isobus_tmpPool[ref_y] ) + (uint16_t)(  Isobus_tmpPool[ref_y+1U] << 8U );
//                    auxRect = auxRect / 2U;
//                    Isobus_tmpPool[ref_y] = (uint8_t)( auxRect );
//                    Isobus_tmpPool[ref_y + 1U] = (uint8_t)( auxRect >> 8U);                    
//                    
//                }
//            }            
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    }    
    else if ( step == StepGraphAttributesValue ) 
    {
        Isobus_opByteNum = SIZE_OF_GRAPH_ATTRIBUTES + 1U;
        Isobus_opPacketNum = Isobus_opByteNum / 7UL;
        if ( 0U < ( Isobus_opByteNum % 7UL ) )
        {
            Isobus_opPacketNum++;
        }        
        if ( 1U < Isobus_opByteNum ) 
        {
            Isobus_PoolMeasureScale();
            for ( j=1U; j<Isobus_opByteNum; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolGraphAttribute[j];
            }
            /* scalatura*/
            objectnum = Isobus_poolGraphAttribute[0U];
            poolBufferIndex = &Isobus_poolGraphAttribute[0U];
            for ( j=0U; j<objectnum-1U; j++ ) 
            {
                offsetindex = j * (uint16_t)( 8U ) + (uint16_t)( 3U );
                if ( Isobus_tmpPool[offsetindex] == TYPEID_FONTATTR ) 
                {
                    Isobus_tmpPool[ offsetindex + (uint16_t)( 2U ) ] = FontSize;
                    Isobus_tmpPool[ offsetindex + (uint16_t)( 4U ) ] = FontStyle;
                } 
                else if ( Isobus_tmpPool[offsetindex] == TYPEID_LINEATTR ) 
                {
                    if ( 0U == isobus_Vt.graphic ) 
                    {
                        Isobus_tmpPool[ offsetindex + (uint16_t)( 1U ) ] = COLOR_BLACK;
                    }
                } 
                else if ( Isobus_tmpPool[offsetindex] == TYPEID_FILLATTR ) 
                {
                    if ( 0U == isobus_Vt.graphic ) 
                    {
                        if ( j < ( objectnum - 4U ) ) 
                        { 
                            coloraux = COLOR_BLACK; 
                        }
                        else 
                        { 
                            coloraux = COLOR_WHITE; 
                        }
                        Isobus_tmpPool[ offsetindex + (uint16_t)( 2U ) ] = coloraux;
                    }
                }
            }
            /**/
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    } 
    else if ( step == StepAuxInputValue ) 
    {
        if ( 0U < AuxInNum ) 
        {
            offsetindex = 0U;
            Isobus_opByteNum = 1U;
            k = 1U;
            for ( i=0U; i<AUX_INPUT_NUM_MAX; i++ ) 
            {
                auxID = (uint16_t)( Isobus_poolAuxInput[k] );
                auxID = auxID + ( (uint16_t)( Isobus_poolAuxInput[k+1U] ) * 256U );
                offsetindex = (uint16_t)( k );
                offsetindex = (uint16_t)( k ) + (uint16_t)( 5U ); //vado al numero di oggetti di cui è composto il disegnatore
                auxBytes = Isobus_poolAuxInput[offsetindex] * 6U; //per ogni oggetto ho un XYREF (ID + coordineate) di 6 byte
                auxBytes = auxBytes + 6U;  //aggiungo 6 per tenere conto della parte fissa di byte presenti nel designatore
                for ( j=0U; j<AuxInNum; j++ ) 
                {
                    if ( auxID == AuxInput[j].ID ) 
                    {
                        for ( z=0U; z<(uint16_t)( auxBytes ); z++ ) 
                        { 
                            Isobus_tmpPool[Isobus_opByteNum+z] = Isobus_poolAuxInput[k+z]; 
                        }
                        Isobus_tmpPool[Isobus_opByteNum+4U] = AuxInput[j].type;
                        Isobus_opByteNum = Isobus_opByteNum + (uint16_t)( auxBytes );
                        if ( 0x8DU == AuxInput[j].type ) 
                        { 
                            //cambio la picture a cui deve puntare se è XY quadrature
                            Isobus_tmpPool[Isobus_opByteNum+6U] = 0x37U; 
                        } 		          
                    }
                }
                k = k + (uint16_t)( auxBytes );
            }
            poolBufferIndex = &Isobus_poolAuxInput[0U];
            Isobus_opPacketNum = Isobus_opByteNum / 7UL;
            if ( 0U < ( Isobus_opByteNum % 7UL ) )
            {
                Isobus_opPacketNum++;
            }
            objectnum = AuxInNum;
            offset = (uint16_t)( 6U );
            for ( j=0u; j<objectnum; j++ ) 
            {
                offsetindex = j * (uint16_t)( 6U ) + offset;
                for ( i=0U; i<Isobus_tmpPool[offsetindex]; i++ ) 
                {
                    ref_x = i * (uint16_t)( 6U ) + offsetindex;
                    ref_x = ref_x + (uint16_t)( 3U );
                    ref_y = ref_x + (uint16_t)( 2U );
                    Isobus_PoolObjectFieldScale( ref_x, ref_y, isobus_Vt.keysXPixel, isobus_Vt.keysYPixel, DESIGNATOR_WIDTH, 0U, 0U );
                }
                offset = offset + ( (uint16_t)( Isobus_tmpPool[offsetindex] ) * 6U );
            }
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    } 
    else if ( step == StepObjectPointerValue ) 
    {
        if ( 0U < SIZE_OF_OBJ_POINTER ) 
        {
            for ( j=1U; j<=SIZE_OF_OBJ_POINTER; j++ ) 
            {
                Isobus_tmpPool[j] = Isobus_poolObjPointer[j];
            }
            for ( z=0U; z<AuxInAnNum; z++ ) 
            {
                for ( j=0U; j<AUX_IN_AN_NUM_MAX; j++ ) 
                {
                    if ( AuxInput[z].ID == AUX_INPUT_ID[j] ) 
                    {
                        Isobus_tmpPool[(j*6U)+5U] = (uint8_t)( AUX_INPUT_ID[j] );
                        Isobus_tmpPool[(j*6U)+6U] = (uint8_t)( AUX_INPUT_ID[j] >> 8U );
                    }
                }
            }
            for ( z=0U; z<AuxInBoolNum; z++ ) 
            {
                for ( j=AUX_IN_AN_NUM_MAX; j<(AUX_IN_AN_NUM_MAX + AUX_IN_BOOL_SHIFT_NUM); j++ ) 
                {
                    if ( AuxInput[AuxInAnNum+z].ID == AUX_INPUT_ID[j] ) 
                    {
                        Isobus_tmpPool[(j*6U)+5U] = (uint8_t)( AUX_INPUT_ID[j] );
                        Isobus_tmpPool[(j*6U)+6U] = (uint8_t)( AUX_INPUT_ID[j] >> 8U );
                    }
                }
                for ( j=(AUX_IN_AN_NUM_MAX + AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM ); j<(AUX_IN_AN_NUM_MAX + AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SINGLE_NUM + AUX_IN_BOOL_DIRECTION_NUM ); j++ ) 
                {
                    if ( AuxInput[AuxInAnNum+z].ID == AUX_INPUT_ID[j] ) 
                    {
                        Isobus_tmpPool[((j-AUX_IN_BOOL_SHIFT_NUM)*6U)+5U] = (uint8_t)( AUX_INPUT_ID[j] );
                        Isobus_tmpPool[((j-AUX_IN_BOOL_SHIFT_NUM)*6U)+6U] = (uint8_t)( AUX_INPUT_ID[j] >> 8U );
                    }
                }
            }
            Isobus_opByteNum = SIZE_OF_OBJ_POINTER + 1U;
            Isobus_opPacketNum = Isobus_opByteNum / 7UL;
            if ( 0U < ( Isobus_opByteNum % 7UL ) )
            {
                Isobus_opPacketNum++;
            }
            poolBufferIndex = &Isobus_poolObjPointer[0U];
        } 
        else 
        {
            Isobus_opByteNum = 0U;
            Isobus_opPacketNum = 0U;
        }
    } 
    else 
    {
        if ( 0U < NumPictureObject ) 
        {
            for ( k=1u; k<=NumPictureObject; k++ ) 
            {
                if ( step == ( ISOBUS_OP_LAST_STEP_BEFORE_PICS + k ) ) 
                {
                    if ( 1U == k )
                    {
                        //inizializzo al primo picture che devo trasmettere, così se devo ritrasmettere per qualche motivo, reinizializzo sempre
                        picAddressOffset = &Isobus_poolPictureMono[0U];
                        if ( 1U == isobus_Vt.graphic )
                        {
                            picAddressOffset = &Isobus_poolPicture16[0U];
                        }
                        else if ( 2U == isobus_Vt.graphic )
                        {
                            picAddressOffset = &Isobus_poolPicture256[0U];
                        }
                    }
                    Isobus_opByteNum = (uint32_t)( *( picAddressOffset + 12U + PIC_DATA_OFFSET ) ); 
                    Isobus_opByteNum = Isobus_opByteNum + (uint32_t)( *( picAddressOffset + 13U + PIC_DATA_OFFSET ) << 8U ); 
                    Isobus_opByteNum = Isobus_opByteNum + (uint32_t)( *( picAddressOffset + 14U + PIC_DATA_OFFSET ) << 16U ); 
                    Isobus_opByteNum = Isobus_opByteNum + (uint32_t)( *( picAddressOffset + 15U + PIC_DATA_OFFSET ) << 24U ); 
                    Isobus_opByteNum = Isobus_opByteNum + (uint32_t)( 18U );

                    if ( SAEJ_TP_BYTE_NUM_MAX < Isobus_opByteNum ) 
                    {
                        auxNumByte = SAEJ_TP_BYTE_NUM_MAX;
                    } 
                    else 
                    {
                        auxNumByte = Isobus_opByteNum;
                    }
                    for ( j=1U; j<auxNumByte; j++ ) 
                    {
                        Isobus_tmpPool[j] = (uint32_t)( *( picAddressOffset + j ) );
                    }
//                    picBuffOffsetIndex = picAddressOffset + 1U;
                    poolBufferIndex = picAddressOffset;
//                  picBuffOffsetIndex = picAddressOffset;
//                  picBuffOffsetIndex = picBuffOffsetIndex + (UL_32)( E2_PIC_DATA_OFFSET );
                    Isobus_opPacketNum = Isobus_opByteNum / 7UL;
                    if ( 0U < ( Isobus_opByteNum % 7UL ) )
                    {
                        Isobus_opPacketNum++;
                    }
                    /* scalatura */
//                    uint16_t picID;
//                    uint8_t auxPicID1;
//                    uint8_t auxPicID2;
                    scale_ref = *picAddressOffset;
//                    auxPicID1 = Isobus_tmpPool[1U];
//                    auxPicID2 = Isobus_tmpPool[2U];
//                    picID = (uint16_t)( auxPicID1 ) + (uint16_t)( auxPicID2 << 8U );
//                    if ( picID == 0x4E37 ) //ID della Header Mask
//                    {
//                        if ( 400 > isobus_Vt.xPixel )
//                        {
//                            ref_x = 4U;
//                            ScaleDimTarget = 200U;
//                            Isobus_tmpPool[ref_x] = (uint8_t)( ScaleDimTarget );
//                            Isobus_tmpPool[ref_x+1U] = (uint8_t)( ScaleDimTarget >> 8U );
//                        }
//                    }
//                    else
//                    {
                        if ( 0x01U == ( scale_ref & 0x01U ) ) 
                        { 
                            //verifico se devo scalare l'immagine
                            ref_x = 4U;
                            if( 0x00U == ( scale_ref & 0x08U ) ) 
                            {
                                ScaleDimTarget = isobus_Vt.xPixel;
                            } 
                            else 
                            {
                                ScaleDimTarget = isobus_Vt.keysYPixel;
                                if ( isobus_Vt.keysXPixel < isobus_Vt.keysYPixel ) 
                                { 
                                    ScaleDimTarget = isobus_Vt.keysXPixel; 
                                }
                            }
                            if ( 0x00U == ( scale_ref & 0x80U ) ) 
                            {
                                ScaleDimOriginal = DM_SIZE;
                            } 
                            else 
                            {
                                ScaleDimOriginal = DESIGNATOR_WIDTH;
                            }
                            Isobus_PoolObjectSingleScale( ref_x, ScaleDimTarget, ScaleDimOriginal );
                         }
//                    }
                    picAddressOffset =  picAddressOffset + Isobus_opByteNum;
                    //picAddressOffset = picAddressOffset + (UL_32)( E2_PIC_DATA_OFFSET );
                }
            } 
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AuxInCfgUpdate( void )
{
    uint16_t aux1;
    uint16_t aux2;
    uint8_t index1;
    uint8_t index2;
    uint8_t j;
    bool axisYused = false;
    
    if ( SAEJ_AXIS_X_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_X_ENABLED ) ) 
    {
        aux1 = CfgAuxAn[SAEJ_AXIS_X] & 0x00F0U;
        aux1 = aux1 >> 4U;
        index1 = (uint8_t)( aux1 );
        aux2 = CfgAuxAn[SAEJ_AXIS_X] & 0x0F00U;
        aux2 = aux2 >> 8U;
        index2 = (uint8_t)( aux2 );
        if ( ( CfgAuxAn[SAEJ_AXIS_X] & ANALOG_RETURN_TO_50 ) == ANALOG_RETURN_TO_50 ) 
        {
            if ( index1 == ( SAEJ_AXIS_X + 1U ) ) 
            {
                AuxInput[AuxInAnNum].ID = AUX_INPUT_ID[SAEJ_AXIS_X];
                ContainerShow |= 0x00000001UL;
                if ( index2 == ( SAEJ_AXIS_Y + 1U ) ) 
                {
                    AuxInput[AuxInAnNum].type = 141U;  //quadrature return 50%
                    AuxInput[AuxInAnNum].indexInput2 = index2-1U;
                    Isobus_functionalities |= 0x2000U;
                    ContainerAnShow = 0x00U;
                    axisYused = true;
                } 
                else 
                {
                    if ( ( CfgAuxAn[SAEJ_AXIS_X] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) 
                    {
                        AuxInput[AuxInAnNum].type = 137U;  //return 50% with latch
                        Isobus_functionalities |= 0x0200;
                    } 
                    else 
                    {
                        AuxInput[AuxInAnNum].type = 131U;  //analog return 50%
                        Isobus_functionalities |= 0x0008;
                    }
                }
                AuxInput[AuxInAnNum].indexInput = index1-1U;
                AuxInAnNum++;
            }
        }
    }

    if ( ( SAEJ_AXIS_Y_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_Y_ENABLED ) ) && ( false == axisYused ) )
    {
        aux1 = CfgAuxAn[SAEJ_AXIS_Y] & 0x00F0U;
        aux1 = aux1 >> 4U;
        index1 = (uint8_t)( aux1 );
        if ( ( CfgAuxAn[SAEJ_AXIS_Y] & ANALOG_RETURN_TO_50 ) == ANALOG_RETURN_TO_50 ) 
        {
            if ( index1 == ( SAEJ_AXIS_Y + 1U ) ) 
            {
                AuxInput[AuxInAnNum].ID = AUX_INPUT_ID[SAEJ_AXIS_Y];
                ContainerShow |= 0x00000002UL;
                if ( ( CfgAuxAn[SAEJ_AXIS_Y] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) 
                {
                    AuxInput[AuxInAnNum].type = 137U;
                    Isobus_functionalities |= 0x0200U;
                } 
                else 
                {
                    AuxInput[AuxInAnNum].type = 131U;
                    Isobus_functionalities |= 0x0008U;
                }
                AuxInput[AuxInAnNum].indexInput = index1-1U;
                AuxInAnNum++;
            }   
        }   
    }

    if ( SAEJ_AXIS_W_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_W_ENABLED ) ) 
    {
        aux1 = CfgAuxAn[SAEJ_AXIS_W] & 0x00F0U;
        aux1 = aux1 >> 4U;
        index1 = (uint8_t)( aux1 );
        if ( ( CfgAuxAn[SAEJ_AXIS_W] & ANALOG_RETURN_TO_50 ) == ANALOG_RETURN_TO_50 ) 
        {
            if ( index1 == ( SAEJ_AXIS_W + 1U ) ) 
            {
                AuxInput[AuxInAnNum].ID = AUX_INPUT_ID[SAEJ_AXIS_W];
                ContainerShow |= 0x00000004UL;
                if ( ( CfgAuxAn[SAEJ_AXIS_W] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) 
                {
                    AuxInput[AuxInAnNum].type = 137U;
                    Isobus_functionalities |= 0x0200U;
                } 
                else 
                {
                    AuxInput[AuxInAnNum].type = 131U;
                    Isobus_functionalities |= 0x0008U;
                }
                AuxInput[AuxInAnNum].indexInput = index1-1U;
                AuxInAnNum++;
            }
        }
    }

    if ( SAEJ_AXIS_Z_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_Z_ENABLED ) )
    {
        aux1 = CfgAuxAn[SAEJ_AXIS_Z] & 0x00F0U;
        aux1 = aux1 >> 4U;
        index1 = (uint8_t)( aux1 );
        if ( ( CfgAuxAn[SAEJ_AXIS_Z] & ANALOG_RETURN_TO_50 ) == ANALOG_RETURN_TO_50 ) 
        {
            if ( index1 == ( SAEJ_AXIS_Z + 1U ) ) 
            {
                AuxInput[AuxInAnNum].ID = AUX_INPUT_ID[SAEJ_AXIS_Z];
                ContainerShow |= 0x00000008UL;
                if ( ( CfgAuxAn[SAEJ_AXIS_Z] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) 
                {
                    AuxInput[AuxInAnNum].type = 137U;
                    Isobus_functionalities |= 0x0200U;
                } 
                else 
                {
                    AuxInput[AuxInAnNum].type = 131U;
                    Isobus_functionalities |= 0x0008U;
                }
                AuxInput[AuxInAnNum].indexInput = index1-1U;
                AuxInAnNum++;
            }
        }
    }

    if ( SAEJ_AXIS_J_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_J_ENABLED ) )
    {
        aux1 = CfgAuxAn[SAEJ_AXIS_J] & 0x00F0U;
        aux1 = aux1 >> 4U;
        index1 = (uint8_t)( aux1 );
        if ( ( CfgAuxAn[SAEJ_AXIS_J] & ANALOG_RETURN_TO_50 ) == ANALOG_RETURN_TO_50 ) 
        {
            if ( index1 == ( SAEJ_AXIS_J + 1U ) ) 
            {
                AuxInput[AuxInAnNum].ID = AUX_INPUT_ID[SAEJ_AXIS_J];
                ContainerShow |= 0x00000010UL;
                if ( ( CfgAuxAn[SAEJ_AXIS_J] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) 
                {
                    AuxInput[AuxInAnNum].type = 137U;
                    Isobus_functionalities |= 0x0200U;
                } 
                else 
                {
                    AuxInput[AuxInAnNum].type = 131U;
                    Isobus_functionalities |= 0x0008U;
                }
                AuxInput[AuxInAnNum].indexInput = index1-1U;
                AuxInAnNum++;
            }
        }
    }

    for ( j=0u; j<ISOBUS_AUX_BUTTON_NUM_MAX; j++ ) 
    {
        aux1 = CfgAuxBool[j] & 0x0F00U;
        aux1 = aux1 >> 8U;
        index1 = (uint8_t)( aux1 );

        aux2 = CfgAuxBool[j]>>12U;
        aux2 = aux2  & 0x000FU;
        index2 = (uint8_t)( aux2 );

        if ( ( CfgAuxBool[j] & 0x000C ) != 0U ) 
        {
            if ( ( index1 > 0U ) && ( index1 < ( ISOBUS_PUSH_BUTTON_NUM + 1u ) ) ) 
            {
                AuxInput[AuxInBoolNum + AuxInAnNum].ID = AUX_INPUT_ID[AUX_IN_AN_NUM_MAX + j];
                if ( j < AUX_IN_BOOL_SHIFT_NUM ) 
                {
                    AuxInput[AuxInBoolNum + AuxInAnNum].objPointer =  (uint16_t)( Isobus_poolObjPointer[((AUX_IN_AN_NUM_MAX + j)*6U)+1U] ) + ((uint16_t)( Isobus_poolObjPointer[((AUX_IN_AN_NUM_MAX + j)*6U)+2U] ) << 8U );
                    ContainerShow |= ( 1U << (j+AUX_IN_AN_NUM_MAX) );
                } 
                else 
                {
                    AuxInput[AuxInBoolNum + AuxInAnNum].objPointer =  (uint16_t)( Isobus_poolObjPointer[((AUX_IN_AN_NUM_MAX + (j-AUX_IN_BOOL_SHIFT_NUM))*6U)+1U] ) + ((uint16_t)( Isobus_poolObjPointer[((AUX_IN_AN_NUM_MAX + (j-AUX_IN_BOOL_SHIFT_NUM))*6U)+2U] ) << 8U );
                    ContainerShow |= ( 1U << (j+AUX_IN_AN_NUM_MAX-AUX_IN_BOOL_SHIFT_NUM) );
                    if ( j == KEY_MODE ) 
                    { 
                        ContainerP0Show = 0x00U; 
                    }
                }
                if ( CfgAuxBool[j] & 0x0001U ) 
                {
                    AuxInput[AuxInBoolNum + AuxInAnNum].indexInput = index1 + 8U;  //-1 + 9 (per tenere conto dello shift)
                } 
                else 
                {
                    if ( j >= ( AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM ) ) 
                    {
                        AuxInput[AuxInBoolNum + AuxInAnNum].indexInput = ( index1 + 8U );
                    } 
                    else 
                    {
                        AuxInput[AuxInBoolNum + AuxInAnNum].indexInput = index1 - 1U;
                    }
                }
                if ( ( CfgAuxBool[j] & 0x000CU ) == BOOL_ISO_LATCH ) 
                {
                    AuxInput[AuxInBoolNum + AuxInAnNum].type = 128U;
                    Isobus_functionalities |= 0x0001U;
                } 
                else 
                {
                    AuxInput[AuxInBoolNum + AuxInAnNum].type = 130U;
                    Isobus_functionalities |= 0x0004U;
                }
                if ( ( CfgAuxBool[j] & 0x00F0U ) == ( BOOL_ISO_LATCH * 4U ) ) 
                {
                    if ( ( index2 > 0U ) && ( index2 < ( ISOBUS_PUSH_BUTTON_NUM + 1u ) ) ) 
                    {
                        if ( CfgAuxBool[j] & 0x0002U ) 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].indexInput2 = index2 + 8U;  //-1 + 9 (per tenere conto dello shift)
                        } 
                        else 
                        {
                            if ( j >= ( AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM ) ) 
                            {
                                AuxInput[AuxInBoolNum + AuxInAnNum].indexInput2 = ( index2 + 8U );
                            } 
                            else 
                            {
                                AuxInput[AuxInBoolNum + AuxInAnNum].indexInput2 = index2 - 1U;
                            }
                        }
                        if ( ( CfgAuxBool[j] & 0x000CU ) == BOOL_ISO_LATCH ) 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].type = 133U;
                            Isobus_functionalities |= 0x0020U;
                        } 
                        else 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].type = 136U;
                            Isobus_functionalities |= 0x0100U;
                        }
                    }
                } 
                else if ( ( CfgAuxBool[j] & 0x00F0U ) == ( BOOL_ISO_NOT_LATCH * 4U ) ) 
                {
                    if ( ( index2 > 0U ) && ( index2 < 15U ) ) 
                    {
                        if ( CfgAuxBool[j] & 0x0002U ) 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].indexInput = index2 + 8U;  //-1 + 9 (per tenere conto dello shift)
                        } 
                        else 
                        {
                            if ( j >= ( AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM ) ) 
                            {
                                AuxInput[AuxInBoolNum + AuxInAnNum].indexInput2 = ( index2 + 8U );
                            } 
                            else 
                            {
                                AuxInput[AuxInBoolNum + AuxInAnNum].indexInput2 = index2 - 1U;
                            }
                        }
                        if ( ( CfgAuxBool[j] & 0x000CU ) == BOOL_ISO_LATCH ) 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].type = 135U;
                            Isobus_functionalities |= 0x0080U;
                        } 
                        else 
                        {
                            AuxInput[AuxInBoolNum + AuxInAnNum].type = 134U;
                            Isobus_functionalities |= 0x0040U;
                        }
                    }
                }
                AuxInBoolNum++;
            }   
        }
    }
    AuxInNum = AuxInBoolNum + AuxInAnNum; 
         
    return;
}

//------------------------------------------------------------------------------
static void Isobus_KeyInputUpdate( uint8_t index )
{
//#define ISOBUS_PHYSICAL_BUTTON_NUM ( 10U )
//#define ISOBUS_LOGIC_DIRECTION_NUM ( 4U )

  
    if ( ISOBUS_PHYSICAL_BUTTON_NUM > index )
    {
       Isobus_key[index].input = Digital_GetValue( Isobus_pushButtonIndex[index] );
    }
    else if ( ( ISOBUS_PHYSICAL_BUTTON_NUM + ISOBUS_LOGIC_DIRECTION_NUM ) > index )
    {
       if ( ISOBUS_X_SX_KEY_INPUT == index )
       {
           if ( SAEJ_AXIS_STATUS_NEGATIVE == Saej_AxisGet( SAEJ_AXIS_X ).status )
           {
               Isobus_key[index].input = DIGITAL_IN_ON;
           }
           else
           {
               Isobus_key[index].input = DIGITAL_IN_OFF;
           }
       }
       else if ( ISOBUS_X_DX_KEY_INPUT == index )
       {
           if ( SAEJ_AXIS_STATUS_POSITIVE == Saej_AxisGet( SAEJ_AXIS_X ).status )
           {
               Isobus_key[index].input = DIGITAL_IN_ON;
           }
           else
           {
               Isobus_key[index].input = DIGITAL_IN_OFF;
           }
       }
       else if ( ISOBUS_Y_BWD_KEY_INPUT == index )
       {
           if ( SAEJ_AXIS_STATUS_NEGATIVE == Saej_AxisGet( SAEJ_AXIS_Y ).status )
           {
               Isobus_key[index].input = DIGITAL_IN_ON;
           }
           else
           {
               Isobus_key[index].input = DIGITAL_IN_OFF;
           }
       }
       else if ( ISOBUS_Y_FWD_KEY_INPUT == index )
       {
           if ( SAEJ_AXIS_STATUS_POSITIVE == Saej_AxisGet( SAEJ_AXIS_Y ).status )
           {
               Isobus_key[index].input = DIGITAL_IN_ON;
           }
           else
           {
               Isobus_key[index].input = DIGITAL_IN_OFF;
           }
       } 
       else
       {
           Isobus_key[index].input = DIGITAL_IN_OFF;
       }
       
    }
    
    if ( ISOBUS_P_DISABLE == Isobus_pushButtonEn[index] )
    {
        Isobus_key[index].output = ISOBUS_KEY_STATUS_UP;
    }
    else
    {
        switch ( Isobus_key[index].output ) 
        {
        case ISOBUS_KEY_STATUS_WAIT:
            if ( DIGITAL_IN_OFF == Isobus_key[index].input )
            {
                Isobus_key[index].output = ISOBUS_KEY_STATUS_UP;
            }
            break;
        case ISOBUS_KEY_STATUS_UP:
            if ( DIGITAL_IN_ON == Isobus_key[index].input ) 
            {
              Isobus_key[index].output = ISOBUS_KEY_STATUS_CLICK;
            }
            break;
        case ISOBUS_KEY_STATUS_CLICK:
            Isobus_key[index].output = ISOBUS_KEY_STATUS_DOWN;
            break;
        case ISOBUS_KEY_STATUS_DOWN:
            if ( DIGITAL_IN_OFF == Isobus_key[index].input )
            {
              Isobus_key[index].output = ISOBUS_KEY_STATUS_RELEASE;
            }
            break;
        case ISOBUS_KEY_STATUS_RELEASE:
            Isobus_key[index].output = ISOBUS_KEY_STATUS_UP;
            break;
        default:
            break;
        }    
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_KeyAuxUpdate( uint8_t index, uint8_t key )
{
    switch ( Isobus_keyAux[index].output ) 
    {
    case ISOBUS_KEY_STATUS_WAIT:
        if ( Isobus_key[key].output == ISOBUS_KEY_STATUS_UP ) 
        {
            Isobus_keyAux[index].output = ISOBUS_KEY_STATUS_UP;
        }
        break;
    case ISOBUS_KEY_STATUS_UP:
        Isobus_keyAux[index].cnt = 0U;
        if ( Isobus_key[key].output == ISOBUS_KEY_STATUS_CLICK ) 
        {
            Isobus_keyAux[index].output = ISOBUS_KEY_STATUS_DOWN;
            Isobus_keyAux[index].transitionCnt = Isobus_keyAux[index].transitionCnt+1U;
        }
        break;
    case ISOBUS_KEY_STATUS_DOWN:
        if ( Isobus_key[key].output == ISOBUS_KEY_STATUS_DOWN ) 
        { 
            if ( ++Isobus_keyAux[index].cnt >= Isobus_keyAux[index].cntTh ) 
            {
                Isobus_keyAux[index].output = ISOBUS_KEY_STATUS_HOLD;
            }
        } 
        else 
        {
            Isobus_keyAux[index].output = ISOBUS_KEY_STATUS_UP;
        }
        break;
    case ISOBUS_KEY_STATUS_HOLD:
        if ( Isobus_key[key].output != ISOBUS_KEY_STATUS_DOWN ) 
        {
            Isobus_keyAux[index].output = ISOBUS_KEY_STATUS_UP;
        }
        break;
    default:
        break;
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_KeyModeUpdate( void ) 
{
    uint8_t aux;
    uint8_t j;

    aux = 0U;
    for( j=0U; j<( AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM ); j++ )
    {
        if ( Isobus_keyAux[j].output != ISOBUS_KEY_STATUS_UP ) 
        {
            aux++;
        }
    }
    
    if ( 0U == aux ) 
    {
        if ( true == isobus_KeyModeFirstEntry )
        {
            Isobus_key[Isobus_keyModeIndex-1U].output = ISOBUS_KEY_STATUS_UP;
            isobus_KeyModeFirstEntry = false;
        }
        switch ( Isobus_keyModeStatus ) 
        {
        case ISOBUS_KEY_MODE_STATUS_01:
            if ( Isobus_key[Isobus_keyModeIndex-1U].output == ISOBUS_KEY_STATUS_DOWN ) 
            {
                Isobus_keyModeStatus = ISOBUS_KEY_MODE_STATUS_02;
                isobus_LedOut[ISOBUS_LED_KEY_MODE] = ISOBUS_LED_ON;
            }
            break;
        case ISOBUS_KEY_MODE_STATUS_02:
            if ( Isobus_key[Isobus_keyModeIndex-1U].output == ISOBUS_KEY_STATUS_UP ) 
            {
                Isobus_keyModeStatus = ISOBUS_KEY_MODE_STATUS_01;
                isobus_LedOut[ISOBUS_LED_KEY_MODE] = ISOBUS_LED_OFF;
            }
            break;
        }
    }

    return;
}

//------------------------------------------------------------------------------
static void Isobus_KeyModeStatusChangeMain( void ) 
{
    uint8_t j;
    static uint32_t ChangeCounterTh = 0U;
    static uint8_t KeyNum = AUX_IN_BOOL_SHIFT_NUM;
    
//    if ( ! ( ( isobus_Vt.ownerSA == Saej_SourceAddressGet() ) && ( ( isobus_Vt.ownerID & 0x0000FFFFUL ) == 1000UL ) ) )
//    {
//        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_OFF;
//        for ( j=0U; j<(AUX_IN_BOOL_SHIFT_NUM+AUX_IN_BOOL_SHIFT_NUM); j++ ) 
//        {
//            Isobus_keyAux[j].pressedPrevious = BOOL_INPUT_TEMP_CHANGE_MODE;
//        }        
//    }
//    else
//    {

        if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1 )     
        {
            ObjectID = Container_3028; 
            if ( ISOBUS_KEY_MODE_STATUS_01 == KeyChangeValue ) 
            {
                AIDValue = 0x00U;
            } 
            else 
            {
                AIDValue = 0x01U;
            }
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            HideShowRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_1;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_1 )
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_2;
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_2;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1;
                    }
                }
            }
        }
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_2 )     
        {
            ObjectID = Container_3029; 
            if ( ISOBUS_KEY_MODE_STATUS_01 == KeyChangeValue ) 
            {
                AIDValue = 0x01U;
            } 
            else 
            {
                AIDValue = 0x00U;
            }
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            HideShowRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_2;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_RX_HEADER_MODE_2 )
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                LineBarChangeCounter = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_CONTAINER_AXIS_CHECK;
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_CONTAINER_AXIS_CHECK;
                        //Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_START;
                        LineBarChangeCounter = 0U;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_2;
                    }
                }
            }
        }
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_CONTAINER_AXIS_CHECK )     
        {
            ObjectID = Container_3030 + LineBarChangeCounter; 
            AIDValue = ContainerAxisShow[LineBarChangeCounter];
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            HideShowRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_CONTAINER_AXIS_CHECK_RX;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_CONTAINER_AXIS_CHECK_RX )
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                LineBarChangeCounter++;
                if ( LineBarChangeCounter < 4U )
                {
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_CONTAINER_AXIS_CHECK;
                }
                else
                {
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_START;
                }
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_START;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_CONTAINER_AXIS_CHECK;
                    }
                }
            }
        }        
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_START ) 
        {
            if ( Isobus_keyModeStatus == ISOBUS_KEY_MODE_STATUS_02 ) 
            {
                KeyChangeCounter = AUX_IN_BOOL_SHIFT_NUM;
                KeyNum = AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM;
            } 
            else 
            {
                KeyChangeCounter = 0U;
                KeyNum = AUX_IN_BOOL_SHIFT_NUM;
            }
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_CHECK;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_PRESS_CHECK ) 
        {
            if ( Isobus_keyAux[KeyChangeCounter].pressed != Isobus_keyAux[KeyChangeCounter].pressedPrevious ) 
            {
                Isobus_keyAux[KeyChangeCounter].pressChangeValue = Isobus_keyAux[KeyChangeCounter].pressed;
                if ( KeyChangeCounter < AUX_IN_BOOL_SHIFT_NUM ) 
                {
                    ObjectID = Ellipse_15002 + KeyChangeCounter;
                } 
                else 
                {
                    ObjectID = Ellipse_15002 + KeyChangeCounter - AUX_IN_BOOL_SHIFT_NUM;
                }
                Isobus_AttributeId = 7U;
                if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_AS_ROLLER ) 
                {
                    AIDValue = 0x61ADU;
                } 
                else if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_ON ) 
                {
                    AIDValue = 0x61A8U + Isobus_keyModeStatus;
                } 
                else if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_OFF) 
                {
                    AIDValue = 0x61ABU;
                } 
                else 
                {
                    AIDValue = 0x61AAU;
                }
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                ChangeAttribRequest = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_CHECK_RX;
            } 
            else 
            {
                  ChangeAttribOk = 1U;
                  ChangeAttribNumTx = 0U;
                  KeyChangeCounter++;
                  if ( KeyChangeCounter < KeyNum ) 
                  {
                      ChangeAttribRequest = 0U;
                  } 
                  else 
                  {
                      ChangeAttribRequest = 0U;
                      Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;
                  }                  
//                ChangeAttribOk = 1U;
//                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_CHECK_RX;
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_PRESS_CHECK_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                  Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK;
//                Isobus_keyAux[KeyChangeCounter].pressedPrevious = Isobus_keyAux[KeyChangeCounter].pressChangeValue;
//                ChangeAttribNumTx = 0U;
//                KeyChangeCounter++;
//                if ( KeyChangeCounter < KeyNum ) 
//                {
//                    ChangeAttribRequest = 0U;
//                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_CHECK;
//                } 
//                else 
//                {
//                    ChangeAttribRequest = 0U;
//                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_START;
//                }
            } 
            else 
            {
                if( ChangeCounterTh++> ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK;
//                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;  
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_START; 
                    }
                }
            }
        } 
        //--- header DM
//        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_HEADER_START ) 
//        {
//            if ( Isobus_keyModeStatus == ISOBUS_KEY_MODE_STATUS_02 ) 
//            {
//                KeyChangeCounter = AUX_IN_BOOL_SHIFT_NUM;
//                KeyNum = AUX_IN_BOOL_SHIFT_NUM + AUX_IN_BOOL_SHIFT_NUM;
//            } 
//            else 
//            {
//                KeyChangeCounter = 0U;
//                KeyNum = AUX_IN_BOOL_SHIFT_NUM;
//            }
//            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK;
//        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK ) 
        {
//            if ( Isobus_keyAux[KeyChangeCounter].pressed != Isobus_keyAux[KeyChangeCounter].pressedPrevious ) 
//            {
//                Isobus_keyAux[KeyChangeCounter].pressChangeValue = Isobus_keyAux[KeyChangeCounter].pressed;
                if ( KeyChangeCounter < AUX_IN_BOOL_SHIFT_NUM ) 
                {
                    ObjectID = Ellipse_15012 + KeyChangeCounter;
                } 
                else 
                {
                    ObjectID = Ellipse_15012 + KeyChangeCounter - AUX_IN_BOOL_SHIFT_NUM;
                }
                //Isobus_AttributeId = 7U;
                Isobus_AttributeId = 5U;
//                if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_AS_ROLLER ) 
//                {
//                    AIDValue = 0x61ADU;
//                } 
                if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_ON ) 
                {
                    AIDValue = 0x61A8U + Isobus_keyModeStatus;
                } 
                else if ( Isobus_keyAux[KeyChangeCounter].pressed == BOOL_INPUT_OFF) 
                {
                    AIDValue = 0x61ABU;
                } 
                else 
                {
                    AIDValue = 0x61AAU;
                }
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                ChangeAttribRequest = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK_RX;
//            } 
//            else 
//            {
//                ChangeAttribOk = 1U;
//                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK_RX;
//            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_HEADER_PRESS_CHECK_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                Isobus_keyAux[KeyChangeCounter].pressedPrevious = Isobus_keyAux[KeyChangeCounter].pressChangeValue;
                ChangeAttribNumTx = 0U;
                KeyChangeCounter++;
                if ( KeyChangeCounter < KeyNum ) 
                {
                    ChangeAttribRequest = 0U;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_CHECK;
                } 
                else 
                {
                    ChangeAttribRequest = 0U;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;
                }
            } 
            else 
            {
                if( ChangeCounterTh++> ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_START; 
                    }
                }
            }
        }         
        //--- header DM - fine        
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_PRESS_0 ) 
        {
            if ( Isobus_keyAux[KEY_MODE].pressed != Isobus_keyAux[KEY_MODE].pressedPrevious ) 
            {
                Isobus_keyAux[KEY_MODE].pressChangeValue = Isobus_keyAux[KEY_MODE].pressed;
                ObjectID = Ellipse_15011;
                Isobus_AttributeId = 7U;
                if ( Isobus_keyAux[KEY_MODE].pressed == BOOL_INPUT_ON ) 
                {
                    AIDValue = 0x61ACU;
                } 
                else if ( Isobus_keyAux[KEY_MODE].pressed == BOOL_INPUT_OFF) 
                {
                    AIDValue = 0x61ABU;
                } 
                else 
                {
                    AIDValue = 0x61AAU;
                }
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                ChangeAttribRequest = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0_RX;
            } 
            else 
            {
                ChangeAttribOk = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0_RX;
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_PRESS_0_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                Isobus_keyAux[KEY_MODE].pressedPrevious = Isobus_keyAux[KEY_MODE].pressChangeValue;
                ChangeAttribNumTx = 0U;
                ChangeAttribRequest = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_OFF;
            } 
            else 
            {
                if ( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    {
                        //Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;    //da verificare
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_OFF;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0; 
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_OFF ) 
        {
            if ( Isobus_keyModeStatusPre != Isobus_keyModeStatus ) 
            {
                KeyChangeValue = Isobus_keyModeStatus;
                KeyChangeCounter = 0U;
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                ChangeAttribRequest = 0U;
                for ( j=0U; j<(AUX_IN_BOOL_SHIFT_NUM+AUX_IN_BOOL_SHIFT_NUM); j++ ) 
                {
                    Isobus_keyAux[j].pressedPrevious = BOOL_INPUT_TEMP_CHANGE_MODE;
                }
                Isobus_keyModeStatusPre = Isobus_keyModeStatus;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_POINTER;
            } 
            else 
            {
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_POINTER ) 
        {
            ObjectID = 0x80EDU + KeyChangeCounter;
            Isobus_AttributeId = 2U;
            AIDValue = 0x6D65U + ( KeyChangeValue * AUX_IN_BOOL_SHIFT_NUM ) + KeyChangeCounter;
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            ChangeAttribRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_RX_POINTER;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_RX_POINTER ) 
        {
            if ( 0U < ChangeAttribOk )  
            {
                ChangeAttribNumTx = 0U;
                KeyChangeCounter++;
                if ( KeyChangeCounter < AUX_IN_BOOL_SHIFT_NUM ) 
                {
                    ChangeAttribRequest = 0U;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_POINTER;
                } 
                else 
                {
                    ChangeAttribRequest = 0U;
                    KeyChangeCounter = 0U;
                    if ( 0U < isobus_Vt.graphic ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_RECT;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE;
                    }
                }
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        KeyChangeCounter = 0U;
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        if ( 0U < isobus_Vt.graphic ) 
                        { 
                            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_RECT;
                        } 
                        else 
                        {
                            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE;
                        }
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_RECT ) 
        {
            ObjectID = 0x36B0U;
            Isobus_AttributeId = 1U;
            AIDValue = 0x5DC0U + KeyChangeValue;
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            ChangeAttribRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_RX_RECT;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_ON_RX_RECT ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                ChangeAttribRequest = 0U;
                ChangeAttribNumTx = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK; 
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        KeyChangeCounter = 0U;
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_RECT;
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE ) 
        {
            ObjectID = Container_3019; 
            if ( KeyChangeValue ) 
            {
                AIDValue = 0x00U;
            } 
            else 
            {
                AIDValue = 0x01U;
            }
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            HideShowRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE_RX;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE;
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_1_MASK_CHANGE;
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE ) 
        {
            ObjectID = Container_3020; 
            if ( KeyChangeValue ) 
            {
                AIDValue = 0x01U;
            } 
            else 
            {
                AIDValue = 0x00U;
            }
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            HideShowRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE_RX;
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_2_MASK_CHANGE;
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_UP_CHECK ) 
        {
            if ( Isobus_deadManShowPrevious != Isobus_deadManShow ) 
            {
                ObjectID = Container_3022; 
                AIDValue = Isobus_deadManShow;
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                HideShowRequest = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK_RX;
            } 
            else 
            {
                KeyChangeCounter = 0U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK;
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_UP_CHECK_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                KeyChangeCounter = 0U;
                Isobus_deadManShowPrevious = Isobus_deadManShow;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK;
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_UP_CHECK;
                    }
                }
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_LATCH_CHECK ) 
        {
            if ( Isobus_analogAux[KeyChangeCounter].latchShowPrevious != Isobus_analogAux[KeyChangeCounter].latchShow ) 
            {
                ObjectID = Container_3023 + KeyChangeCounter; 
                AIDValue = Isobus_analogAux[KeyChangeCounter].latchShow;
                ChangeAttribOk = 0U;
                ChangeCounterTh = 0U;
                HideShowRequest = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK_RX;
            } 
            else 
            {
                ChangeAttribOk = 1U;
                Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK_RX;
            }
        } 
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_KEY_MODE_LATCH_CHECK_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                HideShowRequest = 0U;
                ChangeAttribNumTx = 0U;
                ChangeAttribRequest = 0U;                
                Isobus_analogAux[KeyChangeCounter].latchShowPrevious = Isobus_analogAux[KeyChangeCounter].latchShow;
                KeyChangeCounter++;
                if ( KeyChangeCounter < ISOBUS_AXIS_NUM ) 
                {
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK;
                } 
                else 
                {
                    //Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK;
                    LineBarChangeCounter = 0U;
                }
            } 
            else 
            {
                if( ChangeCounterTh++ > ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        //Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1;
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK;
                        LineBarChangeCounter = 0U;
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_LATCH_CHECK;
                    }
                }
            }
        }
//        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_LINEBAR_START ) 
//        {
//            LineBarChangeCounter = 0U;
//            Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK;          
//        }
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_LINEBAR_CHECK ) 
        {
            ObjectID = LineBar_18000 + LineBarChangeCounter;
            AIDValue = LineBarSetPoint[LineBarChangeCounter];
            ChangeAttribOk = 0U;
            ChangeCounterTh = 0U;
            ChangeNumValueRequest = 1U;
            Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK_RX;
        }
        else if ( Isobus_keyModeStatusChange == ISOBUS_STATUS_LINEBAR_CHECK_RX ) 
        {
            if ( 0U < ChangeAttribOk ) 
            {
                ChangeAttribNumTx = 0U;
                LineBarChangeCounter++;
                if ( LineBarChangeCounter < 4 ) 
                {
                    ChangeNumValueRequest = 0U;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK;
                } 
                else 
                {
                    ChangeNumValueRequest = 0U;
                    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1;
                }
            } 
            else 
            {
                if( ChangeCounterTh++> ( 1000000U / SCHEDULE_COUNTER_TH_SAEJ ) ) 
                { 
                    if ( ChangeAttribNumTx++ > 1U ) 
                    { 
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_ON_TX_CHANGE_HEADER_MODE_1;
//                        Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_PRESS_0;  
                        ChangeAttribNumTx = 0U;
                    } 
                    else 
                    {
                        Isobus_keyModeStatusChange = ISOBUS_STATUS_LINEBAR_CHECK; 
                    }
                }
            }
        }        
//    }

    return;
}

//------------------------------------------------------------------------------
static void Isobus_KeyMain( void )
{
    uint8_t i;
    uint8_t j;
    
    for ( i=0U; i<ISOBUS_PUSH_BUTTON_NUM; i++ )
    {
        Isobus_KeyInputUpdate( i );
    }
    
    if ( ( Isobus_keyModeIndex > 0U ) && ( Isobus_keyModeIndex <= ISOBUS_PUSH_BUTTON_NUM ) ) 
    {
        Isobus_KeyModeUpdate();
    } 
    else 
    {
        Isobus_keyModeStatus = ISOBUS_KEY_MODE_STATUS_01;
    }

    if ( ( Isobus_keyDeadManIndex > 0U ) && ( Isobus_keyDeadManIndex <= ISOBUS_PUSH_BUTTON_NUM ) ) 
    {
        if ( Isobus_key[Isobus_keyDeadManIndex-1U].output == ISOBUS_KEY_STATUS_DOWN ) 
        { 
            Isobus_deadManEn = true; 
            Isobus_deadManShow = 1U; 
        } 
        else 
        {
            Isobus_deadManEn = false; 
            Isobus_deadManShow = 0U; 
        }
    } 
    else 
    { 
        Isobus_deadManEn = true; 
    }

//IL LATCH LO FACCIAMO COL DETENT  
//    if ( ( Isobus_keyLatchIndex > 0U ) && ( Isobus_keyLatchIndex <= ISOBUS_PUSH_BUTTON_NUM ) ) 
//    {
//        for ( i=0U; i<ISOBUS_AXIS_NUM; i++ ) 
//        {
//            for ( j=0U; j<AuxInAnNum; j++ ) 
//            {
//                if ( ( AuxInput[j].ID == AUX_INPUT_ID[i] ) && ( ( CfgAuxAn[i] & ANALOG_WITH_LATCH ) == ANALOG_WITH_LATCH ) ) 
//                {
//                    if ( LatchEn[i] ) {
//              if ( LatchOtherDir[i] ) { LatchEn[i] = 0u; LatchOtherDir[i] = 0u; }
//            } else {
//              LatchShow[i] = 0u;
//              if ( PKStatus[KeyLatchIndex-1u] == PK_STATUS_KEY_CLICK ) { 
//                if ( UpEn ) { 
//                  if ( JoyDgt[i].OutA > 0 ) { LatchEn[i] = 1u; LatchOtherDir[i] = JoyDgt[i].OutB; LatchShow[i] = 1u;}
//                  if ( JoyDgt[i].OutB > 0 ) { LatchEn[i] = 2u; LatchOtherDir[i] = JoyDgt[i].OutA; LatchShow[i] = 1u;}
//                }
//              } 
//            }
//          }
//        }
//      } 
//    } 
  
    if ( true == Isobus_deadManEn ) 
    {
        Isobus_KeyAuxUpdate( KEY_MODE, CfgkeyIndex[PHYS_KEY_09] );
        if ( ISOBUS_KEY_MODE_STATUS_01 == Isobus_keyModeStatus ) 
        {
            for ( j=0U; j<AUX_IN_BOOL_SHIFT_NUM; j++ ) 
            {
                Isobus_KeyAuxUpdate( j, CfgkeyIndex[j] );  
                Isobus_keyAux[j+AUX_IN_BOOL_SHIFT_NUM].output = ISOBUS_KEY_STATUS_UP;  
            }
        } 
        else 
        {
            for ( j=0U; j<AUX_IN_BOOL_SHIFT_NUM; j++ ) 
            {
                Isobus_keyAux[j].output = ISOBUS_KEY_STATUS_UP;  
                Isobus_KeyAuxUpdate( j+AUX_IN_BOOL_SHIFT_NUM, CfgkeyIndex[j] );  
            }
        }
        Isobus_KeyAuxUpdate( KEY_DIR_X_SX, PHYS_KEY_10 );
        Isobus_KeyAuxUpdate( KEY_DIR_X_DX, PHYS_KEY_11 );
        Isobus_KeyAuxUpdate( KEY_DIR_Y_BW, PHYS_KEY_12 );
        Isobus_KeyAuxUpdate( KEY_DIR_Y_FW, PHYS_KEY_13 );
    } 
    else 
    {
        for ( j=0U; j<AUX_IN_BOOL_NUM_MAX; j++ ) 
        {
            Isobus_keyAux[j].output = ISOBUS_KEY_STATUS_UP;
        }
    }   
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AnalogMain( void )
{
    uint32_t aux;
    uint8_t i;
    
    for ( i=0U; i<ISOBUS_AXIS_NUM; i++ )
    {
        if ( false == Isobus_deadManEn )
        {
            Isobus_analogAux[i].value = ISOBUS_NEUTRAL;
        }
        else
        {
            if ( SAEJ_AXIS_STATUS_ERROR == Saej_AxisGet(i).status )
            {
                Isobus_analogAux[i].value = ISOBUS_ERROR;    
                Isobus_analogAux[i].latchEn = 0U;
                Isobus_analogAux[i].latchOtherDir = 0U;
                Isobus_analogAux[i].latchShow = 0U;
            }
            else if ( SAEJ_AXIS_STATUS_NEUTRAL == Saej_AxisGet(i).status )
            {
                Isobus_analogAux[i].value = ISOBUS_NEUTRAL;    
                Isobus_analogAux[i].latchEn = 0U;
                Isobus_analogAux[i].latchShow = 0U;
            }
            else if ( SAEJ_AXIS_STATUS_NEGATIVE == Saej_AxisGet(i).status )
            {
                if ( ( ANALOG_WITH_LATCH == ( CfgAuxAn[i] & ANALOG_WITH_LATCH ) ) && ( SAEJ_DETENT_ON == Saej_AxisGet(i).detentOut ) )
                {
                    Isobus_analogAux[i].value = ISOBUS_LATCH_BWD;
                    Isobus_analogAux[i].latchEn = 1U;
                    Isobus_analogAux[i].latchShow = 1U;
                }
                else
                {
                    aux = (uint32_t)( Saej_AxisGet(i).value - SAEJ_AXIS_OUT_NEUTRAL );
                    aux = aux * (uint32_t)( ISOBUS_NEUTRAL - ISOBUS_MIN );
                    aux = aux / (uint32_t)( SAEJ_AXIS_OUT_MAX - SAEJ_AXIS_OUT_NEUTRAL );
                    Isobus_analogAux[i].value = ISOBUS_NEUTRAL - (uint16_t)( aux );
                    Isobus_analogAux[i].latchEn = 0U;
                    Isobus_analogAux[i].latchShow = 0U;                    
                }
            }
            else if ( SAEJ_AXIS_STATUS_POSITIVE == Saej_AxisGet(i).status )
            {
                if ( ( ANALOG_WITH_LATCH == ( CfgAuxAn[i] & ANALOG_WITH_LATCH ) ) && ( SAEJ_DETENT_ON == Saej_AxisGet(i).detentOut ) )
                {
                    Isobus_analogAux[i].value = ISOBUS_LATCH_FWD;
                    Isobus_analogAux[i].latchEn = 2U;
                    Isobus_analogAux[i].latchShow = 1U;                    
                }
                else
                {
                    aux = (uint32_t)( Saej_AxisGet(i).value - SAEJ_AXIS_OUT_NEUTRAL );
                    aux = aux * (uint32_t)( ISOBUS_MAX - ISOBUS_NEUTRAL );
                    aux = aux / (uint32_t)( SAEJ_AXIS_OUT_MAX - SAEJ_AXIS_OUT_NEUTRAL );
                    Isobus_analogAux[i].value = ISOBUS_NEUTRAL + (uint16_t)( aux );
                    Isobus_analogAux[i].latchEn = 0U;
                    Isobus_analogAux[i].latchShow = 0U;                    
                }   
            }
            else
            {
                Isobus_analogAux[i].value = ISOBUS_ERROR;    
                Isobus_analogAux[i].latchEn = 0U;
                Isobus_analogAux[i].latchShow = 0U;
            }    
        }
    }
    
    if ( ( SAEJ_AXIS_STATUS_NEUTRAL == Saej_AxisGet(SAEJ_AXIS_X).status ) || ( SAEJ_AXIS_STATUS_ERROR == Saej_AxisGet(SAEJ_AXIS_X).status ) )
    {
        LineBarSetPoint[0U] = 0U;
        LineBarSetPoint[1U] = 0U;
    }
    else
    {
        if ( SAEJ_AXIS_STATUS_POSITIVE == Saej_AxisGet(SAEJ_AXIS_X).status )
        {
            LineBarSetPoint[0U] = 0U;
            LineBarSetPoint[1U] = Saej_AxisGet(SAEJ_AXIS_X).value;
        }
        else if ( SAEJ_AXIS_STATUS_NEGATIVE == Saej_AxisGet(SAEJ_AXIS_X).status )
        {
            LineBarSetPoint[1U] = 0U;
            LineBarSetPoint[0U] = Saej_AxisGet(SAEJ_AXIS_X).value;          
        }
        else
        {
            LineBarSetPoint[0U] = 0U;
            LineBarSetPoint[1U] = 0U;          
        }
    }

    if ( ( SAEJ_AXIS_STATUS_NEUTRAL == Saej_AxisGet(SAEJ_AXIS_Y).status ) || ( SAEJ_AXIS_STATUS_ERROR == Saej_AxisGet(SAEJ_AXIS_Y).status ) )
    {
        LineBarSetPoint[2U] = 0U;
        LineBarSetPoint[3U] = 0U;
    }
    else
    {
        if ( SAEJ_AXIS_STATUS_POSITIVE == Saej_AxisGet(SAEJ_AXIS_Y).status )
        {
            LineBarSetPoint[3U] = 0U;
            LineBarSetPoint[2U] = Saej_AxisGet(SAEJ_AXIS_Y).value;
        }
        else if ( SAEJ_AXIS_STATUS_NEGATIVE == Saej_AxisGet(SAEJ_AXIS_Y).status )
        {
            LineBarSetPoint[2U] = 0U;
            LineBarSetPoint[3U] = Saej_AxisGet(SAEJ_AXIS_Y).value;          
        }
        else
        {
            LineBarSetPoint[2U] = 0U;
            LineBarSetPoint[3U] = 0U;          
        }
    }    
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AuxInputAnalogRefresh( uint8_t index )
{
    uint8_t keyaux;  

    AuxInput[index].value1Pre = AuxInput[index].value1;
    AuxInput[index].value2Pre = AuxInput[index].value2;
    if ( AuxInput[index].type == 141U ) 
    {
        AuxInput[index].value1 = Isobus_analogAux[AuxInput[index].indexInput2].value;
        AuxInput[index].value2 = Isobus_analogAux[AuxInput[index].indexInput].value;
    }
    else
    {
        AuxInput[index].value1 = Isobus_analogAux[AuxInput[index].indexInput].value;
        AuxInput[index].value2 = 0xFFFFU;
    }
    if ( ( Isobus_analogAux[AuxInput[index].indexInput].value != ISOBUS_NEUTRAL ) && ( Isobus_analogAux[AuxInput[index].indexInput].value != ISOBUS_ERROR ) ) 
    {
        keyaux = BOOL_INPUT_AS_ROLLER;
    } 
    else 
    {
        keyaux = BOOL_INPUT_OFF;
    }
    if ( AuxInput[index].ID == AUX_INPUT_ID[SAEJ_AXIS_W] ) 
    {
        Isobus_keyAux[KEY_5].pressed = keyaux;
        Isobus_keyAux[KEY_5_M2].pressed = keyaux;      
    } 
    else if ( AuxInput[index].ID == AUX_INPUT_ID[SAEJ_AXIS_Z] ) 
    {
        Isobus_keyAux[KEY_7].pressed = keyaux;
        Isobus_keyAux[KEY_7_M2].pressed = keyaux;      
    } else if ( AuxInput[index].ID == AUX_INPUT_ID[SAEJ_AXIS_J] ) {
        Isobus_keyAux[KEY_9].pressed = keyaux;
        Isobus_keyAux[KEY_9_M2].pressed = keyaux;      
    }
    if ( AuxInput[index].type == 137U ) 
    {
        if ( Isobus_analogAux[AuxInput[index].indexInput].latchEn == 1U ) 
        { 
            AuxInput[index].value1 = 0xFB01U; 
        }
        if ( Isobus_analogAux[AuxInput[index].indexInput].latchEn == 2U ) 
        { 
            AuxInput[index].value1 = 0xFB00U; 
        }
    }
//    if ( AuxInput[index].type == 141U ) 
//    {
//        AuxInput[index].value2Pre = AuxInput[index].value2;
//        AuxInput[index].value2 = Isobus_analogAux[AuxInput[index].indexInput2].value;
//    } 
//    else 
//    {
//        AuxInput[index].value2 = 0xFFFFU;
//    }  
  
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AuxInputDigitalRefresh( uint8_t index )
{
    static uint8_t latchup[AUX_IN_BOOL_NUM_MAX];
    static uint8_t latchdw[AUX_IN_BOOL_NUM_MAX];
    static uint16_t PkTransitionPre1[AUX_IN_BOOL_NUM_MAX];
    static uint16_t PkTransitionPre2[AUX_IN_BOOL_NUM_MAX];
    static bool first_entry = true;
    
    if ( true == first_entry )
    {
        memset( latchup, 0U, AUX_IN_BOOL_NUM_MAX );
        memset( latchdw, 0U, AUX_IN_BOOL_NUM_MAX );
        memset( PkTransitionPre1, 0U, AUX_IN_BOOL_NUM_MAX );
        memset( PkTransitionPre2, 0U, AUX_IN_BOOL_NUM_MAX );
        first_entry = false;
    }

    AuxInput[index].value1Pre = AuxInput[index].value1;
    if ( false == Isobus_deadManEn ) { 
        AuxInput[index].value1 = 0U; 
    }
    if ( ( AuxInput[index].enabled == 0u ) && ( AuxInput[index].learnMode == 0u ) ) 
    {
        Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED;
    }
    else
    {
        switch ( AuxInput[index].value1 ) 
        {
        case 0U:
            if ( 0U == AuxInput[index].learnMode ) 
            { 
                Isobus_keyAux[AuxInput[index].indexInput].pressed = 0U; 
            }
            else
            {
                Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED;
            }
            if ( AuxInput[index].type > 0x82U ) 
            {
                if ( 0U == AuxInput[index].learnMode ) 
                { 
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = 0U; 
                }
                else
                {
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = BOOL_INPUT_UNUSED;
                }
            }
            if ( true == Isobus_deadManEn ) 
            {
                if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_UP ) 
                { 
                    latchup[index] = 0U; 
                }
                if ( ( AuxInput[index].type > 0x82U ) && ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_UP ) ) 
                { 
                    latchdw[index] = 0U; 
                }
                if ( ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_DOWN ) && ( 0U == latchup[index] ) ) 
                {
                    AuxInput[index].value1 = 1U;
                    if ( ( AuxInput[index].type == 0x82U ) || ( AuxInput[index].type == 0x88U ) || ( AuxInput[index].type == 0x86U ) ) 
                    {
                        AuxInput[index].value2 = AuxInput[index].value2 - PkTransitionPre1[index] + Isobus_keyAux[AuxInput[index].indexInput].transitionCnt;
                        PkTransitionPre1[index] = Isobus_keyAux[AuxInput[index].indexInput].transitionCnt;
                    } 
                    else 
                    {
                        AuxInput[index].value2++;
                    }
                } 
                else if ( ( AuxInput[index].type > 0x82U ) && ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_DOWN ) && ( 0U == latchdw[index] ) ) 
                {
                    AuxInput[index].value1 = 4U;
                    if ( ( AuxInput[index].type == 0x86U ) || ( AuxInput[index].type == 0x87U ) ) 
                    {
                        AuxInput[index].value2 = AuxInput[index].value2 - PkTransitionPre2[index] + Isobus_keyAux[AuxInput[index].indexInput2].transitionCnt;
                        PkTransitionPre2[index] = Isobus_keyAux[AuxInput[index].indexInput2].transitionCnt;
                    } 
                    else 
                    {
                        AuxInput[index].value2++;
                    }
                } 
                else if ( ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_WAIT ) || ( ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_WAIT ) && ( AuxInput[index].type > 0x82U ) ) ) 
                {
                    AuxInput[index].value1 = ISOBUS_ERROR;
                }
            }
            break;
        case 1U:
            if ( 0U == AuxInput[index].learnMode ) 
            { 
                if ( 1U == AuxInput[index].enabled )
                {
                    Isobus_keyAux[AuxInput[index].indexInput].pressed = 1U; 
                }
                else
                {
                    Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED; 
                }
            }
            else
            {
                Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED;
            }
            if ( ( AuxInput[index].type == 0x82U ) || ( AuxInput[index].type == 0x88U ) || ( AuxInput[index].type == 0x86U ) ) 
            {
                if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_HOLD ) 
                {
                    AuxInput[index].value1 = 2U;
                }
                if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_UP ) 
                {
                    AuxInput[index].value1 = 0U;
                }
            } 
            else 
            {
                if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_UP ) 
                { 
                    latchup[index] = 1U;
                }
                if ( ( 1U == latchup[index] ) && ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_DOWN ) ) 
                {
                    AuxInput[index].value1 = 0U;
                }
            }
            break;
        case 2U:
            if ( 0U == AuxInput[index].learnMode ) 
            { 
                if ( 1U == AuxInput[index].enabled )
                {
                    Isobus_keyAux[AuxInput[index].indexInput].pressed = 1U; 
                }
                else
                {
                    Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED; 
                }
            }
            else
            {
                Isobus_keyAux[AuxInput[index].indexInput].pressed = BOOL_INPUT_UNUSED;
            }
            if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_UP ) 
            {
                AuxInput[index].value1 = 0U;
            }
            break;
        case 4U:
            if ( 0U == AuxInput[index].learnMode ) 
            { 
                if ( 1U == AuxInput[index].enabled )
                {
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = 1U; 
                }
                else
                {
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = BOOL_INPUT_UNUSED; 
                }
            }     
            else
            {
                Isobus_keyAux[AuxInput[index].indexInput2].pressed = BOOL_INPUT_UNUSED; 
            }
            if ( ( AuxInput[index].type == 0x87U ) || ( AuxInput[index].type == 0x86U ) ) 
            {
                if ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_HOLD ) 
                {
                    AuxInput[index].value1 = 8U;
                }
                if ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_UP ) 
                {
                    AuxInput[index].value1 = 0U;
                }
            } 
            else 
            {
                if ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_UP ) 
                {
                    latchdw[index] = 1U;
                }
                if ( ( 1U == latchdw[index] ) && ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_DOWN ) ) 
                {
                    AuxInput[index].value1 = 0U;
                }
            }
            break;
        case 8U:
            if ( 0U == AuxInput[index].learnMode ) 
            { 
                if ( 1U == AuxInput[index].enabled )
                {
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = 1U; 
                }
                else
                {
                    Isobus_keyAux[AuxInput[index].indexInput2].pressed = BOOL_INPUT_UNUSED; 
                }
            }
            else
            {
                Isobus_keyAux[AuxInput[index].indexInput2].pressed = BOOL_INPUT_UNUSED; 
            }
            if ( Isobus_keyAux[AuxInput[index].indexInput2].output == ISOBUS_KEY_STATUS_UP ) 
            {
                AuxInput[index].value1 = 0U;
            }
            break;
        case ISOBUS_ERROR:
            if ( Isobus_keyAux[AuxInput[index].indexInput].output == ISOBUS_KEY_STATUS_UP ) 
            {
                AuxInput[index].value1 = 0U;
            }
            if ( ( AuxInput[index].type > 0x82U ) && ( Isobus_keyAux[AuxInput[index].indexInput2].output != ISOBUS_KEY_STATUS_UP ) ) 
            {
                AuxInput[index].value1 = ISOBUS_ERROR;
            }
            break;
        default:
            break;
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_AuxInMain( void )
{
  //static UC_08 TxWsMainTimeout=0u;
  //static UC_08 TxAuxInMainTimeout=0u;
  //static uint8_t TxAuxInStatusTimeout[AUX_INPUT_NUM_MAX];
  //static UC_08 TxAuxInChangeTimeout[AUX_INPUT_NUM_MAX];
  
  //static UC_08 AuxInCounter=0u;
  //static UC_08 AuxInChangeCounter=0u;
  //UI_16 MsgToServe;
  
    static uint32_t TxAuxInStatusCounter[AUX_INPUT_NUM_MAX];
    uint32_t TxTimeoutCounterTh;
    uint8_t AuxInTxEnable[AUX_INPUT_NUM_MAX];
    uint8_t AuxInEn[AUX_INPUT_NUM_MAX];
    static uint8_t AuxInEnPrev[AUX_INPUT_NUM_MAX] = 
    { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
      0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    };
    
    uint8_t j;

    for ( j=0U; j<AuxInNum; j++ )
    {
        AuxInput[j].learnMode = isobus_Vt.learnModeStatus;
        AuxInEn[j] = 0U;
        if ( (AuxInput[j].enabled != 0U ) || ( AuxInput[j].learnMode != 0U ) )
        {
            AuxInEn[j] = 1U;
        } 
        if ( AuxInEn[j] != 0U ) 
        {
            if ( ++AuxInput[j].cnt >= AuxInput[j].cntTh )
            {
                AuxInput[j].cnt = AuxInput[j].cntTh;
            }
        } 
        else 
        {
            if ( AuxInEn[j] != AuxInEnPrev[j] )
            {
                if ( ++AuxInput[j].cnt >= AuxInput[j].cntTh )
                {
                    AuxInput[j].cnt = AuxInput[j].cntTh;
                }
            }
            else
            {
                AuxInput[j].cnt = 0U;
                TxAuxInStatusCounter[j]=0U;
            }
        }
        if ( AuxInput[j].cnt >= AuxInput[j].cntTh )
        {
            AuxInEnPrev[j] = AuxInEn[j];
        }
    }
    
    ContainerAxisShow[0U] = 1U;
    ContainerAxisShow[1U] = 1U;
    ContainerAxisShow[2U] = 1U;
    ContainerAxisShow[3U] = 1U;
    /* per test scalatura DP */
    for ( j=0U; j<AuxInNum; j++ )    
    {
        if ( ( 0x6D78 == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[0U] = 0U;
        }
        if ( ( 0x6D79 == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[1U] = 0U;
        }        
        if ( ( 0x6D60 == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[0U] = 0U;
            ContainerAxisShow[1U] = 0U;
        }
        
        if ( ( 0x6D7A == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[3U] = 0U;
        }
        if ( ( 0x6D7B == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[2U] = 0U;
        }        
        if ( ( 0x6D61 == AuxInput[j].ID ) && ( 0x00 != AuxInput[j].enabled ) )
        {
            ContainerAxisShow[2U] = 0U;
            ContainerAxisShow[3U] = 0U;
        }        
    }
    /**/
  
//    if ( CAN_NO_MSG == CANMsgTx_Isobus_EcuToVt.NewMsg )
//    {
        for ( j=0U; j<AuxInNum; j++ )
        {
            AuxInTxEnable[j] = 0U;
            if ( AuxInput[j].cnt == AuxInput[j].cntTh )
            {
                AuxInput[j].cnt = 0U;
                TxTimeoutCounterTh = ( 1000000U / AuxInput[j].cntTh ) / SCHEDULE_COUNTER_TH_SAEJ;
                if ( j >= AuxInAnNum ) 
                {
                    if ( ( AuxInput[j].value1 == 2U ) || ( AuxInput[j].value1 == 8U ) ) 
                    {
                        TxTimeoutCounterTh = ( 200000U / AuxInput[j].cntTh ) / SCHEDULE_COUNTER_TH_SAEJ;
                    }
                }
                if ( ++TxAuxInStatusCounter[j] >= TxTimeoutCounterTh) 
                {
                    AuxInTxEnable[j] = 1U;
                    TxAuxInStatusCounter[j] = 0U;
                }
                if ( j < AuxInAnNum ) 
                {
                    Isobus_AuxInputAnalogRefresh(j);
                }
                else 
                {
                    Isobus_AuxInputDigitalRefresh(j);
                }
                if ( ( AuxInput[j].value1 != AuxInput[j].value1Pre) || ( ( AuxInput[j].type == 141U ) && ( AuxInput[j].value2 != AuxInput[j].value2Pre ) ) ) 
                {
                    AuxInTxEnable[j] = 1U;
                    if ( ( j >= AuxInAnNum ) && ( ( AuxInput[j].value1 == 2U ) || ( AuxInput[j].value1 == 8U ) ) ) 
                    {
                        TxAuxInStatusCounter[j]=0U;
                    }
                }
                if ( AuxInTxEnable[j] == 1U ) 
                {
                    Isobus_AuxInStatusMsgTx( j );
                    break;
                }
            }    
        }
//    }

//        for ( j=0U; j<AuxInNum; j++ )
//        {
//            if ( AuxInTxEnable[j] == 1U )
//            {
//                Isobus_AuxInStatusMsgTx( j ); //UpdateAuxInStatusMsg(j);
//            }
//  }
//  if (MsgToServe==1u){
//    UpdateWSMaintenanceMsg();
//    TxWsMainTimeout = 0u;
//    TXMsg[MSG_TX_ECU_TO_VT].NewMsg = CAN_NEW_MSG;
//  } else if (MsgToServe==2u) {
//    UpdateAuxInMaintenanceMsg();
//    TxAuxInMainTimeout = 0u;
//    TXMsg[MSG_TX_ECU_TO_VT].NewMsg = CAN_NEW_MSG;
//  } else {
//    if ( ChangeAttribRequest ) {
//      UpdateECUToVTMsg(ATTRIB_CHANGE);
//      ChangeAttribRequest = 0u;
//      TXMsg[MSG_TX_ECU_TO_VT].NewMsg = CAN_NEW_MSG;
//    }
//    if ( HideShowRequest ) {
//      UpdateECUToVTMsg(HIDE_SHOW_OBJECT);
//      HideShowRequest = 0u;
//      TXMsg[MSG_TX_ECU_TO_VT].NewMsg = CAN_NEW_MSG;
//    }
//
//  }
//  
    return;
}

//------------------------------------------------------------------------------
static wres_t Isobus_ClaimCheckForVtFunction( void )
{
    wres_t retVal;
    uint8_t rxMsg;
    static uint32_t counter = 0U;
    
    retVal = WRES_OK;
    if ( ( isobus_Net.status.value != ISOBUS_NET_OFF_STATUS ) && ( isobus_Net.status.value != ISOBUS_NET_STARTUP_STATUS ) )
    {
        if ( ++counter >= ( 5000000U / SCHEDULE_COUNTER_TH_SAEJ ) )
        {
            counter = 0U;
            Saej_SendPGNRequest( 0xEE00UL, isobus_Vt.sourceAddr );
        } 
        if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_ADDRESS_CLAIM, &rxMsg ) )
        {
            if ( CAN_StoredMsgGet( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg ).SA == isobus_Vt.sourceAddr )
            {
                if ( ( 0x1DU != CAN_StoredMsgGetData( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg, 5U ) ) || ( 0x00U != ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg, 4U ) & 0xF8U ) ) )
                {
                    retVal = WRES_ERROR;
                }
            }
            CAN_StoredMsgClear( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg );
        } 
    }
    else
    {
        counter = 0U;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static void Isobus_paramToRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED
#ifdef ISOBUS_DJW
    uint32_t aux;

    aux = (uint32_t)( Isobus_certification[0U] ) + (uint32_t)( Isobus_certification[1U] << 8U ) + (uint32_t)( Isobus_certification[2U] << 16U ) + (uint32_t)( Isobus_certification[3U] << 24U );
    MemData_SetRAMItem( ADDR_ISOBUS_CERT_0_3, aux );
    aux = (uint32_t)( Isobus_certification[4U] ) + (uint32_t)( Isobus_certification[5U] << 8U ) + (uint32_t)( Isobus_certification[6U] << 16U ) + (uint32_t)( Isobus_certification[7U] << 24U );
    MemData_SetRAMItem( ADDR_ISOBUS_CERT_4_7, aux );    

    aux = (uint32_t)( CfgAuxBool[0U] ) + (uint32_t)( CfgAuxBool[1U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_0_1, aux );
    aux = (uint32_t)( CfgAuxBool[2U] ) + (uint32_t)( CfgAuxBool[3U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_2_3, aux );
    aux = (uint32_t)( CfgAuxBool[4U] ) + (uint32_t)( CfgAuxBool[5U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_4_5, aux );
    aux = (uint32_t)( CfgAuxBool[6U] ) + (uint32_t)( CfgAuxBool[7U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_6_7, aux );
    aux = (uint32_t)( CfgAuxBool[8U] ) + (uint32_t)( CfgAuxBool[9U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_8_9, aux );
    aux = (uint32_t)( CfgAuxBool[10U] ) + (uint32_t)( CfgAuxBool[11U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_10_11, aux );
    aux = (uint32_t)( CfgAuxBool[12U] ) + (uint32_t)( CfgAuxBool[13U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_12_13, aux );
    aux = (uint32_t)( CfgAuxBool[14U] ) + (uint32_t)( CfgAuxBool[15U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_14_15, aux );
    aux = (uint32_t)( CfgAuxBool[16U] ) + (uint32_t)( CfgAuxBool[17U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_16_17, aux );
    aux = (uint32_t)( CfgAuxBool[18U] ) + (uint32_t)( CfgAuxBool[19U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_18_19, aux );
    aux = (uint32_t)( CfgAuxBool[20U] ) + (uint32_t)( CfgAuxBool[21U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_20_21, aux );
    aux = (uint32_t)( CfgAuxBool[22U] );
    MemData_SetRAMItem( ADDR_CFG_AUX_BOOL_22_23, aux );    
    
    aux = (uint32_t)( CfgAuxAn[0U] ) + (uint32_t)( CfgAuxAn[1U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_AN_0_1, aux );
    aux = (uint32_t)( CfgAuxAn[2U] ) + (uint32_t)( CfgAuxAn[3U] << 16U );
    MemData_SetRAMItem( ADDR_CFG_AUX_AN_2_3, aux );
    aux = (uint32_t)( CfgAuxAn[4U] );
    MemData_SetRAMItem( ADDR_CFG_AUX_AN_4_5, aux );    

    aux = (uint32_t)( CfgkeyIndex[0U] ) + (uint32_t)( CfgkeyIndex[1U] << 8U ) + (uint32_t)( CfgkeyIndex[2U] << 16U ) + (uint32_t)( CfgkeyIndex[3U] << 24U );
    MemData_SetRAMItem( ADDR_CFG_KEY_INDEX_0_3, aux );
    aux = (uint32_t)( CfgkeyIndex[4U] ) + (uint32_t)( CfgkeyIndex[5U] << 8U ) + (uint32_t)( CfgkeyIndex[6U] << 16U ) + (uint32_t)( CfgkeyIndex[7U] << 24U );
    MemData_SetRAMItem( ADDR_CFG_KEY_INDEX_4_7, aux );    
    aux = (uint32_t)( CfgkeyIndex[8U] ) + (uint32_t)( CfgkeyIndex[9U] << 8U ) + (uint32_t)( CfgkeyIndex[10U] << 16U ) + (uint32_t)( CfgkeyIndex[11U] << 24U );
    MemData_SetRAMItem( ADDR_CFG_KEY_INDEX_8_11, aux );    
    aux = (uint32_t)( CfgkeyIndex[12U] ) + (uint32_t)( CfgkeyIndex[13U] << 8U );
    MemData_SetRAMItem( ADDR_CFG_KEY_INDEX_12_15, aux );    
    
    aux = (uint32_t)( Isobus_keyModeIndex ) + (uint32_t)( Isobus_keyDeadManIndex << 8U );
    MemData_SetRAMItem( ADDR_ISOBUS_KEY_CFG, aux );

#endif
#endif    
    return;
}

//------------------------------------------------------------------------------
static void Isobus_paramFromRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED
#ifdef ISOBUS_DJW
  
    uint8_t i;
    uint32_t datoRead;

    if ( 0U == MemData_GetRAMItem( ADDR_ISOBUS_CERT_0_3, &datoRead ) )
    {
        Isobus_certification[0U] = (uint8_t)( datoRead );
        Isobus_certification[1U] = (uint8_t)( datoRead >> 8U );
        Isobus_certification[2U] = (uint8_t)( datoRead >> 16U );
        Isobus_certification[3U] = (uint8_t)( datoRead >> 24U );
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_ISOBUS_CERT_4_7, &datoRead ) )
    {
        Isobus_certification[4U] = (uint8_t)( datoRead );
        Isobus_certification[5U] = (uint8_t)( datoRead >> 8U );
        Isobus_certification[6U] = (uint8_t)( datoRead >> 16U );
        Isobus_certification[7U] = (uint8_t)( datoRead >> 24U );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_0_1, &datoRead ) )
    {
        CfgAuxBool[0U] = (uint16_t)( datoRead );
        CfgAuxBool[1U] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_2_3, &datoRead ) )
    {
        CfgAuxBool[2U] = (uint16_t)( datoRead );
        CfgAuxBool[3U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_4_5, &datoRead ) )
    {
        CfgAuxBool[4U] = (uint16_t)( datoRead );
        CfgAuxBool[5U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_6_7, &datoRead ) )
    {
        CfgAuxBool[6U] = (uint16_t)( datoRead );
        CfgAuxBool[7U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_8_9, &datoRead ) )
    {
        CfgAuxBool[8U] = (uint16_t)( datoRead );
        CfgAuxBool[9U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_10_11, &datoRead ) )
    {
        CfgAuxBool[10U] = (uint16_t)( datoRead );
        CfgAuxBool[11U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_12_13, &datoRead ) )
    {
        CfgAuxBool[12U] = (uint16_t)( datoRead );
        CfgAuxBool[13U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_14_15, &datoRead ) )
    {
        CfgAuxBool[14U] = (uint16_t)( datoRead );
        CfgAuxBool[15U] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_16_17, &datoRead ) )
    {
        CfgAuxBool[16U] = (uint16_t)( datoRead );
        CfgAuxBool[17U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_18_19, &datoRead ) )
    {
        CfgAuxBool[18U] = (uint16_t)( datoRead );
        CfgAuxBool[19U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_20_21, &datoRead ) )
    {
        CfgAuxBool[20U] = (uint16_t)( datoRead );
        CfgAuxBool[21U] = (uint16_t)( datoRead >> 16U );
    }   
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_BOOL_22_23, &datoRead ) )
    {
        CfgAuxBool[22U] = (uint16_t)( datoRead );
    }    

    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_AN_0_1, &datoRead ) )
    {
        CfgAuxAn[0U] = (uint16_t)( datoRead );
        CfgAuxAn[1U] = (uint16_t)( datoRead >> 16U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_AN_2_3, &datoRead ) )
    {
        CfgAuxAn[2U] = (uint16_t)( datoRead );
        CfgAuxAn[3U] = (uint16_t)( datoRead >> 16U );
    }    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_AUX_AN_4_5, &datoRead ) )
    {
        CfgAuxAn[4U] = (uint16_t)( datoRead );
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_KEY_INDEX_0_3, &datoRead ) )
    {
        CfgkeyIndex[0U] = (uint8_t)( datoRead );
        CfgkeyIndex[1U] = (uint8_t)( datoRead >> 8U );
        CfgkeyIndex[2U] = (uint8_t)( datoRead >> 16U );
        CfgkeyIndex[3U] = (uint8_t)( datoRead >> 24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_KEY_INDEX_4_7, &datoRead ) )
    {
        CfgkeyIndex[4U] = (uint8_t)( datoRead );
        CfgkeyIndex[5U] = (uint8_t)( datoRead >> 8U );
        CfgkeyIndex[6U] = (uint8_t)( datoRead >> 16U );
        CfgkeyIndex[7U] = (uint8_t)( datoRead >> 24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_KEY_INDEX_8_11, &datoRead ) )
    {
        CfgkeyIndex[8U] = (uint8_t)( datoRead );
        CfgkeyIndex[9U] = (uint8_t)( datoRead >> 8U );
        CfgkeyIndex[10U] = (uint8_t)( datoRead >> 16U );
        CfgkeyIndex[11U] = (uint8_t)( datoRead >> 24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_CFG_KEY_INDEX_12_15, &datoRead ) )
    {
        CfgkeyIndex[12U] = (uint8_t)( datoRead );
        CfgkeyIndex[13U] = (uint8_t)( datoRead >> 8U );
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_ISOBUS_KEY_CFG, &datoRead ) )
    {
        Isobus_keyModeIndex = (uint8_t)( datoRead );
        Isobus_keyDeadManIndex = (uint8_t)( datoRead >> 8U );
    }
    
    Isobus_cfgAxes = (uint16_t)( Saej_CfgAxesGet() & 0x0000FFFFUL );
    Isobus_cfgPushButton = Saej_CfgPushButtonGet();
//        Isobus_cfgDetent = Saej_CfgDetentGet();
    for ( i=0U; i<ISOBUS_PHYSICAL_BUTTON_NUM; i++ )
    {
        Isobus_pushButtonIndex[i] = Saej_PushButtonIndexGet( i );
    }

#endif
#endif
    return;
}    

//------------------------------------------------------------------------------
uint16_t Isobus_FunctionalitiesGet( void )
{
    return Isobus_functionalities;
}

//------------------------------------------------------------------------------
void Isobus_UpdateMsgSourceAddress( uint8_t sourceAddr )
{
    uint8_t i;
    //metto qua tutti i messaggi che devo aggiornare quando cambia il SA del joystick
    CAN_MsgUpdateID( &CANMsgTx_Isobus_workingSetInit, ( ISOBUS_WORKING_SET_MSG_ID + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_workingSetMaintenance, ( ( CANMsgTx_Isobus_workingSetMaintenance.ID & 0xFFFFFF00UL ) + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_EcuToVt, ( ( CANMsgTx_Isobus_EcuToVt.ID & 0xFFFFFF00UL ) + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInMaintenance, ( ( CANMsgTx_Isobus_auxInMaintenance.ID & 0xFFFFFF00UL ) + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInStatusMsg, ( ( CANMsgTx_Isobus_auxInStatusMsg.ID & 0xFFFFFF00UL ) + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInStatusEnMsg, ( ( CANMsgTx_Isobus_auxInStatusEnMsg.ID & 0xFFFFFF00UL ) + (uint32_t)( sourceAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CAN_StoredMsgUpdate( CAN_MSG_VT_TO_ECU, i, ( ( CAN_StoredMsgGetID( CAN_MSG_VT_TO_ECU, i ) & 0xFFFF00FFUL ) + (uint32_t)( sourceAddr * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_YES );
    } 
  
    return;
}

//------------------------------------------------------------------------------
void Isobus_UpdateMsgVtAddress( void )
{
    //metto qua tutti i messaggi che devo aggiornare quando cambia il SA del VT
    uint8_t i;
    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CAN_StoredMsgUpdate( CAN_MSG_VT_TO_ECU, i, ( ( CAN_StoredMsgGetID( CAN_MSG_VT_TO_ECU, i ) & 0xFFFFFF00UL ) + (uint32_t)( isobus_Vt.sourceAddr ) ), CAN_SOURCE_ADDRESS_CHECK_YES );
    }    
    CAN_MsgUpdateID( &CANMsgTx_Isobus_EcuToVt, ( ( CANMsgTx_Isobus_EcuToVt.ID & 0xFFFF00FFUL ) + (uint32_t)( isobus_Vt.sourceAddr * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_workingSetMaintenance, ( ( CANMsgTx_Isobus_workingSetMaintenance.ID & 0xFFFF00FFUL ) + (uint32_t)( isobus_Vt.sourceAddr * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    CAN_MsgUpdateID( &CANMsgTx_Isobus_auxInStatusEnMsg, ( ( CANMsgTx_Isobus_auxInStatusEnMsg.ID & 0xFFFF00FFUL ) + (uint32_t)( isobus_Vt.sourceAddr * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
  
    return;
}

//------------------------------------------------------------------------------
void Isobus_MsgRxCounterUpdate( void )
{
    if ( ++isobus_MsgRxCounter >= ISOBUS_MSG_RX_COUNTER_TH )
    {
        isobus_LedMsgDrive = true;
        isobus_MsgRxCounter = 0U;
    }
    else
    {
        isobus_LedMsgDrive = false;
    }
    return;
}

//------------------------------------------------------------------------------
void Isobus_Reset( void )
{
    uint8_t i;
    
    isobus_Net.status.value = ISOBUS_NET_OFF_STATUS;
    isobus_Net.status.previous = ISOBUS_NET_OFF_STATUS;
    isobus_Net.status.cnt = 0U;
    isobus_Net.status.cntTh = 0U;
    
    isobus_Vt.virtualKeysNum = 0U;
    isobus_Vt.physicalKeysNum = 0U;
    isobus_Vt.keysXPixel = 0U;
    isobus_Vt.keysYPixel = 0U;
    isobus_Vt.bootTime = 0U;
    isobus_Vt.graphic = 0U;
    isobus_Vt.hardware = 0U;
    isobus_Vt.xPixel = 0U;
    isobus_Vt.yPixel = 0U;
    isobus_Vt.isoVersion = 0U;
    isobus_Vt.memStatus = 0U;
    isobus_Vt.ownerSA = 0xFFU;
    isobus_Vt.ownerID = 0xFFFFU;
    isobus_Vt.function = 0U;
    isobus_Vt.fontSmall = 0U;
    isobus_Vt.fontLarge = 0U;
    isobus_Vt.typeAttribute = 0xFFU;
    isobus_Vt.objectPoolReady = 0U;
    isobus_Vt.sourceAddr = 0xFFU;
    isobus_Vt.preferredAddr = 0xFFU;
    isobus_Vt.status.cnt = 0U;
    isobus_Vt.status.cntTh = 0U;
    isobus_Vt.busy = 0U;
    isobus_Vt.learnModeStatus = 0U;    
    isobus_Vt.startupRetryCnt = 0U;
    isobus_Vt.msgRetryCnt = 0U;    
    isobus_Vt.startupRetryCntTh = ISOBUS_VT_STARTUP_RETRY_CNT_TH;
    isobus_Vt.msgRetryCntTh = ISOBUS_VT_MSG_TX_RETRY_CNT_TH;
    isobus_Vt.status.value = ISOBUS_VT_STATUS_OFF;
    isobus_Vt.status.previous = ISOBUS_VT_STATUS_OFF;    
    Isobus_UpdateMsgVtAddress();
    
    isobus_LedOut[ISOBUS_LED_OP] = ISOBUS_LED_OFF;
    isobus_LedOut[ISOBUS_LED_KEY_MODE] = ISOBUS_LED_OFF;
    
    isobus_MsgRxCounter = 0U;
    isobus_LedMsgDrive = false;
    isobus_KeyModeFirstEntry = true;
    Isobus_keyModeStatus = ISOBUS_KEY_MODE_STATUS_01;
    Isobus_keyModeStatusPre = ISOBUS_KEY_MODE_STATUS_01;   


    for ( i=0U; i<ISOBUS_AUX_BUTTON_NUM_MAX; i++ )
    {
        Isobus_keyAux[i].output = ISOBUS_KEY_STATUS_WAIT;
        Isobus_keyAux[i].pressed = BOOL_INPUT_UNUSED;
        Isobus_keyAux[i].pressedPrevious = BOOL_INPUT_UNUSED;
        Isobus_keyAux[i].pressChangeValue = BOOL_INPUT_UNUSED;        
    }    
   
    return;
}

//------------------------------------------------------------------------------
void Isobus_POR( void )
{
    uint32_t Cnt=0u;

    S32K_SysPin_SetOutputPins(GPIO_OUT_CAN_STB);  // CanTranceiver Off

    while(true)
    {
        // Do nothing
        PINS_DRV_ClearPins((GPIO_Type *)PTB,(uint32_t)1u<<17); //GPIO_OUT_TLF_CS
        while(true)
        {
        }
    }
}  

//------------------------------------------------------------------------------
uint8_t Isobus_CertificationGet( uint8_t byteNum )
{
    return Isobus_certification[byteNum];
}

//------------------------------------------------------------------------------
uint8_t IsobusTestEnableSet( uint32_t value )
{
#define ISOBUS_TEST_ENABLE_ON 0xA55A55AAUL
    uint8_t res = 3U;
  
    if ( ISOBUS_VT_STATUS_ON == isobus_Vt.status.value )
    {
        IsobusTestEnable = false;
    }
    else
    {
        if ( ISOBUS_TEST_ENABLE_ON == value )
        {
            IsobusTestEnable = true;
        }
        else
        {
            IsobusTestEnable = false;
        }
        res = 0U;
    }
    
    return ( res );
}

//------------------------------------------------------------------------------
bool IsobusTestEnableGet( void )
{
    return IsobusTestEnable;
}

//------------------------------------------------------------------------------
uint8_t IsobusVTSymEnableSet( uint32_t value )
{
#define ISOBUS_VT_SYM_ENABLE_ON 0xA55A5500UL
    uint8_t res = 3U;
  
    if ( ISOBUS_VT_SYM_ENABLE_ON == ( value & 0xFFFFFF00u ) )
    {
        IsobusVTSymEnable = true;
        isobus_Vt.sourceAddr = (uint8_t)( value & 0x000000FFu );
        isobus_Vt.learnModeStatus = 0x01U;
    }
    else
    {
        IsobusVTSymEnable = false;
        isobus_Vt.learnModeStatus = 0x00U;
    }
    res = 0U;
    
    return ( res );
}

//------------------------------------------------------------------------------
uint16_t IsobusVTSymEnableGet( void )
{
    uint16_t aux;
    
    aux = 0u;
    if ( true == IsobusVTSymEnable )
    {
        aux = 1u;
    }
    aux = aux + (uint16_t)( (uint16_t)( isobus_Vt.sourceAddr ) << 8u );
    
    return aux;
}

//------------------------------------------------------------------------------
uint8_t Isobus_LabelInit( uint8_t inputChar )
{
    uint8_t res;
    uint8_t auxChar;

    auxChar = inputChar + 0x30u;
    if ( 0x3A == auxChar )
    {
        res = 0x2B;
    }
    else if ( 0x3C == auxChar )
    {
        res = 0x2C;
    }
    else if ( 0x3E == auxChar )
    {
        res = 0x2E;
    }
    else if ( 0x3F == auxChar )
    {
        res = 0x4F;
    }
    else
    {
        res = auxChar;
    }
    
    return res;
}

//------------------------------------------------------------------------------
void Isobus_Init( void )
{
    uint8_t i;
    /* per test
    CAN_MsgEnable( &CANMsgTx_Isobus_Test, CAN_MODULE_0, 37, CAN_MSG_TX, CAN_ID_29BIT, 0x18CCBBAA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgRx_Isobus_Test, CAN_MODULE_0, 38, CAN_MSG_RX, CAN_ID_29BIT, 0x18BBCCDD, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnableEvent( &CANMsgRx_Isobus_Test,CAN_EVENT_RX, 7U );
    CAN_MsgEnableEvent( &CANMsgTx_Isobus_Test,CAN_EVENT_TX, 6U );
    */
    CAN_MsgEnable( &CANMsgTx_Isobus_workingSetInit, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_WORKING_SET_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Isobus_workingSetMaintenance, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_ECU_TO_VT_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Isobus_EcuToVt, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_ECU_TO_VT_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Isobus_auxInMaintenance, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_ECU_TO_VT_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Isobus_auxInStatusMsg, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_VT_TO_ECU_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Isobus_auxInStatusEnMsg, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( ISOBUS_ECU_TO_VT_MSG_ID ) + (uint32_t)( SAEJ_DEFAULT_SA ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    
    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_ADDRESS_CLAIM, i, SAEJ_CAN_MSG_ID_ADDRESS_CLAIM, CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_VT_TO_ECU, i, ( ( ISOBUS_VT_TO_ECU_MSG_ID & 0xFFFF00FFU ) + (uint32_t)( SAEJ_DEFAULT_SA * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );
    }
  
    memset( CANMsgTx_Isobus_workingSetInit.Data, 0xFF, 8U );
    memset( CANMsgTx_Isobus_workingSetMaintenance.Data, 0xFF, 8U );
    memset( CANMsgTx_Isobus_EcuToVt.Data, 0xFF, 8U );
    memset( CANMsgTx_Isobus_auxInMaintenance.Data, 0xFF, 8U );
    memset( CANMsgTx_Isobus_auxInStatusMsg.Data, 0xFF, 8U );
    memset( CANMsgTx_Isobus_auxInStatusEnMsg.Data, 0xFF, 8U );
    
    isobus_Net.status.value = ISOBUS_NET_OFF_STATUS;
    isobus_Net.status.previous = ISOBUS_NET_OFF_STATUS;
    isobus_Net.status.cnt = 0U;
    isobus_Net.status.cntTh = 0U;
    isobus_Net.startupCnt = 0U;
 
    isobus_Vt.virtualKeysNum = 0U;
    isobus_Vt.physicalKeysNum = 0U;
    isobus_Vt.keysXPixel = 0U;
    isobus_Vt.keysYPixel = 0U;
    isobus_Vt.bootTime = 0U;
    isobus_Vt.graphic = 0U;
    isobus_Vt.hardware = 0U;
    isobus_Vt.xPixel = 0U;
    isobus_Vt.yPixel = 0U;
    isobus_Vt.isoVersion = 0U;
    isobus_Vt.memStatus = 0U;
    isobus_Vt.ownerSA = 0xFFU;
    isobus_Vt.ownerID = 0xFFFFU;
    isobus_Vt.function = 0U;
    isobus_Vt.fontSmall = 0U;
    isobus_Vt.fontLarge = 0U;
    isobus_Vt.typeAttribute = 0xFFU;
    isobus_Vt.objectPoolReady = 0U;
    isobus_Vt.sourceAddr = 0xFFU;
    isobus_Vt.preferredAddr = 0xFFU;
    isobus_Vt.status.cnt = 0U;
    isobus_Vt.status.cntTh = 0U;
    isobus_Vt.startupRetryCnt = 0U;
    isobus_Vt.msgRetryCnt = 0U;   
    isobus_Vt.busy = 0U;
    isobus_Vt.learnModeStatus = 0U;
    isobus_Vt.startupRetryCntTh = ISOBUS_VT_STARTUP_RETRY_CNT_TH;
    isobus_Vt.msgRetryCntTh = ISOBUS_VT_MSG_TX_RETRY_CNT_TH;  
    isobus_Vt.status.value = ISOBUS_VT_STATUS_OFF;
    isobus_Vt.status.previous = ISOBUS_VT_STATUS_OFF;
    
    ContainerShow = 0UL;
    ContainerP0Show = 0x01U;    
    ContainerAnShow = 0x01U;
    //ContainerAnShow = 0x00U; //per test posizionamenti
    
    NumPictureObject = NUM_PICTURE_OBJ;
    AuxInNum = 0U;
    AuxInAnNum = 0U;
    AuxInBoolNum = 0U;
    Isobus_functionalities = 0U;

    ChangeAttribOk = 0U;
    ChangeAttribRequest = 0U;
    ChangeAttribNumTx = 0U; 
    HideShowRequest = 0U;
    ChangeNumValueRequest = 0U;

    ObjectID = 0xFFFFU;
    Isobus_AttributeId = 0xFFU;
    AIDValue = 0xFFFFU;    
    
    stringWidthScaled = STRING_WIDTH;
    
    for ( i=0U; i<ISOBUS_PUSH_BUTTON_NUM; i++ )
    {
      Isobus_pushButtonEn[i] = ISOBUS_P_DISABLE;
    }
    
    for ( i=0U; i<AUX_INPUT_NUM_MAX ; i++ )
    {
	AuxInput[i].ID = 0x0000U;
        AuxInput[i].type = 0x00U;
        AuxInput[i].value1 = 0x00U;
        AuxInput[i].value2 = 0x00U;
        AuxInput[i].value1Pre = 0x00U;
        AuxInput[i].value2Pre = 0x00U;
        AuxInput[i].status = 0x00U;
        AuxInput[i].enabled = 0x00U;
        AuxInput[i].learnMode = 0x00U;
        AuxInput[i].indexInput = 0x00U;
        AuxInput[i].indexInput2 = 0x00U;
        AuxInput[i].objPointer = 0xFFFFU;  
        AuxInput[i].cntTh =  ( ISOBUS_AUX_IN_STATUS_MSG_TX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
        AuxInput[i].cnt = 0U;    
    }
    
    /*
        CfgAuxAn[i]:
        bit 0-3: 0x0 -> aux input non abilitato
                 0x2 -> aux input return 50%
                 0xA -> aux input return 50% con latch
        bit 4-7: indice hardware associato all'aux input: 0x1 -> asse X, 0x2 -> asse Y, 0x3-> asse W, 0x4-> asse Z, 0x5-> asse J
        bit 8-11: indice secondario, usato per definire un ingresso in quadratura 50% (se indice è diverso da 0, l'ingresso viene considerato in quadratura e come ingressi vengono utilizzati gli indici 1 e 2
        bit 12-15: non usati
        
        AXIS_X 1
        AXIS_Y 2
        AXIS_W 3
        AXIS_Z 4
        AXIS_J 5 
    */    
    
    CfgAuxAn[SAEJ_AXIS_X] = 0x0012U; 
    CfgAuxAn[SAEJ_AXIS_Y] = 0x0022U;
    CfgAuxAn[SAEJ_AXIS_W] = 0x0000U;
    CfgAuxAn[SAEJ_AXIS_Z] = 0x0000U;
    CfgAuxAn[SAEJ_AXIS_J] = 0x0000U;
    
    /*
        CfgAuxBool[i]:
        tiene conto di tutti i pulsanti logici (quindi 23: (9 impugnatura * 2) + pulsante on/off (eventualmente utilizzabile non per lo shift) + 4 digitali di direzione
        bit 0-1: 0x0 -> il pulsante è usato in mode 1
                 0x1 -> il pulsante è usato in mode 2 (devo quindi metterlo a 1 per i pulsanti shiftati
        bit 2-3: 0x1 -> pulsante latch
                 0x2 -> pulsante not latch
        bit 4-5: non usati
        bit 6-7: sono definiti se devo fare un ingresso dual boolean.
                 0x0 -> il pulsante non è dual boolean
                 0x1 -> il pulante è dual boolean. Se bit 2-3=0x1 -> dual boolean both latch up and down
                                                   Se bit 2-3=0x2 -> dual boolean latch down - not latch up
                 0x2 -> il pulsante è dual boolean. Se bit 2-3=0x1 -> dual boolean latch up - not latch down
                                                    Se bit 2-3=0x2 -> dual boolean latch both not latch
        bit 8-11: indice logico dell'ingresso pulsante (da 1 a 14)
        bit 12-15: se pulsante dual boolean, indica l'ingresso secondario boolean down (da 1 a 14)

        KEY_1          ( 0U )
        KEY_2          ( 1U )
        KEY_3          ( 2U )
        KEY_4          ( 3U )
        KEY_5          ( 4U )
        KEY_6          ( 5U )
        KEY_7          ( 6U )
        KEY_8          ( 7U )
        KEY_9          ( 8U )
        KEY_1_M2       ( 9U )
        KEY_2_M2       ( 10U )
        KEY_3_M2       ( 11U )
        KEY_4_M2       ( 12U )
        KEY_5_M2       ( 13U )
        KEY_6_M2       ( 14U )
        KEY_7_M2       ( 15U )
        KEY_8_M2       ( 16U )
        KEY_9_M2       ( 17U )
        KEY_MODE       ( 18U )
        KEY_DIR_X_SX   ( 19U )
        KEY_DIR_X_DX   ( 20U )
        KEY_DIR_Y_BW   ( 21U )
        KEY_DIR_Y_FW   ( 22U )

        PHYS_KEY_00       ( 0U )
        PHYS_KEY_01       ( 1U )
        PHYS_KEY_02       ( 2U )
        PHYS_KEY_03       ( 3U )
        PHYS_KEY_04       ( 4U )
        PHYS_KEY_05       ( 5U )
        PHYS_KEY_06       ( 6U )
        PHYS_KEY_07       ( 7U )
        PHYS_KEY_08       ( 8U )
        PHYS_KEY_09       ( 9U )   //pulsante on/off
        PHYS_KEY_10       ( 10U )
        PHYS_KEY_11       ( 11U )
        PHYS_KEY_12       ( 12U )
        PHYS_KEY_13       ( 13U )
    */
    
    CfgAuxBool[KEY_1]  = 0x0108U;
    CfgAuxBool[KEY_2]  = 0x0208U;
    CfgAuxBool[KEY_3]  = 0x0308U;
    CfgAuxBool[KEY_4]  = 0x0408U;
    CfgAuxBool[KEY_5]  = 0x0508U;
    CfgAuxBool[KEY_6]  = 0x0608U;
    CfgAuxBool[KEY_7]  = 0x0708U;
    CfgAuxBool[KEY_8]  = 0x0808U;
    CfgAuxBool[KEY_9]  = 0x0908U;
    CfgAuxBool[KEY_1_M2]  = 0x0109U;
    CfgAuxBool[KEY_2_M2]  = 0x0209U;
    CfgAuxBool[KEY_3_M2]  = 0x0309U;
    CfgAuxBool[KEY_4_M2]  = 0x0409U;
    CfgAuxBool[KEY_5_M2]  = 0x0509U;
    CfgAuxBool[KEY_6_M2]  = 0x0609U;
    CfgAuxBool[KEY_7_M2]  = 0x0709U;
    CfgAuxBool[KEY_8_M2]  = 0x0809U;
    CfgAuxBool[KEY_9_M2]  = 0x0909U;
    CfgAuxBool[KEY_MODE]  = 0x0000U;
    CfgAuxBool[KEY_DIR_X_SX]  = 0x0B08U; //0x0000U; 
    CfgAuxBool[KEY_DIR_X_DX]  = 0x0C08U; //0x0000U; 
    CfgAuxBool[KEY_DIR_Y_BW]  = 0x0D08U; //0x0000U; 
    CfgAuxBool[KEY_DIR_Y_FW]  = 0x0E08U; //0x0000U; 

    CfgkeyIndex[PHYS_KEY_00] = PHYS_KEY_00;
    CfgkeyIndex[PHYS_KEY_01] = PHYS_KEY_01;
    CfgkeyIndex[PHYS_KEY_02] = PHYS_KEY_02;
    CfgkeyIndex[PHYS_KEY_03] = PHYS_KEY_03;
    CfgkeyIndex[PHYS_KEY_04] = PHYS_KEY_04;
    CfgkeyIndex[PHYS_KEY_05] = PHYS_KEY_05;
    CfgkeyIndex[PHYS_KEY_06] = PHYS_KEY_06;
    CfgkeyIndex[PHYS_KEY_07] = PHYS_KEY_07;
    CfgkeyIndex[PHYS_KEY_08] = PHYS_KEY_08;
    CfgkeyIndex[PHYS_KEY_09] = PHYS_KEY_09;
    CfgkeyIndex[PHYS_KEY_10] = PHYS_KEY_10;
    CfgkeyIndex[PHYS_KEY_11] = PHYS_KEY_11;
    CfgkeyIndex[PHYS_KEY_12] = PHYS_KEY_12;
    CfgkeyIndex[PHYS_KEY_13] = PHYS_KEY_13;
    
    Isobus_keyModeIndex = 10U;    
    Isobus_keyDeadManIndex = 0U;
    Isobus_deadManEn = false;
    Isobus_deadManShow = 0U;
    Isobus_deadManShowPrevious = 0U;
    Isobus_keyModeStatus = ISOBUS_KEY_MODE_STATUS_01;
    Isobus_keyModeStatusPre = ISOBUS_KEY_MODE_STATUS_01;
    Isobus_keyModeStatusChange = ISOBUS_STATUS_KEY_MODE_OFF;
    KeyChangeValue = ISOBUS_KEY_MODE_STATUS_01;
    KeyChangeCounter = 0U;
    
    for ( i=0U; i<ISOBUS_PUSH_BUTTON_NUM; i++ )
    {
        Isobus_key[i].output = ISOBUS_KEY_STATUS_WAIT;
        Isobus_key[i].input = DIGITAL_IN_OFF;
    }
    
    for ( i=0U; i<ISOBUS_AUX_BUTTON_NUM_MAX; i++ )
    {
        Isobus_keyAux[i].output = ISOBUS_KEY_STATUS_WAIT;
        Isobus_keyAux[i].cntTh = ( 100000UL / SCHEDULE_COUNTER_TH_SAEJ );
        Isobus_keyAux[i].pressed = BOOL_INPUT_UNUSED;
        Isobus_keyAux[i].pressedPrevious = BOOL_INPUT_UNUSED;
        Isobus_keyAux[i].pressChangeValue = BOOL_INPUT_UNUSED;        
    }
      
    for ( i=0U; i<ISOBUS_AXIS_NUM; i++ )
    {
        Isobus_analogAux[i].value = ISOBUS_NEUTRAL;
        Isobus_analogAux[i].latchEn = 0U;
        Isobus_analogAux[i].latchOtherDir = 0U;
        Isobus_analogAux[i].latchShow = 0U;
        Isobus_analogAux[i].latchShowPrevious = 0U;
    }
    
    memset( Isobus_certification, 0x00U, 8U );
    
    Isobus_manufacturerIdCode = MANUFACTURER_CODE;
    
    LineBarSetPoint[0U] = 0U;
    LineBarSetPoint[1U] = 0U;
    LineBarSetPoint[2U] = 0U;
    LineBarSetPoint[3U] = 0U;
    
    ContainerAxisShow[0U] = 1U;
    ContainerAxisShow[1U] = 1U;
    ContainerAxisShow[2U] = 1U;
    ContainerAxisShow[3U] = 1U;
    
    for ( i=0U; i<LED_OUT_NUM; i++ )
    {
        isobus_LedOut[i] = ISOBUS_LED_OFF;
    }
    
    isobus_MsgRxCounter = 0U;
    isobus_LedMsgDrive = false;
    isobus_KeyModeFirstEntry = true;
    
    IsobusTestEnable = false;
    IsobusVTSymEnable = false;
    
    Isobus_paramToRAMUpdate();
    
    return;
}

//------------------------------------------------------------------------------
void Isobus_Main( void )
{
    uint8_t i;
    uint8_t rxMsg;
    uint8_t resVtRxMsg;
    wres_t retVtRxMsg;
    static uint8_t tpIndex = 0xFFU;  
    static uint8_t opUploadStep = ISOBUS_OP_STEP_FIRST;
    static uint8_t opLoadTry = 0U;
    static bool first_entry_in_main = true;
    static uint8_t Isobus_LedMsgOnCounter = 0U;
//    static uint8_t auxEnCounter = 0U;
    
    if ( true == first_entry_in_main )
    {
        Isobus_paramFromRAMUpdate();
//        for ( i=0U; i<ISOBUS_ROLLER_NUM; i++ )
//        {        
//            Isobus_rollerIndex[i] = Saej_RollerIndexGet( i );      
//        }
        
        if ( SAEJ_BUTTON_1_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_1_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[0U] = ISOBUS_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_2_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_2_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[1U] = ISOBUS_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_3_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_3_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[2U] = ISOBUS_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_4_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_4_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[3U] = ISOBUS_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_5_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_5_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[4U] = ISOBUS_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_6_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_6_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[5U] = ISOBUS_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_7_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_7_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[6U] = ISOBUS_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_8_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_8_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[7U] = ISOBUS_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_9_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_9_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[8U] = ISOBUS_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_10_ENABLED == ( Isobus_cfgPushButton & SAEJ_BUTTON_10_ENABLED ) ) 
        { 
            Isobus_pushButtonEn[9U] = ISOBUS_P_ENABLE; 
        }     
        if ( SAEJ_AXIS_X_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_X_ENABLED ) )
        {
            Isobus_pushButtonEn[10U] = ISOBUS_P_ENABLE;
            Isobus_pushButtonEn[11U] = ISOBUS_P_ENABLE;
        }
        if ( SAEJ_AXIS_Y_ENABLED == ( Isobus_cfgAxes & SAEJ_AXIS_Y_ENABLED ) )
        {
            Isobus_pushButtonEn[12U] = ISOBUS_P_ENABLE;
            Isobus_pushButtonEn[13U] = ISOBUS_P_ENABLE;
        }        
        
        Isobus_AuxInCfgUpdate();
        
        CAN_MsgUpdateTimeOut( &CANMsgTx_Isobus_auxInMaintenance, ( ISOBUS_AUX_IN_MAINTENANCE_MSG_TX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ ) );

#ifdef ISOBUS_CAN_RX_COUNTER
        isobus_LedOut[ISOBUS_LED_SUPPLY_ON] = ISOBUS_LED_OFF;
#else        
        //isobus_LedOut[ISOBUS_LED_SUPPLY_ON] = ISOBUS_LED_ON;
        isobus_LedOut[ISOBUS_LED_SUPPLY_ON] = ISOBUS_LED_BLINK;
#endif        
        
        uint32_t aux;
        aux = Isobus_ChecksumGet();
        
        isobus_Label[0u] = ISO_VERSION_LABEL_CHAR_0;
        isobus_Label[1u] = ISO_VERSION_LABEL_CHAR_1;
        isobus_Label[2u] = ISO_VERSION_LABEL_CHAR_2;
        isobus_Label[3u] = Isobus_LabelInit( (uint8_t)( (uint8_t)( aux >>  4 ) + (uint8_t)( aux       ) ) & 0x0FU );
        isobus_Label[4u] = Isobus_LabelInit( (uint8_t)( (uint8_t)( aux >>  8 ) + (uint8_t)( aux >> 12 ) ) & 0x0FU );
        isobus_Label[5u] = Isobus_LabelInit( (uint8_t)( (uint8_t)( aux >> 16 ) + (uint8_t)( aux >> 20 ) ) & 0x0FU );
        isobus_Label[6u] = Isobus_LabelInit( (uint8_t)( (uint8_t)( aux >> 24 ) + (uint8_t)( aux >> 28 ) ) & 0x0FU );
        
        first_entry_in_main = false;
    }
    
    if ( isobus_Net.status.cntTh > 0U )
    {
        if ( ++isobus_Net.status.cnt >= isobus_Net.status.cntTh )
        {
            isobus_Net.status.previous = isobus_Net.status.value;
            isobus_Net.status.value = ISOBUS_NET_ERROR_STATUS;
        }
    }      
    
    Isobus_AuxInMaintenanceMsgTx();
    if ( WRES_ERROR == Isobus_ClaimCheckForVtFunction() )
    {
        Isobus_Reset();
    }

#ifdef ISOBUS_DEBUG    
    Isobus_AnalogMain();
    Isobus_KeyMain();
#endif
 
#ifdef ISOBUS_CAN_RX_COUNTER
    if ( true == isobus_LedMsgDrive )
    {
        Isobus_LedMsgOnCounter = 1u;
    }
    if ( Isobus_LedMsgOnCounter > 0U )
    {
        if ( ++Isobus_LedMsgOnCounter > ISOBUS_MSG_LED_ON_COUNTER_TH )
        {
            isobus_LedOut[ISOBUS_LED_SUPPLY_ON] = ISOBUS_LED_OFF;
            Isobus_LedMsgOnCounter = 0U;
        }
        else
        {
            isobus_LedOut[ISOBUS_LED_SUPPLY_ON] = ISOBUS_LED_BLINK;
        }
    }
#endif    
    
    uint8_t ledOut = 0U;
    uint8_t ledDuty = 0U;
    ledOut = isobus_LedOut[0U] & 0x03U;
    ledOut = ledOut | ( ( isobus_LedOut[1U] << 2U ) & 0x0CU );
    ledOut = ledOut | ( ( isobus_LedOut[2U] << 4U ) & 0x30U );
    ledOut = ledOut | ( ( isobus_LedOut[3U] << 6U ) & 0xC0U );
    ledDuty = ( DEFAULT_ISOBUS_LED_DUTY_SUPPLY_ON << 2U ) & 0x0CU;
    
    Saej_LedInternalValueSet( ledOut );
    Saej_LedInternalDutySet( ledDuty );

    if ( true == IsobusVTSymEnable )
    {
        //isobus_Vt.sourceAddr = 0xF9U;
        isobus_Vt.learnModeStatus = 0x01U;
        if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_VT_TO_ECU, &rxMsg ) )
        {
            resVtRxMsg = Isobus_VtToEcuMsgRxCheck( rxMsg, &retVtRxMsg );
            CAN_StoredMsgClear( CAN_MSG_VT_TO_ECU, rxMsg );
        }        
        Isobus_AnalogMain();
        Isobus_KeyMain();
        Isobus_KeyModeStatusChangeMain();
        Isobus_AuxInMain();    
        return;
    }
  
    switch ( isobus_Net.status.value )
    {
    case ISOBUS_NET_OFF_STATUS:
        if ( isobus_Net.status.previous != isobus_Net.status.value )
        {
            isobus_Net.status.previous = isobus_Net.status.value;
        }
        Saej_SendPGNRequest( 0xEE00UL, 0xFFU );  //invio il request address claim per identificare il VT
        //isobus_Net.status.value = ISOBUS_NET_STARTUP_STATUS;
        isobus_Net.status.value = ISOBUS_NET_CLAIM_ANSWER_STATUS;
        break;
    case ISOBUS_NET_CLAIM_ANSWER_STATUS:
        Isobus_TxClaim();
        isobus_Net.status.value = ISOBUS_NET_STARTUP_STATUS;
        break;
    case ISOBUS_NET_STARTUP_STATUS:
        if ( isobus_Net.status.previous != isobus_Net.status.value )
        {
            isobus_Net.status.previous = isobus_Net.status.value;
            isobus_Net.startupCnt = 0U;
        }      
        if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_ADDRESS_CLAIM, &rxMsg ) )
        {
            if ( ( 0x1DU == CAN_StoredMsgGetData( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg, 5U ) ) && ( 0x00U == ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg, 4U ) & 0xF8U ) ) )
            {
                //ho trovato il VT principale: Function = 0x1D, Function Instance = 0
                isobus_Vt.sourceAddr = CAN_StoredMsgGet( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg ).SA;
                isobus_Net.status.value = ISOBUS_NET_WS_INIT_STATUS;
                Isobus_UpdateMsgVtAddress();
            }
            CAN_StoredMsgClear( CAN_MSG_SAEJ_ADDRESS_CLAIM, rxMsg );
        }
        else
        {
            if ( ++isobus_Net.startupCnt >= ( ( 5000000U / SCHEDULE_COUNTER_TH_SAEJ ) - 1UL ) )
            {
                isobus_Net.status.value = ISOBUS_NET_OFF_STATUS;
            }
        }
        break;      
    case ISOBUS_NET_WS_INIT_STATUS:
        if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_VT_TO_ECU, &rxMsg ) )
        {
            resVtRxMsg = Isobus_VtToEcuMsgRxCheck( rxMsg, &retVtRxMsg );
            CAN_StoredMsgClear( CAN_MSG_VT_TO_ECU, rxMsg );
        }            
        if ( ISOBUS_VT_TO_ECU_MSG_RX_VT_STATUS == resVtRxMsg )
        {
            //ho ricevuto il primo VT status message: posso procedere con l'inizializzazione del Working Set
            Isobus_WorkingSetInitMsgTx();
            //abilito il timeout del WS maintenance message così inizia a trasmettere
            CAN_MsgUpdateTimeOut( &CANMsgTx_Isobus_workingSetMaintenance, ( ISOBUS_WS_MAINTENANCE_MSG_TX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ ) );
            isobus_Net.status.value = ISOBUS_NET_WS_START_STATUS;
            isobus_Net.status.cnt = 0U;
            isobus_Net.status.cntTh = ( ISOBUS_VT_STATUS_MSG_RX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
        }
        break;      
    case ISOBUS_NET_WS_START_STATUS:
        Isobus_WorkingSetMaintenanceMsgTx();
        isobus_Vt.status.value = ISOBUS_VT_STATUS_INIT;
        isobus_Vt.status.previous = ISOBUS_VT_STATUS_OFF;
        isobus_Net.status.value = ISOBUS_NET_OK_STATUS;
        break;
    case ISOBUS_NET_OK_STATUS:
        if ( 0U < isobus_Vt.status.cntTh )
        {
            if ( ++isobus_Vt.status.cnt >= isobus_Vt.status.cntTh )
            {
                if ( isobus_Vt.msgRetryCnt++ < isobus_Vt.msgRetryCntTh )
                {
                    isobus_Vt.status.previous--;
                }
                else
                {
                    isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                }
            }
        }
        else
        {
            isobus_Vt.status.cnt = 0U;
        }
      
        Isobus_WorkingSetMaintenanceMsgTx();
        
        
        if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_VT_TO_ECU, &rxMsg ) )
        {
            resVtRxMsg = Isobus_VtToEcuMsgRxCheck( rxMsg, &retVtRxMsg );
            CAN_StoredMsgClear( CAN_MSG_VT_TO_ECU, rxMsg );
        }        
        if ( ISOBUS_VT_TO_ECU_MSG_RX_VT_STATUS == resVtRxMsg )
        {
            //ho ricevuto il VT status message, azzero il counter sul timeout di ricezione del VT status
            isobus_Net.status.cnt = 0U;
        }

        switch ( isobus_Vt.status.value )
        {
        case ISOBUS_VT_STATUS_OFF:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.startupRetryCnt = 0U;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = 0U;
                isobus_Vt.status.cnt = 0U;
            }
            else
            {
            }     
            break;
        case ISOBUS_VT_STATUS_INIT:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.status.value = ISOBUS_VT_STATUS_GET_SOFTKEY_NUM;
            }
            else
            {
            }
            break;
        case ISOBUS_VT_STATUS_GET_SOFTKEY_NUM:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.status.cntTh = ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                isobus_Vt.msgRetryCnt = 0U;
                if ( Isobus_EcutToVtMsgTx( ISOBUS_GET_SOFTKEY_NUM, 0U, 0U ) == 0U )
                {
                    isobus_Vt.status.previous--;
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_GET_SOFTKEY_NUM == resVtRxMsg ) 
                {
                    if ( WRES_OK == retVtRxMsg )
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_GET_HARDWARE;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }
            }          
            break;
        case ISOBUS_VT_STATUS_GET_HARDWARE:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;  
                if ( Isobus_EcutToVtMsgTx( ISOBUS_GET_HARDWARE, 0U, 0U ) == 0U ) 
                {
                    isobus_Vt.status.previous--;
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_GET_HARDWARE == resVtRxMsg ) 
                {
                    if ( WRES_OK == retVtRxMsg )
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_GET_TEXT_FONT;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }              
            }          
            break;
        case ISOBUS_VT_STATUS_GET_TEXT_FONT:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                if ( Isobus_EcutToVtMsgTx( ISOBUS_GET_TEXT_FONT, 0U, 0U ) == 0U )
                {
                    isobus_Vt.status.previous--;
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_GET_TEXT_FONT == resVtRxMsg )
                {
                    if ( WRES_OK == retVtRxMsg )
                    {
                        opLoadTry = 0U;
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_GET_MEMORY;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }              
            }          
            break;
        case ISOBUS_VT_STATUS_GET_MEMORY:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                if ( Isobus_EcutToVtMsgTx( ISOBUS_GET_MEMORY, 0U, 0U ) == 0U ) 
                {
                    isobus_Vt.status.previous--;
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_GET_MEMORY == resVtRxMsg )
                {
                    if ( WRES_OK == retVtRxMsg )
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_LOAD_OP_VER;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }              
            }          
            break;            
        case ISOBUS_VT_STATUS_LOAD_OP_VER:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US * 3UL ) / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                if ( Isobus_EcutToVtMsgTx( ISOBUS_LOAD_OP_VER, 0U, 0U ) == 0U )
                {
                    isobus_Vt.status.previous--;                  
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_LOAD_OP_VER == resVtRxMsg ) 
                {
                    if ( WRES_OK == retVtRxMsg ) 
                    {
                        isobus_Vt.objectPoolReady = 1U;
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ON;
                    }
                    else
                    {
                        if ( ++opLoadTry > 5U )
                        {
                            opUploadStep = 0U;
                            opLoadTry = 0U;
                            isobus_Vt.status.value = ISOBUS_VT_STATUS_OP_UPLOAD;                          
                        }
                        else
                        {
                            isobus_Vt.status.previous--;
                        }
                    }
                }              
            }          
            break;            
        case ISOBUS_VT_STATUS_OP_UPLOAD:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = 0U;
                isobus_Vt.status.cnt = 0U;              
                isobus_Vt.status.previous = isobus_Vt.status.value;
                tpIndex = 0xFFU;
                if ( isobus_Vt.isoVersion < ISOBUS_VT_VERSION_3 )
                {
                    OPStepNumMax = 1U;
                    Isobus_opByteNum = SIZE_OF_OP_FAIL;
                    Isobus_opPacketNum = SIZE_OF_OP_FAIL / 7U;
                    if ( 0U < ( SIZE_OF_OP_FAIL % 7U ) )
                    {
                        Isobus_opPacketNum++;
                    }
                    for ( i=0U; i<SIZE_OF_OP_FAIL; i++ )
                    {
                        Isobus_tmpPool[i] = Isobus_poolFail[i];
                    }                
                    for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
                    {
                        if ( WRES_OK == Saej_TPTxEnable( i, ISOBUS_ECU_TO_VT_PGN, SAEJ_TP_IS_NOT_BROADCAST, isobus_Vt.sourceAddr, Isobus_opByteNum, Isobus_opPacketNum, &Isobus_tmpPool[0U], &Isobus_poolFail[0U] ) )
                        {
                            tpIndex = i;
                            break;
                        }
                    }
                }
                else
                {
                    if ( opUploadStep < OPStepNumMax )
                    {
                        Isobus_PoolBufferBuild( opUploadStep );
                        if ( 0U < Isobus_opByteNum )
                        {
                            for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
                            {
                                if ( WRES_OK == Saej_TPTxEnable( i, ISOBUS_ECU_TO_VT_PGN, SAEJ_TP_IS_NOT_BROADCAST, isobus_Vt.sourceAddr, Isobus_opByteNum, Isobus_opPacketNum, &Isobus_tmpPool[0U], poolBufferIndex ) )
                                {
                                    tpIndex = i;
                                    break;
                                }
                            }
                        }                    
                    }
                }
            }
            else
            {
                if ( 0U < Isobus_opByteNum )
                {
                    if ( tpIndex < SAEJ_TP_SLOT_NUM_MAX )
                    {
                        if ( WRES_OPERATION_COMPLETED == Saej_TPTxStatusGet( tpIndex ) )
                        {
                            opLoadTry = 0U;
                            if ( ++opUploadStep >= OPStepNumMax )
                            {
                                isobus_Vt.status.value = ISOBUS_VT_STATUS_OP_END;
                            }
                            else
                            {
                                isobus_Vt.status.previous--;   
                            }
                        }
                        else if ( WRES_ERROR == Saej_TPTxStatusGet( tpIndex ) )
                        {
                              if ( ++opLoadTry > 10U )
                              {
                                  isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                              }
                              else
                              {
                                  isobus_Vt.status.previous--;
                                  if ( ( opUploadStep > ISOBUS_OP_LAST_STEP_BEFORE_PICS ) && ( opUploadStep <= ( ISOBUS_OP_LAST_STEP_BEFORE_PICS + NumPictureObject ) ) )
                                  {
                                      opUploadStep = ISOBUS_OP_LAST_STEP_BEFORE_PICS + 1U;
                                  }
                              }
                        }
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }
                else
                {
                    if ( ++opUploadStep >= OPStepNumMax )
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_OP_END;
                    }
                    else
                    {
                        isobus_Vt.status.previous--;   
                    }                  
                }
            }          
            break;
        case ISOBUS_VT_STATUS_OP_END:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US * 3UL ) / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                if ( Isobus_EcutToVtMsgTx( ISOBUS_OP_END, 0U, 0U ) == 0U )
                {
                    isobus_Vt.status.previous--;
                }
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_OP_END == resVtRxMsg ) 
                {
                    if ( WRES_OK == retVtRxMsg ) 
                    {
                        if ( 0U == isobus_Vt.memStatus )
                        {
                            isobus_Vt.status.value = ISOBUS_VT_STATUS_STORE_OP_VER;
                        }
                        else
                        {
                            isobus_Vt.status.value = ISOBUS_VT_STATUS_ON;
                        }
                        isobus_Vt.objectPoolReady = 1U;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ERROR;
                    }
                }
            }          
            break;
        case ISOBUS_VT_STATUS_STORE_OP_VER:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = ( ( ISOBUS_VT_TO_ECU_MSG_RX_TIMEOUT_US * 3UL ) / SCHEDULE_COUNTER_TH_SAEJ );
                isobus_Vt.status.cnt = 0U;
                Isobus_EcutToVtMsgTx( ISOBUS_STORE_OP_VER, 0U, 0U ); 
            }
            else
            {
                if ( ISOBUS_VT_TO_ECU_MSG_RX_STORE_OP_VER == resVtRxMsg ) 
                {
                    if ( WRES_OK == retVtRxMsg ) 
                    {                  
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ON;
                    }
                    else
                    {
                        isobus_Vt.status.value = ISOBUS_VT_STATUS_ON;
                    }
                }
            }          
            break;
        case ISOBUS_VT_STATUS_ON:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.msgRetryCnt = 0U;
                isobus_Vt.status.cntTh = 0U;
                isobus_Vt.status.cnt = 0U;
                isobus_LedOut[ISOBUS_LED_OP] = ISOBUS_LED_ON;
            }
            else
            {
#ifndef ISOBUS_DEBUG              
                Isobus_AnalogMain();
                Isobus_KeyMain();
#endif
                Isobus_KeyModeStatusChangeMain();
                Isobus_AuxInMain();
//                if ( true == AuxInput[auxEnCounter].enMsgRx )
//                {
//                    if ( 0U != Isobus_EcutToVtMsgTx( ISOBUS_AUX_IN_2_STATUS_EN, AuxInput[auxEnCounter].ID, (uint16_t)( AuxInput[auxEnCounter].enabled & 0x00FFU ) ) )
//                    {
//                        AuxInput[auxEnCounter].enMsgRx = false;
//                        auxEnCounter++;
//                        if ( auxEnCounter >= AUX_INPUT_NUM_MAX )
//                        {
//                            auxEnCounter = 0U;
//                            
//                        }
//                    }
//                }
                if ( ISOBUS_VT_TO_ECU_MSG_RX_AUX_IN_2_STATUS_EN == resVtRxMsg )
                {
                    
                }
                if ( ( ISOBUS_VT_TO_ECU_MSG_RX_ATTRIB_CHANGE == resVtRxMsg ) || ( ISOBUS_VT_TO_ECU_MSG_RX_HIDE_SHOW_OBJECT == resVtRxMsg ) || ( ISOBUS_VT_TO_ECU_MSG_RX_CHANGE_NUM_VALUE == resVtRxMsg ) )
                {
                    if ( WRES_OK == retVtRxMsg )
                    {
                        ChangeAttribOk = 1U;
                    }
                    else
                    {
                        ChangeAttribOk = 0U;
                    }
                }        
                if ( 0U != ChangeAttribRequest ) 
                {
                    if ( Isobus_EcutToVtMsgTx( ISOBUS_ATTRIB_CHANGE, 0U, 0U ) != 0U )
                    {
                        ChangeAttribRequest = 0U;
                    }
                }
                if ( 0U != HideShowRequest ) 
                {
                    if ( Isobus_EcutToVtMsgTx( ISOBUS_HIDE_SHOW_OBJECT, 0U, 0U ) != 0U )
                    HideShowRequest = 0U;
                }            
                if ( 0U != ChangeNumValueRequest )
                {
                    if ( Isobus_EcutToVtMsgTx( ISOBUS_CHANGE_NUM_VALUE, 0U, 0U ) != 0U )
                    {
                        ChangeNumValueRequest = 0U;
                    }                  
                }
            }          
            break;
        case ISOBUS_VT_STATUS_ERROR:
            if ( isobus_Vt.status.previous != isobus_Vt.status.value )
            {
                isobus_Vt.status.previous = isobus_Vt.status.value;
                isobus_Vt.status.cntTh = 0U;
                isobus_Vt.status.cnt = 0U;
            }
            else
            {
                if ( ++isobus_Vt.startupRetryCnt < isobus_Vt.startupRetryCntTh )
                {
                    isobus_Vt.status.value = ISOBUS_VT_STATUS_INIT;
                }
                else
                {
                    //ho esaurito il numero di tentativi di comunicazione, rimango in errore e non faccio niente
                    //isobus_Vt.status.value = ISOBUS_VT_STATUS_OFF;
                  
                    //ho esaurito il numero di tentativi di comunicazione, resetto lo stato e provo a ripartire
                    Isobus_Reset();                  
                }
            }
            break;            
        default:
            break;
        }
        break;      
    case ISOBUS_NET_ERROR_STATUS:
        Isobus_POR();
        break;      
    default:
        break;
    }
    
    return;
}

#endif
