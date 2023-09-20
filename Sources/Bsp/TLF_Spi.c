
#include "S32K_ObjectConfig.h"
#include "TLF_Spi.h"
#include "S32K_LpSpi.h"
#include "S32K_SysPin.h"
#include "S32K_Driver.h"

//------------------------------------------------------------------------------
#define TLF_STATUS_INIT                (  0U )
#define TLF_STATUS_UNLOCK_01           (  1U )
#define TLF_STATUS_UNLOCK_02           (  2U )
#define TLF_STATUS_UNLOCK_03           (  3U )
#define TLF_STATUS_UNLOCK_04           (  4U )
#define TLF_STATUS_UNLOCK_CHECK        (  5U )
#define TLF_STATUS_SYS_CFG             (  6U )
#define TLF_STATUS_WDT_CFG             (  7U )
#define TLF_STATUS_WWDT_CFG_CW         (  8U )
#define TLF_STATUS_WWDT_CFG_OW         (  9U )
#define TLF_STATUS_LOCK_01             ( 10U )
#define TLF_STATUS_LOCK_02             ( 11U )
#define TLF_STATUS_LOCK_03             ( 12U )
#define TLF_STATUS_LOCK_04             ( 13U )
#define TLF_STATUS_LOCK_CHECK          ( 14U )
#define TLF_STATUS_WDT_FIRST_READ      ( 15U )
#define TLF_STATUS_WDT_FIRST_SERVE     ( 16U )
#define TLF_STATUS_GO_TO_NORMAL        ( 17U )
#define TLF_STATUS_GO_TO_NORMAL_NEG    ( 18U )
#define TLF_STATUS_GO_TO_NORMAL_CHECK  ( 19U )
#define TLF_STATUS_CW                  ( 20U )
#define TLF_STATUS_OW_READ             ( 21U )
#define TLF_STATUS_OW_SERVE            ( 22U )
#define TLF_STATUS_OW_SERVE_CHECK      ( 23U )
#define TLF_STATUS_IDLE                ( 24U )
#define TLF_STATUS_BCK_FRE_SPREAD      ( 25U )
#define TLF_STATUS_BCK_CFG_UPDATE_EN   ( 26U )
#define TLF_STATUS_BCK_CFG_CHECK       ( 27U )
#define TLF_STATUS_BCK_CFG_UPDATE_GO   ( 28U )
#define TLF_STATUS_BCK_FRE_READ        ( 29U )
#define TLF_STATUS_MONSF0_READ         ( 30U )
#define TLF_STATUS_MONSF1_READ         ( 31U )
#define TLF_STATUS_MONSF2_READ         ( 32U )
#define TLF_STATUS_MONSF3_READ         ( 33U )
#define TLF_STATUS_DIAGNOSE_CHECK      ( 34U )
#define TLF_STATUS_DIAGNOSE_COVER      ( 35U )
#define TLF_STATUS_DIAGNOSE_READ       ( 36U )

#define TLF_CMD_READ                   ( 0U )
#define TLF_CMD_WRITE                  ( 1U )

#define ADDR_WWDSCMD                   ( 0x17u )
#define ADDR_WDCFG0                    ( 0x06u )
#define ADDR_WDCFG1                    ( 0x07u )
#define ADDR_WWDCFG0                   ( 0x09u )
#define ADDR_WWDCFG1                   ( 0x0Au )
#define ADDR_SYSPCFG1                  ( 0x05u )
#define ADDR_PROTCFG                   ( 0x03u )
#define ADDR_DEVCTRL                   ( 0x15u )
#define ADDR_DEVCTRLN                  ( 0x16u )
#define ADDR_MONSF0                    ( 0x20u )
#define ADDR_MONSF1                    ( 0x21u )
#define ADDR_MONSF2                    ( 0x22u )
#define ADDR_MONSF3                    ( 0x23u )
#define ADDR_OTFAIL                    ( 0x24u )
#define ADDR_OTWRNSF                   ( 0x25u )
#define ADDR_PROTSTAT                  ( 0x28u )
#define ADDR_BCK_FRE_SPREAD            ( 0x32u )
#define ADDR_BCK_CTRL                  ( 0x33u )

#define TLF_WDT_EXT_LEVEL_HI           ( 0x01u )
#define TLF_WDT_EXT_LEVEL_LO           ( 0x00u )

#define WWD_EN                         ( 0x08u )
#define WWD_EN_NOT                     ( 0x00u )
#define FWD_EN                         ( 0x04u )
#define FWD_EN_NOT                     ( 0x00u )
#define WWD_TRIG_SPI                   ( 0x02u )
#define WWD_TRIG_EXT                   ( 0x00u )
#define WD_CYC_1MS                     ( 0x01u )
#define WD_CYC_100US                   ( 0x00u )

#define BUCK_SPREAD_OFF                ( 0x00u )
#define BUCK_SPREAD_1                  ( 0x2Bu )
#define BUCK_SPREAD_2                  ( 0x55u )
#define BUCK_SPREAD_3                  ( 0x80u )
#define BUCK_SPREAD_4                  ( 0xAAu )
#define BUCK_SPREAD_5                  ( 0xD5u )
#define BUCK_SPREAD_6                  ( 0xFFu )

#define BUCK_DATA_UPDATE               ( 0x40u )
#define BUCK_BUSY                      ( 0x01u )

#define MONSF0_CHECK_MASK              ( 0xD0u ) //controllo gli errori relativi ai tracker TR1, TR2 e COM

/*****************************************************************************/
/*   MACRO MODIFICABILI DA UTENTE                                            */
/*****************************************************************************/
//assegnazione macro per BCK_FRE_SPREAD
#define BUCK_SPREAD_SELECTED           ( BUCK_SPREAD_6 )

//assegnazione macro per WDCFG0
#define WWDEN                ( WWD_EN       )   //WWD_EN = Window Watchdog Enabled; WWD_EN_NOT = Window Watchdog Not Enabled
#define FWDEN                ( FWD_EN_NOT   )   //FWD_EN = Functional Watchdog Enabled; FWD_EN_NOT = Functional Watchdog Not Enabled
#define WWDTSEL              ( WWD_TRIG_SPI )   //WWD_TRIG_SPI = Window Watchdog Triggered via SPI; WWD_TRIG_EXT = Window Watchdog Triggered via pin PWR_WDI_M4 (PTA17)
#define WDCYC                ( WD_CYC_1MS   )   //WD_CYC_1MS = tempo di ciclo 1ms per il calcolo dei timeout; WD_CYC_100US = tempo di ciclo 0,1ms per il calcolo dei timeout
#define WWDETHR              (           4U )   //numero di tentativi di attesa del trigger corretto, prima di andare in errore. Valori ammessi: 0 ÷ 7

//assegnazione macro per WWDCFG0
#define WDT_CW_TIME_MS       (      350000U )   //durata in ms della Close Window, come da configurazione del TLF
                                                //questa durata deve essere un multiplo (range 1x ÷ 32x) di 50WDCYC

//assegnazione macro per WWDCFG1
#define WDT_OW_TIME_MS       (      600000U )   //durata in ms della Open Window, come da configurazione del TLF 
                                                //questa durata deve essere un multiplo (range 1x ÷ 32x) di 50WDCYC 

//assegnazione macro per gestione trigger del WWD
#define WDT_OW_READ_TIME_MS  (      200000U )   //tempo dopo il quale, nella Open Window, vado a leggere lo stato del trigger per fare il toggle
#define WDT_OW_SERVE_TIME_MS (      200000U )   //tempo dopo il quale, nella Open Window, vado a fare effettivamente il toggle del trigger. 
                                                //N.B.: deve essere WDT_OW_TIME_MS > ( WDT_OW_READ_TIME_MS + WDT_OW_SERVE_TIME_MS ) 

//                                  |                                  ------ qui leggo WWDSCMD per vedere lo stato del trigger (se SPI), oppure alzo il WDI (se trigger EXT)                      
//                                  |                                  |                                 ------ qui togglo il trigger (se SPI), oppure abbasso il WDI (se trigger EXT)                                        
//                                  |                                  |                                 |                                 |
//                                  |                                  |                                 |                                 |
//                                  |                                  |                                 |                                 |
//                                  |            Open Window Read     \|/          Open Window Serve    \|/                                |
//                                  |<-------------------------------->|<------------------------------->|                                 |
//        Close Window              |                                                    Open Window                                       | 
// <------------------------------->|<---------------------------------------------------------------------------------------------------->|

/*****************************************************************************/

//NON MODIFICARE!! - default da utilizzare per la configurazione dei registri e la macchina a stati
//#define TLF_CW_TIME_TH       ( WDT_CW_TIME_MS / SCHEDULE_COUNTER_TH_SPI_TLF )
//#define TLF_OW_READ_TIME_TH  ( WDT_OW_READ_TIME_MS / SCHEDULE_COUNTER_TH_SPI_TLF )
//#define TLF_OW_SERVE_TIME_TH ( WDT_OW_SERVE_TIME_MS / SCHEDULE_COUNTER_TH_SPI_TLF )

#define TLF_CW_TIME_TH       ( WDT_CW_TIME_MS / 10000u )
#define TLF_OW_READ_TIME_TH  ( WDT_OW_READ_TIME_MS / 10000u )
#define TLF_OW_SERVE_TIME_TH ( WDT_OW_SERVE_TIME_MS / 10000u )


#define WWD_CW_LENGTH        ( WDCYC == WD_CYC_1MS ) ? ( ( WDT_CW_TIME_MS / 50000u ) - 1U ) : ( ( WDT_CW_TIME_MS / 5000u ) - 1U )
#define WWD_OW_LENGTH        ( WDCYC == WD_CYC_1MS ) ? ( ( WDT_OW_TIME_MS / 50000u ) - 1U ) : ( ( WDT_OW_TIME_MS / 5000u ) - 1U )

#define DEFAULT_WDCFG0       ( ( ( ( WWDETHR & 0x07u ) << 5U ) | 0x10u ) | WWDEN | FWDEN | WWDTSEL | WDCYC )
#define DEFAULT_WWDCFG0      (uint8_t)( WWD_CW_LENGTH & 0x1Fu )
#define DEFAULT_WWDCFG1      (uint8_t)( WWD_OW_LENGTH & 0x1Fu )

#define DEFAULT_BUCK_SPREAD  ( BUCK_SPREAD_SELECTED )

//------------------------------------------------------------------------------
s_LpSpi_msg_t TLFmaster_recv_WDCFG0;
s_LpSpi_msg_t TLFmaster_recv_WDCFG1;
s_LpSpi_msg_t TLFmaster_recv_WWDCFG0;
s_LpSpi_msg_t TLFmaster_recv_WWDCFG1;
//static s_LpSpi_msg_t TLFmaster_recv_WWDSCMD;
s_LpSpi_msg_t TLFmaster_recv_Lock1;
s_LpSpi_msg_t TLFmaster_recv_Lock2;
s_LpSpi_msg_t TLFmaster_recv_Lock3;
s_LpSpi_msg_t TLFmaster_recv_Lock4;
s_LpSpi_msg_t TLFmaster_recv_ProtStat;
s_LpSpi_msg_t TLFmaster_recv_ProtStat2;
s_LpSpi_msg_t TLFmaster_recv_Sys;
s_LpSpi_msg_t TLFmaster_recv_UnLock1;
s_LpSpi_msg_t TLFmaster_recv_UnLock2;
s_LpSpi_msg_t TLFmaster_recv_UnLock3;
s_LpSpi_msg_t TLFmaster_recv_UnLock4;
s_LpSpi_msg_t TLFmaster_recv_DevCtrl;
s_LpSpi_msg_t TLFmaster_recv_DevCtrlN;
s_LpSpi_msg_t TLFmaster_recv_BuckFreSpread;
s_LpSpi_msg_t TLFmaster_recv_BuckFreSpreadRead;
s_LpSpi_msg_t TLFmaster_recv_BuckCfg;
s_LpSpi_msg_t TLFmaster_recv_BuckCfgCheck;
s_LpSpi_msg_t TLFmaster_recv_Window;
s_LpSpi_msg_t TLFmaster_recv_Window2;
s_LpSpi_msg_t TLFmaster_recv_MONSF0;
s_LpSpi_msg_t TLFmaster_recv_MONSF1;
s_LpSpi_msg_t TLFmaster_recv_MONSF2;
s_LpSpi_msg_t TLFmaster_recv_MONSF3;

static uint16_t tlfCounter;
uint8_t tlfStatus;
uint8_t tlfStatusPre;
uint8_t tlfTrigger;
uint8_t tlfMONSFVal;

uint8_t TLF_StartedOK =0;
//------------------------------------------------------------------------------
static void TLF_WDT_Ext_Drive( uint8_t level );
static void TLF_Spi_TxBufferBuild( uint8_t command, uint8_t address, uint8_t data, s_LpSpi_msg_t *msg );

//------------------------------------------------------------------------------
static void TLF_WDT_Ext_Drive( uint8_t level )
{
    if ( TLF_WDT_EXT_LEVEL_HI == level )
    {
        S32K_SysPin_SetOutputPins( GPIO_OUT_PWR_WDI );
    }
    else
    {
        S32K_SysPin_ClearOutputPins( GPIO_OUT_PWR_WDI );
    }
    
    return;
}

//------------------------------------------------------------------------------
static void TLF_Spi_TxBufferBuild( uint8_t command, uint8_t address, uint8_t data, s_LpSpi_msg_t *msg )
{
    uint16_t aux;
    uint8_t parity;
    uint8_t i;
    
    aux = (uint16_t)( data );
    aux = aux | (uint16_t)( ( address & 0x3FU ) << 8U );
    aux = aux | (uint16_t)( ( command & 0x01U ) << 14U );    
    
    parity = 0U;
    for ( i=0U; i<15U; i++ )
    {
        parity = ( parity ^ ( (uint8_t) ( aux >> i ) & 0x01U ) ) & 0x01U; 
    }
    
    aux = ( aux << 1U ) & 0xFFFEU;
    msg->data[0U] = (uint8_t)( aux >> 8U );
    msg->data[1U] = (uint8_t)( parity & 0x01U ) | (uint8_t)( aux );
    
    return;
}

//------------------------------------------------------------------------------
void TLF_Spi_Init( void )
{
    tlfCounter = 0U;
    tlfStatus = TLF_STATUS_INIT;
    tlfStatusPre = TLF_STATUS_INIT;
    tlfTrigger = 0U;
    tlfIntFlag = 0U;
    tlfStatusCheckWIP = 0U;
    tlfGoToRecovery = 0U;
    tlfMONSFVal = 0U;
    tlfMONSFValToCAN = 0U;
    S32K_SysPin_ClearOutputPins( GPIO_OUT_PWR_WDI );
        
    return;
}

//------------------------------------------------------------------------------
void TLF_Spi_Main( uint8_t index )
{
#define TRIG_ERR_CNT_TH ( 3U )
    static s_LpSpi_msg_t TLFmaster_send;
    static s_LpSpi_msg_t TLFmaster_recv;
    (void)(index);
    static uint8_t tlfErrorCounter = 0U;
    static uint8_t retryCnt = 0U;
    wres_t res = WRES_OK;
    uint8_t trigActualValue;
    static uint8_t trigPrevValue;
    static uint8_t trigErrCounter = 0U;
    static uint16_t edgeResetCounter = 0U;
    (void)(res);
    
    if ( 0x01 == tlfIntFlag )
    {
        if ( 0U == tlfGoToRecovery )
        {
            tlfStatusPre = tlfStatus;
            tlfStatus = TLF_STATUS_DIAGNOSE_CHECK;
            tlfIntFlag = 0U;
        }
    }
    if ( edgeCounter >= 12U )
    {
        tlfStatus = TLF_STATUS_IDLE;
        edgeResetCounter = 0U;
    }
    if ( TLF_STATUS_IDLE != tlfStatus ) 
    {
        if ( edgeResetCounter++ >= 55U )
        {
            edgeResetCounter = 0U;
            if ( 1U < edgeCounter )
            {
                edgeCounter = edgeCounter - 1U;
            }
        }
    }

    switch ( tlfStatus )
    {
    case TLF_STATUS_INIT:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_DEVCTRL, 0xFAu, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_DevCtrl.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_UNLOCK_01;
        TLF_StartedOK = 0x00U;
        break;        
    case TLF_STATUS_UNLOCK_01:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0xAB, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_UnLock1.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_UNLOCK_02;
        break;
    case TLF_STATUS_UNLOCK_02:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0xEF, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_UnLock2.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_UNLOCK_03;
        break;
    case TLF_STATUS_UNLOCK_03:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0x56, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_UnLock3.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_UNLOCK_04;
        break;        
    case TLF_STATUS_UNLOCK_04:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0x12u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_UnLock4.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_UNLOCK_CHECK;
        break;        
    case TLF_STATUS_UNLOCK_CHECK:
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_PROTSTAT, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_ProtStat.data[0U], GPIO_OUT_TLF_CS );
        if ( 0U != ( TLFmaster_recv_ProtStat.data[1U] & 0x02u ) )                      //verifico se il dispositivo è stato unlockato correttamente
        {
            tlfStatus = TLF_STATUS_UNLOCK_01;
            ++tlfErrorCounter;
            if ( tlfErrorCounter >= 2U )
            {
                tlfStatus = TLF_STATUS_IDLE;
                tlfErrorCounter = 0U; 
            }
        }
        else
        {
            tlfStatus = TLF_STATUS_SYS_CFG;
            tlfErrorCounter = 0U;
        }
        break;
    case TLF_STATUS_SYS_CFG:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_SYSPCFG1, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_Sys.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_WDT_CFG;
        break;
    case TLF_STATUS_WDT_CFG:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_WDCFG0, DEFAULT_WDCFG0, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_WDCFG0.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_WWDT_CFG_CW;
        break;
    case TLF_STATUS_WWDT_CFG_CW:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_WWDCFG0, DEFAULT_WWDCFG0, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_WDCFG0.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_WWDT_CFG_OW;
        break;
    case TLF_STATUS_WWDT_CFG_OW:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_WWDCFG1, DEFAULT_WWDCFG1, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_WWDCFG1.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_BCK_FRE_SPREAD;
        break;
    case TLF_STATUS_BCK_FRE_SPREAD:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_BCK_FRE_SPREAD, DEFAULT_BUCK_SPREAD, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_BuckFreSpread.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_BCK_CFG_UPDATE_EN;
//        tlfStatus = TLF_STATUS_BCK_FRE_READ;
        break;
    case TLF_STATUS_BCK_CFG_UPDATE_EN:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_BCK_CTRL, BUCK_DATA_UPDATE, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_BuckCfg.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_BCK_CFG_CHECK;      
        break;
    case TLF_STATUS_BCK_CFG_CHECK:
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_BCK_CTRL, 0x00u, &TLFmaster_send );
        TLFmaster_recv_BuckCfgCheck.data[0U] = 0U;
        TLFmaster_recv_BuckCfgCheck.data[1U] = 0U;
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_BuckCfgCheck.data[0U], GPIO_OUT_TLF_CS );
        if ( BUCK_BUSY == ( TLFmaster_recv_BuckCfgCheck.data[0U] & 0x01u ) )
        {
            retryCnt = retryCnt + 1U;
            if ( retryCnt > 1U )
            {
                tlfStatus = TLF_STATUS_IDLE;
            }
        }
        else
        {
            tlfStatus = TLF_STATUS_BCK_CFG_UPDATE_GO;      
        }
        break;      
    case TLF_STATUS_BCK_CFG_UPDATE_GO:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_BCK_CTRL, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_BuckCfg.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_BCK_FRE_READ;
        break;      
    case TLF_STATUS_BCK_FRE_READ:
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_BCK_FRE_SPREAD, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_BuckFreSpreadRead.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_LOCK_01;
        tlfErrorCounter = 0U;        
        break;        
    case TLF_STATUS_LOCK_01:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0xDFu, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_Lock1.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_LOCK_02;
        break;
    case TLF_STATUS_LOCK_02:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0x34u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_Lock2.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_LOCK_03;
        break;        
    case TLF_STATUS_LOCK_03:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0xBEu, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_Lock3.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_LOCK_04;
        break;        
    case TLF_STATUS_LOCK_04:
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_PROTCFG, 0xCAu, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_Lock4.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_LOCK_CHECK;
        break;  
    case TLF_STATUS_LOCK_CHECK:
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_PROTSTAT, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_ProtStat2.data[0U], GPIO_OUT_TLF_CS );
        if ( 0U == ( TLFmaster_recv_ProtStat2.data[1U] & 0x02u ) )                      //verifico se il dispositivo è stato lockato correttamente
        {
            ++tlfErrorCounter;
            if ( tlfErrorCounter >= 2U )
            {
                tlfStatus = TLF_STATUS_IDLE;
                tlfErrorCounter = 0U; 
            }
            else
            {
                tlfStatus = TLF_STATUS_LOCK_01; 
            }            
        }
        else
        {
            tlfStatus = TLF_STATUS_WDT_FIRST_READ;
        }
        break;        
    case TLF_STATUS_WDT_FIRST_READ:
        if ( WWD_TRIG_SPI == WWDTSEL ) 
        {
            TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_WWDSCMD, 0x00u, &TLFmaster_send );
            res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv.data[0U], GPIO_OUT_TLF_CS );
        }
        else
        {
            TLF_WDT_Ext_Drive( TLF_WDT_EXT_LEVEL_HI );
        }
        tlfStatus = TLF_STATUS_WDT_FIRST_SERVE;
        tlfErrorCounter = 0U;
        break;
    case TLF_STATUS_WDT_FIRST_SERVE:
        if ( WWD_TRIG_SPI == WWDTSEL ) 
        {
            tlfTrigger = 0U;
            if ( 0U == ( TLFmaster_recv.data[1U] & 0x02u ) )
            {
                tlfTrigger = 1U;
            }
            trigPrevValue = tlfTrigger;
            TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_WWDSCMD, tlfTrigger, &TLFmaster_send );
            res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv.data[0U], GPIO_OUT_TLF_CS );
        }
        else 
        {
            TLF_WDT_Ext_Drive( TLF_WDT_EXT_LEVEL_LO );
        }
        tlfStatus = TLF_STATUS_GO_TO_NORMAL;      
        tlfCounter = 0U;
        break;
    case TLF_STATUS_GO_TO_NORMAL:
        if ( ( TLF_STATUS_CW == tlfStatusPre ) || ( TLF_STATUS_OW_READ == tlfStatusPre ) || ( TLF_STATUS_OW_SERVE == tlfStatusPre ) )
        {
            ++tlfCounter;
        }      
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_DEVCTRL, 0xFAu, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_DevCtrl.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_GO_TO_NORMAL_NEG;
        break;
    case TLF_STATUS_GO_TO_NORMAL_NEG:
        if ( ( TLF_STATUS_CW == tlfStatusPre ) || ( TLF_STATUS_OW_READ == tlfStatusPre ) || ( TLF_STATUS_OW_SERVE == tlfStatusPre ) )
        {
            ++tlfCounter;
        }      
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_DEVCTRLN, 0x05u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_DevCtrlN.data[0U], GPIO_OUT_TLF_CS );
        tlfStatus = TLF_STATUS_CW;
        //tlfCounter = 0U;
        TLF_StartedOK = 0x01U;
        if ( 1U == tlfGoToRecovery )
        {
            tlfStatus = tlfStatusPre;
            tlfGoToRecovery = 0U; 
        }
        break;
//    case TLF_STATUS_GO_TO_NORMAL_CHECK:
//        res = S32K_LpSpi_Task(SPI_TLF);
//        res = S32K_LpSpi_RecvMsg(SPI_TLF, &TLFmaster_recv_DevCtrlN);
//        tlfStatus = TLF_STATUS_CW;        
//        break;
    case TLF_STATUS_CW:
        ++tlfCounter ;
        if ( tlfCounter >= (uint16_t)( TLF_CW_TIME_TH ) )
        {
            if ( WWD_TRIG_SPI == WWDTSEL ) 
            {      
                TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_WWDSCMD, 0x00u, &TLFmaster_send );
                res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv.data[0U], GPIO_OUT_TLF_CS );
            }
            tlfStatus = TLF_STATUS_OW_READ;
            tlfCounter = 0U;
        }
        break;
    case TLF_STATUS_OW_READ:
        ++tlfCounter;
        if ( tlfCounter >= (uint16_t)( TLF_OW_READ_TIME_TH ) )
        {
            if ( WWD_TRIG_SPI == WWDTSEL ) 
            {          
                tlfTrigger = 0U;
                trigActualValue = ( TLFmaster_recv.data[0U] & 0x01u );
                if ( 0U == trigActualValue )
                {
                    tlfTrigger = 1U;
                }
                if ( trigPrevValue == trigActualValue )
                {
                    trigErrCounter++;
                }
                else
                {
                    trigErrCounter = 0U;
                }
                trigPrevValue = trigActualValue;
            }
            else 
            {
                TLF_WDT_Ext_Drive( TLF_WDT_EXT_LEVEL_HI );
            }
            tlfStatus = TLF_STATUS_OW_SERVE;
            if ( trigErrCounter >= TRIG_ERR_CNT_TH ) 
            {
                tlfStatus = TLF_STATUS_IDLE;
            }
            tlfCounter = 0U;
        }
        break;
    case TLF_STATUS_OW_SERVE:
        ++tlfCounter;
        if ( tlfCounter >= (uint16_t)( TLF_OW_SERVE_TIME_TH ) )
        { 
            if ( WWD_TRIG_SPI == WWDTSEL ) 
            {          
                TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_WWDSCMD, tlfTrigger, &TLFmaster_send );
                res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv.data[0U], GPIO_OUT_TLF_CS );
            }
            else
            {
                TLF_WDT_Ext_Drive( TLF_WDT_EXT_LEVEL_LO );
            }
            //tlfStatus = TLF_STATUS_OW_SERVE_CHECK;
            tlfStatus = TLF_STATUS_CW;
            tlfCounter = 0U;
        }
        break;
//    case TLF_STATUS_OW_SERVE_CHECK:
//        if ( WWD_TRIG_SPI == WWDTSEL ) 
//        {      
//            res = S32K_LpSpi_Task(SPI_TLF);    
//            res = S32K_LpSpi_RecvMsg(SPI_TLF, &TLFmaster_recv);
//        }
//        tlfStatus = TLF_STATUS_CW;
//        break;
    case TLF_STATUS_IDLE:
        break;      
    case TLF_STATUS_DIAGNOSE_CHECK:
        if ( ( TLF_STATUS_CW == tlfStatusPre ) || ( TLF_STATUS_OW_READ == tlfStatusPre ) || ( TLF_STATUS_OW_SERVE == tlfStatusPre ) )
        {
            ++tlfCounter;
        }
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_MONSF0, 0x00u, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_MONSF0.data[0U], GPIO_OUT_TLF_CS );
        tlfMONSFVal = ( ( TLFmaster_recv_MONSF0.data[1U] >> 1U ) & 0x7FU ) | ( ( TLFmaster_recv_MONSF0.data[0U] << 7U ) & 0x80U );
        tlfMONSFValToCAN = tlfMONSFVal;
        //tlfMONSFVal = tlfMONSFVal & MONSF0_CHECK_MASK;
        if ( 0U != tlfMONSFVal )
        {
            tlfStatus = TLF_STATUS_DIAGNOSE_COVER;
            tlfGoToRecovery = 1U;
        }
        else
        {
            tlfStatus = tlfStatusPre;
            //tlfStatus = TLF_STATUS_GO_TO_NORMAL;
            tlfStatusCheckWIP = 0U;
            //tlfGoToRecovery = 1U;
        }
        break;
    case TLF_STATUS_DIAGNOSE_COVER:
        if ( ( TLF_STATUS_CW == tlfStatusPre ) || ( TLF_STATUS_OW_READ == tlfStatusPre ) || ( TLF_STATUS_OW_SERVE == tlfStatusPre ) )
        {
            ++tlfCounter;
        }      
        //TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_MONSF0, tlfMONSFVal, &TLFmaster_send );
        TLF_Spi_TxBufferBuild( TLF_CMD_WRITE, ADDR_MONSF0, 0xFFU, &TLFmaster_send );  //scrivo 1 a tappeto per svalidare tutto a prescindere
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_MONSF0.data[0U], GPIO_OUT_TLF_CS );
        //tlfStatus = TLF_STATUS_GO_TO_NORMAL;
        tlfStatus = TLF_STATUS_DIAGNOSE_READ;
        tlfStatusCheckWIP = 0U;
        break;
    case TLF_STATUS_DIAGNOSE_READ:
        if ( ( TLF_STATUS_CW == tlfStatusPre ) || ( TLF_STATUS_OW_READ == tlfStatusPre ) || ( TLF_STATUS_OW_SERVE == tlfStatusPre ) )
        {
            ++tlfCounter;
        }      
        TLF_Spi_TxBufferBuild( TLF_CMD_READ, ADDR_MONSF0, 0x00, &TLFmaster_send );
        res = S32K_LpSpi_TxRxMaster(SPI_TLF, &TLFmaster_send.data[0U], &TLFmaster_recv_MONSF0.data[0U], GPIO_OUT_TLF_CS );
        tlfMONSFVal = ( ( TLFmaster_recv_MONSF0.data[1U] >> 1U ) & 0x7FU ) | ( ( TLFmaster_recv_MONSF0.data[0U] << 7U ) & 0x80U );
        tlfMONSFValToCAN = tlfMONSFVal;
        tlfStatus = TLF_STATUS_GO_TO_NORMAL;
        tlfStatusCheckWIP = 0U;
        break;        
    default:
        break;
    }
    
    (void)res;
    return;
}
extern uint8_t TLF_Spi_GetStartedOK( void )
{
  return TLF_StartedOK;
}
