/*
 * saej1939.h
 *
 *  Created on: 20 jun 2018
 *      Author: PoliD
 */

#ifndef _SAEJ1939_H
#define _SAEJ1939_H

#define SAEJ_IS_SELF_CONFIGURABLE     ( 1U )
#define SAEJ_IS_NOT_SELF_CONFIGURABLE ( 0U ) 

#define SAEJ_CHR_SW_ID_D0    PRG_CHR_APP_ID_D0
#define SAEJ_CHR_SW_ID_D1    PRG_CHR_APP_ID_D1
#define SAEJ_CHR_SW_ID_D2    PRG_CHR_APP_ID_D2
#define SAEJ_CHR_SW_ID_D3    PRG_CHR_APP_ID_D3
#define SAEJ_CHR_SW_ID_D4    PRG_CHR_APP_ID_D4
#define SAEJ_CHR_SW_ID_D5    PRG_CHR_APP_ID_D5

#define SAEJ_CAN_MSG_ID_BJM                   ( 0x0CFDD600UL )
#define SAEJ_CAN_MSG_ID_EJM                   ( 0x0CFDD700UL )
#define SAEJ_CAN_MSG_ID_EJM2                  ( 0x0CFDD900UL )
#define SAEJ_CAN_MSG_ID_DM1                   ( 0x18FECA00UL )
#define SAEJ_CAN_MSG_ID_SOFTWARE_ID           ( 0x18FEDA00UL )
#define SAEJ_CAN_MSG_ID_ECU_DIAGNOSTIC        ( 0x18FD3200UL )
#define SAEJ_CAN_MSG_ID_REQUEST_ADDRESS_CLAIM ( 0x18EA00FEUL )
#define SAEJ_CAN_MSG_ID_ADDRESS_CLAIM         ( 0x18EEFF00UL ) 
#define SAEJ_CAN_MSG_ID_CONTENDING_ADDRESS    ( 0x18EEFF00UL )
#define SAEJ_CAN_MSG_ID_DM14                  ( 0x18D9FFF9UL ) 
#define SAEJ_CAN_MSG_ID_DM15                  ( 0x18D8F9FFUL )
#define SAEJ_CAN_MSG_ID_DM16                  ( 0x18D7FFFFUL )
#define SAEJ_CAN_MSG_ID_REQUEST_PGN           ( 0x18EAFFFFUL )
#define SAEJ_CAN_MSG_ID_SEND_PGN              ( 0x18E8FFFFUL )
#define SAEJ_CAN_MSG_ID_TP                    ( 0x1CECFFFFUL )
#define SAEJ_CAN_MSG_ID_TP_DT                 ( 0x1CEBFFFFUL )
#define SAEJ_CAN_MSG_ID_ETP                   ( 0x1CC8FFFFUL )
#define SAEJ_CAN_MSG_ID_ETP_DT                ( 0x1CC7FFFFUL )
#define SAEJ_CAN_MSG_ID_LED                   ( 0x0C99FFFFUL )

#define SAEJ_CAN_MSG_DM1_TX_TIME_OUT_US       ( 1000000U )

/* definizione assi e roller */
#define SAEJ_AXIS_X            ( 0U )
#define SAEJ_AXIS_Y            ( 1U )
#define SAEJ_AXIS_W            ( 2U )
#define SAEJ_AXIS_Z            ( 3U )
#define SAEJ_AXIS_J            ( 4U )
#define SAEJ_AXIS_Q            ( 5U )
#define SAEJ_AXIS_V            ( 6U )
#define SAEJ_AXIS_NUM          ( 7U )

#define SAEJ_ROLLER_W          ( 0U )
#define SAEJ_ROLLER_Z          ( 1U )
#define SAEJ_ROLLER_J          ( 2U )
#define SAEJ_ROLLER_Q          ( 3U )
#define SAEJ_ROLLER_V          ( 4U )
#define SAEJ_ROLLER_NUM        ( 5U )

//------------------------------------------------------------------------------
#define SAEJ_PUSH_BUTTON_STATUS_BYTE_NUM  ( 4U )
   
#define SAEJ_AXIS_X_ENABLED       ( 0x00000001U )
#define SAEJ_AXIS_Y_ENABLED       ( 0x00000004U )
#define SAEJ_AXIS_W_ENABLED       ( 0x00000010U )
#define SAEJ_AXIS_Z_ENABLED       ( 0x00000040U )
#define SAEJ_AXIS_J_ENABLED       ( 0x00000100U )
#define SAEJ_AXIS_Q_ENABLED       ( 0x00000400U )
#define SAEJ_AXIS_V_ENABLED       ( 0x00001000U )

#define SAEJ_AXIS_WZJ_ENABLED     ( 0x00000150U )
#define SAEJ_AXIS_QV_ENABLED      ( 0x00001400U )


#define SAEJ_AXIS_X_INVERTED      ( 0x00000002U )
#define SAEJ_AXIS_Y_INVERTED      ( 0x00000008U )
#define SAEJ_AXIS_W_INVERTED      ( 0x00000020U )
#define SAEJ_AXIS_Z_INVERTED      ( 0x00000080U )
#define SAEJ_AXIS_J_INVERTED      ( 0x00000200U )
#define SAEJ_AXIS_Q_INVERTED      ( 0x00000800U )
#define SAEJ_AXIS_V_INVERTED      ( 0x00002000U )
   
#define SAEJ_AXIS_X_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00010000U )
#define SAEJ_AXIS_Y_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00020000U )
#define SAEJ_AXIS_W_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00040000U )
#define SAEJ_AXIS_Z_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00080000U )
#define SAEJ_AXIS_J_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00100000U )
#define SAEJ_AXIS_Q_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00200000U )
#define SAEJ_AXIS_V_NOT_WAIT_NEUTRAL_AT_STARTUP      ( 0x00400000U )   

#define SAEJ_DETENT_EN_OFF        ( 0x00U )
#define SAEJ_DETENT_EN_POS        ( 0x01U )
#define SAEJ_DETENT_EN_NEG        ( 0x02U )

#define SAEJ_DEFAULT_CFG_DETENT   ( SAEJ_AXIS_X_DETENT_EN | ( SAEJ_AXIS_Y_DETENT_EN << 2U ) | ( SAEJ_AXIS_W_DETENT_EN << 4U ) | ( SAEJ_AXIS_Z_DETENT_EN << 6U ) | ( SAEJ_AXIS_J_DETENT_EN << 8U ) | ( SAEJ_AXIS_Q_DETENT_EN << 10U ) | ( SAEJ_AXIS_V_DETENT_EN << 12U ) )

#define SAEJ_AXIS_STATUS_NEUTRAL       ( 0x01U )
#define SAEJ_AXIS_STATUS_NEGATIVE      ( 0x04U )
#define SAEJ_AXIS_STATUS_POSITIVE      ( 0x10U )
#define SAEJ_AXIS_STATUS_ERROR         ( 0x2AU )
#define SAEJ_AXIS_STATUS_NOT_AVAILABLE ( 0x3FU )

#define SAEJ_DETENT_OFF        ( 0x00U )
#define SAEJ_DETENT_ON         ( 0x01U )

#define SAEJ_AXIS_OUT_NEUTRAL          ( 0U )
#define SAEJ_AXIS_OUT_MAX              ( 1000U )
#define SAEJ_AXIS_OUT_ERROR            ( 1022U )
#define SAEJ_AXIS_OUT_NOT_AVAILABLE    ( 1023U )


/* definizione pulsanti abilitati */
//------------------------------------------------------------------------------
#define SAEJ_PUSH_BUTTON_NUM   ( 13U )
#define SAEJ_IN_NOT_CONNECTED  ( 0xFFU )

#define SAEJ_BUTTON_ALL_DISABLED       ( 0x0000U )
#define SAEJ_BUTTON_1_ENABLED          ( 0x0001U )
#define SAEJ_BUTTON_2_ENABLED          ( 0x0002U )
#define SAEJ_BUTTON_3_ENABLED          ( 0x0004U )
#define SAEJ_BUTTON_4_ENABLED          ( 0x0008U )
#define SAEJ_BUTTON_5_ENABLED          ( 0x0010U )
#define SAEJ_BUTTON_6_ENABLED          ( 0x0020U )
#define SAEJ_BUTTON_7_ENABLED          ( 0x0040U )
#define SAEJ_BUTTON_8_ENABLED          ( 0x0080U )
#define SAEJ_BUTTON_9_ENABLED          ( 0x0100U )
#define SAEJ_BUTTON_10_ENABLED         ( 0x0200U )
#define SAEJ_BUTTON_11_ENABLED         ( 0x0400U )
#define SAEJ_BUTTON_12_ENABLED         ( 0x0800U )  
#define SAEJ_BUTTON_13_ENABLED         ( 0x1000U )

#define SAEJ_BUTTON_1    ( 0U )
#define SAEJ_BUTTON_2    ( 1U )   
#define SAEJ_BUTTON_3    ( 2U ) 
#define SAEJ_BUTTON_4    ( 3U ) 
#define SAEJ_BUTTON_5    ( 4U )
#define SAEJ_BUTTON_6    ( 5U )
#define SAEJ_BUTTON_7    ( 6U )
#define SAEJ_BUTTON_8    ( 7U )
#define SAEJ_BUTTON_9    ( 8U )
#define SAEJ_BUTTON_10   ( 9U ) 
#define SAEJ_BUTTON_11   ( 10U )
#define SAEJ_BUTTON_12   ( 11U ) 
#define SAEJ_BUTTON_13   ( 12U )

#define SAEJ_WST_INDEX_NOT_CONNECTED ( 0x0FuL )

/* definizione led */
#define SAEJ_LED_OFF     ( 0x00U )
#define SAEJ_LED_0_ON    ( 0x01U )
#define SAEJ_LED_0_BLINK ( 0x02U )
#define SAEJ_LED_0_NA    ( 0x03U )
#define SAEJ_LED_1_ON    ( 0x04U )
#define SAEJ_LED_1_BLINK ( 0x08U )
#define SAEJ_LED_1_NA    ( 0x0CU )
#define SAEJ_LED_2_ON    ( 0x10U )
#define SAEJ_LED_2_BLINK ( 0x20U )
#define SAEJ_LED_2_NA    ( 0x30U )
#define SAEJ_LED_3_ON    ( 0x40U )
#define SAEJ_LED_3_BLINK ( 0x80U )
#define SAEJ_LED_3_NA    ( 0xC0U )

#define SAEJ_LED_0_INDEX ( 0U )
#define SAEJ_LED_1_INDEX ( 1U )
#define SAEJ_LED_2_INDEX ( 2U )
#define SAEJ_LED_3_INDEX ( 3U )

//errori
#define SAEJ_ERROR_NO            ( 0x0000 )

#define SAEJ_SPN_MLX_0           ( 0x00U )
#define SAEJ_SPN_MLX_1           ( 0x01U )
#define SAEJ_SPN_AXIS_W          ( 0x02U ) 
#define SAEJ_SPN_AXIS_Z          ( 0x03U )
#define SAEJ_SPN_AXIS_J          ( 0x04U )
#define SAEJ_SPN_AXIS_Q          ( 0x05U )
#define SAEJ_SPN_AXIS_V          ( 0x06U )
#define SAEJ_SPN_BUTTON_1        ( 0x07U )
#define SAEJ_SPN_BUTTON_2        ( 0x08U )
#define SAEJ_SPN_BUTTON_3        ( 0x09U )
#define SAEJ_SPN_BUTTON_4        ( 0x0AU )
#define SAEJ_SPN_BUTTON_5        ( 0x0BU )
#define SAEJ_SPN_BUTTON_6        ( 0x0CU )
#define SAEJ_SPN_BUTTON_7        ( 0x0DU )
#define SAEJ_SPN_BUTTON_8        ( 0x0EU )
#define SAEJ_SPN_BUTTON_9        ( 0x0FU )
#define SAEJ_SPN_BUTTON_10       ( 0x10U )
#define SAEJ_SPN_BUTTON_11       ( 0x11U )
#define SAEJ_SPN_BUTTON_12       ( 0x12U )
#define SAEJ_SPN_BUTTON_13       ( 0x13U )
#define SAEJ_SPN_MEMORY          ( 0x14U )
#define SAEJ_SPN_SUPPLY          ( 0x15U ) 
#define SAEJ_SPN_CAN             ( 0x16U )
#define SAEJ_SPN_ADC0_GROUP_0    ( 0x17U )   
#define SAEJ_SPN_ADC0_GROUP_1    ( 0x18U )
#define SAEJ_SPN_ADC1_GROUP_0    ( 0x19U )
#define SAEJ_SPN_ADC1_GROUP_1    ( 0x1AU )
#define SAEJ_SPN_SYNC_TE         ( 0x1BU )
#define SAEJ_SPN_TLF             ( 0x1CU )
#define SAEJ_SPN_WSL             ( 0x1DU )
#define SAEJ_SPN_CF              ( 0x1EU )
#define SAEJ_SPN_NETWORK         ( 0x1FU )
#define SAEJ_SPN_SPI_TE          ( 0x20U )
#define SAEJ_SPN_SPI_TE_INIT     ( 0x21U )
#define SAEJ_SPN_SOFT_TE         ( 0x22U )
#define SAEJ_SPN_LED             ( 0x23U )
#define SAEJ_SPN_LED_STATUS_0    ( 0x24U )
#define SAEJ_SPN_LED_STATUS_1    ( 0x25U )
#define SAEJ_SPN_LED_STATUS_2    ( 0x26U )
#define SAEJ_SPN_LED_STATUS_3    ( 0x27U )   
#define SAEJ_SPN_NUM             ( 0x28U ) 

//SPN degli error codes
#define SPN_ERR_MLX_0              ( 0x0A64UL )   //MLX 0
#define SPN_ERR_MLX_1              ( 0x0A65UL )   //MLX 1
#define SPN_ERR_ROLLER             ( 0x0A66UL )   //roller
#define SPN_ERR_BUTTON             ( 0x0A7DUL )   //pulsante
#define SPN_ERR_MEMORY             ( 0x0274UL )   //eeprom
#define SPN_ERR_SUPPLY             ( 0x009EUL )   //alimentazione
#define SPN_ERR_CAN                ( 0x0263UL )   //can
#define SPN_ERR_ADC0_GROUP_0       ( 0x043BUL )   //adc0.0
#define SPN_ERR_ADC0_GROUP_1       ( 0x043CUL )   //adc0.1
#define SPN_ERR_ADC1_GROUP_0       ( 0x103DUL )   //adc1.0
#define SPN_ERR_ADC1_GROUP_1       ( 0x103EUL )   //adc1.1
#define SPN_ERR_SYNC_TE            ( 0x0271UL )   //TE
#define SPN_ERR_TLF                ( 0x15D4UL )   //TLF
#define SPN_ERR_WSL                ( 0x2522UL )   //WSL
#define SPN_ERR_CF                 ( 0x2523UL )   //control flow
#define SPN_ERR_NETWORK            ( 0x027FUL )   //J1939 network
#define SPN_ERR_SPI_TE             ( 0x0D29UL )   //network service status 1
#define SPN_ERR_SPI_TE_INIT        ( 0x0D73UL )   //network service status 2
#define SPN_ERR_SOFT_TE            ( 0x0D28UL )   //TE soft error
#define SPN_ERR_LED                ( 0x4460UL )   //led
#define SPN_ERR_TE                 ( 0x0UL )   //led

#define SAEJ_TP_IS_BROADCAST     ( true )
#define SAEJ_TP_IS_NOT_BROADCAST ( false )

#define SAEJ_TP_SLOT_NUM_MAX              ( 2U )     //numero massimo di messaggi TP che posso gestire in parallelo

#define SAEJ_TP_BYTE_NUM_MIN              ( 9UL )
#define SAEJ_TP_BYTE_NUM_MAX              ( 1785UL )
#define SAEJ_TP_PACKET_NUM_MIN            ( 2UL )
#define SAEJ_TP_PACKET_NUM_MAX            ( 256UL )

#define SAEJ_ETP_BYTE_NUM_MIN             ( 1786UL )
#define SAEJ_ETP_BYTE_NUM_MAX             ( 117440505UL )
#define SAEJ_ETP_PACKET_NUM_MIN           ( 256UL )
#define SAEJ_ETP_PACKET_NUM_MAX           ( 16777215UL )

#define ASCII_CH_LIMIT          ( 0x2AU )
#define SAEJ_LED_EN_OFF         ( 0x00U )
#define SAEJ_LED_0_EN           ( 0x01U )
#define SAEJ_LED_1_EN           ( 0x02U )
#define SAEJ_LED_2_EN           ( 0x04U )
#define SAEJ_LED_3_EN           ( 0x08U )

typedef enum
{
    SAEJ_FMI_DATA_HIGH_SEVERITY_HIGH = 0U,
    SAEJ_FMI_DATA_LOW_SEVERITY_HIGH = 1U,
    SAEJ_FMI_DATA_NOT_VALID    = 2U,
    SAEJ_FMI_VOLTAGE_HIGH = 3U,
    SAEJ_FMI_VOLTAGE_LOW = 4U,
    SAEJ_FMI_CURRENT_HIGH = 5U,
    SAEJ_FMI_CURRENT_LOW  = 6U,
    SAEJ_FMI_SYSTEM_ERROR = 7U,
    SAEJ_FMI_FREQ_PWM_ERROR = 8U,
    SAEJ_FMI_UPDATE_RATE_ERROR = 9U,
    SAEJ_FMI_CHANGE_RATE_ERROR = 10U,
    SAEJ_FMI_UNKNOWN_ERROR = 11U,
    SAEJ_FMI_BAD_COMPONENT_ERROR = 12U,
    SAEJ_FMI_OUT_OF_CALIBRATION_ERROR = 13U,
    SAEJ_FMI_SPECIAL_INSTRUCTION_ERROR = 14U,
    SAEJ_FMI_DATA_HIGH_SEVERITY_LOW = 15U,
    SAEJ_FMI_DATA_HIGH_SEVERITY_MEDIUM = 16U,
    SAEJ_FMI_DATA_LOW_SEVERITY_LOW = 17U,    
    SAEJ_FMI_DATA_LOW_SEVERITY_MEDIUM = 18U,
    SAEJ_FMI_DATA_INPUT_ERROR = 19U,
    SAEJ_FMI_NUM_MAX = 20U,
    SAEJ_FMI_NA = 31U,    
} Saej_FMI_t;

typedef enum
{
    SAEJ_LAMP_OFF = 0U,
    SAEJ_LAMP_PROTECT_ON = 1U,
    SAEJ_LAMP_AMBER_WARNING_ON = 4U,
    SAEJ_LAMP_RED_STOP_ON = 16U,
    SAEJ_LAMP_MALFUNCTION_ON = 64U,
    SAEJ_LAMP_NA = 255U,
} Saej_lampStatus_t;

typedef struct Saej_axisOut_e
{
    uint16_t value;
    uint16_t previousValue;
    uint8_t status;
    uint8_t detentOut;   
    uint8_t detentOutPrevious;
    uint16_t detentThPos;
    uint16_t detentThNeg;
    uint8_t detentEn;
    bool outInverted;
    bool changed;
    bool notWaitNeutralAtStartup;
    bool startupDone;
} Saej_axisOut_t;

extern uint8_t Saej_PushButtonStatusGet( uint8_t byteIndex );
extern wres_t Saej_TPTxEnable( uint8_t index, uint32_t PGN, bool isBroadCast, uint8_t destAddr, uint32_t byteNum, uint32_t packetNum, uint8_t *buffInitSource, const uint8_t *buffSource );
extern wres_t Saej_TPTxStatusGet( uint8_t index );
extern void Saej_SendPGNRequest( uint32_t PGN, uint8_t destAddr );
extern uint32_t Saej_CfgAxesGet( void );
extern uint16_t Saej_CfgPushButtonGet( void );
extern uint16_t Saej_CfgDetentGet( void );
extern uint8_t Saej_PushButtonIndexGet( uint8_t buttonNum );
extern uint8_t Saej_PushButtonEnGet( uint8_t buttonNum );
extern uint8_t Saej_RollerIndexGet( uint8_t rollerNum );
extern uint8_t Saej_SourceAddressGet( void );
extern Saej_axisOut_t Saej_AxisGet( uint8_t axisIndex );
extern void Saej_LedInternalValueSet( uint8_t ledValue );
extern void Saej_LedInternalDutySet( uint8_t dutyValue );
extern uint8_t Saej_LedInternalValueGet( void );
extern uint8_t Saej_LedInternalDutyGet( void );

#ifdef ISOBUS_DJW
extern void Isobus_TxClaim( void );
#endif
extern void Saej_Init( void );
extern void Saej_Main( uint8_t index );

#endif