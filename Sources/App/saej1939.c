/*
 * saej1939.c
 *
 *  Created on: 20 jun 2018
 *      Author: PoliD
 */
#include "string.h"
#include "application.h"
#include "AppTranslate.h"
//#include "S32K_Error.h"
//#include "SafetySlave.h"

//------------------------------------------------------------------------------
#define CAN_MSG_SAEJ_CONTENDING_ADDRESS (  0U )
#define CAN_MSG_SAEJ_DM14               (  1U )
#define CAN_MSG_SAEJ_DM16               (  2U )
#define CAN_MSG_SAEJ_REQUEST_PGN        (  3U )
#define CAN_MSG_SAEJ_REQUEST_PGN_ALL    (  4U )
#define CAN_MSG_SAEJ_TP                 (  5U )
#define CAN_MSG_SAEJ_TP_ALL             (  6U )
#define CAN_MSG_SAEJ_TP_DT              (  7U )
#define CAN_MSG_SAEJ_TP_DT_ALL          (  8U )
#define CAN_MSG_SAEJ_LED                (  9U )
#define CAN_MSG_SAEJ_ETP                ( 10U )

//------------------------------------------------------------------------------
#define SAEJ_STATUS_INIT                   ( 0U )
#define SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM ( 1U )
#define SAEJ_STATUS_WAIT_CANNOT_CLAIM      ( 2U )
#define SAEJ_STATUS_WAIT_TO_NORMAL         ( 3U )
#define SAEJ_STATUS_NORMAL                 ( 4U )
#define SAEJ_STATUS_CANNOT_CLAIM           ( 5U )
#define SAEJ_STATUS_OFF                    ( 6U )

#define SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM_CNT_TH   ( 250000UL )
#define SAEJ_STATUS_WAIT_TO_NORMAL_CNT_TH           ( 250000UL )

//#define SAEJ_MAX_FREE_SOURCE_ADDRESS ( 247U )
#define SAEJ_MAX_FREE_SOURCE_ADDRESS ( 160U )  //considero come ultimo source address claimable 0xA0, che è tra quelli reserved ed è l'ultimo valore tra i non occupati dai default definiti per le elettroniche Walvoil
#define SAEJ_MIN_FREE_SOURCE_ADDRESS ( 128U )  //0x80, primo dei source address reserved for future assignments da AEF

#define SAEJ_ISO_NAME_SELF_CONFIGURABLE_BIT_MASK    ( 0x80000000UL )
#define SAEJ_ISO_NAME_HW_ID_MASK                    ( 0xFFFFFFFFUL ) 

//------------------------------------------------------------------------------
#define SAEJ_STATUS_TP_OFF                ( 0U )
#define SAEJ_STATUS_TP_INIT               ( 1U )
#define SAEJ_STATUS_TP_CM_RTS             ( 2U )
#define SAEJ_STATUS_TP_CM_CTS             ( 3U )
#define SAEJ_STATUS_TP_DPO                ( 4U )
#define SAEJ_STATUS_TP_DT                 ( 5U )
#define SAEJ_STATUS_TP_CM_EOM_ACK         ( 6U )
#define SAEJ_STATUS_TP_CM_ABORT           ( 7U )
#define SAEJ_STATUS_TP_EVAL               ( 8U )
#define SAEJ_STATUS_TP_COMPLETED          ( 9U )

//---
//#define SAEJ_STATUS_TP_CM_CNT_TH           ( 1250000U ) //ufficiale secondo specifica
//#define SAEJ_STATUS_TP_CM_PAUSE_ETP_CNT_TH ( 1050000U ) //ufficiale secondo specifica
//#define SAEJ_STATUS_TP_CM_PAUSE_CNT_TH     (  550000U ) //ufficiale secondo specifica
//---
#define SAEJ_STATUS_TP_CM_CNT_TH           ( 3000000U )
#define SAEJ_STATUS_TP_CM_PAUSE_ETP_CNT_TH ( 3000000U )
#define SAEJ_STATUS_TP_CM_PAUSE_CNT_TH     ( 3000000U )
//---
#define SAEJ_STATUS_TP_DT_CNT_TH           ( 200000U )
#define SAEJ_DEFAULT_TP_PACKET_NUM         ( 255U ) 
#define SAEJ_DEFAULT_TP_BYTE_NUM           ( 1785U ) //( 7U * SAEJ_DEFAULT_TP_PACKET_NUM )

#define SAEJ_TP_CM_RX_NO                   ( 0x00U )
#define SAEJ_TP_CM_RX_CTS                  ( 0x01U )
#define SAEJ_TP_CM_RX_EOMA                 ( 0x02U )
#define SAEJ_TP_CM_RX_ABORT                ( 0xFFU )

#define SAEJ_TP_ABORT_NULL                 ( 0x00U )
#define SAEJ_TP_ABORT_CM_TIMEOUT           ( 0x03U )
#define SAEJ_TP_ABORT_PAUSE_TIMEOUT        ( 0x07U )
#define SAEJ_TP_ABORT_WRONG_PACKET_NUM     ( 0x0BU )
#define SAEJ_TP_ABORT_BAD_PGN_ECTS         ( 0x0EU )
#define SAEJ_TP_ABORT_OTHER_ERROR          ( 0xFFU )

//------------------------------------------------------------------------------
#define SAEJ_TP_CTRL_BYTE_RTS             ( 0x10U )
#define SAEJ_TP_CTRL_BYTE_BAM             ( 0x20U )
#define SAEJ_TP_CTRL_BYTE_CTS             ( 0x11U )
#define SAEJ_TP_CTRL_BYTE_EOMA            ( 0x13U )
#define SAEJ_TP_CTRL_CONN_ABORT           ( 0xFFU )

#define SAEJ_ETP_CTRL_BYTE_RTS            ( 0x14U )
#define SAEJ_ETP_CTRL_BYTE_CTS            ( 0x15U )
#define SAEJ_ETP_CTRL_BYTE_DPO            ( 0x16U )
#define SAEJ_ETP_CTRL_BYTE_EOMA           ( 0x17U )

//------------------------------------------------------------------------------
#define SAEJ_STATUS_DM1_OFF     ( 0U )
#define SAEJ_STATUS_DM1_ON      ( 1U )
#define SAEJ_STATUS_DM1_EXIT    ( 2U )

//------------------------------------------------------------------------------
#define SAEJ_COMMANDED_ADDRESS_STATUS_OFF       ( 0U )
#define SAEJ_COMMANDED_ADDRESS_STATUS_ENTER     ( 1U )
#define SAEJ_COMMANDED_ADDRESS_STATUS_BUSY      ( 2U )
#define SAEJ_COMMANDED_ADDRESS_STATUS_COMPLETED ( 3U )

#define SAEJ_OPERATION_COMPLETED          ( 2U )
#define SAEJ_OPERATION_BUSY               ( 1U )
#define SAEJ_OPERATION_OFF                ( 0U )
   
//------------------------------------------------------------------------------
#define SAEJ_STATUS_NOT_AVAILABLE ( 0x03U ) 
#define SAEJ_STATUS_ERROR         ( 0x02U )

//------------------------------------------------------------------------------
#define SAEJ_DIRECTION_POS     ( 0x00U )
#define SAEJ_DIRECTION_NEG     ( 0x01U )
#define SAEJ_DIRECTION_NUM     ( 0x02U )

//------------------------------------------------------------------------------
#define SAEJ_DETENT_SAFE_BAND  ( 50U )

//------------------------------------------------------------------------------
#define SAEJ_POS1      ( 0x00U )
#define SAEJ_POS2      ( 0x02U )
#define SAEJ_POS3      ( 0x04U )
#define SAEJ_POS4      ( 0x06U )

//------------------------------------------------------------------------------
#define SAEJ_P_ENABLE  ( 0x01U )
#define SAEJ_P_DISABLE ( 0x00U )

#define SAEJ_IN_ON     ( 0x01U )
#define SAEJ_IN_OFF    ( 0x00U )

//------------------------------------------------------------------------------
#define SAEJ_ISOBUS_ENABLE_YES   ( 0xAAU )
#define SAEJ_ISOBUS_ENABLE_NO    ( 0x00U )
#define EMPTY_ARRAY {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

//------------------------------------------------------------------------------
typedef enum
{
    SAEJ_ADDRESS_TO_CLAIM     = 0U,
    SAEJ_ADDRESS_CLAIMED      = 1U,
    SAEJ_ADDRESS_NOT_CLAIMED  = 2U,
} Saej_claimResult;

//------------------------------------------------------------------------------
typedef struct Saej_spn_e
{
    Saej_lampStatus_t lampStatus;
    Saej_lampStatus_t flashLampStatus;
    uint32_t value;
    uint32_t previous;
    Saej_FMI_t FMI;
    Saej_FMI_t FMIold;
    uint8_t count;
    uint8_t FMI_count[SAEJ_FMI_NUM_MAX];
} Saej_spn_t;

//------------------------------------------------------------------------------
typedef struct Saej_tpInstance_e
{
    bool isBroadcast;
    bool isETP;
    bool isBusy;
    uint8_t cmRx;
    uint32_t byteNum;
    uint32_t packetNum;
    uint32_t PGN;
    uint8_t packetNumToSend;
    uint32_t nextPacket;
    uint32_t packetCnt;
    uint32_t byteCnt;
    uint32_t packetSent;
    uint32_t packetToSend;
    uint8_t abortValue;
    uint8_t destAddress;
    uint8_t sourceAddress;
    const uint8_t *buffSource;
    uint8_t Saej_tpTxBuff[SAEJ_TP_BYTE_NUM_MAX];
    s_TxRxCANMsg CANMsgRx_Saej_TP_CM;
    w_status_t status;
    
} Saej_tpInstance_t;

//------------------------------------------------------------------------------
typedef struct Saej_fnr_e
{
  bool enabled;
  uint8_t inputF;
  uint8_t inputR;
  uint8_t outputN;
} Saej_fnr_t;

//------------------------------------------------------------------------------
uint32_t Saej_cfgAxes;
static uint16_t Saej_cfgPushButton;
static uint16_t Saej_cfgDetent;
uint8_t Saej_pushButtonStatus[SAEJ_PUSH_BUTTON_STATUS_BYTE_NUM];
static uint8_t Saej_pushButtonIndex[SAEJ_PUSH_BUTTON_NUM];
static uint8_t Saej_pushButtonEn[SAEJ_PUSH_BUTTON_NUM];
static wres_t Saej_pushButtonGetResult[SAEJ_PUSH_BUTTON_NUM];
static uint8_t Saej_rollerIndex[SAEJ_ROLLER_NUM];

static s_TxRxCANMsg CANMsgTx_Saej_BJM;
static s_TxRxCANMsg CANMsgTx_Saej_EJM;
static s_TxRxCANMsg CANMsgTx_Saej_EJM2;
static s_TxRxCANMsg CANMsgTx_Saej_DM1;
static s_TxRxCANMsg CANMsgTx_Saej_RequestAddressClaim;
static s_TxRxCANMsg CANMsgTx_Saej_AddressClaim;
//static s_TxRxCANMsg CANMsgRx_Saej_ContendingAddress;
//static s_TxRxCANMsg CANMsgRx_Saej_DM14;
static s_TxRxCANMsg CANMsgTx_Saej_DM15;
static s_TxRxCANMsg CANMsgTx_Saej_DM16;
//static s_TxRxCANMsg CANMsgRx_Saej_DM16;
//static s_TxRxCANMsg CANMsgRx_Saej_RequestPGN;
//static s_TxRxCANMsg CANMsgRx_Saej_RequestPGNAll;
static s_TxRxCANMsg CANMsgTx_Saej_SendPGN;
//static s_TxRxCANMsg CANMsgRx_Saej_TP;
//static s_TxRxCANMsg CANMsgRx_Saej_ETP;
//static s_TxRxCANMsg CANMsgRx_Saej_TPAll;
//static s_TxRxCANMsg CANMsgRx_Saej_TP_DT;
//static s_TxRxCANMsg CANMsgRx_Saej_TPAll_DT;
static s_TxRxCANMsg CANMsgTx_Saej_TP[SAEJ_TP_SLOT_NUM_MAX];
static s_TxRxCANMsg CANMsgTx_Saej_TP_DT[SAEJ_TP_SLOT_NUM_MAX];
static s_TxRxCANMsg CANMsgTx_Saej_ETP[SAEJ_TP_SLOT_NUM_MAX];
static s_TxRxCANMsg CANMsgTx_Saej_ETP_DT[SAEJ_TP_SLOT_NUM_MAX];
//static s_TxRxCANMsg CANMsgRx_Saej_Led;
static s_TxRxCANMsg CANMsgTx_Saej_RequestPGN;
//static s_TxRxCANMsg CANMsgRx_Saej_TP_CM[SAEJ_TP_SLOT_NUM_MAX];

uint8_t Saej_bufferTxBJM[8U];
uint8_t Saej_bufferTxEJM[8U];
uint8_t Saej_bufferTxEJM2[8U];

static uint32_t Saej_isoName[2U];

Saej_axisOut_t Saej_axis[SAEJ_AXIS_NUM];

static w_status_t Saej_status;
static w_status_t Saej_statusTPRx;
static w_status_t Saej_statusDM1;
static uint8_t Saej_addressClaimedNum;
static Saej_claimResult Saej_addressClaimResult;
static uint8_t Saej_defaultAddress;
static uint8_t Saej_address;
static uint8_t Saej_isSelfConfigurableDevice;
static Saej_tpInstance_t Saej_TPRx;
static Saej_tpInstance_t Saej_TPTx[SAEJ_TP_SLOT_NUM_MAX];
//static uint8_t Saej_tmpTPBuff[48U];

//static uint8_t Saej_tpTxBuff[SAEJ_TP_SLOT_NUM_MAX][SAEJ_TP_BYTE_NUM_MAX];
static uint8_t Saej_tpRxBuff[48U];

static uint8_t Saej_commandedAddressStatus;

static Saej_spn_t Saej_SPN[SAEJ_SPN_NUM];

static w_error_entry_t Saej_error;
#ifdef DM1_ENABLED
static bool Saej_DM1reqPGN;
#endif

static w_error_entry_t Saej_ledCmd;

static uint8_t Saej_ledCmdEn;
static bool Saej_ledEn;
static bool Saej_txOnChange;
static uint32_t Saej_msgJoyTimeOut_us;
static uint32_t Saej_msgJoyOnChangeBaseTimeOut_us;
static uint32_t Saej_msgJoyOnChangeRefreshTimeOut_us;

static Saej_fnr_t saej_FNR;

static uint32_t saej_wstButtonIndex_1_8;
static uint32_t saej_wstButtonIndex_9_15;

static uint8_t Saej_dutAsciiCode[9U];
static uint8_t Saej_dutSerialNum[8U];
static uint8_t Saej_dutHardwareId[6U];
static uint16_t Saej_dutManufacturerCode;
static uint8_t Saej_dutAsciiManufacturerCode[7U];
static uint8_t Saej_dutSerialNumToShow[10u];

static uint8_t Saej_ledInternalValue;
static uint8_t Saej_ledInternalDuty;
static uint8_t Saej_ledErrorIndex;

static uint8_t Saej_ledDrive;
static uint8_t Saej_ledDuty;

static uint8_t Saej_isobusEn;

static wres_t resultTPTx[SAEJ_TP_SLOT_NUM_MAX];
static wres_t resultETPTx[SAEJ_TP_SLOT_NUM_MAX];

//------------------------------------------------------------------------------
static uint16_t Saej_SetpointEval( uint16_t axisInput, Saej_axisOut_t *axisOutput, bool isRoller )
{
    /*
       il dato in ingresso è formattato in modo da avere escursione 0 ÷ 2*SAEJ_AXIS_OUT_MAX,
       in neutro vale quindi SAEJ_AXIS_OUT_MAX.
       Lo devo convertire in modo tale da avere il setpoint che varia sempre da 0 a SAEJ_AXIS_OUT_MAX
       e le coppie di bit di direzione "left negative", "center", "right positive" che
       segnalano in quale direzione sto andando.
       Il dato in ingresso è già al netto della banda morta, quindi:
       se > 0 il dato è già valido fuori neutro
       se > 2 * SAEJ_OUT_MAX è in errore
       se 0xFFFF è not available
    */
    
    uint16_t retVal;
    uint32_t aux;
    
    
    axisOutput->previousValue = axisOutput->value;
    axisOutput->detentOutPrevious = axisOutput->detentOut;
    
    if ( false == axisOutput->startupDone )
    {
        if ( false == axisOutput->notWaitNeutralAtStartup )
        {
            if ( SAEJ_AXIS_OUT_MAX != axisInput )
            {
                axisOutput->status = SAEJ_AXIS_STATUS_ERROR;                                    
                axisOutput->value = SAEJ_AXIS_OUT_ERROR;
                if ( SAEJ_DETENT_OFF != axisOutput->detentEn )
                {
                    axisOutput->detentOut = SAEJ_STATUS_ERROR;
                }
                else
                {
                    axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
                }              
            }
            else
            {
                axisOutput->startupDone = true;
            }
        }
        else
        {
            axisOutput->startupDone = true;
        }
    }

    if ( true == axisOutput->startupDone )
    {
        if ( 0xFFFEU == axisInput ) //error
        {
            axisOutput->status = SAEJ_AXIS_STATUS_ERROR;                                    
            axisOutput->value = SAEJ_AXIS_OUT_ERROR;
            if ( SAEJ_DETENT_OFF != axisOutput->detentEn )
            {
                axisOutput->detentOut = SAEJ_STATUS_ERROR;
            }
            else
            {
                axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
            }
        }
        else if ( 0xFFFFU == axisInput ) //not available
        {
            axisOutput->status = SAEJ_AXIS_STATUS_NOT_AVAILABLE;
            axisOutput->value = SAEJ_AXIS_OUT_NOT_AVAILABLE;                                    
            axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
        }
        else if ( SAEJ_AXIS_OUT_MAX == axisInput )  //asse in neutro      
        {
            axisOutput->status = SAEJ_AXIS_STATUS_NEUTRAL;
            axisOutput->value = SAEJ_AXIS_OUT_NEUTRAL;                                   
            if ( SAEJ_DETENT_OFF != axisOutput->detentEn )
            {
                axisOutput->detentOut = SAEJ_DETENT_OFF;
            }
            else
            {
                axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
            }
        }
        else
        {
            if ( true == isRoller )
            {
                if ( true == axisOutput->outInverted )    
                {
                    axisInput = ( 2U * SAEJ_AXIS_OUT_MAX ) - axisInput;
                }
            }
            else if ( false == axisOutput->outInverted )
            {
                axisInput = ( 2U * SAEJ_AXIS_OUT_MAX ) - axisInput;
            }
            else
            {
            }
      
            if ( axisInput > SAEJ_AXIS_OUT_MAX ) //asse in direzione "right positive"
            {
                axisOutput->status = SAEJ_AXIS_STATUS_POSITIVE;                                  
                axisOutput->value = axisInput - SAEJ_AXIS_OUT_MAX;
                if ( SAEJ_DETENT_EN_POS == ( axisOutput->detentEn & SAEJ_DETENT_EN_POS ) )
                {
                    if ( axisOutput->value >= ( axisOutput->detentThPos - SAEJ_DETENT_SAFE_BAND ) )
                    {
                        aux = (uint32_t)( SAEJ_AXIS_OUT_MAX );
                    }
                    else
                    {
                        aux = (uint32_t)( axisOutput->value ) * ( uint32_t)( SAEJ_AXIS_OUT_MAX );
                        aux = aux / (uint32_t)( axisOutput->detentThPos - SAEJ_DETENT_SAFE_BAND );
                    }
           
                    if ( SAEJ_DETENT_ON == axisOutput->detentOut )
                    {
                        if ( axisOutput->value < ( axisOutput->detentThPos - SAEJ_DETENT_SAFE_BAND ) )
                        {
                            axisOutput->detentOut = SAEJ_DETENT_OFF;
                        }
                    }
                    else
                    {
                        //if ( axisOutput->value > ( axisOutput->detentThPos + SAEJ_DETENT_SAFE_BAND ) )
                        if ( axisOutput->value > ( SAEJ_AXIS_OUT_MAX - 50U ) )
                        {
                            axisOutput->detentOut = SAEJ_DETENT_ON;
                        }
                    }
                    axisOutput->value = (uint16_t)( aux );
                }
                else
                {
                    if ( SAEJ_DETENT_EN_OFF == axisOutput->detentEn )
                    {
                        axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
                    }
                    else
                    {
                        axisOutput->detentOut = SAEJ_DETENT_OFF;
                    }
                }
            }
            else if ( axisInput < SAEJ_AXIS_OUT_MAX ) //asse in direzione "left negative"
            {
                axisOutput->status = SAEJ_AXIS_STATUS_NEGATIVE;                                  
                axisOutput->value = SAEJ_AXIS_OUT_MAX - axisInput;
                if ( SAEJ_DETENT_EN_NEG == ( axisOutput->detentEn & SAEJ_DETENT_EN_NEG ) )
                {
                    if ( axisOutput->value >= ( axisOutput->detentThNeg - SAEJ_DETENT_SAFE_BAND ) )
                    {
                        aux = (uint32_t)( SAEJ_AXIS_OUT_MAX );
                    }
                    else
                    {
                        aux = (uint32_t)( axisOutput->value ) * ( uint32_t)( SAEJ_AXIS_OUT_MAX );
                        aux = aux / (uint32_t)( axisOutput->detentThNeg - SAEJ_DETENT_SAFE_BAND );
                    }
           
                    if ( SAEJ_DETENT_ON == axisOutput->detentOut )
                    {
                        if ( axisOutput->value < ( axisOutput->detentThNeg - SAEJ_DETENT_SAFE_BAND ) )
                        {
                            axisOutput->detentOut = SAEJ_DETENT_OFF;
                        }
                    }
                    else
                    {
                        //if ( axisOutput->value > ( axisOutput->detentThNeg + SAEJ_DETENT_SAFE_BAND ) )
                        if ( axisOutput->value > ( SAEJ_AXIS_OUT_MAX - 50U ) )
                        {
                            axisOutput->detentOut = SAEJ_DETENT_ON;
                        }
                    }
                    axisOutput->value = (uint16_t)( aux );
                }
                else
                {
                    if ( SAEJ_DETENT_EN_OFF == axisOutput->detentEn )
                    {
                        axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
                    }
                    else
                    {
                        axisOutput->detentOut = SAEJ_DETENT_OFF;
                    }
                }          
            }
            else
            {
                axisOutput->status = SAEJ_AXIS_STATUS_NOT_AVAILABLE;
                axisOutput->value = SAEJ_AXIS_OUT_NOT_AVAILABLE;                                    //not available
                axisOutput->detentOut = SAEJ_STATUS_NOT_AVAILABLE;
            }        
        }
    }
    
    retVal = (uint16_t)( axisOutput->status & 0x003FU ) | ( axisOutput->value << 6U );
    
    if ( ( axisOutput->value != axisOutput->previousValue ) || ( axisOutput->detentOut != axisOutput->detentOutPrevious ) )
    {
        axisOutput->changed = true;
    }
    else
    {
        axisOutput->changed = false;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static uint8_t Saej_PushButtonInputGet( uint8_t en, uint8_t inVal, uint8_t pos, wres_t *result )
{
    uint8_t retVal;
    
    *result = WRES_OK;
    
    if ( SAEJ_P_DISABLE == en)       // Not Avalable
    {
        retVal = ( 0x03U << pos ); 
    }      
    else if ( SAEJ_IN_ON == inVal  ) // On
    {
        retVal = ( 0x01U << pos ); 
    }  
    else if ( SAEJ_IN_OFF == inVal ) //Off
    {
        retVal = ( 0x00U << pos ); 
    } 
    else                             //Error
    {
        retVal = ( 0x02U << pos );
        *result = WRES_ERROR;
    }    
    
    return retVal;
}

//------------------------------------------------------------------------------
static uint8_t Saej_PushButtonOutputSet( Saej_fnr_t fnr, uint8_t input, uint8_t *status )
{
    uint8_t retVal;  
    uint8_t inF;
    uint8_t inR;
  
    retVal = SAEJ_IN_OFF;
    inF = SAEJ_IN_OFF;
    inR = SAEJ_IN_OFF;

    if ( true == fnr.enabled )
    {
        inF = Digital_GetValue( Saej_pushButtonIndex[fnr.inputF] );
        inR = Digital_GetValue( Saej_pushButtonIndex[fnr.inputR] );
        if ( fnr.outputN == input )
        {
            if ( ( SAEJ_STATUS_ERROR == inF ) || ( SAEJ_STATUS_ERROR == inR ) ) 
            {
                retVal = SAEJ_STATUS_ERROR;
            }
            else
            {
                if ( ( SAEJ_IN_OFF == inF ) && ( SAEJ_IN_OFF == inR ) )
                {
                    retVal = SAEJ_IN_ON;
                }
                else
                {
                    if ( ( ( inF == SAEJ_IN_ON ) && ( inR == SAEJ_IN_OFF ) ) || ( ( inF == SAEJ_IN_OFF ) && ( inR == SAEJ_IN_ON ) ) )
                    {
                        retVal = SAEJ_IN_OFF;
                    }
                    else
                    {
                        retVal = SAEJ_STATUS_ERROR;
                    }
                }
            }
        } 
        else if ( fnr.inputF == input )
        {
            if ( ( inF == SAEJ_IN_ON ) && ( inR == SAEJ_IN_OFF ) )
            {
                retVal = SAEJ_IN_ON;
            }
            else if ( ( inF == SAEJ_IN_OFF ) && ( inR == SAEJ_IN_ON ) )
            {
                retVal = SAEJ_IN_OFF;
            }
            else if ( ( inF == SAEJ_IN_OFF ) && ( inR == SAEJ_IN_OFF ) ) 
            {
                retVal = SAEJ_IN_OFF;
            }
            else
            {
                retVal = SAEJ_STATUS_ERROR;
            }
        }
        else if ( fnr.inputR == input )
        {
            if ( ( inF == SAEJ_IN_ON ) && ( inR == SAEJ_IN_OFF ) )
            {
                retVal = SAEJ_IN_OFF;
            }
            else if ( ( inF == SAEJ_IN_OFF ) && ( inR == SAEJ_IN_ON ) )
            {
                retVal = SAEJ_IN_ON;
            }
            else if ( ( inF == SAEJ_IN_OFF ) && ( inR == SAEJ_IN_OFF ) ) 
            {
                retVal = SAEJ_IN_OFF;
            }
            else
            {
                retVal = SAEJ_STATUS_ERROR;
            }          
        }
        else
        {
            retVal = Digital_GetValue( Saej_pushButtonIndex[input] );
        }
    }
    else
    {
        retVal = Digital_GetValue( Saej_pushButtonIndex[input] );
    }
    
    if ( *status == SAEJ_STATUS_ERROR )
    {
        if ( ( true == fnr.enabled ) && ( fnr.outputN == input ) )
        {
            if ( retVal == SAEJ_IN_ON )
            {
                *status = retVal;
            }
        }
        else
        {
            if ( retVal == SAEJ_IN_OFF )
            {
                *status = retVal;
            }          
        }
    }
    else
    {
        *status = retVal;
    }
                                                                                                                                               
    return retVal;                                                                                                                                               
}

//------------------------------------------------------------------------------
static void Saej_PushButtonUpdate( void )
{
    wres_t resDummy;
    
    static uint8_t statusButton[13U] = { SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF,  
                                         SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF, SAEJ_IN_OFF }; 
    uint8_t aux;
  
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_1, &statusButton[0U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_2, &statusButton[1U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_3, &statusButton[2U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_4, &statusButton[3U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_5, &statusButton[4U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_6, &statusButton[5U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_7, &statusButton[6U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_8, &statusButton[7U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_9, &statusButton[8U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_10, &statusButton[9U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_11, &statusButton[10U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_12, &statusButton[11U] );
    aux = Saej_PushButtonOutputSet( saej_FNR, SAEJ_BUTTON_13, &statusButton[12U] );
    
    Saej_pushButtonStatus[0U] = 0x00U;
    Saej_pushButtonStatus[0U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_4], statusButton[3U], SAEJ_POS1, &Saej_pushButtonGetResult[SAEJ_BUTTON_4] );
    Saej_pushButtonStatus[0U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_3], statusButton[2U], SAEJ_POS2, &Saej_pushButtonGetResult[SAEJ_BUTTON_3] );
    Saej_pushButtonStatus[0U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_2], statusButton[1U], SAEJ_POS3, &Saej_pushButtonGetResult[SAEJ_BUTTON_2] );
    Saej_pushButtonStatus[0U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_1], statusButton[0U], SAEJ_POS4, &Saej_pushButtonGetResult[SAEJ_BUTTON_1] );

    Saej_pushButtonStatus[1U] = 0x00U;
    Saej_pushButtonStatus[1U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_8], statusButton[7U], SAEJ_POS1, &Saej_pushButtonGetResult[SAEJ_BUTTON_8] );
    Saej_pushButtonStatus[1U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_7], statusButton[6U], SAEJ_POS2, &Saej_pushButtonGetResult[SAEJ_BUTTON_7] );
    Saej_pushButtonStatus[1U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_6], statusButton[5U], SAEJ_POS3, &Saej_pushButtonGetResult[SAEJ_BUTTON_6] );
    Saej_pushButtonStatus[1U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_5], statusButton[4U], SAEJ_POS4, &Saej_pushButtonGetResult[SAEJ_BUTTON_5] );

    Saej_pushButtonStatus[2U] = 0x00U;
    Saej_pushButtonStatus[2U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_12], statusButton[11U], SAEJ_POS1, &Saej_pushButtonGetResult[SAEJ_BUTTON_12] ); 
    Saej_pushButtonStatus[2U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_11], statusButton[10U], SAEJ_POS2, &Saej_pushButtonGetResult[SAEJ_BUTTON_11] );     
    Saej_pushButtonStatus[2U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_10], statusButton[9U], SAEJ_POS3, &Saej_pushButtonGetResult[SAEJ_BUTTON_10] );
    Saej_pushButtonStatus[2U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_9], statusButton[8U], SAEJ_POS4, &Saej_pushButtonGetResult[SAEJ_BUTTON_9] );
    
    Saej_pushButtonStatus[3U] = 0x00U;
    Saej_pushButtonStatus[3U] |= Saej_PushButtonInputGet( SAEJ_P_DISABLE, SAEJ_IN_OFF, SAEJ_POS1, &resDummy ); 
    Saej_pushButtonStatus[3U] |= Saej_PushButtonInputGet( SAEJ_P_DISABLE, SAEJ_IN_OFF, SAEJ_POS2, &resDummy ); 
    Saej_pushButtonStatus[3U] |= Saej_PushButtonInputGet( SAEJ_P_DISABLE, SAEJ_IN_OFF, SAEJ_POS3, &resDummy ); 
    Saej_pushButtonStatus[3U] |= Saej_PushButtonInputGet( Saej_pushButtonEn[SAEJ_BUTTON_13], statusButton[12U], SAEJ_POS4, &Saej_pushButtonGetResult[SAEJ_BUTTON_13] );     
    
    Saej_bufferTxBJM[5U] = Saej_pushButtonStatus[0U];
    Saej_bufferTxBJM[6U] = Saej_pushButtonStatus[1U];
    Saej_bufferTxBJM[7U] = Saej_pushButtonStatus[2U];
    Saej_bufferTxEJM[7U] = Saej_pushButtonStatus[3U];
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_AxisUpdate( void )
{
    uint16_t auxAxis;
    
    if ( SAEJ_AXIS_X_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_X_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( MLX_LinearAlphaGet() , &Saej_axis[SAEJ_AXIS_X], false );
        Saej_bufferTxBJM[0U] = (uint8_t)( auxAxis );
        Saej_bufferTxBJM[1U] = (uint8_t)( auxAxis >> 8U );
    }
    else
    {
        Saej_bufferTxBJM[0U] = 0xFFU;
        Saej_bufferTxBJM[1U] = 0xFFU;
    }
    
    if ( SAEJ_AXIS_Y_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_Y_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( MLX_LinearBetaGet(), &Saej_axis[SAEJ_AXIS_Y], false );
        Saej_bufferTxBJM[2U] = (uint8_t)( auxAxis );
        Saej_bufferTxBJM[3U] = (uint8_t)( auxAxis >> 8U );
    }
    else
    {
        Saej_bufferTxBJM[2U] = 0xFFU;
        Saej_bufferTxBJM[3U] = 0xFFU;
    }   

    Saej_bufferTxBJM[4U] = 0x0FU | ( ( Saej_axis[SAEJ_AXIS_X].detentOut & 0x03U ) << 6U ) | ( ( Saej_axis[SAEJ_AXIS_Y].detentOut & 0x03U ) << 4U );    
    
    if ( SAEJ_AXIS_W_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_W_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( Roller_GetOutput( Saej_rollerIndex[SAEJ_ROLLER_W] ), &Saej_axis[SAEJ_AXIS_W], true );
        Saej_bufferTxEJM[0U] = (uint8_t)( auxAxis );
        Saej_bufferTxEJM[1U] = (uint8_t)( auxAxis >> 8U );            
    }
    else
    {
        Saej_bufferTxEJM[0U] = 0xFFU;
        Saej_bufferTxEJM[1U] = 0xFFU;
    }
    
    if ( SAEJ_AXIS_Z_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_Z_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( Roller_GetOutput( Saej_rollerIndex[SAEJ_ROLLER_Z] ), &Saej_axis[SAEJ_AXIS_Z], true );
        Saej_bufferTxEJM[2U] = (uint8_t)( auxAxis );
        Saej_bufferTxEJM[3U] = (uint8_t)( auxAxis >> 8U );   
    }
    else
    {
        Saej_bufferTxEJM[2U] = 0xFFU;
        Saej_bufferTxEJM[3U] = 0xFFU;
    }    

    if ( SAEJ_AXIS_J_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_J_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( Roller_GetOutput( Saej_rollerIndex[SAEJ_ROLLER_J] ), &Saej_axis[SAEJ_AXIS_J], true );
        Saej_bufferTxEJM[4U] = (uint8_t)( auxAxis );
        Saej_bufferTxEJM[5U] = (uint8_t)( auxAxis >> 8U );            
    }
    else
    {
        Saej_bufferTxEJM[4U] = 0xFFU;
        Saej_bufferTxEJM[5U] = 0xFFU;
    }    
    
    Saej_bufferTxEJM[6U] = 0x03U | ( ( Saej_axis[SAEJ_AXIS_W].detentOut & 0x03U ) << 6U ) | ( ( Saej_axis[SAEJ_AXIS_Z].detentOut & 0x03U ) << 4U ) | ( ( Saej_axis[SAEJ_AXIS_J].detentOut & 0x03U ) << 2U );


//      auxAxis = ADC_GetValue_mV( ADC_INPUT_G0_CH0 );
//      Saej_bufferTxEJM[0U] = (uint8_t)( auxAxis );
//      Saej_bufferTxEJM[1U] = (uint8_t)( auxAxis >> 8U );            
//      auxAxis = ADC_GetValue_mV( ADC_INPUT_G0_CH2 );
//      Saej_bufferTxEJM[2U] = (uint8_t)( auxAxis );
//      Saej_bufferTxEJM[3U] = (uint8_t)( auxAxis >> 8U );                  

    

    if ( SAEJ_AXIS_Q_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_Q_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( Roller_GetOutput( Saej_rollerIndex[SAEJ_ROLLER_Q] ), &Saej_axis[SAEJ_AXIS_Q], true );
        Saej_bufferTxEJM2[0U] = (uint8_t)( auxAxis );
        Saej_bufferTxEJM2[1U] = (uint8_t)( auxAxis >> 8U );            
    }
    else
    {
        Saej_bufferTxEJM2[0U] = 0xFFU;
        Saej_bufferTxEJM2[1U] = 0xFFU;
    }
    
    if ( SAEJ_AXIS_V_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_V_ENABLED ) )
    {
        auxAxis = Saej_SetpointEval( Roller_GetOutput( Saej_rollerIndex[SAEJ_ROLLER_V] ), &Saej_axis[SAEJ_AXIS_V], true );
        Saej_bufferTxEJM2[2U] = (uint8_t)( auxAxis );
        Saej_bufferTxEJM2[3U] = (uint8_t)( auxAxis >> 8U );            
    }
    else
    {
        Saej_bufferTxEJM2[2U] = 0xFFU;
        Saej_bufferTxEJM2[3U] = 0xFFU;
    } 
    Saej_bufferTxEJM2[6U] = 0x0FU | ( ( Saej_axis[SAEJ_AXIS_Q].detentOut & 0x03U ) << 6U ) | ( ( Saej_axis[SAEJ_AXIS_V].detentOut & 0x03U ) << 4U );    

    return;
}

//------------------------------------------------------------------------------
extern void Saej_LedInternalValueSet( uint8_t ledValue )
{
    Saej_ledInternalValue = ledValue;
    return;
}

//------------------------------------------------------------------------------
extern void Saej_LedInternalDutySet( uint8_t dutyValue )
{
    Saej_ledInternalDuty = dutyValue;
    return;
}

//------------------------------------------------------------------------------
extern uint8_t Saej_LedInternalValueGet( void )
{
    return Saej_ledInternalValue;
}

//------------------------------------------------------------------------------
extern uint8_t Saej_LedInternalDutyGet( void )
{
    return Saej_ledInternalDuty;
}

//------------------------------------------------------------------------------
static uint8_t Saej_LedBlink( uint8_t ledValue, uint8_t ledErrorIndex )
{
    static uint32_t blinkCounter = 0U;
    static uint32_t blinkCounterTh = 500000U / SCHEDULE_COUNTER_TH_SAEJ;
    
    static uint8_t blinkStatus = 0U;
    
    uint8_t aux;
    
    aux = ledValue;
    if ( 0U == blinkStatus )
    {
        ++blinkCounter;
        if ( blinkCounter >= blinkCounterTh )
        {
            blinkCounter = 0U;
            blinkStatus = 1U;
        }
        aux = aux & (~( 0x03U << ( 2U * ledErrorIndex ) ) );
    }
    else
    {
        ++blinkCounter;
        if ( blinkCounter >= blinkCounterTh )
        {
            blinkCounter = 0U;
            blinkStatus = 0U;
        }
        aux = aux | ( 0x01U << ( 2U * ledErrorIndex ) );
    }
    
    return aux;
     
}

//------------------------------------------------------------------------------
static uint8_t Saej_ledFilterEn( uint8_t ledInput, uint8_t ledMask )
{
    uint8_t aux;
    
    aux = 0x00U;
    
    if ( SAEJ_LED_0_EN == ( SAEJ_LED_0_EN & ledMask ) )
    {
        //aux = ledInput & 0xFCU;
        aux = aux | 0x03U;
    }
    if ( SAEJ_LED_1_EN == ( SAEJ_LED_1_EN & ledMask ) )
    {
        //aux = ledInput & 0xF3U;
        aux = aux | 0x0CU;
    }    
    if ( SAEJ_LED_2_EN == ( SAEJ_LED_2_EN & ledMask ) )
    {
        //aux = ledInput & 0xCFU;
        aux = aux | 0x30U;
    }    
    if ( SAEJ_LED_3_EN == ( SAEJ_LED_3_EN & ledMask ) )
    {
        //aux = ledInput & 0x3FU;
        aux = aux | 0xC0U;
    }    
    
    return (uint8_t)( ledInput & aux );
}

//------------------------------------------------------------------------------
static void Saej_LedCmdUpdate( void )
{
    uint8_t ledOut;
    uint8_t ledDuty;
    uint8_t j;
  
    Error_Update( &Saej_ledCmd, ( true == Saej_ledCmd.en ), ERROR_RESET_IMMEDIATE );
    
    ledOut = 0U;
    ledDuty = 0XFFU;
    
//    if ( true == Saej_error.active )
//    {
//        if ( true == Saej_ledEn )
//        {
//            ledOut = Saej_error.value;
//            //BSP_OutLedUpdate( Saej_error.value );
//        }
//    }

    if ( Saej_ledCmdEn != 0u )
    {
        ledOut = Saej_ledDrive;
        ledDuty = Saej_ledDuty;
        //BSP_OutLedUpdate( CANMsgRx_Saej_Led.Data[0U] );
    }
    else if ( true == Saej_ledEn )
    {
        ledOut = Saej_LedInternalValueGet();
        ledDuty = Saej_LedInternalDutyGet();
    }
    else
    {
      // Do Nothing
    }

    if ( true == Saej_error.active )
    {
        if ( true == Saej_ledEn )
        {
            if ( 0x00U != Saej_error.value )
            {
                ledOut = Saej_LedBlink( ledOut, Saej_ledErrorIndex );
            }
            //BSP_OutLedUpdate( Saej_error.value );
        }
    }    
    
    BSP_OutLedUpdate( ledOut, ledDuty );
    
//    for ( j=0U; j<LED_OUT_NUM; j++ )
//    {
//        BSP_OutLedCheck( j, ledOut, ( 0U != Saej_ledCmdEn ), Saej_ledEn, Saej_ledErrorIndex );
//    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_paramToRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED  
    uint32_t aux;
    uint32_t buttonaux;
    uint32_t rolleraux;
    uint32_t fnraux;
    MemData_SetRAMItem( ADDR_SAEJ_CFG_PUSHBUTTON, (uint32_t)Saej_cfgPushButton );
    MemData_SetRAMItem( ADDR_SAEJ_CFG_AXES, Saej_cfgAxes );
    MemData_SetRAMItem( ADDR_SAEJ_CFG_DETENT, (uint32_t)Saej_cfgDetent );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_X, (uint32_t)( Saej_axis[SAEJ_AXIS_X].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_X].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_Y, (uint32_t)( Saej_axis[SAEJ_AXIS_Y].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_Y].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_W, (uint32_t)( Saej_axis[SAEJ_AXIS_Z].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_Z].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_Z, (uint32_t)( Saej_axis[SAEJ_AXIS_W].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_W].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_J, (uint32_t)( Saej_axis[SAEJ_AXIS_J].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_J].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_Q, (uint32_t)( Saej_axis[SAEJ_AXIS_Q].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_Q].detentThNeg << 16U ) );
    MemData_SetRAMItem( ADDR_SAEJ_DETENT_AXIS_V, (uint32_t)( Saej_axis[SAEJ_AXIS_V].detentThPos & 0x0000FFFFUL ) | (uint32_t)( Saej_axis[SAEJ_AXIS_V].detentThNeg << 16U ) );
    aux = (uint16_t)( Saej_defaultAddress );
    aux |= (uint16_t)( Saej_isSelfConfigurableDevice << 8U );    
    MemData_SetRAMItem( ADDR_SAEJ_DEFAULT_ADDRESS, aux );
    aux = (uint32_t)( Saej_ledCmdEn );
    aux |= (( (Saej_ledEn?1uL:0uL) & 0x01u ) << 4U );
    aux |= ( ((uint32_t)Saej_ledErrorIndex & 0x03U ) << 5U );
    aux |= ( (Saej_txOnChange?1uL:0uL) << 8U );
    aux |= ( ( ( Saej_ledCmd.cntTh * SCHEDULE_COUNTER_TH_SAEJ ) / 1000U ) << 16U );
    MemData_SetRAMItem( ADDR_SAEJ_JOY_MSG_CFG, aux );
    MemData_SetRAMItem( ADDR_SAEJ_MSG_JOY_FREE_TIME_OUT, Saej_msgJoyTimeOut_us );
    MemData_SetRAMItem( ADDR_SAEJ_MSG_JOY_ON_CHANGE_BASE_TIME_OUT, Saej_msgJoyOnChangeBaseTimeOut_us ); 
    MemData_SetRAMItem( ADDR_SAEJ_MSG_JOY_ON_CHANGE_REFRESH_TIME_OUT, Saej_msgJoyOnChangeRefreshTimeOut_us );
    buttonaux = ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_1] ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_2] << 8U  ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_3] << 16U ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_4] << 24U );
    MemData_SetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_1_4, buttonaux );
    buttonaux = ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_5] ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_6] << 8U  ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_7] << 16U ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_8] << 24U );
    MemData_SetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_5_8, buttonaux );
    buttonaux = ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_9] ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_10] << 8U ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_11] << 16U ) | 
                ( (uint32_t)Saej_pushButtonIndex[SAEJ_BUTTON_12] << 24U );
    MemData_SetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_9_12, buttonaux );
    buttonaux = (uint32_t)( Saej_pushButtonIndex[SAEJ_BUTTON_13] );
    MemData_SetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_13_16, buttonaux );
    rolleraux =  ( (uint32_t)Saej_rollerIndex[SAEJ_ROLLER_W] & 0x0000000FUL );
    rolleraux |= ( (uint32_t)Saej_rollerIndex[SAEJ_ROLLER_Z] & 0x0000000FUL ) << 4U;
    rolleraux |= ( (uint32_t)Saej_rollerIndex[SAEJ_ROLLER_J] & 0x0000000FUL ) << 8U;
    rolleraux |= ( (uint32_t)Saej_rollerIndex[SAEJ_ROLLER_Q] & 0x0000000FUL ) << 12U;
    rolleraux |= ( (uint32_t)Saej_rollerIndex[SAEJ_ROLLER_V] & 0x0000000FUL ) << 16U;
    MemData_SetRAMItem( ADDR_SAEJ_ROLLER_INDEX, rolleraux );
    fnraux =  ( saej_FNR.enabled? 1uL : 0uL );
    fnraux |= ( (uint32_t)saej_FNR.inputF << 8U );
    fnraux |= ( (uint32_t)saej_FNR.inputR << 16U );
    fnraux |= ( (uint32_t)saej_FNR.outputN << 24U );
    MemData_SetRAMItem( ADDR_SAEJ_FNR, fnraux );
    MemData_SetRAMItem( ADDR_WST_BUTTON_INDEX_1_8, saej_wstButtonIndex_1_8 );
    MemData_SetRAMItem( ADDR_WST_BUTTON_INDEX_9_15, saej_wstButtonIndex_9_15 );
    aux = ( (uint32_t)Saej_dutSerialNum[0U] ) + 
          ( (uint32_t)Saej_dutSerialNum[1U] << 8U ) + 
          ( (uint32_t)Saej_dutSerialNum[2U] << 16U ) + 
          ( (uint32_t)Saej_dutSerialNum[3U] << 24U );
    MemData_SetRAMItem( ADDR_SERIAL_NUM_0_3, aux );
    aux = ( (uint32_t)Saej_dutSerialNum[4U] ) + 
          ( (uint32_t)Saej_dutSerialNum[5U] << 8U ) + 
          ( (uint32_t)Saej_dutSerialNum[6U] << 16U ) + 
          ( (uint32_t)Saej_dutSerialNum[7U] << 24U );    
    MemData_SetRAMItem( ADDR_SERIAL_NUM_4_7, aux );
    aux = ( (uint32_t)Saej_dutAsciiCode[0U] ) + 
          ( (uint32_t)Saej_dutAsciiCode[1U] << 8U ) + 
          ( (uint32_t)Saej_dutAsciiCode[2U] << 16U ) + 
          ( (uint32_t)Saej_dutAsciiCode[3U] << 24U );
    MemData_SetRAMItem( ADDR_ASCII_DUT_CODE_0_3, aux );
    aux = ( (uint32_t)Saej_dutAsciiCode[4U] ) + 
          ( (uint32_t)Saej_dutAsciiCode[5U] << 8U ) + 
          ( (uint32_t)Saej_dutAsciiCode[6U] << 16U ) + 
          ( (uint32_t)Saej_dutAsciiCode[7U] << 24U );
    MemData_SetRAMItem( ADDR_ASCII_DUT_CODE_4_7, aux );
    aux = (uint32_t)( Saej_dutAsciiCode[8U] );
    MemData_SetRAMItem( ADDR_ASCII_DUT_CODE_8_11, aux );
    aux = ( (uint32_t)Saej_dutHardwareId[0U] ) + 
          ( (uint32_t)Saej_dutHardwareId[1U] << 8U ) + 
          ( (uint32_t)Saej_dutHardwareId[2U] << 16U ) + 
          ( (uint32_t)Saej_dutHardwareId[3U] << 24U );
    MemData_SetRAMItem( ADDR_HW_ID_0_3, aux );
    aux = ( (uint32_t)Saej_dutHardwareId[4U] ) + 
          ( (uint32_t)Saej_dutHardwareId[5U] << 8U );
    MemData_SetRAMItem( ADDR_HW_ID_4_7, aux );
    MemData_SetRAMItem( ADDR_SAEJ_ISOBUS_EN, (uint32_t)( Saej_isobusEn ) );

#endif
    return;
}

//------------------------------------------------------------------------------
static void Saej_paramFromRAMUpdate( void )
{
#ifndef DJW_MEM_DISABLED
    uint32_t datoRead;
    uint8_t i;
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_CFG_PUSHBUTTON, &datoRead ) )
    {
        Saej_cfgPushButton = (uint16_t)( datoRead );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_CFG_AXES, &datoRead ) )
    {
        Saej_cfgAxes = datoRead;
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_CFG_DETENT, &datoRead ) )
    {
        Saej_cfgDetent = (uint16_t)( datoRead );
    }
    for ( i=0U; i<SAEJ_AXIS_NUM; i++ )
    {
        Saej_axis[i].detentEn = (uint8_t)( Saej_cfgDetent >> ( 2U * i ) ) & 0x03U;
        Saej_axis[i].outInverted = ( (uint8_t)( Saej_cfgAxes >> ( 1U + ( 2U* i ) ) ) & 0x01U ) != 0u;
        Saej_axis[i].notWaitNeutralAtStartup = ( (uint8_t)( Saej_cfgAxes >> ( 16U + i ) ) & 0x01U ) != 0u;
    }    

    for ( i=0U; i<SAEJ_AXIS_NUM; i++ )
    {
        if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_DETENT_AXIS_X + i, &datoRead ) )
        {
            Saej_axis[SAEJ_AXIS_X + i].detentThPos = (uint16_t)( datoRead );
            Saej_axis[SAEJ_AXIS_X + i].detentThNeg = (uint16_t)( datoRead >> 16U );
        }
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_DEFAULT_ADDRESS, &datoRead ) )
    {
        //Saej_defaultAddress = (uint8_t)( datoRead );
        Saej_defaultAddress = Application_sourceAddressGet();
        Saej_isSelfConfigurableDevice = (uint8_t)( ( datoRead & 0x0000FF00UL ) >> 8U ) ;
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_JOY_MSG_CFG, &datoRead ) )
    {
        Saej_ledCmdEn = (uint8_t)( datoRead & 0x0000000FUL );
        //Saej_ledEn = (bool)( datoRead & 0x000000F0UL );
        Saej_ledEn = ( datoRead & 0x00000010UL ) != 0u;
        Saej_ledErrorIndex = (uint8_t)( ( datoRead & 0x00000060UL ) >> 5U );
        Saej_txOnChange = ( datoRead & 0x0000FF00UL ) != 0u;
        Saej_ledCmd.cntTh = (uint32_t)( ( datoRead & 0xFFFF0000UL ) >> 16U ) * 1000UL;
        Saej_ledCmd.cntTh = Saej_ledCmd.cntTh / (uint32_t)( SCHEDULE_COUNTER_TH_SAEJ );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_MSG_JOY_FREE_TIME_OUT, &datoRead ) )
    {
        Saej_msgJoyTimeOut_us = datoRead;
    }

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_MSG_JOY_ON_CHANGE_BASE_TIME_OUT, &datoRead ) )
    {
        Saej_msgJoyOnChangeBaseTimeOut_us = datoRead;
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_MSG_JOY_ON_CHANGE_REFRESH_TIME_OUT, &datoRead ) )
    {
        Saej_msgJoyOnChangeRefreshTimeOut_us = datoRead;
    }    

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_1_4, &datoRead ) )
    {
        Saej_pushButtonIndex[SAEJ_BUTTON_1] = (uint8_t)( datoRead );
        Saej_pushButtonIndex[SAEJ_BUTTON_2] = (uint8_t)( datoRead >> 8U );
        Saej_pushButtonIndex[SAEJ_BUTTON_3] = (uint8_t)( datoRead >> 16U );
        Saej_pushButtonIndex[SAEJ_BUTTON_4] = (uint8_t)( datoRead >> 24U );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_5_8, &datoRead ) )
    {
        Saej_pushButtonIndex[SAEJ_BUTTON_5] = (uint8_t)( datoRead );
        Saej_pushButtonIndex[SAEJ_BUTTON_6] = (uint8_t)( datoRead >> 8U );
        Saej_pushButtonIndex[SAEJ_BUTTON_7] = (uint8_t)( datoRead >> 16U );
        Saej_pushButtonIndex[SAEJ_BUTTON_8] = (uint8_t)( datoRead >> 24U );
    }    

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_9_12, &datoRead ) )
    {
        Saej_pushButtonIndex[SAEJ_BUTTON_9] = (uint8_t)( datoRead );
        Saej_pushButtonIndex[SAEJ_BUTTON_10] = (uint8_t)( datoRead >> 8U );
        Saej_pushButtonIndex[SAEJ_BUTTON_11] = (uint8_t)( datoRead >> 16U );
        Saej_pushButtonIndex[SAEJ_BUTTON_12] = (uint8_t)( datoRead >> 24U );
    }    

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_PUSH_BUTTON_INDEX_13_16, &datoRead ) )
    {
        Saej_pushButtonIndex[SAEJ_BUTTON_13] = (uint8_t)( datoRead );
    }  
    
    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_ROLLER_INDEX, &datoRead ) )
    {
        Saej_rollerIndex[SAEJ_ROLLER_W] = (uint8_t)(   datoRead & 0x0000000FU );
        Saej_rollerIndex[SAEJ_ROLLER_Z] = (uint8_t)( ( datoRead & 0x000000F0U ) >> 4U );
        Saej_rollerIndex[SAEJ_ROLLER_J] = (uint8_t)( ( datoRead & 0x00000F00U ) >> 8U );
        Saej_rollerIndex[SAEJ_ROLLER_Q] = (uint8_t)( ( datoRead & 0x0000F000U ) >> 12U );
        Saej_rollerIndex[SAEJ_ROLLER_V] = (uint8_t)( ( datoRead & 0x000F0000U ) >> 16U );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_SAEJ_FNR, &datoRead ) )
    {    
        saej_FNR.enabled = ( datoRead & 0x000000FFU ) != 0u;
        saej_FNR.inputF = (uint8_t)( datoRead >> 8U );
        saej_FNR.inputR = (uint8_t)( datoRead >> 16U );
        saej_FNR.outputN = (uint8_t)( datoRead >> 24U );
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_WST_BUTTON_INDEX_1_8, &datoRead ) )
    {
        saej_wstButtonIndex_1_8 = datoRead;
    }
    if ( 0U == MemData_GetRAMItem( ADDR_WST_BUTTON_INDEX_9_15, &datoRead ) )
    {
        saej_wstButtonIndex_9_15 = datoRead;
    }    
    
    if ( 0U == MemData_GetRAMItem( ADDR_SERIAL_NUM_0_3, &datoRead ) )
    {
        Saej_dutSerialNum[0U] = (uint8_t)( datoRead );
        Saej_dutSerialNum[1U] = (uint8_t)( datoRead >>8U );
        Saej_dutSerialNum[2U] = (uint8_t)( datoRead >>16U );
        Saej_dutSerialNum[3U] = (uint8_t)( datoRead >>24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_SERIAL_NUM_4_7, &datoRead ) )
    {
        Saej_dutSerialNum[4U] = (uint8_t)( datoRead );
        Saej_dutSerialNum[5U] = (uint8_t)( datoRead >>8U );
        Saej_dutSerialNum[6U] = (uint8_t)( datoRead >>16U );
        Saej_dutSerialNum[7U] = (uint8_t)( datoRead >>24U );
    }
    
    
    uint16_t aux;
    uint16_t auxTest;
    
    memset( Saej_dutSerialNumToShow, 0x00u, 10U );
    i = 0u;
    auxTest = (uint16_t)( Saej_dutSerialNum[0u] );
    aux = auxTest;
    while ( ( auxTest > 0u ) && ( i < 10u ) )
    {
        auxTest = aux / 10u;
        Saej_dutSerialNumToShow[i] = aux  - ( auxTest * 10u );
        i = i + 1u;
        aux = auxTest;
    }    
    auxTest = (uint16_t)( Saej_dutSerialNum[4u] ) + (uint16_t)( (uint16_t)( Saej_dutSerialNum[5] ) << 8u );
    aux = auxTest;
    while ( ( auxTest > 0u ) && ( i < 10u ) )
    {
        auxTest = aux / 10u;
        Saej_dutSerialNumToShow[i] = aux  - ( auxTest * 10u );
        i = i + 1u;
        aux = auxTest;
    }
    auxTest = (uint16_t)( Saej_dutSerialNum[6u] ) + (uint16_t)( (uint16_t)( Saej_dutSerialNum[7] ) << 8u );
    aux = auxTest;
    while ( ( auxTest > 0u ) && ( i < 10u ) )
    {
        auxTest = aux / 10u;
        Saej_dutSerialNumToShow[i] = aux  - ( auxTest * 10u );
        i = i + 1u;
        aux = auxTest;
    }    
        
    if ( 0U == MemData_GetRAMItem( ADDR_ASCII_DUT_CODE_0_3, &datoRead ) )
    {
        Saej_dutAsciiCode[0U] = (uint8_t)( datoRead );
        Saej_dutAsciiCode[1U] = (uint8_t)( datoRead >>8U );
        Saej_dutAsciiCode[2U] = (uint8_t)( datoRead >>16U );
        Saej_dutAsciiCode[3U] = (uint8_t)( datoRead >>24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_ASCII_DUT_CODE_4_7, &datoRead ) )
    {
        Saej_dutAsciiCode[4U] = (uint8_t)( datoRead );
        Saej_dutAsciiCode[5U] = (uint8_t)( datoRead >>8U );
        Saej_dutAsciiCode[6U] = (uint8_t)( datoRead >>16U );
        Saej_dutAsciiCode[7U] = (uint8_t)( datoRead >>24U );
    }
    if ( 0U == MemData_GetRAMItem( ADDR_ASCII_DUT_CODE_8_11, &datoRead ) )
    {
        Saej_dutAsciiCode[8U] = (uint8_t)( datoRead );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_HW_ID_0_3, &datoRead ) )
    {
        Saej_dutHardwareId[0U] = (uint8_t)( datoRead );
        Saej_dutHardwareId[1U] = (uint8_t)( datoRead >>8U );
        Saej_dutHardwareId[2U] = (uint8_t)( datoRead >>16U );
        Saej_dutHardwareId[3U] = (uint8_t)( datoRead >>24U );
    }

    if ( 0U == MemData_GetRAMItem( ADDR_HW_ID_4_7, &datoRead ) )
    {
        Saej_dutHardwareId[4U] = (uint8_t)( datoRead );
        Saej_dutHardwareId[5U] = (uint8_t)( datoRead >>8U );
        Saej_dutManufacturerCode = (uint16_t)( datoRead >> 16U );
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_ASCII_MANUFACTURER_CODE_0_3, &datoRead ) )
    {
        Saej_dutAsciiManufacturerCode[0U] = (uint8_t)( datoRead );
        Saej_dutAsciiManufacturerCode[1U] = (uint8_t)( datoRead >>8U );
        Saej_dutAsciiManufacturerCode[2U] = (uint8_t)( datoRead >> 16U );
        Saej_dutAsciiManufacturerCode[3U] = (uint8_t)( datoRead >> 24U );
    }
    
    if ( 0U == MemData_GetRAMItem( ADDR_ASCII_MANUFACTURER_CODE_4_7, &datoRead ) )
    {
        Saej_dutAsciiManufacturerCode[4U] = (uint8_t)( datoRead );
        Saej_dutAsciiManufacturerCode[5U] = (uint8_t)( datoRead >>8U );
        Saej_dutAsciiManufacturerCode[6U] = (uint8_t)( datoRead >> 16U );
    }
    
 #endif    
    
    return;
}

//------------------------------------------------------------------------------
static Saej_claimResult Saej_CheckAddressArbitration( void )
{
    Saej_claimResult retVal;
    
    retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 7U ) < (uint8_t)( Saej_isoName[1U] >> 24U ) )
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 7U ) > (uint8_t)( Saej_isoName[1U] >> 24U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 6U ) < (uint8_t)( Saej_isoName[1U] >> 16U ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 6U ) > (uint8_t)( Saej_isoName[1U] >> 16U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    } 
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 5U ) < (uint8_t)( Saej_isoName[1U] >> 8U ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 5U ) > (uint8_t)( Saej_isoName[1U] >> 8U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }    
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 4U ) < (uint8_t)( Saej_isoName[1U] ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 4U ) > (uint8_t)( Saej_isoName[1U] ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }    
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 3U ) < (uint8_t)( Saej_isoName[0U] >> 24U ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 3U ) > (uint8_t)( Saej_isoName[0U] >> 24U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }    
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 2U ) < (uint8_t)( Saej_isoName[0U] >> 16U ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 2U ) > (uint8_t)( Saej_isoName[0U] >> 16U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }    
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 1U ) < (uint8_t)( Saej_isoName[0U] >> 8U ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 1U ) > (uint8_t)( Saej_isoName[0U] >> 8U ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }    
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 0U ) < (uint8_t)( Saej_isoName[0U] ) ) 
    {
        retVal = SAEJ_ADDRESS_NOT_CLAIMED;
    }
    else if ( CAN_StoredMsgGetData( CAN_MSG_SAEJ_CONTENDING_ADDRESS, 0U, 0U ) > (uint8_t)( Saej_isoName[0U] ) )
    {
        retVal = SAEJ_ADDRESS_CLAIMED;
    }
    else 
    {
      // Do Nothing
    }
  
  //  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].NewMsg == 0x00 ){return(CAN_NO_MSG);}
//  RXMsg[MSG_ACL_RX_ECU][RxIndex].NewMsg = 0x00;
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[7u]< IsoName[7u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[7u]> IsoName[7u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[6u]< IsoName[6u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[6u]> IsoName[6u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[5u]< IsoName[5u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[5u]> IsoName[5u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[4u]< IsoName[4u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[4u]> IsoName[4u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[3u]< IsoName[3u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[3u]> IsoName[3u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[2u]< IsoName[2u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[2u]> IsoName[2u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[1u]< IsoName[1u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[1u]> IsoName[1u] ){return(I_WIN);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[0u]< IsoName[0u] ){return(I_LOSE);}
//  if ( RXMsg[MSG_ACL_RX_ECU][RxIndex].Data.BYTE[0u]> IsoName[0u] ){return(I_WIN);}
//  return(I_LOSE);

    return retVal;
}

//------------------------------------------------------------------------------
static void Saej_TxRequestAddressClaim( void )
{
    uint8_t i;
    
    CANMsgTx_Saej_RequestAddressClaim.Data[0U] = 0x00U;
    CANMsgTx_Saej_RequestAddressClaim.Data[1U] = 0xEEU;
    CANMsgTx_Saej_RequestAddressClaim.Data[2U] = 0x00U;
    CAN_MsgUpdateID( &CANMsgTx_Saej_RequestAddressClaim, ( SAEJ_CAN_MSG_ID_REQUEST_ADDRESS_CLAIM + ( (uint32_t)( Saej_address ) * 256UL ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    //CAN_MsgUpdateID( &CANMsgRx_Saej_ContendingAddress, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_CONTENDING_ADDRESS, i, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_SOURCE_ADDRESS_CHECK_YES );
    }
    CAN_MsgUpdateData( &CANMsgTx_Saej_RequestAddressClaim, &CANMsgTx_Saej_RequestAddressClaim.Data[0u], CAN_MSG_TX_YES );  
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_TxAddressClaim( Saej_claimResult claimResult )
{
    uint8_t i;
  
    if ( SAEJ_ADDRESS_CLAIMED == claimResult )
    {
        CAN_MsgUpdateID( &CANMsgTx_Saej_AddressClaim, ( SAEJ_CAN_MSG_ID_ADDRESS_CLAIM + (uint32_t)( Saej_address ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        //CAN_MsgUpdateID( &CANMsgRx_Saej_ContendingAddress, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
        {
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_CONTENDING_ADDRESS, i, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_SOURCE_ADDRESS_CHECK_YES );
        }
        CAN_MsgUpdateData( &CANMsgTx_Saej_AddressClaim, (uint8_t *)( &Saej_isoName[0u] ), CAN_MSG_TX_YES );  
    }
    else //if ( SAEJ_ADDRESS_NOT_CLAIMED == claimResult )
    {
        CAN_MsgUpdateID( &CANMsgTx_Saej_AddressClaim, ( SAEJ_CAN_MSG_ID_ADDRESS_CLAIM + (uint32_t)( 0xFEUL ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CAN_MsgUpdateData( &CANMsgTx_Saej_AddressClaim, (uint8_t *)( &Saej_isoName[0u] ), CAN_MSG_TX_YES );  
    }
//    else 
//    {
//      // Do Nothing
//    }
    
    return;
}

//------------------------------------------------------------------------------
//static void Saej_BuildDM1Msg( void )
//{
//  //Aux = 0x00;
//  if (ErrorNum > 0x01 ){
// // Se ho piu di un errore rispondo occupato, sto gi' trasmetterndo il TP
//    TXMsg[MSG_TX_REQ_PGN].ID = 0x18E8FF00+CANSaECU; TXMsg[MSG_TX_REQ_PGN].DLC = 0x08;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[0u] = 0x03; //  Busy
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[1u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[2u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[3u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[4u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[5u] = 0xCA;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[6u] = 0xFE;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[7u] = 0x00;
//    TXMsg[MSG_TX_REQ_PGN].NewMsg = CAN_NEW_MSG;
// }else{ // Ho un errore o Nessuno - Trasmetto il DM1
//    //Aux = 0x00;
//    //if (ErrorNum > 0x00){Aux = 0x04;} // Amber Warning Lamp ON
//    TXMsg[MSG_TX_DM1].ID = 0x18FECA00+CANSaECU; TXMsg[MSG_TX_DM1].DLC = 0x08;
//
//    TXMsg[MSG_TX_DM1].Data.BYTE[0u] = 0xFF; //Nell'ISOBUS l'informazione sulla lampada non viene usata //Aux; // 00 NO Error 4 Error
//    TXMsg[MSG_TX_DM1].Data.BYTE[1u] = 0xFF;
//    TXMsg[MSG_TX_DM1].Data.BYTE[2u] = SpnLow;  // SNP L - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[3u] = SpnHigh; // SNP H - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[4u] = Fmi;     // FMI   - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[5u] = DTCCounter; // Counter 0 - 0x7F - Valutare se implementare, non è necessario per la compliance con la norma
//    TXMsg[MSG_TX_DM1].Data.BYTE[6u] = 0xFF;
//    TXMsg[MSG_TX_DM1].Data.BYTE[7u] = 0xFF;
//    TXMsg[MSG_TX_DM1].NewMsg = CAN_NEW_MSG; //Commento per non trasmettere il msg
//    return;
// }  
//  
//  
//    CAN_MsgUpdateData( &CANMsgTx_Saej_DM1, &CANMsgTx_Saej_DM1.Data[0U], CAN_MSG_TX_YES );
//  
//    return;
//}

//------------------------------------------------------------------------------
static void Saej_StatusExecWaitForAddressClaim( void ) 
{
    if ( Saej_status.value != Saej_status.previous )  // On Enter
    { 
        Saej_status.previous = Saej_status.value;
        Saej_status.cnt = 0U;
        Saej_status.cntTh = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
        Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM;

        if ( 0U == Saej_addressClaimedNum )
        {
            Saej_address = Saej_defaultAddress;
//            CAN_MsgUpdateID( &CANMsgTx_Saej_RequestAddressClaim, ( SAEJ_CAN_MSG_ID_REQUEST_ADDRESS_CLAIM + ( (uint32_t)( Saej_address ) * 256UL ) ), CAN_ID_29BIT );
//            CAN_MsgUpdateID( &CANMsgRx_Saej_ContendingAddress, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_ID_29BIT );
//            CAN_MsgUpdateData( &CANMsgTx_Saej_RequestAddressClaim, &CANMsgTx_Saej_RequestAddressClaim.Data[0u], CAN_MSG_TX_YES );
            Saej_TxRequestAddressClaim();
            if ( SAEJ_IS_SELF_CONFIGURABLE == Saej_isSelfConfigurableDevice )
            {
                Saej_addressClaimedNum++;
            }
        } 
        else 
        {
            if ( 1U == Saej_addressClaimedNum )
            {
                Saej_address = SAEJ_MIN_FREE_SOURCE_ADDRESS;
            }
            else 
            {
                Saej_address++;
            }            
          
          
            if ( Saej_address < SAEJ_MAX_FREE_SOURCE_ADDRESS ) 
            {
//                CAN_MsgUpdateID( &CANMsgTx_Saej_RequestAddressClaim, ( SAEJ_CAN_MSG_ID_REQUEST_ADDRESS_CLAIM + ( (uint32_t)( Saej_address ) * 256UL ) ), CAN_ID_29BIT );
//                CAN_MsgUpdateID( &CANMsgRx_Saej_ContendingAddress, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_ID_29BIT );
//                CAN_MsgUpdateData( &CANMsgTx_Saej_RequestAddressClaim, &CANMsgTx_Saej_RequestAddressClaim.Data[0u], CAN_MSG_TX_YES );
                Saej_TxRequestAddressClaim();
                Saej_addressClaimedNum++;
            }
            else 
            {
                Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
                Saej_status.value = SAEJ_STATUS_CANNOT_CLAIM;
	        Saej_addressClaimResult = SAEJ_ADDRESS_NOT_CLAIMED;
            }
        }   
    }
    else   // On Running
    {   
        ++Saej_status.cnt;
        if ( Saej_status.cnt >= Saej_status.cntTh )  
        { 
            // Dopo 250 ms vado in stato normale
            if( SAEJ_ADDRESS_TO_CLAIM == Saej_addressClaimResult )
            { 
                Saej_addressClaimResult = SAEJ_ADDRESS_CLAIMED; 
            }
            if( SAEJ_ADDRESS_CLAIMED == Saej_addressClaimResult )
            { 
                Saej_TxAddressClaim( SAEJ_ADDRESS_CLAIMED );
                Saej_status.value = SAEJ_STATUS_WAIT_TO_NORMAL;
                Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM;
            }
            if( SAEJ_ADDRESS_NOT_CLAIMED == Saej_addressClaimResult )
            {
                if ( SAEJ_IS_SELF_CONFIGURABLE == Saej_isSelfConfigurableDevice )
                {
                    Saej_status.previous--;
                }
                else
                {
                    Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
                    Saej_status.value = SAEJ_STATUS_CANNOT_CLAIM;
	            Saej_addressClaimResult = SAEJ_ADDRESS_NOT_CLAIMED;
                }
            }
      
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_StatusExecWaitToNormal( void ) 
{
    if ( Saej_status.value != Saej_status.previous )  // On Enter
    { 
        Saej_status.previous = Saej_status.value;
        Saej_status.cnt = 0U;
        Saej_status.cntTh = SAEJ_STATUS_WAIT_TO_NORMAL_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
    }
    else  // On Running
    { 
        if ( SAEJ_ADDRESS_CLAIMED == Saej_addressClaimResult )
        {
            Saej_TxAddressClaim( SAEJ_ADDRESS_CLAIMED );
            Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM;
        }
        else if ( SAEJ_ADDRESS_NOT_CLAIMED == Saej_addressClaimResult )
        {
            Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM; 
            Saej_status.value = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM;
        }
        else
        {
            ++Saej_status.cnt;
            if ( Saej_status.cnt >= Saej_status.cntTh )
            {
                // Dopo 250 ms vado in stato normale
                Saej_status.value = SAEJ_STATUS_NORMAL;
                CAN_MsgUpdateID( &CANMsgTx_Saej_DM1, ( SAEJ_CAN_MSG_ID_DM1 + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
            }
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_StatusExecNormal( void ) 
{
    uint8_t i;
    uint8_t j;
    uint8_t aux;
    static uint32_t joyRefreshCnt = 0U;
    static bool refreshPeriodElapsed = false;
    
  
    if ( Saej_status.value != Saej_status.previous ) // On Enter
    {     
        Saej_status.previous = Saej_status.value;
        CAN_MsgUpdateID( &CANMsgTx_Saej_BJM, ( SAEJ_CAN_MSG_ID_BJM + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
        CAN_MsgUpdateID( &CANMsgTx_Saej_EJM, ( SAEJ_CAN_MSG_ID_EJM + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
        CAN_MsgUpdateID( &CANMsgTx_Saej_EJM2, ( SAEJ_CAN_MSG_ID_EJM2 + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
        CAN_MsgUpdateID( &CANMsgTx_Saej_DM1, ( SAEJ_CAN_MSG_ID_DM1 + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );    
        //CAN_MsgUpdateID( &CANMsgRx_Saej_DM14, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF00FFUL ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );
        //CAN_MsgUpdateID( &CANMsgRx_Saej_RequestPGN, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFF00FF ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_MsgUpdateID( &CANMsgTx_Saej_RequestPGN, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFFFF00U ) + (uint32_t)( Saej_address ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( SAEJ_CAN_MSG_ID_SEND_PGN + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );        
        CAN_MsgUpdateIDGeneral( ( (uint32_t)( Saej_address << 8U ) & 0x0000FF00UL ), CAN_ID_29BIT );
        //CAN_MsgUpdateID( &CANMsgRx_Saej_TP, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FF ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
        //CAN_MsgUpdateID( &CANMsgRx_Saej_ETP, ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF00FF ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
        //CAN_MsgUpdateID( &CANMsgRx_Saej_TP_DT, ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFF00FF ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
        //CAN_MsgUpdateIDGeneral( (uint32_t)( Saej_address << 8U ), CAN_ID_29BIT );
        for ( j=0U; j<SAEJ_TP_SLOT_NUM_MAX; j++ )
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_TP[j], ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFFFF00U ) + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
            CAN_MsgUpdateID( &CANMsgTx_Saej_TP_DT[j], ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFFFF00U ) + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_MsgUpdateID( &CANMsgTx_Saej_ETP[j], ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFFFF00U ) + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
            CAN_MsgUpdateID( &CANMsgTx_Saej_ETP_DT[j], ( ( SAEJ_CAN_MSG_ID_ETP_DT & 0xFFFFFF00U ) + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );          
        }
        //CAN_MsgUpdateID( &CANMsgRx_Saej_Led, ( ( SAEJ_CAN_MSG_ID_LED & 0xFFFF00FFUL ) + (uint32_t)( Saej_address * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );                                             
        for ( j=0U; j<CAN_RX_BUFFER_NUM_MAX; j++ )
        {
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_DM14,         j, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF00FFUL ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_REQUEST_PGN,  j, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFF00FFU ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP,           j, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FFU ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_ETP,          j, ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF00FFU ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_DT,        j, ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFF00FFU ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
            CAN_StoredMsgUpdate( CAN_MSG_SAEJ_LED,          j, ( ( SAEJ_CAN_MSG_ID_LED & 0xFFFF00FFU ) + ( (uint32_t) Saej_address * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );  
        }
#ifdef ISOBUS_DJW        
        Isobus_UpdateMsgSourceAddress( Saej_address );
#endif        
        CAN_MsgUpdateRxFilter();
    }
    else // On Running 
    { 
        if ( SAEJ_ADDRESS_CLAIMED == Saej_addressClaimResult )
        {
            Saej_TxAddressClaim( SAEJ_ADDRESS_CLAIMED); 
            Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM; 
        }
        else if( SAEJ_ADDRESS_NOT_CLAIMED == Saej_addressClaimResult )
        {
            Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM;
            if ( SAEJ_IS_SELF_CONFIGURABLE == Saej_isSelfConfigurableDevice )
            {
                Saej_status.value = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM;
            }
            else
            {
                Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
                Saej_status.value = SAEJ_STATUS_CANNOT_CLAIM;
	        Saej_addressClaimResult = SAEJ_ADDRESS_NOT_CLAIMED;
            }            
        }
        else
        {
            Saej_AxisUpdate();
            Saej_PushButtonUpdate();
            Saej_LedCmdUpdate();
#ifdef ISOBUS_DJW            
            Isobus_Main();

            if ( true == IsobusTestEnableGet() ) 
            {
                if ( true == Saej_txOnChange )
                {
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );            
                }
                else
                {
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
                    CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
                }
                Saej_ledCmdEn = 0x0FU;
            } else {
                CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, 0U );
                CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, 0U );
                CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, 0U );        
                Saej_ledCmdEn = 0x00U;   
                Saej_ledCmd.en = false;
            }            
#endif                
            ++joyRefreshCnt;
            if ( joyRefreshCnt >= (Saej_msgJoyOnChangeRefreshTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) )
            {
                refreshPeriodElapsed = true;
                joyRefreshCnt = 0U;
            }
            
            if ( true == Saej_txOnChange )
            {
                aux = 0U;
                for ( i=SAEJ_AXIS_X; i<=SAEJ_AXIS_Y; i++ )
                {
                    if ( true == Saej_axis[i].changed )
                    {
                        aux++;
                    }
                }
                if ( ( aux > 0U ) && ( true == refreshPeriodElapsed ) )
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_BJM, &Saej_bufferTxBJM[0u], CAN_MSG_TX_YES );  
                }
                else
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_BJM, &Saej_bufferTxBJM[0u], CAN_MSG_TX_NO );
                }
            }
            else
            {
                CAN_MsgUpdateData( &CANMsgTx_Saej_BJM, &Saej_bufferTxBJM[0u], CAN_MSG_TX_NO );
            }
            CAN_MsgTx( &CANMsgTx_Saej_BJM, false, 0xFF );
    
            if ( ( SAEJ_AXIS_Z_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_Z_ENABLED ) ) 
              || ( SAEJ_AXIS_W_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_W_ENABLED ) )  
              || ( SAEJ_AXIS_J_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_J_ENABLED ) ) 
              || ( SAEJ_P_ENABLE == Saej_pushButtonEn[12U] ) )
            {
                if ( true == Saej_txOnChange )
                {
                    aux = 0U;
                    for ( i=SAEJ_AXIS_W; i<=SAEJ_AXIS_J; i++ )
                    {
                        if ( true == Saej_axis[i].changed )
                        {
                            aux++;
                        }
                    }
                    if ( ( aux > 0U ) && ( true == refreshPeriodElapsed ) )
                    {
                        CAN_MsgUpdateData( &CANMsgTx_Saej_EJM, &Saej_bufferTxEJM[0u], CAN_MSG_TX_YES );
                    }
                    else
                    {
                        CAN_MsgUpdateData( &CANMsgTx_Saej_EJM, &Saej_bufferTxEJM[0u], CAN_MSG_TX_NO );
                    }
                }
                else
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_EJM, &Saej_bufferTxEJM[0u], CAN_MSG_TX_NO );
                }
                CAN_MsgTx( &CANMsgTx_Saej_EJM, false, 0xFF );
            }

            if ( ( SAEJ_AXIS_Q_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_Q_ENABLED ) ) 
              || ( SAEJ_AXIS_V_ENABLED == ( Saej_cfgAxes & SAEJ_AXIS_V_ENABLED ) ) ) 
            {
                if ( true == Saej_txOnChange )
                {
                    aux = 0U;
                    for ( i=SAEJ_AXIS_Q; i<=SAEJ_AXIS_V; i++ )
                    {
                        if ( true == Saej_axis[i].changed )
                        {
                            aux++;
                        }
                    }
                    if ( ( aux > 0U ) && ( true == refreshPeriodElapsed ) )
                    {              
                        CAN_MsgUpdateData( &CANMsgTx_Saej_EJM2, &Saej_bufferTxEJM2[0u], CAN_MSG_TX_YES );  
                    }
                    else
                    {
                        CAN_MsgUpdateData( &CANMsgTx_Saej_EJM2, &Saej_bufferTxEJM2[0u], CAN_MSG_TX_NO );
                    }
                }
                else
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_EJM2, &Saej_bufferTxEJM2[0u], CAN_MSG_TX_NO );
                }
                CAN_MsgTx( &CANMsgTx_Saej_EJM2, false, 0xFF );
            }            
            
            refreshPeriodElapsed = false;
        }
        
    }
    
//    if ( ( Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) & SUPPLY_VBB_PROT_ERR_CAN_OFF ) == SUPPLY_VBB_PROT_ERR_CAN_OFF )
//    {
//#ifndef DP_DEBUG_EN
//        TLE_WDTRequestStopSet();
//#endif      
//        CAN_SetInitBit( CAN_MODULE_0 );
//        Saej_status.value = SAEJ_STATUS_OFF;
//    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_StatusExecWaitCanNotClaim( void ) 
{
    if ( Saej_status.value != Saej_status.previous )  // On Enter
    { 
        Saej_status.previous = Saej_status.value;
    }
    else  // On Running
    { 
        if ( SAEJ_ADDRESS_TO_CLAIM != Saej_addressClaimResult )
        {
            Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
            Saej_status.value = SAEJ_STATUS_CANNOT_CLAIM;
        }
    }

    return;
}

//------------------------------------------------------------------------------
static void Saej_StatusExecCanNotClaim( void )
{
    if ( Saej_status.value != Saej_status.previous )  // On Enter
    { 
        Saej_status.previous = Saej_status.value;
//        S32K_ErrorUpdate( ERR_NETWORK, WRES_ERROR_NOT_READY );
    }
    else  // On Running
    {
    }
   
    return;
}

//------------------------------------------------------------------------------
static void Saej_StatusExecOff( void ) 
{
    if ( Saej_status.value != Saej_status.previous )  // On Enter
    { 
        Saej_status.previous = Saej_status.value;
    }
    else  // On Running
    { 
        if ( !( ( Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) & SUPPLY_VBB_PROT_ERR_CAN_OFF ) == SUPPLY_VBB_PROT_ERR_CAN_OFF ) )
        {
            CAN_ResetInitBit( CAN_MODULE_0 );
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Saej_addressClaimedNum = 0U;
            Saej_status.value = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM;
        }
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_RequestPGNBuildTxMsg( s_TxRxCANMsg msg )
{
  static const uint8_t Saej_EcuIdBuff[47U] = 
  {
    ASC_DUT_CODE_1, ASC_DUT_CODE_2, ASC_DUT_CODE_3, ASC_DUT_CODE_4, ASC_DUT_CODE_5, ASC_DUT_CODE_6, ASC_DUT_CODE_7,
    ASC_DUT_CODE_8, ASC_DUT_CODE_9, ASCII_CH_LIMIT, SERIAL_NUM_0  , SERIAL_NUM_1  , SERIAL_NUM_2  , SERIAL_NUM_3  ,
    SERIAL_NUM_5  , SERIAL_NUM_4  , SERIAL_NUM_7  , SERIAL_NUM_6  , SERIAL_NUM_8  , SERIAL_NUM_9  , ASCII_CH_LIMIT, 
    ECU_LOCATION_1, ECU_LOCATION_2, ECU_LOCATION_3, ECU_LOCATION_4, ECU_LOCATION_5, ECU_LOCATION_6, ASCII_CH_LIMIT, 
    ECU_TYPE_00   , ECU_TYPE_01   , ECU_TYPE_02   , ASCII_CH_LIMIT, 
    ASC_MANUFACTURER_CODE_1, ASC_MANUFACTURER_CODE_2, ASC_MANUFACTURER_CODE_3, ASC_MANUFACTURER_CODE_4, 
    ASC_MANUFACTURER_CODE_5, ASC_MANUFACTURER_CODE_6, ASC_MANUFACTURER_CODE_7, ASCII_CH_LIMIT, 
    HWID_0, HWID_1, HWID_2, HWID_3, HWID_4, HWID_5, ASCII_CH_LIMIT,
  };
  static const uint8_t Saej_ProductIdBuff[23U] = 
  {  
    SERIAL_NUM_0  , SERIAL_NUM_1  , SERIAL_NUM_2  , SERIAL_NUM_3  , SERIAL_NUM_5  , SERIAL_NUM_4  , SERIAL_NUM_7  ,
    SERIAL_NUM_6  , SERIAL_NUM_8  , SERIAL_NUM_9  , ASCII_CH_LIMIT, 
    ASC_MANUFACTURER_CODE_1, ASC_MANUFACTURER_CODE_2, ASC_MANUFACTURER_CODE_3, ASC_MANUFACTURER_CODE_4, ASC_MANUFACTURER_CODE_5,
    ASC_MANUFACTURER_CODE_6, ASC_MANUFACTURER_CODE_7, ASCII_CH_LIMIT, 
    ASC_DEV_TYPE_CODE_1, ASC_DEV_TYPE_CODE_2, ASC_DEV_TYPE_CODE_3, ASCII_CH_LIMIT,
  };
  
    uint32_t rxPGN = (uint32_t)( (uint32_t)msg.Data[0U] ) + 
                     (uint32_t)( (uint32_t)msg.Data[1U] << 8U ) +
                     (uint32_t)( (uint32_t)msg.Data[2U] << 16U );
    uint8_t msgDA = msg.SA;
    uint8_t i;
    bool isBroadCast;
    uint8_t tpIndex;
    static uint8_t Saej_tmpTPBuff[48U];
    
    if ( 0xFEU == msg.SA )
    {
        msgDA = 0xFFU;
    }
       
    switch ( rxPGN )
    {
    case 0x00EE00UL:  //address claim
        Saej_TxAddressClaim( SAEJ_ADDRESS_CLAIMED );  //ho già gestito nella RxMsgServe la condizione relativa al cannot claim
        break;
#ifdef DM1_ENABLED        
    case 0x00FECAUL:  //DM1
        if ( true == Saej_DM1reqPGN )
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFFFF00UL ) + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
            CANMsgTx_Saej_SendPGN.Data[0U] = 0x03U;   //1 identificativo
            CANMsgTx_Saej_SendPGN.Data[1U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[4U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[5U] = 0xCAU; // Asc(*) Fine identificativo
            CANMsgTx_Saej_SendPGN.Data[6U] = 0xFEU;
            CANMsgTx_Saej_SendPGN.Data[7U] = 0x00U;
            CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );          
        }
        else
        {
            Saej_DM1reqPGN = true;
            CAN_MsgUpdateData( &CANMsgTx_Saej_DM1, &CANMsgTx_Saej_DM1.Data[0U], CAN_MSG_TX_YES );    
        }
      
//  if (ErrorNum > 0x01 ){
// // Se ho piu di un errore rispondo occupato, sto gi' trasmetterndo il TP
//    TXMsg[MSG_TX_REQ_PGN].ID = 0x18E8FF00+CANSaECU; TXMsg[MSG_TX_REQ_PGN].DLC = 0x08;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[0u] = 0x03; //  Busy
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[1u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[2u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[3u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[4u] = 0xFF;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[5u] = 0xCA;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[6u] = 0xFE;
//    TXMsg[MSG_TX_REQ_PGN].Data.BYTE[7u] = 0x00;
//    TXMsg[MSG_TX_REQ_PGN].NewMsg = CAN_NEW_MSG;
// }else{ // Ho un errore o Nessuno - Trasmetto il DM1
//    //Aux = 0x00;
//    //if (ErrorNum > 0x00){Aux = 0x04;} // Amber Warning Lamp ON
//    TXMsg[MSG_TX_DM1].ID = 0x18FECA00+CANSaECU; TXMsg[MSG_TX_DM1].DLC = 0x08;
//
//    TXMsg[MSG_TX_DM1].Data.BYTE[0u] = 0xFF; //Nell'ISOBUS l'informazione sulla lampada non viene usata //Aux; // 00 NO Error 4 Error
//    TXMsg[MSG_TX_DM1].Data.BYTE[1u] = 0xFF;
//    TXMsg[MSG_TX_DM1].Data.BYTE[2u] = SpnLow;  // SNP L - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[3u] = SpnHigh; // SNP H - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[4u] = Fmi;     // FMI   - 0x00 se no errori attivi
//    TXMsg[MSG_TX_DM1].Data.BYTE[5u] = DTCCounter; // Counter 0 - 0x7F - Valutare se implementare, non è necessario per la compliance con la norma
//    TXMsg[MSG_TX_DM1].Data.BYTE[6u] = 0xFF;
//    TXMsg[MSG_TX_DM1].Data.BYTE[7u] = 0xFF;
//    TXMsg[MSG_TX_DM1].NewMsg = CAN_NEW_MSG; //Commento per non trasmettere il msg
//    return;
      
//        Saej_BuildDM1Msg(); 
        break;   
#endif        
    case 0x00FEDAUL:  //software ID
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( SAEJ_CAN_MSG_ID_SOFTWARE_ID + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CANMsgTx_Saej_SendPGN.Data[0U] = 0x01U;   //1 identificativo
        CANMsgTx_Saej_SendPGN.Data[1U] = SAEJ_CHR_SW_ID_D0;
        CANMsgTx_Saej_SendPGN.Data[2U] = SAEJ_CHR_SW_ID_D1;
        CANMsgTx_Saej_SendPGN.Data[3U] = SAEJ_CHR_SW_ID_D2;
        CANMsgTx_Saej_SendPGN.Data[4U] = SAEJ_CHR_SW_ID_D3;
        CANMsgTx_Saej_SendPGN.Data[5U] = SAEJ_CHR_SW_ID_D4;
        CANMsgTx_Saej_SendPGN.Data[6U] = SAEJ_CHR_SW_ID_D5; // Asc(*) Fine identificativo;
        CANMsgTx_Saej_SendPGN.Data[7U] = 0x2AU;
        CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );
        break;
    case 0x00FD32UL: //ECU Diagnostic Protocol
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( SAEJ_CAN_MSG_ID_ECU_DIAGNOSTIC + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CANMsgTx_Saej_SendPGN.Data[0U] = 0x01U;   
        CANMsgTx_Saej_SendPGN.Data[1U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[4U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[5U] = 0xFFU; // Asc(*) Fine identificativo
        CANMsgTx_Saej_SendPGN.Data[6U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[7U] = 0xFFU;
        CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );
        break;
    case 0x00FDC5UL: // ECU id OK
        if ( 0xFFU == msgDA )
        {
            isBroadCast = SAEJ_TP_IS_BROADCAST;
        }
        else
        {
            if ( 0xFFU == msg.DA )
            {
                isBroadCast = SAEJ_TP_IS_BROADCAST;
                msgDA = 0xFFU;
            }
            else
            {
                isBroadCast = SAEJ_TP_IS_NOT_BROADCAST;
            }
        }
        for ( i=0U; i<47U; i++ )
        {
            Saej_tmpTPBuff[i] = Saej_EcuIdBuff[i];
        }
        for ( i=0U; i<9U; i++ )
        {
            Saej_tmpTPBuff[i] = Saej_dutAsciiCode[i];
        }
//        for ( i=0U; i<4U; i++ )
//        {
//            Saej_tmpTPBuff[i+10U] = Saej_dutSerialNum[i];
//        }
//        Saej_tmpTPBuff[14U] = Saej_dutSerialNum[5U];
//        Saej_tmpTPBuff[15U] = Saej_dutSerialNum[4U];
//        Saej_tmpTPBuff[16U] = Saej_dutSerialNum[7U];
//        Saej_tmpTPBuff[17U] = Saej_dutSerialNum[6U];        
        for ( i=0U; i<10U; i++ )
        {
            Saej_tmpTPBuff[i+10U] = Saej_dutSerialNumToShow[9u-i];
        }        
        
        for ( i=0U; i<7U; i++ )
        {
            Saej_tmpTPBuff[i+32U] = Saej_dutAsciiManufacturerCode[i];
        }        
        for ( i=0U; i<6U; i++ )
        {
            Saej_tmpTPBuff[i+40U] = Saej_dutHardwareId[i];
        }        
        tpIndex = 0xFFU;
        for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
        {
            if ( WRES_OK == Saej_TPTxEnable( i, 0x00FDC5UL, isBroadCast, msgDA, 0x2FU, 0x07U, &Saej_tmpTPBuff[0U], &Saej_EcuIdBuff[0U] ) )
            {
                tpIndex = i;
                break;
            }
        }   
        if ( tpIndex == 0xFFU )
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + ( (uint32_t)msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
            CANMsgTx_Saej_SendPGN.Data[0U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[1U] = 0x03U;
            CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[4U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[5U] = 0xC5U;
            CANMsgTx_Saej_SendPGN.Data[6U] = 0xFDU;
            CANMsgTx_Saej_SendPGN.Data[7U] = 0x00U;
            CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0U], CAN_MSG_TX_YES );
        }      
        break;
    case 0x00FC8DUL: // Product identification
        if ( 0xFFU == msgDA )
        {
            isBroadCast = SAEJ_TP_IS_BROADCAST;
        }
        else
        {
            if ( 0xFFU == msg.DA )
            {
                isBroadCast = SAEJ_TP_IS_BROADCAST;
                msgDA = 0xFFU;
            }
            else
            {
                isBroadCast = SAEJ_TP_IS_NOT_BROADCAST;
            }
        }
        for ( i=0U; i<23U; i++ )
        {
            Saej_tmpTPBuff[i] = Saej_ProductIdBuff[i];
        }
//        for ( i=0U; i<4U; i++ )
//        {
//            Saej_tmpTPBuff[i] = Saej_dutSerialNum[i];
//        }
//        Saej_tmpTPBuff[4U] = Saej_dutSerialNum[5U];
//        Saej_tmpTPBuff[5U] = Saej_dutSerialNum[4U];
//        Saej_tmpTPBuff[6U] = Saej_dutSerialNum[7U];
//        Saej_tmpTPBuff[7U] = Saej_dutSerialNum[6U];        
        for ( i=0U; i<10U; i++ )
        {
            Saej_tmpTPBuff[i] = Saej_dutSerialNumToShow[9u-i];
        }        
        
        for ( i=0U; i<7U; i++ )
        {
            Saej_tmpTPBuff[i+11U] = Saej_dutAsciiManufacturerCode[i];
        }        
        tpIndex = 0xFFU;
        for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
        {
            if ( WRES_OK == Saej_TPTxEnable( i, 0x00FC8DUL, isBroadCast, msgDA, 0x17U, 0x04U, &Saej_tmpTPBuff[0U], &Saej_ProductIdBuff[0U] ) )
            {
                tpIndex = i;
                break;
            }
        }   
        if ( tpIndex == 0xFFU )
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + (uint32_t)( msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
            CANMsgTx_Saej_SendPGN.Data[0U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[1U] = 0x03U;
            CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[4U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[5U] = 0x8DU;
            CANMsgTx_Saej_SendPGN.Data[6U] = 0xFCU;
            CANMsgTx_Saej_SendPGN.Data[7U] = 0x00U;
            CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0U], CAN_MSG_TX_YES );
        }      
        break;        
#ifndef ISOBUS_DJW        
    case 0x009900UL:        
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + (uint32_t)( msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        if ( 0U != Saej_ledCmdEn )
        {
            CANMsgTx_Saej_SendPGN.Data[0U] = 0x00U; 
        }
        else
        {
            CANMsgTx_Saej_SendPGN.Data[0U] = 0x01U; 
        }
        CANMsgTx_Saej_SendPGN.Data[1U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU; 
        CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[4U] = 0XFFU;
        CANMsgTx_Saej_SendPGN.Data[5U] = msg.Data[0U];
        CANMsgTx_Saej_SendPGN.Data[6U] = msg.Data[1U];
        CANMsgTx_Saej_SendPGN.Data[7U] = msg.Data[2U];
        CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );        
        break;
#endif        
    case 0x00D800UL: //DM14
    case 0x00D900UL: //DM15
    case 0x00D700UL: //DM16
    case 0x00FED8UL:
    case 0x00EB00UL: //Data Transfer
    case 0x00EC00UL: //TP
    case 0x00C700UL: //ETP DT 
    case 0x00C800UL: //ETP
#ifdef ISOBUS_DJW      
    case 0x00E700: //ECU To VT Command      
#endif      
#ifndef ISOBUS_DJW      
    case 0x00FDD6UL:
    case 0x00FDD7UL:
#endif      
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + (uint32_t)( msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CANMsgTx_Saej_SendPGN.Data[0U] = 0x00U; 
        CANMsgTx_Saej_SendPGN.Data[1U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU; 
        CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
        CANMsgTx_Saej_SendPGN.Data[4U] = 0XFFU;
        CANMsgTx_Saej_SendPGN.Data[5U] = msg.Data[0U];
        CANMsgTx_Saej_SendPGN.Data[6U] = msg.Data[1U];
        CANMsgTx_Saej_SendPGN.Data[7U] = msg.Data[2U];
        CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );
        break;
#ifdef ISOBUS_DJW
    case 0x00FD42UL: //ISOBUS Certification Information  OK
        CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ISOBUS_CERT_INFO_ID + Saej_address ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        CANMsgTx_Saej_SendPGN.Data[0U] = Isobus_CertificationGet( 0U );   
        CANMsgTx_Saej_SendPGN.Data[1U] = Isobus_CertificationGet( 1U );
        CANMsgTx_Saej_SendPGN.Data[2U] = Isobus_CertificationGet( 2U );
        CANMsgTx_Saej_SendPGN.Data[3U] = Isobus_CertificationGet( 3U );
        CANMsgTx_Saej_SendPGN.Data[4U] = Isobus_CertificationGet( 4U );
        CANMsgTx_Saej_SendPGN.Data[5U] = Isobus_CertificationGet( 5U ); 
        CANMsgTx_Saej_SendPGN.Data[6U] = Isobus_CertificationGet( 6U );
        CANMsgTx_Saej_SendPGN.Data[7U] = Isobus_CertificationGet( 7U );
        CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );      
        break;        
    case 0x00FE0DUL:
        Isobus_WorkingSetInitMsgTx();
        break;
    case 0x00FC8EUL:
        if ( 0xFFU == msgDA )
        {
            isBroadCast = SAEJ_TP_IS_BROADCAST;
        }
        else
        {
            if ( 0xFFU == msg.DA )
            {
                isBroadCast = SAEJ_TP_IS_BROADCAST;
                msgDA = 0xFFU;
            }
            else
            {
                isBroadCast = SAEJ_TP_IS_NOT_BROADCAST;
            }
        }
        for ( i=0U; i<11U; i++ )
        {
            Saej_tmpTPBuff[i] = Isobus_FunctionalitiesBuff[i];
        }
        Saej_tmpTPBuff[11U] = (uint8_t)( Isobus_FunctionalitiesGet() );
        Saej_tmpTPBuff[12U] = (uint8_t)( Isobus_FunctionalitiesGet() >> 8U );
        tpIndex = 0xFFU;
        for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
        {
            if ( WRES_OK == Saej_TPTxEnable( i, 0x00FC8EUL, isBroadCast, msgDA, 0x0DU, 0x02U, &Saej_tmpTPBuff[0U], &Isobus_FunctionalitiesBuff[0U] ) )
            {
                tpIndex = i;
                break;
            }
        }   
        if ( tpIndex == 0xFFU )
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + (uint32_t)( msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
            CANMsgTx_Saej_SendPGN.Data[0U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[1U] = 0x03U;
            CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[4U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[5U] = 0x8EU;
            CANMsgTx_Saej_SendPGN.Data[6U] = 0xFCU;
            CANMsgTx_Saej_SendPGN.Data[7U] = 0x00U;
            CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0U], CAN_MSG_TX_YES );
        }    
        break;
#endif
    default: // PGN non supportato
	if ( msg.DA == Saej_address ) 
        {
            CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFF0000UL ) + Saej_address + (uint32_t)( msgDA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
            CANMsgTx_Saej_SendPGN.Data[0U] = 0x01U; 
            CANMsgTx_Saej_SendPGN.Data[1U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[2U] = 0xFFU; 
            CANMsgTx_Saej_SendPGN.Data[3U] = 0xFFU;
            CANMsgTx_Saej_SendPGN.Data[4U] = 0XFFU;
            CANMsgTx_Saej_SendPGN.Data[5U] = msg.Data[0U];
            CANMsgTx_Saej_SendPGN.Data[6U] = msg.Data[1U];
            CANMsgTx_Saej_SendPGN.Data[7U] = msg.Data[2U];
            CAN_MsgUpdateData( &CANMsgTx_Saej_SendPGN, &CANMsgTx_Saej_SendPGN.Data[0u], CAN_MSG_TX_YES );
        }
	break;        
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_TPRxInit( s_TxRxCANMsg msg, bool isBroadcast )
{
    Saej_TPRx.isBroadcast = isBroadcast;
    Saej_TPRx.byteNum = (uint32_t)( msg.Data[1U] ) + 
                        (uint32_t)( (uint32_t)msg.Data[2U] << 8U );
    Saej_TPRx.packetNum = msg.Data[3U];
    Saej_TPRx.PGN = (uint32_t)( (uint32_t)msg.Data[5U] ) + 
                    (uint32_t)( (uint32_t)msg.Data[6U] << 8U ) + 
                    (uint32_t)( (uint32_t)msg.Data[7U] << 16U );
    Saej_TPRx.packetCnt = 0U;
    Saej_TPRx.byteCnt = 0U;
    if ( true == isBroadcast )
    {
        Saej_TPRx.packetNumToSend = SAEJ_DEFAULT_TP_PACKET_NUM;
        Saej_TPRx.nextPacket = 1U;
//        CAN_MsgUpdateID( &CANMsgRx_Saej_TPAll, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFFFF00 ) + msg.SA ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );           
//        CAN_MsgUpdateID( &CANMsgRx_Saej_TPAll_DT, ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFFFF00 ) + msg.SA ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );           
//        CAN_MsgUpdateID( &CANMsgTx_Saej_TP, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FF ) + (uint32_t)( msg.SA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );        
    }   
    else
    {
          CAN_MsgUpdateID( &CANMsgTx_Saej_TP[0U], ( ( CANMsgTx_Saej_TP[0U].ID & 0xFFFF00FF ) + (uint32_t)( msg.SA * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO ); 
    }  

    return;
}

//------------------------------------------------------------------------------
static void Saej_TPRxData( s_TxRxCANMsg msg )
{
    uint8_t i;
    
    if ( ( Saej_TPRx.nextPacket == msg.Data[0U] ) && ( Saej_TPRx.nextPacket <= Saej_TPRx.packetNum ) )
    {
        for ( i=1U; i<8U; i++ )    
        {
            if ( Saej_TPRx.byteCnt < Saej_TPRx.byteNum )
            {
                Saej_tpRxBuff[Saej_TPRx.byteCnt] = msg.Data[i];
                Saej_TPRx.byteCnt++;
            }
        }
        Saej_TPRx.nextPacket++;
    }
    else
    {
        CANMsgTx_Saej_TP[0U].Data[0U] = 0x11U;
        CANMsgTx_Saej_TP[0U].Data[1U] = (uint8_t)(Saej_TPRx.packetNum - Saej_TPRx.nextPacket + 1U);
        CANMsgTx_Saej_TP[0U].Data[2U] = (uint8_t)Saej_TPRx.nextPacket;
        CANMsgTx_Saej_TP[0U].Data[3U] = 0xFFU;
        CANMsgTx_Saej_TP[0U].Data[4U] = 0xFFU;
        CANMsgTx_Saej_TP[0U].Data[5U] = (uint8_t)( Saej_TPRx.PGN );
        CANMsgTx_Saej_TP[0U].Data[6U] = (uint8_t)( Saej_TPRx.PGN >> 8U );
        CANMsgTx_Saej_TP[0U].Data[7U] = (uint8_t)( Saej_TPRx.PGN >> 16U );
        CAN_MsgUpdateData( &CANMsgTx_Saej_TP[0U], &CANMsgTx_Saej_TP[0U].Data[0U], CAN_MSG_TX_YES );        
    }    
    Saej_statusTPRx.cntTh = SAEJ_STATUS_TP_DT_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
    Saej_statusTPRx.cnt = 0U;
    
    return;
}

//------------------------------------------------------------------------------
static uint8_t Saej_CommandedAddressEval( void )
{
    uint8_t retVal;
    uint16_t aux;
    
    switch ( Saej_commandedAddressStatus )    
    {
    case SAEJ_COMMANDED_ADDRESS_STATUS_OFF:
        if ( ( ( (uint32_t)( (uint32_t)Saej_tpRxBuff[0U]        ) + 
                 (uint32_t)( (uint32_t)Saej_tpRxBuff[1U] << 8U  ) + 
                 (uint32_t)( (uint32_t)Saej_tpRxBuff[2U] << 16U ) + 
                 (uint32_t)( (uint32_t)Saej_tpRxBuff[3U] << 24U )  ) == Saej_isoName[0U] )
           &&  ( ( (uint32_t)( (uint32_t)Saej_tpRxBuff[4U]        ) + 
                   (uint32_t)( (uint32_t)Saej_tpRxBuff[5U] << 8U  ) + 
                   (uint32_t)( (uint32_t)Saej_tpRxBuff[6U] << 16U ) + 
                   (uint32_t)( (uint32_t)Saej_tpRxBuff[7U] << 24U )  ) == Saej_isoName[1U] ) )
        { 
            Saej_defaultAddress = Saej_tpRxBuff[8U];
//            aux = (uint16_t)( Saej_defaultAddress );
//            aux |= (uint16_t)( Saej_isSelfConfigurableDevice ) << 8U;
            aux = Application_ParamGet();
            aux = aux & 0xFFFFFF00UL;
            aux = aux | (uint32_t)( Saej_defaultAddress & 0x000000FFUL );
            Flash_InternalAccessSet( FLASH_INTERNAL_ACCESS_REQUEST_WRITE, (uint32_t)( ADDR_BOOT_SET_DATA_01 ), aux );
            Saej_commandedAddressStatus = SAEJ_COMMANDED_ADDRESS_STATUS_BUSY;
        }          
        else
        {
            Saej_commandedAddressStatus = SAEJ_COMMANDED_ADDRESS_STATUS_COMPLETED;
        }
        retVal = SAEJ_OPERATION_OFF;
        break;
    case SAEJ_COMMANDED_ADDRESS_STATUS_BUSY:
        if ( true == Flash_InternalAccessCompleted() )
        {
            Flash_InternalAccessReset();
            if ( Saej_defaultAddress != Saej_address )
            {
                Saej_addressClaimedNum = 0U;
                Saej_status.value = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM;
            }
            Saej_commandedAddressStatus = SAEJ_COMMANDED_ADDRESS_STATUS_COMPLETED;
        }    
        retVal = SAEJ_OPERATION_BUSY;
        break;
    case SAEJ_COMMANDED_ADDRESS_STATUS_COMPLETED:
        Saej_commandedAddressStatus = SAEJ_COMMANDED_ADDRESS_STATUS_OFF;
        retVal = SAEJ_OPERATION_COMPLETED;
        break;
    default:
        retVal = SAEJ_OPERATION_OFF;
        break;
    }
    
    return retVal;
}

////------------------------------------------------------------------------------
//static void Saej_tpBuffInit( uint8_t slotIndex, uint8_t *buffStart, uint16_t size )
//{
//    uint32_t i;
//    uint32_t aux;
//    
//    for ( i=0U; i<SAEJ_TP_BYTE_NUM_MAX; i++ )
//    {
//        //Saej_TPTx[slotIndex].Saej_tpTxBuff[i] = *(buffSource + aux + i );
//      Saej_TPTx[slotIndex].Saej_tpTxBuff[i] = *(Saej_TPTx[slotIndex].buffSource + aux + i);
//    }
//    
//    return;
//}

//------------------------------------------------------------------------------
static void Saej_tpBuffUpdate( uint8_t slotIndex, uint32_t buffCycleNum )
{
    uint32_t i;
    uint32_t aux;
    
    aux = buffCycleNum * SAEJ_TP_BYTE_NUM_MAX;
    for ( i=0U; i<SAEJ_TP_BYTE_NUM_MAX; i++ )
    {
        //Saej_TPTx[slotIndex].Saej_tpTxBuff[i] = *(buffSource + aux + i );
      Saej_TPTx[slotIndex].Saej_tpTxBuff[i] = *(Saej_TPTx[slotIndex].buffSource + aux + i);
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_TPRxMain( void )
{
    uint8_t i;
//    static uint16_t bufCount = 0U;
  
    if ( Saej_statusTPRx.cntTh > 0U )
    {
        ++Saej_statusTPRx.cnt;
        if ( Saej_statusTPRx.cnt >= Saej_statusTPRx.cntTh )
        {
            Saej_statusTPRx.value = SAEJ_STATUS_TP_CM_ABORT;
        }
    }

    switch ( Saej_statusTPRx.value )
    {
    case SAEJ_STATUS_TP_OFF:
        Saej_statusTPRx.cnt = 0U;
        Saej_statusTPRx.cntTh = 0U;
        break;        
    case SAEJ_STATUS_TP_INIT:
        Saej_statusTPRx.previous = Saej_statusTPRx.value;      
        if ( true == Saej_TPRx.isBroadcast )
        {
            Saej_statusTPRx.cntTh = SAEJ_STATUS_TP_CM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
            Saej_statusTPRx.cnt = 0U;
            Saej_TPRx.packetCnt = 0U;
            Saej_TPRx.byteCnt = 0U;
            Saej_TPRx.nextPacket = 1U;            
            Saej_statusTPRx.value = SAEJ_STATUS_TP_DT;
        }        
        else
        {
            Saej_TPRx.packetNumToSend = (uint8_t)Saej_TPRx.packetNum;
            Saej_TPRx.packetCnt = 0U;
            Saej_TPRx.byteCnt = 0U;
            Saej_TPRx.nextPacket = 1U;
            Saej_statusTPRx.value = SAEJ_STATUS_TP_CM_CTS;
        }
        break;        
    case SAEJ_STATUS_TP_CM_CTS:
        Saej_statusTPRx.previous = Saej_statusTPRx.value;
        //invia il CTS
        CANMsgTx_Saej_TP[0U].Data[0U] = 0x11U;
        CANMsgTx_Saej_TP[0U].Data[1U] = Saej_TPRx.packetNumToSend;
        CANMsgTx_Saej_TP[0U].Data[2U] = (uint8_t)Saej_TPRx.nextPacket;
        CANMsgTx_Saej_TP[0U].Data[3U] = 0xFFU;
        CANMsgTx_Saej_TP[0U].Data[4U] = 0xFFU;
        CANMsgTx_Saej_TP[0U].Data[5U] = (uint8_t)( Saej_TPRx.PGN );
        CANMsgTx_Saej_TP[0U].Data[6U] = (uint8_t)( Saej_TPRx.PGN >> 8U );
        CANMsgTx_Saej_TP[0U].Data[7U] = (uint8_t)( Saej_TPRx.PGN >> 16U );
        CAN_MsgUpdateData( &CANMsgTx_Saej_TP[0U], &CANMsgTx_Saej_TP[0U].Data[0U], CAN_MSG_TX_YES );        
        Saej_statusTPRx.cntTh = SAEJ_STATUS_TP_CM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
        Saej_statusTPRx.cnt = 0U;
        Saej_statusTPRx.value = SAEJ_STATUS_TP_DT;
        break;
    case SAEJ_STATUS_TP_DT:
        if ( Saej_TPRx.nextPacket > Saej_TPRx.packetNum )
        {
            if ( true == Saej_TPRx.isBroadcast )
            {
                Saej_statusTPRx.value = SAEJ_STATUS_TP_EVAL;
            }
            else
            {
                Saej_statusTPRx.value = SAEJ_STATUS_TP_CM_EOM_ACK;
            }
        }
        break;        
    case SAEJ_STATUS_TP_CM_EOM_ACK:
        CANMsgTx_Saej_TP[0U].Data[0U] = 0x13U;
        CANMsgTx_Saej_TP[0U].Data[1U] = (uint8_t)( Saej_TPRx.byteNum );
        CANMsgTx_Saej_TP[0U].Data[2U] = (uint8_t)( Saej_TPRx.byteNum >> 8U );
        CANMsgTx_Saej_TP[0U].Data[3U] = (uint8_t)( Saej_TPRx.packetNum );
        CANMsgTx_Saej_TP[0U].Data[4U] = 0xFFU;
        CANMsgTx_Saej_TP[0U].Data[5U] = (uint8_t)( Saej_TPRx.PGN );
        CANMsgTx_Saej_TP[0U].Data[6U] = (uint8_t)( Saej_TPRx.PGN >> 8U );
        CANMsgTx_Saej_TP[0U].Data[7U] = (uint8_t)( Saej_TPRx.PGN >> 16U );
        CAN_MsgUpdateData( &CANMsgTx_Saej_TP[0U], &CANMsgTx_Saej_TP[0U].Data[0U], CAN_MSG_TX_YES );
        Saej_statusTPRx.value = SAEJ_STATUS_TP_EVAL;
//        bufCount = 1U;
        Saej_statusTPRx.cnt = 0U;
        Saej_statusTPRx.cntTh = 0U;        
        break;
    case SAEJ_STATUS_TP_CM_ABORT:
        if ( true != Saej_TPRx.isBroadcast )
        {
          memset( CANMsgTx_Saej_TP[0U].Data, 0xFF, 8U );
          CANMsgTx_Saej_TP[0U].Data[5U] = (uint8_t)( Saej_TPRx.PGN );
          CANMsgTx_Saej_TP[0U].Data[6U] = (uint8_t)( Saej_TPRx.PGN >> 8U );
          CANMsgTx_Saej_TP[0U].Data[7U] = (uint8_t)( Saej_TPRx.PGN >> 16U );          
          CAN_MsgUpdateData( &CANMsgTx_Saej_TP[0U], &CANMsgTx_Saej_TP[0U].Data[0U], CAN_MSG_TX_YES );
        }   
        Saej_statusTPRx.cnt = 0U;
        Saej_statusTPRx.cntTh = 0U;
        Saej_statusTPRx.value = SAEJ_STATUS_TP_COMPLETED;
        break;        
    case SAEJ_STATUS_TP_EVAL:
        if ( 0x00FED8U == Saej_TPRx.PGN ) //commanded address
        {
            if ( SAEJ_OPERATION_COMPLETED == Saej_CommandedAddressEval() )
            {
                Saej_statusTPRx.value = SAEJ_STATUS_TP_COMPLETED;
            }          
        }
        else
        {
            Saej_statusTPRx.value = SAEJ_STATUS_TP_COMPLETED;
        }
        break;
    case SAEJ_STATUS_TP_COMPLETED:
        Saej_statusTPRx.value = SAEJ_STATUS_TP_OFF;
        if ( true == Saej_TPRx.isBroadcast )
        {
           // CAN_MsgUpdateRxMask( &CANMsgRx_Saej_TPAll_DT, CAN_SOURCE_ADDRESS_CHECK_NO );
            for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
            {
                CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_DT_ALL,  i, CAN_StoredMsgGetID( CAN_MSG_SAEJ_TP_DT_ALL, i ), CAN_SOURCE_ADDRESS_CHECK_NO );
            }
        }        
        else
        {
            for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
            {
                CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_DT,  i, CAN_StoredMsgGetID( CAN_MSG_SAEJ_TP_DT, i ), CAN_SOURCE_ADDRESS_CHECK_NO );
            }
        }          
        break;
    default:
        break;
    }        
    return;
}

//------------------------------------------------------------------------------
static wres_t Saej_TPTxUpdate( uint8_t slotIndex, bool isDataOffset )
{
    wres_t retVal;
    uint32_t aux;
    uint32_t auxID;
    uint16_t j;
    uint8_t ctrlByte;
    static uint32_t bufferIndex[SAEJ_TP_SLOT_NUM_MAX];
    static uint32_t bufferCycleNum[SAEJ_TP_SLOT_NUM_MAX];

    retVal = WRES_ERROR_NOT_READY;
    
    if ( false == Saej_TPTx[slotIndex].isBroadcast )
    {
        auxID = (uint32_t)( (uint32_t)Saej_address ) + 
                (uint32_t)( (uint32_t)Saej_TPTx[slotIndex].destAddress << 8U );
        ctrlByte = SAEJ_TP_CTRL_BYTE_RTS;
    }
    else
    {
        auxID = (uint32_t)( Saej_address ) + 0x0000FF00UL;
        ctrlByte = SAEJ_TP_CTRL_BYTE_BAM;
    }
    
    if ( SAEJ_TP_ABORT_NULL != Saej_TPTx[slotIndex].abortValue )
    {
        if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )                         // è abilitato l'Extended TP
        {
            if ( CAN_NO_MSG == CANMsgTx_Saej_ETP[slotIndex].NewMsg )
            {
                CANMsgTx_Saej_ETP[slotIndex].Data[0U] = 0xFFU;
                CANMsgTx_Saej_ETP[slotIndex].Data[1U] = Saej_TPTx[slotIndex].abortValue;
                CANMsgTx_Saej_ETP[slotIndex].Data[2U] = 0xFFU;
                CANMsgTx_Saej_ETP[slotIndex].Data[3U] = 0xFFU;
                CANMsgTx_Saej_ETP[slotIndex].Data[4U] = 0xFFU;
                CANMsgTx_Saej_ETP[slotIndex].Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
                CANMsgTx_Saej_ETP[slotIndex].Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
                CANMsgTx_Saej_ETP[slotIndex].Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );
                CAN_MsgUpdateData( &CANMsgTx_Saej_ETP[slotIndex], &CANMsgTx_Saej_ETP[slotIndex].Data[0U], CAN_MSG_TX_YES );    
                Saej_TPTx[slotIndex].abortValue = SAEJ_TP_ABORT_NULL;
            }
            else
            {
                retVal = WRES_OPERATION_IN_PROGRESS;
            }
        }
        else 
        {
            if ( CAN_NO_MSG == CANMsgTx_Saej_ETP[slotIndex].NewMsg )
            {          
                CANMsgTx_Saej_TP[slotIndex].Data[0U] = 0xFFU;
                CANMsgTx_Saej_TP[slotIndex].Data[1U] = Saej_TPTx[slotIndex].abortValue;
                CANMsgTx_Saej_TP[slotIndex].Data[2U] = 0xFFU;
                CANMsgTx_Saej_TP[slotIndex].Data[3U] = 0xFFU;
                CANMsgTx_Saej_TP[slotIndex].Data[4U] = 0xFFU;
                CANMsgTx_Saej_TP[slotIndex].Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
                CANMsgTx_Saej_TP[slotIndex].Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
                CANMsgTx_Saej_TP[slotIndex].Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );
                CAN_MsgUpdateData( &CANMsgTx_Saej_TP[slotIndex], &CANMsgTx_Saej_TP[slotIndex].Data[0U], CAN_MSG_TX_YES );
                Saej_TPTx[slotIndex].abortValue = SAEJ_TP_ABORT_NULL;
            }
            else
            {
                retVal = WRES_OPERATION_IN_PROGRESS;
            }
        }
    } 
    else if ( 0x00U == Saej_TPTx[slotIndex].packetCnt ) 
    {
        // RTS
        bufferIndex[slotIndex] = 0U;
        bufferCycleNum[slotIndex] = 0U;
        //Saej_tpBuffUpdate( slotIndex, bufferCycleNum[slotIndex] );
        if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )                         // è abilitato l'Extended TP
        {
            if ( CAN_NO_MSG == CANMsgTx_Saej_ETP[slotIndex].NewMsg )
            {
                auxID = auxID + ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF0000UL );
                CANMsgTx_Saej_ETP[slotIndex].Data[0U] = SAEJ_ETP_CTRL_BYTE_RTS;
                CANMsgTx_Saej_ETP[slotIndex].Data[1U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum );             // n° byte di dati - byte 0
                CANMsgTx_Saej_ETP[slotIndex].Data[2U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum >> 8U );       // n° Byte di dati - byte 1
                CANMsgTx_Saej_ETP[slotIndex].Data[3U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum >> 16U );  // n° byte di dati - byte 2
                CANMsgTx_Saej_ETP[slotIndex].Data[4U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum >> 24U );  // n° byte di dati - byte 3
                CANMsgTx_Saej_ETP[slotIndex].Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
                CANMsgTx_Saej_ETP[slotIndex].Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
                CANMsgTx_Saej_ETP[slotIndex].Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );
                CAN_MsgUpdateID( &CANMsgTx_Saej_ETP[slotIndex], auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );                        
                CAN_MsgUpdateData( &CANMsgTx_Saej_ETP[slotIndex], &CANMsgTx_Saej_ETP[slotIndex].Data[0U], CAN_MSG_TX_YES );
            }
            else
            {
                retVal = WRES_OPERATION_IN_PROGRESS;
            }
        } 
        else 
        {
            if ( CAN_NO_MSG == CANMsgTx_Saej_TP[slotIndex].NewMsg )
            {
                auxID = auxID + ( SAEJ_CAN_MSG_ID_TP & 0xFFFF0000UL );
                CANMsgTx_Saej_TP[slotIndex].Data[0U] = ctrlByte;
                CANMsgTx_Saej_TP[slotIndex].Data[1U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum );             // n° byte di dati - byte 0
                CANMsgTx_Saej_TP[slotIndex].Data[2U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum >> 8U );       // n° Byte di dati - byte 1
                CANMsgTx_Saej_TP[slotIndex].Data[3U] = (uint8_t)Saej_TPTx[slotIndex].packetNum;                    // n° messaggi trasmessi
                CANMsgTx_Saej_TP[slotIndex].Data[4U] = 0xFFU;
                CANMsgTx_Saej_TP[slotIndex].Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
                CANMsgTx_Saej_TP[slotIndex].Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
                CANMsgTx_Saej_TP[slotIndex].Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );
                CAN_MsgUpdateID( &CANMsgTx_Saej_TP[slotIndex], auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES ); 
                CAN_MsgUpdateData( &CANMsgTx_Saej_TP[slotIndex], &CANMsgTx_Saej_TP[slotIndex].Data[0U], CAN_MSG_TX_YES );
            }
            else
            {   
                retVal = WRES_OPERATION_IN_PROGRESS;
            }
        }
//        CAN_MsgUpdateID( &CANMsgTx_Saej_TP, auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
//        CANMsgTx_Saej_TP.Data[1U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum );             // n° byte di dati - byte 0
//        CANMsgTx_Saej_TP.Data[2U] = (uint8_t)( Saej_TPTx[slotIndex].byteNum >> 8U );       // n° Byte di dati - byte 1
//        CANMsgTx_Saej_TP.Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
//        CANMsgTx_Saej_TP.Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
//        CANMsgTx_Saej_TP.Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );
//        bufferIndex[slotIndex] = 0U;
//        bufferCycleNum[slotIndex] = 0U;
//        Saej_tpBuffUpdate( slotIndex, bufferCycleNum[slotIndex], &buffSource[0U] );
//        CAN_MsgUpdateData( &CANMsgTx_Saej_TP, &CANMsgTx_Saej_TP.Data[0U], CAN_MSG_TX_YES );
    } 
    else 
    {
        if ( true == isDataOffset )
        {
//            auxID = auxID + ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF0000UL );
            if ( CAN_NO_MSG == CANMsgTx_Saej_ETP[slotIndex].NewMsg )
            {
                CANMsgTx_Saej_ETP[slotIndex].Data[0U] = SAEJ_ETP_CTRL_BYTE_DPO;
                CANMsgTx_Saej_ETP[slotIndex].Data[1U] = Saej_TPTx[slotIndex].packetNumToSend;
    //            CANMsgTx_Saej_TP.Data[2U] = (uint8_t)( Saej_TPTx[slotIndex].packetSent );
    //            CANMsgTx_Saej_TP.Data[3U] = (uint8_t)( Saej_TPTx[slotIndex].packetSent >> 8U );
    //            CANMsgTx_Saej_TP.Data[4U] = (uint8_t)( Saej_TPTx[slotIndex].packetSent >> 16U );
                CANMsgTx_Saej_ETP[slotIndex].Data[2U] = (uint8_t)( Saej_TPTx[slotIndex].nextPacket - 1U );
                CANMsgTx_Saej_ETP[slotIndex].Data[3U] = (uint8_t)( ( Saej_TPTx[slotIndex].nextPacket - 1U ) >> 8U );
                CANMsgTx_Saej_ETP[slotIndex].Data[4U] = (uint8_t)( ( Saej_TPTx[slotIndex].nextPacket - 1U ) >> 16U );            
                CANMsgTx_Saej_ETP[slotIndex].Data[5U] = (uint8_t)( Saej_TPTx[slotIndex].PGN );
                CANMsgTx_Saej_ETP[slotIndex].Data[6U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 8U );
                CANMsgTx_Saej_ETP[slotIndex].Data[7U] = (uint8_t)( Saej_TPTx[slotIndex].PGN >> 16U );                
    //            CAN_MsgUpdateID( &CANMsgTx_Saej_TP, auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
                CAN_MsgUpdateData( &CANMsgTx_Saej_ETP[slotIndex], &CANMsgTx_Saej_ETP[slotIndex].Data[0U], CAN_MSG_TX_YES );
            }
            else
            {
                retVal = WRES_OPERATION_IN_PROGRESS;
            }
        }
        else
        {
            if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )                         // è abilitato l'Extended TP
            {
                if ( CAN_NO_MSG == CANMsgTx_Saej_ETP_DT[slotIndex].NewMsg )
                {
                    auxID = auxID + ( SAEJ_CAN_MSG_ID_ETP_DT & 0xFFFF0000UL );
                    CANMsgTx_Saej_ETP_DT[slotIndex].Data[0U] = (uint8_t)(Saej_TPTx[slotIndex].packetCnt - Saej_TPTx[slotIndex].packetSent);
                    CAN_MsgUpdateID( &CANMsgTx_Saej_ETP_DT[slotIndex], auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
                }
                else
                {
                    retVal = WRES_OPERATION_IN_PROGRESS;
                }
                //CANMsgTx_Saej_TP_DT.Data[0U] = counterAux;
                //aux = ( tpCounter - 1UL ) * 7UL;
            } 
            else 
            {
                if ( CAN_NO_MSG == CANMsgTx_Saej_TP_DT[slotIndex].NewMsg )
                {
                    auxID = auxID + ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFF0000UL );
                    CANMsgTx_Saej_TP_DT[slotIndex].Data[0U] = (uint8_t)Saej_TPTx[slotIndex].packetCnt; 
                    CAN_MsgUpdateID( &CANMsgTx_Saej_TP_DT[slotIndex], auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
                }
                else
                {
                    retVal = WRES_OPERATION_IN_PROGRESS;
                }
                //aux = ( tpCounter - 1UL ) * 7UL;
            }
            
            if ( WRES_OPERATION_IN_PROGRESS != retVal )
            {
                aux = ( Saej_TPTx[slotIndex].packetCnt - 1UL ) * 7UL;
                //CANMsgTx_Saej_TP_DT.Data[0U] = counterAux;
                //CAN_MsgUpdateID( &CANMsgTx_Saej_TP_DT, auxID, CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );

                for ( j=0U; j<7U; j++ ) 
                {
                    if ( ( aux+j ) == ( Saej_TPTx[slotIndex].byteNum - 1U ) )
                    {
                        retVal = WRES_OK;          
                    }
                    if ( ( aux+j ) > ( Saej_TPTx[slotIndex].byteNum - 1U ) )
                    {
                        if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )
                        {
                            CANMsgTx_Saej_ETP_DT[slotIndex].Data[j+1U] = 0xFFU;
                        }
                        else
                        {
                            CANMsgTx_Saej_TP_DT[slotIndex].Data[j+1U] = 0xFFU;
                        }
                    }  
    //            else if ( 0x00U == ( aux+j ) ) 
    //            {
    //                CANMsgRx_Saej_TP_DT.Data[j+1U] = 0x11;      //questo serve per dire che mando giù il pool. Lo posso evitare qua, lo metto nel pool direttamente
    //            } 
                    else 
                    {
     
                        bufferIndex[slotIndex] = ( aux+j ) - ( bufferCycleNum[slotIndex] * SAEJ_TP_BYTE_NUM_MAX );            
                        if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )         //ho più byte della dimensione del buffer, quindi lo devo aggiornare prendendo un nuovo blocco di dati dalla sorgente
                        {
    //                   if ( ( aux+j ) < 18u ) 
    //                   {
    //                       datoaux = OPData[aux+j];
    //                   } 
    //                   else 
    //                   {
    //                       datoaux = SPI_EEpromRD( picBuffOffsetIndex );
    //                   }
    //                   TXMsg[MSG_TX_TP_P2P].Data.BYTE[j+1u] = datoaux;
    //                   picBuffOffsetIndex++;
                            if ( ( SAEJ_TP_BYTE_NUM_MAX - 1U) < bufferIndex[slotIndex] )
                            {
                                 bufferCycleNum[slotIndex]++;  
                                 Saej_tpBuffUpdate( slotIndex, bufferCycleNum[slotIndex] );
                                 bufferIndex[slotIndex] = 0U;
                            }
                        }  
                        if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )
                        {
                            CANMsgTx_Saej_ETP_DT[slotIndex].Data[j+1U] = Saej_TPTx[slotIndex].Saej_tpTxBuff[bufferIndex[slotIndex]];
                        }   
                        else
                        {
                            CANMsgTx_Saej_TP_DT[slotIndex].Data[j+1U] = Saej_TPTx[slotIndex].Saej_tpTxBuff[bufferIndex[slotIndex]];
                        }
                    }
                }
                if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[slotIndex].byteNum )
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_ETP_DT[slotIndex], &CANMsgTx_Saej_ETP_DT[slotIndex].Data[0U], CAN_MSG_TX_YES );
                }
                else
                {
                    CAN_MsgUpdateData( &CANMsgTx_Saej_TP_DT[slotIndex], &CANMsgTx_Saej_TP_DT[slotIndex].Data[0U], CAN_MSG_TX_YES );
                }
            }
        }
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static void Saej_TPTxMain( uint8_t index, wres_t txResult1, wres_t txResult2 )
{
    static uint32_t timeCMPauseTh = (SAEJ_STATUS_TP_CM_PAUSE_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
    wres_t retVal = WRES_ERROR;

    if ( ( WRES_OK != txResult1 ) || ( WRES_OK != txResult2 ) ) { return; }
      
    
    if ( Saej_TPTx[index].status.cntTh > 0U )
    {
        ++Saej_TPTx[index].status.cnt;
        if ( Saej_TPTx[index].status.cnt >= Saej_TPTx[index].status.cntTh )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
            Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_ABORT;
            Saej_TPTx[index].abortValue = SAEJ_TP_ABORT_CM_TIMEOUT;
        }
    }  
    
    if ( SAEJ_TP_CM_RX_ABORT == Saej_TPTx[index].cmRx )
    {
        Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_ABORT;
        Saej_TPTx[index].cmRx = SAEJ_TP_CM_RX_NO;
    }
    
    switch ( Saej_TPTx[index].status.value )
    {
    case SAEJ_STATUS_TP_CM_RTS:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].packetSent = 0U;
            Saej_TPTx[index].packetCnt = 0U;
            Saej_TPTx[index].byteCnt = 0U;
            Saej_TPTx[index].isETP = false;
            retVal = Saej_TPTxUpdate( index, false );
            if ( WRES_OPERATION_IN_PROGRESS != retVal )
            {
                Saej_TPTx[index].status.cnt = 0U;
                Saej_TPTx[index].packetCnt++;
                Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
            }
        }
        if ( WRES_OPERATION_IN_PROGRESS != retVal ) 
        {
            if ( SAEJ_TP_BYTE_NUM_MAX < Saej_TPTx[index].byteNum )
            {
                Saej_TPTx[index].isETP = true;
                timeCMPauseTh = ( SAEJ_STATUS_TP_CM_PAUSE_ETP_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
            }
            else
            {
                timeCMPauseTh = ( SAEJ_STATUS_TP_CM_PAUSE_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
            }
            if ( true == Saej_TPTx[index].isBroadcast )
            {
                //Saej_TPTx[index].status.cntTh = SAEJ_STATUS_TP_DT_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
                Saej_TPTx[index].status.cntTh = 0U;
                Saej_TPTx[index].status.value = SAEJ_STATUS_TP_DT;  
            }
            else
            {
                Saej_TPTx[index].status.cntTh = ( SAEJ_STATUS_TP_CM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
                Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_CTS;
                Saej_TPTx[index].status.cnt = 0U;
    //            if ( true == Saej_TPTx[index].isETP )
    //            {
    //                CAN_MsgUpdateID( &CANMsgRx_Saej_ETP, ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF00FFU ) + ( (uint32_t)( Saej_address ) * 256UL ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_NO );  
    //                //CAN_MsgUpdateID( &CANMsgRx_Saej_TP_CM[index], ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF0000U ) + ( (uint32_t)( Saej_address ) * 256UL ) + (uint32_t)( destAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );  
    //            }
    //            else
    //            {
    //                //CAN_MsgUpdateID( &CANMsgRx_Saej_TP_CM[index], ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF0000U ) + ( (uint32_t)( Saej_address ) * 256UL ) + (uint32_t)( destAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );  
    //                CAN_MsgUpdateID( &CANMsgRx_Saej_TP, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF0000U ) + ( (uint32_t)( Saej_address ) * 256UL ) + (uint32_t)( destAddr ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
    //            }  
            }
        }
        break;
    case SAEJ_STATUS_TP_CM_CTS:    
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }
        if ( SAEJ_TP_CM_RX_CTS == Saej_TPTx[index].cmRx )
        {
            Saej_TPTx[index].status.cnt = 0U;
            Saej_TPTx[index].cmRx = SAEJ_TP_CM_RX_NO;
            if ( 0U < Saej_TPTx[index].packetNumToSend )
            {
                Saej_TPTx[index].status.cntTh = 0U;
                if ( ( Saej_TPTx[index].nextPacket + Saej_TPTx[index].packetNumToSend - 1U ) >  Saej_TPTx[index].packetNum )
                {
                    Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_ABORT;
                    Saej_TPTx[index].abortValue = SAEJ_TP_ABORT_WRONG_PACKET_NUM;
                }
                else
                {
                    Saej_TPTx[index].packetCnt = Saej_TPTx[index].nextPacket;
                    Saej_TPTx[index].packetSent = Saej_TPTx[index].nextPacket-1U;
                    if ( true == Saej_TPTx[index].isETP )
                    { 
                        Saej_TPTx[index].status.value = SAEJ_STATUS_TP_DPO;
                    }
                    else
                    {
                        Saej_TPTx[index].status.value = SAEJ_STATUS_TP_DT;
                        Saej_TPTx[index].status.cnt = 0U;
                        Saej_TPTx[index].status.cntTh = 0U;
                    }                
                }
            }
        }
        if ( 0U == Saej_TPTx[index].packetNumToSend )
        {
            if ( Saej_TPTx[index].status.cnt >= timeCMPauseTh )
            {
                Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
                Saej_TPTx[index].abortValue = SAEJ_TP_ABORT_PAUSE_TIMEOUT;
                Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_ABORT;
            }          
        }           
        break;
    case SAEJ_STATUS_TP_DPO:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }      
        if ( WRES_OPERATION_IN_PROGRESS != Saej_TPTxUpdate( index, true ) )
        {
            Saej_TPTx[index].status.value = SAEJ_STATUS_TP_DT;
        }
        Saej_TPTx[index].status.cnt = 0U;
        Saej_TPTx[index].status.cntTh = 0U;        
        break;
    case SAEJ_STATUS_TP_DT:
//        if ( SAEJ_TP_CM_RX_CTS == Saej_TPTx[index].cmRx )
//        {
//            Saej_TPTx[index].cmRx = SAEJ_TP_CM_RX_NO;
//            Saej_statusTPTx[index].value = SAEJ_STATUS_TP_CM_CTS;
//        }
//        else
//        {
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }      
        retVal = Saej_TPTxUpdate( index,false );
        if ( WRES_OK == retVal )
        {
            if ( false == Saej_TPTx[index].isBroadcast )
            {
                Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_EOM_ACK;
                Saej_TPTx[index].status.cntTh = ( SAEJ_STATUS_TP_CM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
                Saej_TPTx[index].status.cnt = 0U;
            }
            else
            {
                Saej_TPTx[index].status.value = SAEJ_STATUS_TP_COMPLETED;
            }
        }
        else
        {
            if ( WRES_OPERATION_IN_PROGRESS != retVal )
            {
                if ( false == Saej_TPTx[index].isBroadcast )
                {
                    if ( ( Saej_TPTx[index].packetCnt - Saej_TPTx[index].packetSent ) == Saej_TPTx[index].packetNumToSend ) 
                    {
                        Saej_TPTx[index].packetSent = Saej_TPTx[index].packetCnt;
                        Saej_TPTx[index].status.cntTh = ( SAEJ_STATUS_TP_CM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ ) + (uint32_t)( CAN_RX_BUFFER_NUM_MAX );
                        Saej_TPTx[index].status.cnt = 0U;
                        Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_CTS;                    
                    }
                    Saej_TPTx[index].packetCnt++;
                }
                else
                {
                    Saej_TPTx[index].packetCnt++;
                }
            }
        }
//        }
        break;
    case SAEJ_STATUS_TP_CM_EOM_ACK:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }      
        if ( SAEJ_TP_CM_RX_EOMA == Saej_TPTx[index].cmRx )
        {
            Saej_TPTx[index].cmRx = SAEJ_TP_CM_RX_NO;
            Saej_TPTx[index].status.value = SAEJ_STATUS_TP_COMPLETED;
        }      
        break;
    case SAEJ_STATUS_TP_OFF:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }      
        if ( true == Saej_TPTx[index].isBusy )
        {
            Saej_TPTx[index].status.value = SAEJ_STATUS_TP_CM_RTS;
            Saej_TPTx[index].packetSent = 0U;
            Saej_TPTx[index].packetCnt = 0U;
            Saej_TPTx[index].byteCnt = 0U;
            Saej_TPTx[index].cmRx = SAEJ_TP_CM_RX_NO;
            Saej_TPTx[index].isETP = false;
            Saej_TPTx[index].abortValue = SAEJ_TP_ABORT_NULL;
        } 
        Saej_TPTx[index].status.cnt = 0U;
        Saej_TPTx[index].status.cntTh = 0U;
        break;
    case SAEJ_STATUS_TP_COMPLETED:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }      
        Saej_TPTx[index].isBusy = false;
        Saej_TPTx[index].destAddress = 0xFFU;
        Saej_TPTx[index].status.value = SAEJ_STATUS_TP_OFF;
        break;
    case SAEJ_STATUS_TP_CM_ABORT:
        if ( Saej_TPTx[index].status.previous != Saej_TPTx[index].status.value )
        {
            Saej_TPTx[index].status.previous = Saej_TPTx[index].status.value;
        }
        else
        {
            Saej_TPTxUpdate( index, false );
            Saej_TPTx[index].status.cnt = 0U;
            Saej_TPTx[index].status.cntTh = 0U; 
            Saej_TPTx[index].isBusy = false;
            Saej_TPTx[index].destAddress = 0xFFU;
            Saej_TPTx[index].status.value = SAEJ_STATUS_TP_OFF;
        }
        break;
    default:
        break;
    }
    
    return;
}

//------------------------------------------------------------------------------
static void Saej_RxMsgServe( void )
{
    uint8_t i;
    uint8_t tpAux;
    uint32_t aux;
    uint8_t rxMsg;

    //if ( CAN_NEW_MSG == CAN_MsgRx( &CANMsgRx_Saej_ContendingAddress ) )
    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_CONTENDING_ADDRESS, &rxMsg ) )
    {
        Saej_addressClaimResult = Saej_CheckAddressArbitration();
        CAN_StoredMsgClear( CAN_MSG_SAEJ_CONTENDING_ADDRESS, rxMsg );
    }

    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_DM14, &rxMsg ) )
    {
        if ( SAEJ_STATUS_NORMAL == Saej_status.value ) 
        {
            Flash_CmdUpdate( CAN_StoredMsgGet( CAN_MSG_SAEJ_DM14, rxMsg ).Data, (uint8_t)( CAN_StoredMsgGetID( CAN_MSG_SAEJ_DM14, rxMsg ) & 0x000000FFUL ) );
            for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
            {
                CAN_StoredMsgUpdate( CAN_MSG_SAEJ_DM14, i, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF0000UL ) + ( CAN_StoredMsgGetID( CAN_MSG_SAEJ_DM14, i ) & 0x000000FFUL ) + ( (uint32_t)Saej_address * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_YES );
                CAN_StoredMsgUpdate( CAN_MSG_SAEJ_DM16, i, ( ( SAEJ_CAN_MSG_ID_DM16 & 0xFFFF0000UL ) + ( (uint32_t)Saej_address * 256U ) + (uint32_t)( CAN_StoredMsgGetID( CAN_MSG_SAEJ_DM14, i ) & 0x000000FFUL ) ), CAN_SOURCE_ADDRESS_CHECK_YES );
            }
            CAN_MsgUpdateID( &CANMsgTx_Saej_DM15, ( ( SAEJ_CAN_MSG_ID_DM15 & 0xFFFF0000UL ) + Saej_address + (uint32_t)( ( CAN_StoredMsgGetID( CAN_MSG_SAEJ_DM14, rxMsg ) & 0x000000FFUL ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );            
            CAN_MsgUpdateID( &CANMsgTx_Saej_DM16, ( ( SAEJ_CAN_MSG_ID_DM16 & 0xFFFF0000UL ) + Saej_address + (uint32_t)( ( CAN_StoredMsgGetID( CAN_MSG_SAEJ_DM14, rxMsg ) & 0x000000FFUL ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_DM14, rxMsg );        
    }

    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_DM16, &rxMsg ) )
    {
        if ( SAEJ_STATUS_NORMAL == Saej_status.value )
        {
            Flash_DataUpdate( CAN_StoredMsgGet( CAN_MSG_SAEJ_DM16, rxMsg ).Data );
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_DM16, rxMsg );        
    }
    
    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_REQUEST_PGN, &rxMsg ) )
    {
        if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ).DA )
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value ) 
            {         
                Saej_RequestPGNBuildTxMsg( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ) );
            }
            else if ( SAEJ_STATUS_CANNOT_CLAIM == Saej_status.value ) 
            {
                if ( 0x00EE00UL == ( (uint32_t)CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ).Data[0U]        ) + 
                                   ( (uint32_t)CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ).Data[1U] << 8U  ) + 
                                   ( (uint32_t)CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ).Data[2U] << 16U )  )
                {
                    Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
                }
            }
            else 
            {
              // Do Nothing
            }
        }
        else
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {            
                Saej_RequestPGNBuildTxMsg( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg ) );
            }
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_REQUEST_PGN, rxMsg );
#ifdef ISOBUS_CAN_RX_COUNTER
        Isobus_MsgRxCounterUpdate();
#endif
    }

//    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_REQUEST_PGN_ALL, &rxMsg ) )
//    {
//        if ( SAEJ_STATUS_NORMAL == Saej_status.value )
//        {         
//            Saej_RequestPGNBuildTxMsg( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN_ALL, rxMsg ) );
//        }
//        else if ( SAEJ_STATUS_CANNOT_CLAIM == Saej_status.value ) 
//        {
//            if ( 0x00EE00UL == (uint32_t)( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN_ALL, rxMsg ).Data[0U] ) + (uint32_t)( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN_ALL, rxMsg ).Data[1U] << 8U ) + (uint32_t)( CAN_StoredMsgGet( CAN_MSG_SAEJ_REQUEST_PGN_ALL, rxMsg ).Data[2U] << 16U ) )
//            {
//                Saej_TxAddressClaim( SAEJ_ADDRESS_NOT_CLAIMED );
//            }
//        }
//        CAN_StoredMsgClear( CAN_MSG_SAEJ_REQUEST_PGN_ALL, rxMsg );
//    }    
    
    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_TP, &rxMsg ) )
    {
        if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).DA ) 
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {      
                if ( 0x20U == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).Data[0U] )
                {
                    if ( SAEJ_STATUS_TP_OFF == Saej_statusTPRx.value )
                    {
                        Saej_TPRxInit( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ), true );
                        //Saej_TP.isBroadcast = true;
                        Saej_statusTPRx.value = SAEJ_STATUS_TP_INIT;
                    }
                }
                else if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).Data[0U] )
                {  
                    Saej_statusTPRx.value = SAEJ_STATUS_TP_OFF;
                }
                else 
                {
                  // Do Nothing
                }
            }          
        }
        else
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {    
                tpAux = 0U;
                for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
                {
                    if ( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).SA == Saej_TPTx[i].destAddress )
                    {
                        if ( 0U == tpAux )
                        {
                            memcpy(Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data, CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).Data, (uint32_t)CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).DLC );
                            Saej_TPTx[i].CANMsgRx_Saej_TP_CM.NewMsg = CAN_NEW_MSG;
                            tpAux++;
                        }
                    }
                }
                if ( 0U == tpAux )
                {
                    if ( 0x10U == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).Data[0U] )
                    {
                        if ( SAEJ_STATUS_TP_OFF == Saej_statusTPRx.value )
                        {
                            Saej_TPRxInit( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ), false );
                            Saej_statusTPRx.value = SAEJ_STATUS_TP_INIT;
                        }
                    }
                    else if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP, rxMsg ).Data[0U] )
                    {  
                        Saej_statusTPRx.value = SAEJ_STATUS_TP_OFF;
                    }
                    else  
                    {
                      // Do Nothing
                    }
                }
            }
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_TP, rxMsg );
#ifdef ISOBUS_CAN_RX_COUNTER
        Isobus_MsgRxCounterUpdate();
#endif        
    }

    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_ETP, &rxMsg ) )
    {
        if ( SAEJ_STATUS_NORMAL == Saej_status.value )
        {    
            tpAux = 0U;
            for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
            {
                if ( CAN_StoredMsgGet( CAN_MSG_SAEJ_ETP, rxMsg ).SA == Saej_TPTx[i].destAddress )
                {
                    if ( 0U == tpAux )
                    {
                        memcpy(Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data, CAN_StoredMsgGet( CAN_MSG_SAEJ_ETP, rxMsg ).Data, (uint32_t)CAN_StoredMsgGet( CAN_MSG_SAEJ_ETP, rxMsg ).DLC );
                        Saej_TPTx[i].CANMsgRx_Saej_TP_CM.NewMsg = CAN_NEW_MSG;
                        tpAux++;
                    }
                }
            }
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_ETP, rxMsg );
#ifdef ISOBUS_CAN_RX_COUNTER
        Isobus_MsgRxCounterUpdate();
#endif        
    }    
    
//    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_TP_ALL, &rxMsg ) )
//    {
//        if ( SAEJ_STATUS_NORMAL == Saej_status.value )
//        {      
//            if ( 0x20U == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_ALL, rxMsg ).Data[0U] )
//            {
//                if ( SAEJ_STATUS_TP_OFF == Saej_statusTPRx.value )
//                {
//                    Saej_TPRxInit( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_ALL, rxMsg ), true );
//                    //Saej_TP.isBroadcast = true;
//                    Saej_statusTPRx.value = SAEJ_STATUS_TP_INIT;
//                }
//            }
//            else if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_ALL, rxMsg ).Data[0U] )
//            {  
//                Saej_statusTPRx.value = SAEJ_STATUS_TP_OFF;
//            }            
//        }
//        CAN_StoredMsgClear( CAN_MSG_SAEJ_TP_ALL, rxMsg );
//    } 
    
    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_TP_DT, &rxMsg ) )
    {
        if ( 0xFFU == CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_DT, rxMsg ).DA )
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {      
                if ( ( true == Saej_TPRx.isBroadcast ) && ( SAEJ_STATUS_TP_DT == Saej_statusTPRx.value ) )
                {
                    Saej_TPRxData( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_DT, rxMsg ) );
                }
            }        
        }
        else
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {      
                if ( ( false == Saej_TPRx.isBroadcast ) && ( SAEJ_STATUS_TP_DT == Saej_statusTPRx.value ) )
                {
                    Saej_TPRxData( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_DT, rxMsg ) );
                }
            }
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_TP_DT, rxMsg );
#ifdef ISOBUS_CAN_RX_COUNTER
        Isobus_MsgRxCounterUpdate();
#endif        
    }

//    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_TP_DT_ALL, &rxMsg ) )
//    {
//        if ( SAEJ_STATUS_NORMAL == Saej_status.value )
//        {      
//            if ( ( true == Saej_TPRx.isBroadcast ) && ( SAEJ_STATUS_TP_DT == Saej_statusTPRx.value ) )
//            {
//                Saej_TPRxData( CAN_StoredMsgGet( CAN_MSG_SAEJ_TP_DT_ALL, rxMsg ) );
//            }
//        }
//        CAN_StoredMsgClear( CAN_MSG_SAEJ_TP_DT_ALL, rxMsg );
//    }

//#define SAEJ_TP_CTRL_BYTE_RTS             ( 0x10U )
//#define SAEJ_TP_CTRL_BYTE_CTS             ( 0x11U )
//#define SAEJ_TP_CTRL_BYTE_EOMA            ( 0x13U )
//#define SAEJ_TP_CTRL_CONN_ABORT           ( 0xFFU )
//
//#define SAEJ_ETP_BYTE_NUM_MIN             ( 1786UL )
//#define SAEJ_ETP_BYTE_NUM_MAX             ( 117440505UL )
//#define SAEJ_ETP_PACKET_NUM_MIN           ( 256UL )
//#define SAEJ_ETP_PACKET_NUM_MAX           ( 16777215UL )
//
//#define SAEJ_ETP_CTRL_BYTE_RTS            ( 0x14U )
//#define SAEJ_ETP_CTRL_BYTE_CTS            ( 0x15U )
//#define SAEJ_ETP_CTRL_BYTE_DPO            ( 0x16U )
//#define SAEJ_ETP_CTRL_BYTE_EOMA           ( 0x17U )
    
    
    for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
    {
        if ( CAN_NEW_MSG == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.NewMsg )
        {
            if ( SAEJ_STATUS_NORMAL == Saej_status.value )
            {
                if ( ( (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[5U]        ) + 
                       (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[6U] << 8U  ) + 
                       (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[7U] << 16U )  ) == Saej_TPTx[i].PGN )
                {              
                    if ( ( ( ( true == Saej_TPTx[i].isETP ) && ( ( SAEJ_ETP_CTRL_BYTE_CTS ) == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[0U] ) ) 
                        || ( ( false == Saej_TPTx[i].isETP ) && ( SAEJ_TP_CTRL_BYTE_CTS == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[0U] ) ) ) )
                    {
                        Saej_TPTx[i].packetNumToSend = Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[1U];
                        Saej_TPTx[i].nextPacket = (uint32_t)( Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[2U] );
                        if ( true == Saej_TPTx[i].isETP )
                        {
                            Saej_TPTx[i].nextPacket |= ( (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[3U] << 8U  ) + 
                                                         (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[4U] << 16U )  );    
                        }
                        Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_CTS;
                    }
                    else if ( ( ( ( true == Saej_TPTx[i].isETP ) && ( ( SAEJ_ETP_CTRL_BYTE_EOMA ) == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[0U] ) ) 
                             || ( ( false == Saej_TPTx[i].isETP ) && ( SAEJ_TP_CTRL_BYTE_EOMA == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[0U] ) ) ) )
                    {
                        if ( true == Saej_TPTx[i].isETP )
                        {
                            aux = (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[1U]        ) + 
                                  (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[2U] << 8U  ) + 
                                  (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[3U] << 16U ) + 
                                  (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[4U] << 24U );    
                            if ( aux == Saej_TPTx[i].byteNum )
                            {
                                Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_EOMA;
                            }
                        }
                        else
                        {
                            aux = (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[1U] ) + 
                                  (uint32_t)( (uint32_t)Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[2U] << 8U );
                            if ( ( aux == Saej_TPTx[i].byteNum ) && ( Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[3U] == Saej_TPTx[i].packetNum ) )
                            {
                                Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_EOMA;
                            }
                        }
                    }
                    else if ( 0xFFU == Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[0U] )
                    {
                        Saej_TPTx[i].abortValue = Saej_TPTx[i].CANMsgRx_Saej_TP_CM.Data[1U];
                        Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_ABORT;                      
                    }
                    else  
                    {
                      // Do Nothing
                    }
                }
                else
                {
                    Saej_TPTx[i].abortValue = SAEJ_TP_ABORT_BAD_PGN_ECTS;
                    Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_ABORT;    
                }
            }
            Saej_TPTx[i].CANMsgRx_Saej_TP_CM.NewMsg = CAN_NO_MSG;      
        }   
    }
    
    if ( CAN_NEW_MSG == CAN_StoredMsgRx( CAN_MSG_SAEJ_LED, &rxMsg ) )
    {
        if ( ( SAEJ_STATUS_NORMAL == Saej_status.value ) && ( 0U != Saej_ledCmdEn ) )
        {
            Saej_ledCmd.en = true;
            Error_Reset( &Saej_ledCmd );
            Saej_ledDrive = CAN_StoredMsgGet( CAN_MSG_SAEJ_LED, 0U ).Data[0U];
            Saej_ledDrive = Saej_ledFilterEn( Saej_ledDrive, Saej_ledCmdEn );
            Saej_ledDuty = CAN_StoredMsgGet( CAN_MSG_SAEJ_LED, 0U ).Data[1U];
            //BSP_OutLedUpdate( CAN_StoredMsgGet( CAN_MSG_SAEJ_LED, rxMsg ).Data[0U] );
        }
        else
        {
            Error_Reset( &Saej_ledCmd );
        }
        CAN_StoredMsgClear( CAN_MSG_SAEJ_LED, rxMsg );
#ifdef ISOBUS_CAN_RX_COUNTER
        Isobus_MsgRxCounterUpdate();
#endif        
    }
  
    return;
}

//------------------------------------------------------------------------------
static void Saej_SPN_OccurrenceUpdate( uint8_t index )
{
    if ( Saej_SPN[index].FMI != Saej_SPN[index].FMIold )
    {
        if ( SAEJ_FMI_NA != Saej_SPN[index].FMI )
        {
          ++Saej_SPN[index].FMI_count[Saej_SPN[index].FMI];
            if ( Saej_SPN[index].FMI_count[Saej_SPN[index].FMI] > 0x7FU )
            {
                Saej_SPN[index].FMI_count[Saej_SPN[index].FMI] = 0x7FU;
            }
            Saej_SPN[index].count = Saej_SPN[index].FMI_count[Saej_SPN[index].FMI];
        }
        else
        {
            Saej_SPN[index].count = 0x7FU;
        }
    } 
    return;
}

//------------------------------------------------------------------------------
static uint8_t Saej_LampDecode( uint8_t lampValue )
{
    uint8_t retVal;
    
    retVal = 0x00U;
    if ( (uint8_t)SAEJ_LAMP_MALFUNCTION_ON == ( (uint8_t)lampValue & (uint8_t)SAEJ_LAMP_MALFUNCTION_ON ) )
    {
        retVal = 0x10U;
    }
    else if ( (uint8_t)SAEJ_LAMP_RED_STOP_ON == ( (uint8_t)lampValue & (uint8_t)SAEJ_LAMP_RED_STOP_ON ) )
    {
        retVal = 0x01U;
    }
    else if ( (uint8_t)SAEJ_LAMP_AMBER_WARNING_ON == ( (uint8_t)lampValue & (uint8_t)SAEJ_LAMP_AMBER_WARNING_ON ) )
    {
        retVal = 0x05U;
    }
    else if ( (uint8_t)SAEJ_LAMP_PROTECT_ON == ( (uint8_t)lampValue & (uint8_t)SAEJ_LAMP_PROTECT_ON ) )
    {
        retVal = 0x11U;
    }
    else
    {
      // Do Nothing
    }
  
    return retVal;
}

uint8_t spnIndex = 0xFFU;
//------------------------------------------------------------------------------
static void Saej_DM1Main( void )
{
    bool break_stat = true;
    uint8_t i;
    uint8_t lamp;
    uint8_t flashLamp;
    static uint32_t spnFMI = 0U;
    uint32_t spnFMInew = 0U;
    uint8_t cnt;
    //uint8_t spnIndex = 0xFFU;
    uint8_t txFlag = CAN_MSG_TX_NO;
    static uint16_t Saej_errorTransitionActive = 0U;
    
    if ( 0U == CANMsgTx_Saej_DM1.TimeCnt )
    {
        Saej_errorTransitionActive = 0U;    
    }
    
    
    if ( 0xFFU == spnIndex )
    {
        if ( ( MEM_DATA_ERROR_INVALID_SA == ( MemData_ErrorGet() & MEM_DATA_ERROR_INVALID_SA ) )
          || ( MEM_DATA_ERROR_INVALID_BAUD == ( MemData_ErrorGet() & MEM_DATA_ERROR_INVALID_BAUD ) )
          || ( MEM_DATA_ERROR_INVALID_APP_PARAM == ( MemData_ErrorGet() & MEM_DATA_ERROR_INVALID_APP_PARAM ) ) )
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_MEMORY );
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = Saej_SPN[SAEJ_SPN_MEMORY].FMI;                
            spnIndex = SAEJ_SPN_MEMORY;          
        }                                                    
        else if ( MEM_DATA_ERROR_CALIB_CHECKSUM == ( MemData_ErrorGet() & MEM_DATA_ERROR_CALIB_CHECKSUM ) )
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_OUT_OF_CALIBRATION_ERROR;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_MEMORY );
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = Saej_SPN[SAEJ_SPN_MEMORY].FMI;                
            spnIndex = SAEJ_SPN_MEMORY;
        }      
        else if ( MEM_DATA_ERROR_CHECKSUM == ( MemData_ErrorGet() & MEM_DATA_ERROR_CHECKSUM ) )
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_DATA_INPUT_ERROR;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_MEMORY );
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = Saej_SPN[SAEJ_SPN_MEMORY].FMI;                
            spnIndex = SAEJ_SPN_MEMORY;
        }
#ifdef ISOBUS_DJW
        else if ( MEM_DATA_ERROR_ISOBUS_CHECKSUM == ( MemData_ErrorGet() & MEM_DATA_ERROR_ISOBUS_CHECKSUM ) )
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_DATA_INPUT_ERROR;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_MEMORY );
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = Saej_SPN[SAEJ_SPN_MEMORY].FMI;                
            spnIndex = SAEJ_SPN_MEMORY;
        }        
#endif        
        else if ( FLASH_ERROR_OK != Flash_ErrorGet() )
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_DATA_NOT_VALID;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_MEMORY );
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = Saej_SPN[SAEJ_SPN_MEMORY].FMI;                
            spnIndex = SAEJ_SPN_MEMORY;          
        }
        else
        {
            Saej_SPN[SAEJ_SPN_MEMORY].FMI = SAEJ_FMI_NA;
            Saej_SPN[SAEJ_SPN_MEMORY].FMIold = SAEJ_FMI_NA;
        }
    }
    
    if ( 0xFFU == spnIndex )
    {
        for ( i=0U; i<MLX_SENSOR_NUM; i++ )
        {
            if ( break_stat )
            {
              if ( ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_ALPHA_VALUE_HIGH ) ) || ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_BETA_VALUE_HIGH ) ) )
              {
                Saej_SPN[i].FMI = SAEJ_FMI_VOLTAGE_HIGH;
                Saej_SPN_OccurrenceUpdate( i );
                Saej_SPN[i].FMIold = Saej_SPN[i].FMI;
                spnIndex = i;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_ALPHA_VALUE_LOW ) ) || ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_BETA_VALUE_LOW ) ) )
              {
                Saej_SPN[i].FMI = SAEJ_FMI_VOLTAGE_LOW;
                Saej_SPN_OccurrenceUpdate( i );
                Saej_SPN[i].FMIold = Saej_SPN[i].FMI;            
                spnIndex = i;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_TIME_OUT ) )
              {
                Saej_SPN[i].FMI = SAEJ_FMI_UPDATE_RATE_ERROR;
                Saej_SPN_OccurrenceUpdate( i );
                Saej_SPN[i].FMIold = Saej_SPN[i].FMI;            
                spnIndex = i;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_INVALID_DATA_ALPHA ) ) || ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_INVALID_DATA_BETA ) ) )
              {
                Saej_SPN[i].FMI = SAEJ_FMI_DATA_INPUT_ERROR;
                Saej_SPN_OccurrenceUpdate( i );
                Saej_SPN[i].FMIold = Saej_SPN[i].FMI;            
                spnIndex = i;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( 0U == i )
              {
                if ( ( true == MLX_ErrorGet( MLX_0, MLX_ERROR_INDEX_ALPHA_REDUNDANCE ) ) || ( true == MLX_ErrorGet( MLX_0, MLX_ERROR_INDEX_BETA_REDUNDANCE ) ) ) // || ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_BETA_REDUNDANCE ) ) )
                {
                    Saej_SPN[i].FMI = SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR;
                    Saej_SPN_OccurrenceUpdate( i );
                    Saej_SPN[i].FMIold = Saej_SPN[i].FMI;            
                    spnIndex = i;
                    break_stat = false;
                }
              }
            }
            if ( break_stat )
            {
              if ( 1U == i )
              {
                if ( ( true == MLX_ErrorGet( MLX_1, MLX_ERROR_INDEX_ALPHA_REDUNDANCE ) ) || ( true == MLX_ErrorGet( MLX_1, MLX_ERROR_INDEX_BETA_REDUNDANCE ) ) ) // || ( true == MLX_ErrorGet( i, MLX_ERROR_INDEX_BETA_REDUNDANCE ) ) )
                {
                    Saej_SPN[i].FMI = SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR;
                    Saej_SPN_OccurrenceUpdate( i );
                    Saej_SPN[i].FMIold = Saej_SPN[i].FMI;            
                    spnIndex = i;
                    break_stat = false;
                }
              }
            }
            if  ( break_stat )
            {
              Saej_SPN[i].FMI = SAEJ_FMI_NA;
              Saej_SPN[i].FMIold = SAEJ_FMI_NA;
            }
       }
    }
    
    break_stat = true;
    if ( 0xFFU == spnIndex )
    {
        for ( i=0U; i<ROLLER_NUM_MAX; i++ )
        {
            if ( break_stat )
            {
              if ( true == Roller_ErrorGet( i, ROLLER_ERROR_INDEX_VALUE_HIGH ) )
              {
                Saej_SPN[i + MLX_SENSOR_NUM].FMI = SAEJ_FMI_VOLTAGE_HIGH;
//                spnFMI = ( Saej_SPN[i].value & 0x07FFFFFFFFUL ) + (uint32_t)( ( Saej_SPN[i].FMI & 0x01F ) << 3U );
                Saej_SPN_OccurrenceUpdate( i + MLX_SENSOR_NUM );
                Saej_SPN[i + MLX_SENSOR_NUM].FMIold = Saej_SPN[i + MLX_SENSOR_NUM].FMI;
                spnIndex = i + MLX_SENSOR_NUM;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( true == Roller_ErrorGet( i, ROLLER_ERROR_INDEX_VALUE_LOW ) )
              {
                Saej_SPN[i + MLX_SENSOR_NUM].FMI = SAEJ_FMI_VOLTAGE_LOW;
//                spnFMI = ( Saej_SPN[i].value & 0x07FFFFFFFFUL ) + (uint32_t)( ( Saej_SPN[i].FMI & 0x01F ) << 3U );
                Saej_SPN_OccurrenceUpdate( i + MLX_SENSOR_NUM );
                Saej_SPN[i + MLX_SENSOR_NUM].FMIold = Saej_SPN[i + MLX_SENSOR_NUM].FMI;                
                spnIndex = i + MLX_SENSOR_NUM;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              if ( true == Roller_ErrorGet( i, ROLLER_ERROR_INDEX_REDUNDANCE ) )
              {
                Saej_SPN[i + MLX_SENSOR_NUM].FMI = SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR;
//                spnFMI = ( Saej_SPN[i].value & 0x07FFFFFFFFUL ) + (uint32_t)( ( Saej_SPN[i].FMI & 0x01F ) << 3U );
                Saej_SPN_OccurrenceUpdate( i + MLX_SENSOR_NUM );
                Saej_SPN[i + MLX_SENSOR_NUM].FMIold = Saej_SPN[i + MLX_SENSOR_NUM].FMI;                
                spnIndex = i + MLX_SENSOR_NUM;
                break_stat = false;
              }
            }
            if ( break_stat )
            {
              Saej_SPN[i + MLX_SENSOR_NUM].FMI = SAEJ_FMI_NA;
              Saej_SPN[i + MLX_SENSOR_NUM].FMIold = SAEJ_FMI_NA;            
            }
        }      
    }

    if ( 0xFFU == spnIndex )
    {
        if ( SUPPLY_VCC_5VDC_ERR_LO == Supply_ErrorGet( SUPPLY_VCC_5VDC_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_LOW_SEVERITY_HIGH;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;
        }
        else if ( SUPPLY_VCC_5VDC_ERR_HI == Supply_ErrorGet( SUPPLY_VCC_5VDC_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_HIGH_SEVERITY_HIGH;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;          
        }
        else if ( SUPPLY_VCC_5VDC_EXT_ERR_LO == Supply_ErrorGet( SUPPLY_VCC_5VDC_EXT_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_LOW_SEVERITY_MEDIUM;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;
        }
        else if ( SUPPLY_VCC_5VDC_EXT_ERR_HI == Supply_ErrorGet( SUPPLY_VCC_5VDC_EXT_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_HIGH_SEVERITY_MEDIUM;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;          
        }        
        else if ( SUPPLY_VBB_PROT_ERR_LO == Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_LOW_SEVERITY_LOW;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;
        }
        else if ( SUPPLY_VBB_PROT_ERR_CAN_OFF == ( Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) & SUPPLY_VBB_PROT_ERR_CAN_OFF ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_LOW_SEVERITY_HIGH;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;
        }        
        else if ( SUPPLY_VBB_PROT_ERR_HI == Supply_ErrorGet( SUPPLY_VBB_PROT_ERROR ) )
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_DATA_HIGH_SEVERITY_LOW;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_SUPPLY );
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = Saej_SPN[SAEJ_SPN_SUPPLY].FMI;                
            spnIndex = SAEJ_SPN_SUPPLY;          
        }
        else
        {
            Saej_SPN[SAEJ_SPN_SUPPLY].FMI = SAEJ_FMI_NA;
            Saej_SPN[SAEJ_SPN_SUPPLY].FMIold = SAEJ_FMI_NA;
        }                  
    }

    if ( 0xFFU == spnIndex )
    {
        for ( i=0U; i<SAEJ_PUSH_BUTTON_NUM; i++ )
        {
            if ( WRES_OK != Saej_pushButtonGetResult[i] ) 
            {
              if ( false == Digital_GetExternalError() )
              {
                Saej_SPN[i + MLX_SENSOR_NUM + ROLLER_NUM_MAX].FMI = SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR;   
                Saej_SPN_OccurrenceUpdate( i + MLX_SENSOR_NUM + ROLLER_NUM_MAX);
                Saej_SPN[i + MLX_SENSOR_NUM + ROLLER_NUM_MAX].FMIold = Saej_SPN[i + MLX_SENSOR_NUM + ROLLER_NUM_MAX].FMI;
                spnIndex = i + MLX_SENSOR_NUM + ROLLER_NUM_MAX;                  
                break;
              }
            }
            Saej_SPN[i + MLX_SENSOR_NUM + ROLLER_NUM_MAX].FMI = SAEJ_FMI_NA;
            Saej_SPN[i + MLX_SENSOR_NUM + ROLLER_NUM_MAX].FMIold = SAEJ_FMI_NA;
        }
    }

    if ( 0xFFU == spnIndex )
    {
        if ( true == Error_IsActive( Saej_ledCmd ) )
        {
            Saej_SPN[SAEJ_SPN_LED].FMI = SAEJ_FMI_UPDATE_RATE_ERROR;
            Saej_SPN_OccurrenceUpdate( SAEJ_SPN_LED );
            Saej_SPN[SAEJ_SPN_LED].FMIold = Saej_SPN[SAEJ_SPN_LED].FMI;                
            spnIndex = SAEJ_SPN_LED;
        }
        else
        {
            Saej_SPN[SAEJ_SPN_LED].FMI = SAEJ_FMI_NA;
            Saej_SPN[SAEJ_SPN_LED].FMIold = SAEJ_FMI_NA;
        }
    }
    
    if ( 0xFFU == spnIndex )
    {
        for ( i=0U; i<LED_OUT_NUM; i++ )
        {
            if ( true == LED_ErrorIsActive( i ) )
            {
                Saej_SPN[SAEJ_SPN_LED_STATUS_0 + i].FMI = SAEJ_FMI_SYSTEM_ERROR;   
                Saej_SPN_OccurrenceUpdate( SAEJ_SPN_LED_STATUS_0 + i );
                Saej_SPN[SAEJ_SPN_LED_STATUS_0 + i].FMIold = Saej_SPN[SAEJ_SPN_LED_STATUS_0 + i].FMI;
                spnIndex = SAEJ_SPN_LED_STATUS_0 + i;                  
                break;
            }
            Saej_SPN[SAEJ_SPN_LED_STATUS_0 + i].FMI = SAEJ_FMI_NA;
            Saej_SPN[SAEJ_SPN_LED_STATUS_0 + i].FMIold = SAEJ_FMI_NA;
        }
    }

    if ( 0xFFU != spnIndex )
    {
        spnFMInew =  (   Saej_SPN[spnIndex].value & 0x0FFFFUL ) | 
                     ( ( Saej_SPN[spnIndex].value & 0x30000UL ) << 21U ) |
                     ( ( (uint32_t)Saej_SPN[spnIndex].FMI   & 0x0001FUL ) << 16U );
        cnt = Saej_SPN[spnIndex].count;
        lamp = (uint8_t)Saej_SPN[spnIndex].lampStatus;
        flashLamp = (uint8_t)Saej_SPN[spnIndex].flashLampStatus;
    }
    else
    {
        spnFMInew = 0x000000UL;
        cnt = 0x00U;
        lamp = (uint8_t)SAEJ_LAMP_OFF;
        flashLamp = (uint8_t)SAEJ_LAMP_NA;
    }

    CANMsgTx_Saej_DM1.Data[0U] = lamp;
    CANMsgTx_Saej_DM1.Data[1U] = flashLamp;
    CANMsgTx_Saej_DM1.Data[2U] = (uint8_t)( spnFMInew );
    CANMsgTx_Saej_DM1.Data[3U] = (uint8_t)( spnFMInew >> 8U );
    CANMsgTx_Saej_DM1.Data[4U] = (uint8_t)( spnFMInew >> 16U );
    CANMsgTx_Saej_DM1.Data[5U] = cnt;
    CANMsgTx_Saej_DM1.Data[6U] = 0xFFU;
    CANMsgTx_Saej_DM1.Data[7U] = 0xFFU;    
//    CANMsgTx_Saej_DM1.Data[6U] = (uint8_t)( CANMsgTx_Saej_DM1.TimeCnt >> 8U );
//    CANMsgTx_Saej_DM1.Data[7U] = (uint8_t)( CANMsgTx_Saej_DM1.TimeCnt );    
    
    
    switch ( Saej_statusDM1.value )
    {
    case SAEJ_STATUS_DM1_OFF:
        if ( 0xFFU != spnIndex )
        {
            Saej_error.active = true;
            Saej_error.value = (uint32_t)( Saej_LampDecode( lamp ) );
            Saej_statusDM1.value = SAEJ_STATUS_DM1_ON;
            CAN_MsgUpdateData( &CANMsgTx_Saej_DM1, &CANMsgTx_Saej_DM1.Data[0U], CAN_MSG_TX_YES );
        }
        break;
    case SAEJ_STATUS_DM1_ON:
        if ( spnFMInew != spnFMI )
        {
            Saej_errorTransitionActive++;
            if ( 1U == Saej_errorTransitionActive )
            {
                txFlag = CAN_MSG_TX_YES;
            }
            else
            {
                txFlag = CAN_MSG_TX_NO;
            }
            if ( 0xFFU == spnIndex )
            {
                Saej_statusDM1.value = SAEJ_STATUS_DM1_EXIT;
            }
        }
        else
        { 
            txFlag = CAN_MSG_TX_NO;
        }
        CAN_MsgUpdateData( &CANMsgTx_Saej_DM1, &CANMsgTx_Saej_DM1.Data[0U], txFlag );
        break;
    case SAEJ_STATUS_DM1_EXIT:
        if ( 1U < Saej_errorTransitionActive )
        {
            if ( 0U == CANMsgTx_Saej_DM1.TimeCnt )
            {
                Saej_errorTransitionActive = 0U;
                Saej_error.active = false;
                Saej_error.value = 0U;
                Saej_statusDM1.value = SAEJ_STATUS_DM1_OFF;            
            }
        }
        else
        {
            Saej_errorTransitionActive = 0U;
            Saej_error.active = false;
            Saej_error.value = 0U;
            Saej_statusDM1.value = SAEJ_STATUS_DM1_OFF;          
        }
        break;
        
    default:
        break;
    }
    
    spnFMI = spnFMInew;
    return;
}

//------------------------------------------------------------------------------
static void Saej_TxMsgServe( void )
{
    uint8_t i;
    uint8_t *Flash_txQueue;
    uint8_t txBuffer[FLASH_MSG_BYTE_NUM];
    
    CAN_MsgTx( &CANMsgTx_Saej_RequestAddressClaim, false, 0xFF );
    CAN_MsgTx( &CANMsgTx_Saej_AddressClaim, false, 0xFF ); 
    CAN_MsgTx( &CANMsgTx_Saej_SendPGN, false, 0xFF );

    if ( SAEJ_STATUS_NORMAL == Saej_status.value )
    {
        if ( FLASH_MSG_TX_STATUS_NO != Flash_MsgTxStatusGet() ) 
        {
            Flash_txQueue = Flash_GetTxData( FLASH_MSG_BYTE_NUM );
            for ( i=0U; i<FLASH_MSG_BYTE_NUM; i++ )
            {
                txBuffer[i] = *( Flash_txQueue + i );
            }
            if ( FLASH_MSG_TX_DM15_STATUS_OK == Flash_MsgTxStatusGet() )
            {
                CAN_MsgUpdateData( &CANMsgTx_Saej_DM15, &txBuffer[0u], CAN_MSG_TX_YES );
                CAN_MsgTx( &CANMsgTx_Saej_DM15, false, 0xFF );        
            } 
            else if ( FLASH_MSG_TX_DM16_STATUS_OK == Flash_MsgTxStatusGet() ) 
            {
                CAN_MsgUpdateData( &CANMsgTx_Saej_DM16, &txBuffer[0u], CAN_MSG_TX_YES );
                CAN_MsgTx( &CANMsgTx_Saej_DM16, false, 0xFF );          
            }
            else 
            {
              // Do Nothing
            }
            Flash_MsgTxStatusClear();
        }
        
        for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
        {
            resultTPTx[i] = CAN_MsgTx( &CANMsgTx_Saej_TP[i], true, i );
            resultTPTx[i] = CAN_MsgTx( &CANMsgTx_Saej_TP_DT[i], true, i );
            resultETPTx[i] = CAN_MsgTx( &CANMsgTx_Saej_ETP[i], true, i );
            resultETPTx[i] = CAN_MsgTx( &CANMsgTx_Saej_ETP_DT[i], true, i );
        }
        CAN_MsgTx( &CANMsgTx_Saej_RequestPGN, false, 0xFF );
#ifdef DM1_ENABLED
        if ( true == Saej_error.active )
        {
            CAN_MsgTx( &CANMsgTx_Saej_DM1, false, 0xFF );
        }
        else if ( true == Saej_DM1reqPGN )
        {
            CAN_MsgTx( &CANMsgTx_Saej_DM1, false, 0xFF );
            Saej_DM1reqPGN = false;
        }
        else
        {
            //faccio il free run del timer del DM1 così è sempre sincronizzato
            if ( CANMsgTx_Saej_DM1.TimeOut > 0U ) 
            {
                CANMsgTx_Saej_DM1.TimeCnt++;
                if ( CANMsgTx_Saej_DM1.TimeCnt >= CANMsgTx_Saej_DM1.TimeOut ) 
                {
                    CANMsgTx_Saej_DM1.TimeCnt = 0U;
                }
            }
        }
#endif        
    }

    return;
}

////------------------------------------------------------------------------------
//static void Saej_CANMessageCfgUpdate( void )
//{
//    uint8_t i;
//    uint32_t auxId;
//    
//    for ( i=0U, i<SAEJ_CAN_MSG_NUM_MAX; i++ )
//    {
//        if ( CAN_MSG_RX == CANMsg_Saej[i].RxTx )
//        {
//            auxId = SAEJ_CAN_MSG_DATA[i][SAEJ_CAN_MSG_DATA_INDEX_ID] + ( (uint32_t)( Saej_address ) * 256U );
//        }
//        else
//        {
//            auxId = SAEJ_CAN_MSG_DATA[i][SAEJ_CAN_MSG_DATA_INDEX_ID] + (uint32_t)( Saej_address );
//        }
//        CAN_MsgUpdateID( CANMsg_Saej[i], auxId, CAN_ID_29BIT );
//    }
//      
//    return;
//}

//------------------------------------------------------------------------------
uint8_t Saej_PushButtonStatusGet( uint8_t byteIndex )
{
    return Saej_pushButtonStatus[byteIndex];
}

//------------------------------------------------------------------------------
wres_t Saej_TPTxEnable( uint8_t index, uint32_t PGN, bool isBroadCast, uint8_t destAddr, uint32_t byteNum, uint32_t packetNum, uint8_t *buffInitSource, const uint8_t *buffSource )
{
    uint16_t i;
    wres_t retVal;
    
    retVal = WRES_OPERATION_IN_PROGRESS;
    if ( false == Saej_TPTx[index].isBusy )
    {
        Saej_TPTx[index].isBroadcast = isBroadCast;
        Saej_TPTx[index].byteNum = byteNum;
        Saej_TPTx[index].packetNum = packetNum;
        Saej_TPTx[index].PGN = PGN;
        Saej_TPTx[index].packetNumToSend = (uint8_t)packetNum;
        Saej_TPTx[index].nextPacket = 0U;
        Saej_TPTx[index].packetCnt = 0U; 
        Saej_TPTx[index].byteCnt = 0U;   
        Saej_TPTx[index].destAddress = destAddr;
        Saej_TPTx[index].buffSource = &buffSource[0U];  
        for ( i=0U; i<SAEJ_TP_BYTE_NUM_MAX; i++ )
        {
            Saej_TPTx[index].Saej_tpTxBuff[i] = *(buffInitSource + i);
        }
        Saej_TPTx[index].isBusy = true;
        retVal = WRES_OK;
    }
  
    return retVal;
}

//------------------------------------------------------------------------------
wres_t Saej_TPTxStatusGet( uint8_t index )
{
    wres_t retVal;

    retVal = WRES_OPERATION_IN_PROGRESS; 
    switch ( Saej_TPTx[index].status.value )
    {
    case SAEJ_STATUS_TP_OFF:
        retVal = WRES_OK;
        break;
    case SAEJ_STATUS_TP_COMPLETED:
        retVal = WRES_OPERATION_COMPLETED;
        break;
    case SAEJ_STATUS_TP_CM_ABORT:
        retVal = WRES_ERROR;
        break;
    default:
        retVal = WRES_OPERATION_IN_PROGRESS;
        break;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
void Saej_SendPGNRequest( uint32_t PGN, uint8_t destAddr )
{
    CAN_MsgUpdateID( &CANMsgTx_Saej_RequestPGN, ( ( CANMsgTx_Saej_RequestPGN.ID & 0xFFFF00FF ) + ( (uint32_t)( destAddr ) * 256U ) ), CAN_ID_29BIT, CAN_SOURCE_ADDRESS_CHECK_YES );  
    CANMsgTx_Saej_RequestPGN.Data[0U] = (uint8_t)( PGN );
    CANMsgTx_Saej_RequestPGN.Data[1U] = (uint8_t)( PGN >> 8U );
    CANMsgTx_Saej_RequestPGN.Data[2U] = (uint8_t)( PGN >> 16U );
    CAN_MsgUpdateData( &CANMsgTx_Saej_RequestPGN, &CANMsgTx_Saej_RequestPGN.Data[0U], CAN_MSG_TX_YES );
  
    return;
}

//------------------------------------------------------------------------------
uint8_t Saej_SourceAddressGet( void )
{
    return Saej_address;
}

//------------------------------------------------------------------------------
uint32_t Saej_CfgAxesGet( void )
{
    return Saej_cfgAxes;
}

//------------------------------------------------------------------------------
uint16_t Saej_CfgPushButtonGet( void )
{
    return Saej_cfgPushButton;
}

//------------------------------------------------------------------------------
uint16_t Saej_CfgDetentGet( void )
{
    return Saej_cfgDetent;
}

//------------------------------------------------------------------------------
uint8_t Saej_PushButtonIndexGet( uint8_t buttonNum )
{
    return Saej_pushButtonIndex[buttonNum];
}

//------------------------------------------------------------------------------
uint8_t Saej_PushButtonEnGet( uint8_t buttonNum )
{
    return Saej_pushButtonEn[buttonNum];
}

//------------------------------------------------------------------------------
uint8_t Saej_RollerIndexGet( uint8_t rollerNum )
{
    return Saej_rollerIndex[rollerNum];
}

//------------------------------------------------------------------------------
Saej_axisOut_t Saej_AxisGet( uint8_t axisIndex )
{
    return Saej_axis[axisIndex];
}

#ifdef ISOBUS_DJW
//------------------------------------------------------------------------------
void Isobus_TxClaim( void )
{
    if ( SAEJ_STATUS_NORMAL == Saej_status.value )
    {
        Saej_TxAddressClaim( SAEJ_ADDRESS_CLAIMED );
    }
    return;
}
#endif

//------------------------------------------------------------------------------
void Saej_Init( void )
{
  static const Saej_spn_t SAEJ_SPN[SAEJ_SPN_NUM] = 
  {
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_MLX_0,         0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //MLX 0 - 0x0A64UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_MLX_1,         0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //MLX 1 - 0x0A65UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ROLLER +  2UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //asse w - 0x0A68UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ROLLER,        0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //asse z - 0x0A66UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ROLLER +  1UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //asse j - 0x0A67UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ROLLER +  3UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //asse q - 0x0A69UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ROLLER +  4UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //asse v - 0x0A6AUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON,        0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 1 - 0x0A70UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  1UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 2 - 0x0A71UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  2UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 3 - 0x0A72UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  3UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 4 - 0x0A73UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  4UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 5 - 0x0A74UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  5UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 6 - 0x0A75UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  6UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 7 - 0x0A76UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  7UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 8 - 0x0A77UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  8UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 9 - 0x0A78UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON +  9UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 10 - 0x0A79UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON + 10UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 11 - 0x0A7AUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_BUTTON + 11UL, 0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 12 - 0x0A7BUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, 0x6731,                0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //pulsante 13 - 0x6731UL (come da SAEJ)
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_MEMORY,        0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //eeprom - 0x0274UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_SUPPLY,        0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //alimentazione - 0x009EUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_CAN,           0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //can - 0x0263UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ADC0_GROUP_0,  0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //adc0.0 - 0x043BUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ADC0_GROUP_1,  0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //adc0.1 - 0x043CUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ADC1_GROUP_0,  0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //adc1.0 - 0x103DUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_ADC1_GROUP_1,  0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //adc1.1 - 0x103EUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_SYNC_TE,       0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //SYNC TE - 0x0271UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_TLF,           0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //TLF - 0x15D4UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_WSL,           0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //WSL - 0x2522UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_CF,            0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //control flow - 0x2523UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_NETWORK,       0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //J1939 network - 0x027FUL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_SPI_TE,        0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //network service status 1 - 0x0D29UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_SPI_TE_INIT,   0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //network service status 2 - 0x0D73UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_SOFT_TE,       0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //TE soft error - 0x0D28UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_LED,           0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //LED timeout error - 0x4460UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_LED + 1UL,     0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //LED status 0 - 0x4461UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_LED + 2UL,     0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //LED status 1 - 0x4462UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_LED + 3UL,     0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //LED status 2 - 0x4463UL
      { SAEJ_LAMP_AMBER_WARNING_ON, SAEJ_LAMP_NA, SPN_ERR_LED + 4UL,     0x00UL, SAEJ_FMI_NA, SAEJ_FMI_NA, 0x00U, EMPTY_ARRAY},  //LED status 3 - 0x4464UL
  };

  static const uint16_t SAEJ_AXIS_DETENT_TH[SAEJ_AXIS_NUM][SAEJ_DIRECTION_NUM] = 
  {
    {SAEJ_AXIS_X_DETENT_TH_POS, SAEJ_AXIS_X_DETENT_TH_NEG},
    {SAEJ_AXIS_Y_DETENT_TH_POS, SAEJ_AXIS_Y_DETENT_TH_NEG},
    {SAEJ_AXIS_Z_DETENT_TH_POS, SAEJ_AXIS_Z_DETENT_TH_NEG},
    {SAEJ_AXIS_W_DETENT_TH_POS, SAEJ_AXIS_W_DETENT_TH_NEG},
    {SAEJ_AXIS_J_DETENT_TH_POS, SAEJ_AXIS_J_DETENT_TH_NEG},
    {SAEJ_AXIS_Q_DETENT_TH_POS, SAEJ_AXIS_Q_DETENT_TH_NEG},
    {SAEJ_AXIS_V_DETENT_TH_POS, SAEJ_AXIS_V_DETENT_TH_NEG},
  };
  static const uint8_t SAEJ_PUSH_BUTTON_INDEX[SAEJ_PUSH_BUTTON_NUM] =
  {
    SAEJ_BUTTON_1_INDEX,
    SAEJ_BUTTON_2_INDEX,
    SAEJ_BUTTON_3_INDEX,
    SAEJ_BUTTON_4_INDEX,
    SAEJ_BUTTON_5_INDEX,
    SAEJ_BUTTON_6_INDEX,
    SAEJ_BUTTON_7_INDEX,
    SAEJ_BUTTON_8_INDEX,
    SAEJ_BUTTON_9_INDEX,
    SAEJ_BUTTON_10_INDEX,
    SAEJ_BUTTON_11_INDEX,
    SAEJ_BUTTON_12_INDEX,
    SAEJ_BUTTON_13_INDEX,
  };
  static const uint8_t DUT_ASCII_CODE[9U] = 
  {
    ASC_DUT_CODE_1, ASC_DUT_CODE_2, ASC_DUT_CODE_3, ASC_DUT_CODE_4,
    ASC_DUT_CODE_5, ASC_DUT_CODE_6, ASC_DUT_CODE_7, ASC_DUT_CODE_8, ASC_DUT_CODE_9,
  };
  static const uint8_t DUT_HARDWARE_ID[6U] = 
  {
    HWID_0, HWID_1, HWID_2, HWID_3, HWID_4, HWID_5,
  };
  static const uint8_t DUT_ASCII_MANUFACTURER_CODE[7U] =
  {
    ASC_MANUFACTURER_CODE_1, ASC_MANUFACTURER_CODE_2, ASC_MANUFACTURER_CODE_3,
    ASC_MANUFACTURER_CODE_4, ASC_MANUFACTURER_CODE_5, ASC_MANUFACTURER_CODE_6, ASC_MANUFACTURER_CODE_7,
  };
  
    uint8_t i;  
    uint8_t j;
  
    Saej_cfgAxes = SAEJ_DEFAULT_CFG_AXES;
    Saej_cfgPushButton = SAEJ_DEFAULT_CFG_PUSH_BUTTON;
    Saej_cfgDetent = SAEJ_DEFAULT_CFG_DETENT;
    memset( Saej_pushButtonEn, (int8_t)SAEJ_P_DISABLE, SAEJ_PUSH_BUTTON_NUM );
    memset( Saej_bufferTxBJM, 0xFF, 8U );
    memset( Saej_bufferTxEJM, 0xFF, 8U );
    memset( Saej_bufferTxEJM2, 0xFF, 8U );
    memset( CANMsgTx_Saej_DM1.Data, 0xFF, 8U );
    memset( CANMsgTx_Saej_RequestAddressClaim.Data, 0xFF, 8U );
    memset( CANMsgTx_Saej_AddressClaim.Data, 0xFF, 8U );
    memset( CANMsgTx_Saej_DM15.Data, 0xFF, 8U );
    memset( CANMsgTx_Saej_DM16.Data, 0xFF, 8U );
    

//    CAN_MsgEnable( &CANMsgRx_Saej_ContendingAddress, CAN_MODULE_0, 14, CAN_MSG_RX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
//    CAN_MsgEnable( &CANMsgRx_Saej_DM14, CAN_MODULE_0, 15, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
//    CAN_MsgEnable( &CANMsgRx_Saej_DM16, CAN_MODULE_0, 18, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_DM16 & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
//    CAN_MsgEnable( &CANMsgRx_Saej_RequestPGN, CAN_MODULE_0, 19, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 3U, 0U, CAN_MSG_IS_NOT_RTR );
//    CAN_MsgEnable( &CANMsgRx_Saej_RequestPGNAll, CAN_MODULE_0, 20, CAN_MSG_RX, CAN_ID_29BIT, SAEJ_CAN_MSG_ID_REQUEST_PGN, CAN_SOURCE_ADDRESS_CHECK_NO, 3U, 0U, CAN_MSG_IS_NOT_RTR );    
//    CAN_MsgEnable( &CANMsgRx_Saej_TP, CAN_MODULE_0, 22, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
//    CAN_MsgEnable( &CANMsgRx_Saej_TPAll, CAN_MODULE_0, 23, CAN_MSG_RX, CAN_ID_29BIT, SAEJ_CAN_MSG_ID_TP, CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
//    CAN_MsgEnable( &CANMsgRx_Saej_TP_DT, CAN_MODULE_0, 24, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
//    CAN_MsgEnable( &CANMsgRx_Saej_TPAll_DT, CAN_MODULE_0, 25, CAN_MSG_RX, CAN_ID_29BIT, SAEJ_CAN_MSG_ID_TP_DT, CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
//    CAN_MsgEnable( &CANMsgRx_Saej_Led, CAN_MODULE_0, 28, CAN_MSG_RX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_LED & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR ); 
//    CAN_MsgEnable( &CANMsgRx_Saej_ETP, CAN_MODULE_0, 34, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    
    for ( i=0U; i<CAN_RX_BUFFER_NUM_MAX; i++ )
    {
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_CONTENDING_ADDRESS, i, SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + SAEJ_DEFAULT_SA,                                    CAN_SOURCE_ADDRESS_CHECK_YES );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_DM14,               i, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ),  CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_DM16,               i,  ( SAEJ_CAN_MSG_ID_DM16 & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ),     CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_REQUEST_PGN,        i, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) ) ),  CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_REQUEST_PGN_ALL,    i, SAEJ_CAN_MSG_ID_REQUEST_PGN,                                                             CAN_SOURCE_ADDRESS_CHECK_NO );      
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP,                 i, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ),    CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_ALL,             i, SAEJ_CAN_MSG_ID_TP,                                                                      CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_DT,              i, ( ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_TP_DT_ALL,          i, SAEJ_CAN_MSG_ID_TP_DT,                                                                   CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_LED,                i, ( SAEJ_CAN_MSG_ID_LED & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ),       CAN_SOURCE_ADDRESS_CHECK_NO );
        CAN_StoredMsgUpdate( CAN_MSG_SAEJ_ETP,                i, ( ( SAEJ_CAN_MSG_ID_ETP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ),   CAN_SOURCE_ADDRESS_CHECK_NO );
    }

    CAN_MsgEnable( &CANMsgTx_Saej_BJM,  CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_BJM + SAEJ_DEFAULT_SA ),  CAN_SOURCE_ADDRESS_CHECK_YES, 8U, ( SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US / SCHEDULE_COUNTER_TH_SAEJ ), CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_EJM,  CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_EJM + SAEJ_DEFAULT_SA ),  CAN_SOURCE_ADDRESS_CHECK_YES, 8U, ( SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US / SCHEDULE_COUNTER_TH_SAEJ ), CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_EJM2, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_EJM2 + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, ( SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US / SCHEDULE_COUNTER_TH_SAEJ ), CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_DM1, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_DM1 + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, ( SAEJ_CAN_MSG_DM1_TX_TIME_OUT_US / SCHEDULE_COUNTER_TH_SAEJ ), CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_RequestAddressClaim, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_REQUEST_ADDRESS_CLAIM + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_YES, 3U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Saej_AddressClaim, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_ADDRESS_CLAIM + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Saej_DM15, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_DM15 & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Saej_DM16, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_DM16 & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    CAN_MsgEnable( &CANMsgTx_Saej_SendPGN, CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_SEND_PGN & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA ), CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_TP[0U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_TP & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_TP_DT[0U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_ETP[0U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_ETP & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_ETP_DT[0U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_ETP_DT & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_TP[1U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_TP & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_TP_DT[1U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_TP_DT & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_ETP[1U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_ETP & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_ETP_DT[1U], CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( SAEJ_CAN_MSG_ID_ETP_DT & 0xFFFFFF00UL ) + SAEJ_DEFAULT_SA, CAN_SOURCE_ADDRESS_CHECK_YES, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    CAN_MsgEnable( &CANMsgTx_Saej_RequestPGN,          CAN_MODULE_0, 0U, CAN_MSG_TX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFFFF00UL ) + (uint32_t)SAEJ_DEFAULT_SA ),   CAN_SOURCE_ADDRESS_CHECK_YES, 3U, 0U, CAN_MSG_IS_NOT_RTR );    
    

    //CAN_MsgEnable( &CANMsgRx_Saej_TP_CM[0U], CAN_MODULE_0, 34, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );
    //CAN_MsgEnable( &CANMsgRx_Saej_TP_CM[1U], CAN_MODULE_0, 35, CAN_MSG_RX, CAN_ID_29BIT, ( ( SAEJ_CAN_MSG_ID_TP & 0xFFFF00FFUL ) + ( (uint32_t)( SAEJ_DEFAULT_SA ) * 256U ) ), CAN_SOURCE_ADDRESS_CHECK_NO, 8U, 0U, CAN_MSG_IS_NOT_RTR );    
    
    for ( i=0U; i<SAEJ_AXIS_NUM; i++ )
    {
        Saej_axis[i].value = 0xFFFFU;
        Saej_axis[i].previousValue = 0xFFFFU;
        Saej_axis[i].status = 0x03FU;
        Saej_axis[i].detentOut = 0xFFU; 
        Saej_axis[i].detentOutPrevious = 0xFFU; 
        Saej_axis[i].detentThPos = SAEJ_AXIS_DETENT_TH[i][SAEJ_DIRECTION_POS];
        Saej_axis[i].detentThNeg = SAEJ_AXIS_DETENT_TH[i][SAEJ_DIRECTION_NEG];
        //Saej_axis[i].detentEn = SAEJ_AXIS_DETENT_EN[i];
        Saej_axis[i].detentEn = (uint8_t)( Saej_cfgDetent >> ( 2U * i ) ) & 0x03U;
        //Saej_axis[i].outInverted = SAEJ_AXIS_OUT_INVERTED[i];
        Saej_axis[i].outInverted = ( (uint8_t)( Saej_cfgAxes >> ( 1U + ( 2U* i ) ) ) & 0x01U ) != 0u;
        Saej_axis[i].changed = false;
        Saej_axis[i].notWaitNeutralAtStartup = false;
        Saej_axis[i].startupDone = false;
    }
    
    Saej_rollerIndex[SAEJ_ROLLER_W] = ROLLER_W;
    Saej_rollerIndex[SAEJ_ROLLER_Z] = ROLLER_Z;
    Saej_rollerIndex[SAEJ_ROLLER_J] = ROLLER_J;
    Saej_rollerIndex[SAEJ_ROLLER_Q] = ROLLER_Q;
    Saej_rollerIndex[SAEJ_ROLLER_V] = ROLLER_V;
    
    for ( i=0U; i<SAEJ_SPN_NUM; i++ )
    {
        Saej_SPN[i].lampStatus = SAEJ_SPN[i].lampStatus;
        Saej_SPN[i].flashLampStatus = SAEJ_SPN[i].flashLampStatus;
        Saej_SPN[i].value = SAEJ_SPN[i].value;
        Saej_SPN[i].previous = SAEJ_SPN[i].previous;
        Saej_SPN[i].FMI = SAEJ_SPN[i].FMI;
        Saej_SPN[i].count = SAEJ_SPN[i].count;
        for ( j=0U; j<(uint8_t)SAEJ_FMI_NUM_MAX; j++ )
        {
            Saej_SPN[i].FMI_count[j] = 0x00U;
        }
    }
    
    for ( i=0U; i<SAEJ_PUSH_BUTTON_NUM; i++ )
    {
        Saej_pushButtonIndex[i] = SAEJ_PUSH_BUTTON_INDEX[i];
    }
    
    Saej_status.value = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM;
    Saej_status.previous = SAEJ_STATUS_INIT;
    Saej_status.cnt = 0U;
    Saej_status.cntTh = SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM_CNT_TH / SCHEDULE_COUNTER_TH_SAEJ;
    
    Saej_addressClaimResult = SAEJ_ADDRESS_TO_CLAIM;
    Saej_defaultAddress = SAEJ_DEFAULT_SA;
    Saej_address = SAEJ_DEFAULT_SA;
    Saej_addressClaimedNum = 0U;
    Saej_isSelfConfigurableDevice = SAEJ_SELF_CONFIGURABLE_EN; //SAEJ_IS_SELF_CONFIGURABLE;
        
    Saej_statusTPRx.value = SAEJ_STATUS_TP_OFF;
    Saej_statusTPRx.previous = SAEJ_STATUS_TP_INIT;
    Saej_statusTPRx.cnt = 0U;
    Saej_statusTPRx.cntTh = 0U;
    
    Saej_TPRx.isBroadcast = false;
    Saej_TPRx.byteNum = 0U;
    Saej_TPRx.packetNum = 0U;
    Saej_TPRx.PGN = 0xFFFFFFUL;
    Saej_TPRx.packetNumToSend = 0U;
    Saej_TPRx.nextPacket = 0U;
    Saej_TPRx.packetCnt = 0U; 
    Saej_TPRx.byteCnt = 0U;
    Saej_TPRx.abortValue = SAEJ_TP_ABORT_NULL;

    Saej_statusDM1.value = SAEJ_STATUS_DM1_OFF;
    Saej_error.active = false;
    Saej_error.value = 0U;
#ifdef DM1_ENABLED    
    Saej_DM1reqPGN = false;
#endif    
    
    for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
    {
        Saej_TPTx[i].isBroadcast = false;
        Saej_TPTx[i].isETP = false;
        Saej_TPTx[i].isBusy = false;
        Saej_TPTx[i].byteNum = 0U;
        Saej_TPTx[i].packetNum = 0U;
        Saej_TPTx[i].PGN = 0xFFFFFFUL;
        Saej_TPTx[i].packetNumToSend = 0U;
        Saej_TPTx[i].nextPacket = 0U;
        Saej_TPTx[i].packetCnt = 0U; 
        Saej_TPTx[i].byteCnt = 0U;   
        Saej_TPTx[i].cmRx = SAEJ_TP_CM_RX_NO;
        Saej_TPTx[i].abortValue = SAEJ_TP_ABORT_NULL;
        Saej_TPTx[i].destAddress = 0xFFU;
        Saej_TPTx[i].status.value = SAEJ_STATUS_TP_OFF;
        Saej_TPTx[i].status.previous = SAEJ_STATUS_TP_OFF;
        Saej_TPTx[i].status.cnt = 0U;
        Saej_TPTx[i].status.cntTh = 0U;      
    }
    
    Saej_commandedAddressStatus = SAEJ_COMMANDED_ADDRESS_STATUS_OFF;

    Saej_isoName[0U] = 0x4C200000UL;
    Saej_isoName[1U] = 0x20008100UL;
    
//    if ( SAEJ_IS_SELF_CONFIGURABLE == Saej_isSelfConfigurableDevice )
//    {
        Saej_isoName[1U] |= SAEJ_ISO_NAME_SELF_CONFIGURABLE_BIT_MASK;
//    }
//    else
//    {
//        Saej_isoName[1U] &= ~SAEJ_ISO_NAME_SELF_CONFIGURABLE_BIT_MASK;
//    }
    
    Saej_ledCmd.active = false;
    Saej_ledCmd.en = false;
    Saej_ledCmd.cnt = 0U;
    Saej_ledCmd.cntTh = ( SAEJ_LED_CMD_TIMEOUT_US / SCHEDULE_COUNTER_TH_SAEJ );
    Saej_ledCmdEn = SAEJ_DEFAULT_LED_CMD_EN;
    Saej_ledEn = SAEJ_DEFAULT_LED_EN;
    
    Saej_ledDrive = 0XFFU;
    Saej_ledDuty = 0xFFU;

    Saej_ledInternalValue = 0U;
    Saej_ledInternalDuty = 0xFFU;
    Saej_ledErrorIndex = SAEJ_LED_ERROR_INDEX;    
    
    Saej_msgJoyTimeOut_us = SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US;
    Saej_msgJoyOnChangeBaseTimeOut_us = SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_BASE_TIME_OUT_US;
    Saej_msgJoyOnChangeRefreshTimeOut_us = SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_REFRESH_TIME_OUT_US;
    
    Saej_txOnChange = SAEJ_JOY_TX_ON_CHANGE_EN;
    
    saej_FNR.enabled = SAEJ_FNR_DEFAULT_EN;
    saej_FNR.inputF = SAEJ_F_INDEX;
    saej_FNR.inputR = SAEJ_R_INDEX;
    saej_FNR.outputN = SAEJ_N_INDEX;
    
    saej_wstButtonIndex_1_8 = (uint32_t)( SAEJ_WST_BUTTON_1_INDEX & 0x0Fu );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_2_INDEX & 0x0Fu ) << 4u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_3_INDEX & 0x0Fu ) << 8u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_4_INDEX & 0x0Fu ) << 12u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_5_INDEX & 0x0Fu ) << 16u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_6_INDEX & 0x0Fu ) << 20u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_7_INDEX & 0x0Fu ) << 24u );
    saej_wstButtonIndex_1_8 |= (uint32_t)( ( SAEJ_WST_BUTTON_8_INDEX & 0x0Fu ) << 28u );
    saej_wstButtonIndex_9_15 = (uint32_t)( SAEJ_WST_BUTTON_9_INDEX & 0x0Fu );
    saej_wstButtonIndex_9_15 |= (uint32_t)( ( SAEJ_WST_BUTTON_10_INDEX & 0x0Fu ) << 4u );
    saej_wstButtonIndex_9_15 |= (uint32_t)( ( SAEJ_WST_BUTTON_11_INDEX & 0x0Fu ) << 8u );
    saej_wstButtonIndex_9_15 |= (uint32_t)( ( SAEJ_WST_BUTTON_12_INDEX & 0x0Fu ) << 12u );
    saej_wstButtonIndex_9_15 |= (uint32_t)( ( SAEJ_WST_BUTTON_13_INDEX & 0x0Fu ) << 16u );
    
    memset( Saej_dutSerialNum, 0xFF, 8U );
    for ( i=0U; i<9U; i++ )
    {
        Saej_dutAsciiCode[i] = DUT_ASCII_CODE[i];
    }
    for ( i=0U; i<6U; i++ )
    {
        Saej_dutHardwareId[i] = DUT_HARDWARE_ID[i];
    }
    for ( i=0U; i<7U; i++ )
    {
        Saej_dutAsciiManufacturerCode[i] = DUT_ASCII_MANUFACTURER_CODE[i];
    }
    
    Saej_dutManufacturerCode = MANUFACTURER_CODE;
    Saej_isobusEn = SAEJ_ISOBUS_ENABLE_NO;

#ifdef ISOBUS_DJW  
    Saej_isobusEn = SAEJ_ISOBUS_ENABLE_YES;
    Isobus_Init();
#endif
    
    Saej_paramToRAMUpdate();
    
    return;
}

//------------------------------------------------------------------------------
void Saej_Main( uint8_t index )
{
    (void)index;
    uint8_t i;
    static bool first_entry_in_main = true;
    
    if ( true == first_entry_in_main )
    {
        Saej_paramFromRAMUpdate();

#ifndef ISOBUS_DJW     
        if ( true == Saej_txOnChange )
        {
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM,  (uint16_t)( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM,  (uint16_t)( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, (uint16_t)( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );   
        }
        else
        {
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM,  (uint16_t)( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM,  (uint16_t)( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, (uint16_t)( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
        }
#else
#ifdef ISOBUS_DEBUG
        if ( true == Saej_txOnChange )
        {
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, ( Saej_msgJoyOnChangeBaseTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );            
        }
        else
        {
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
            CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, ( Saej_msgJoyTimeOut_us / SCHEDULE_COUNTER_TH_SAEJ ) );
        }
#else
        CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_BJM, 0U );
        CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM, 0U );
        CAN_MsgUpdateTimeOut( &CANMsgTx_Saej_EJM2, 0U );        
#endif
#endif
        if ( SAEJ_IS_SELF_CONFIGURABLE == Saej_isSelfConfigurableDevice )
        {
            Saej_isoName[1U] |= SAEJ_ISO_NAME_SELF_CONFIGURABLE_BIT_MASK;
        }
        else
        {
            Saej_isoName[1U] &= ~SAEJ_ISO_NAME_SELF_CONFIGURABLE_BIT_MASK;
        }
        
        Saej_isoName[0U] &= ~SAEJ_ISO_NAME_HW_ID_MASK;
        Saej_isoName[0U] |= (uint32_t)( Saej_dutSerialNum[4U] & 0x000000FFUL );
        Saej_isoName[0U] |= ( (uint32_t)( Saej_dutSerialNum[5U] & 0x00000007UL ) << 8U  );
        Saej_isoName[0U] |= ( (uint32_t)( Saej_dutSerialNum[6U] & 0x0000001FUL ) << 11U );
        Saej_isoName[0U] |= ( (uint32_t)( Saej_dutSerialNum[0U] & 0x0000001FUL ) << 16U );
        Saej_isoName[0U] |= ( (uint32_t)( Saej_dutManufacturerCode & 0x07FFUL ) << 21U );
        
        if ( SAEJ_BUTTON_1_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_1_ENABLED ) ) 
        { 
            Saej_pushButtonEn[0U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_2_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_2_ENABLED ) ) 
        { 
            Saej_pushButtonEn[1U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_3_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_3_ENABLED ) ) 
        { 
            Saej_pushButtonEn[2U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_4_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_4_ENABLED ) ) 
        { 
            Saej_pushButtonEn[3U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_5_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_5_ENABLED ) ) 
        { 
            Saej_pushButtonEn[4U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_6_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_6_ENABLED ) ) 
        { 
            Saej_pushButtonEn[5U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_7_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_7_ENABLED ) ) 
        { 
            Saej_pushButtonEn[6U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_8_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_8_ENABLED ) ) 
        { 
            Saej_pushButtonEn[7U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_9_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_9_ENABLED ) ) 
        { 
            Saej_pushButtonEn[8U] = SAEJ_P_ENABLE; 
        }  
        if ( SAEJ_BUTTON_10_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_10_ENABLED ) ) 
        { 
            Saej_pushButtonEn[9U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_11_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_11_ENABLED ) ) 
        { 
            Saej_pushButtonEn[10U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_12_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_12_ENABLED ) ) 
        { 
            Saej_pushButtonEn[11U] = SAEJ_P_ENABLE; 
        }
        if ( SAEJ_BUTTON_13_ENABLED == ( Saej_cfgPushButton & SAEJ_BUTTON_13_ENABLED ) ) 
        { 
            Saej_pushButtonEn[12U] = SAEJ_P_ENABLE; 
        }           
        
        //S32K_Led_Start( 0x00 );

        first_entry_in_main = false;
    }

//    CAN_MsgUpdateID( &CANMsgTx_Saej_AddressClaim, ( SAEJ_CAN_MSG_ID_ADDRESS_CLAIM + Saej_address ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgRx_Saej_ContendingAddress, ( SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS + Saej_address ), CAN_ID_29BIT );    
//    CAN_MsgUpdateID( &CANMsgRx_Saej_DM14, ( ( SAEJ_CAN_MSG_ID_DM14 & 0xFFFF00FFUL ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgTx_Saej_DM15, ( SAEJ_CAN_MSG_ID_DM15 + Saej_address ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgTx_Saej_DM16, ( SAEJ_CAN_MSG_ID_DM16_TX + Saej_address ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgRx_Saej_DM16, ( SAEJ_CAN_MSG_ID_DM16_RX + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgRx_Saej_RequestPGN, ( ( SAEJ_CAN_MSG_ID_REQUEST_PGN & 0xFFFF00FF ) + ( (uint32_t)( Saej_address ) * 256U ) ), CAN_ID_29BIT );
//    CAN_MsgUpdateID( &CANMsgTx_Saej_SendPGN, ( SAEJ_CAN_MSG_ID_SEND_PGN + Saej_address ), CAN_ID_29BIT );        

//    CAN_Main();
    Saej_RxMsgServe();
    
    switch ( Saej_status.value ) 
    {
    case SAEJ_STATUS_WAIT_FOR_ADDRESS_CLAIM:
      Saej_StatusExecWaitForAddressClaim();
      break;
    case SAEJ_STATUS_WAIT_CANNOT_CLAIM:
      Saej_StatusExecWaitCanNotClaim();
      break;
    case SAEJ_STATUS_WAIT_TO_NORMAL:
      Saej_StatusExecWaitToNormal();
      break;
    case SAEJ_STATUS_NORMAL:
      Saej_StatusExecNormal();
      Saej_TPRxMain();
      for ( i=0U; i<SAEJ_TP_SLOT_NUM_MAX; i++ )
      {
          Saej_TPTxMain( i, resultTPTx[i], resultETPTx[i] );
      }
      Saej_DM1Main();
      break;
    case SAEJ_STATUS_CANNOT_CLAIM:
      Saej_StatusExecCanNotClaim();
      break;
    case SAEJ_STATUS_OFF:
      Saej_StatusExecOff();
      break;
      
    default:
      break;
    }  
    
    Saej_TxMsgServe();
    
    return;
}

