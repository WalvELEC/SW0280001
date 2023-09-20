#ifndef _TLF_SPI_H
#define _TLF_SPI_H

#include "Cpu.h"
#include "wtypes.h"
#include "S32K_ObjectConfig.h"
#include "S32K_LpSpi.h"

//#define SPI_TLF S32K_LP_SPI_ID_0

extern uint8_t tlfStatus;
extern uint8_t tlfStatusPre;
extern uint8_t tlfTrigger;

extern s_LpSpi_msg_t TLFmaster_recv_WDCFG0;
extern s_LpSpi_msg_t TLFmaster_recv_WDCFG1;
extern s_LpSpi_msg_t TLFmaster_recv_WWDCFG0;
extern s_LpSpi_msg_t TLFmaster_recv_WWDCFG1;
extern s_LpSpi_msg_t TLFmaster_recv_Lock1;
extern s_LpSpi_msg_t TLFmaster_recv_Lock2;
extern s_LpSpi_msg_t TLFmaster_recv_Lock3;
extern s_LpSpi_msg_t TLFmaster_recv_Lock4;
extern s_LpSpi_msg_t TLFmaster_recv_ProtStat;
extern s_LpSpi_msg_t TLFmaster_recv_ProtStat2;
extern s_LpSpi_msg_t TLFmaster_recv_Sys;
extern s_LpSpi_msg_t TLFmaster_recv_UnLock1;
extern s_LpSpi_msg_t TLFmaster_recv_UnLock2;
extern s_LpSpi_msg_t TLFmaster_recv_UnLock3;
extern s_LpSpi_msg_t TLFmaster_recv_UnLock4;
extern s_LpSpi_msg_t TLFmaster_recv_DevCtrl;
extern s_LpSpi_msg_t TLFmaster_recv_DevCtrlN;
extern s_LpSpi_msg_t TLFmaster_recv_BuckFreSpread;
extern s_LpSpi_msg_t TLFmaster_recv_BuckFreSpreadRead;
extern s_LpSpi_msg_t TLFmaster_recv_BuckCfg;
extern s_LpSpi_msg_t TLFmaster_recv_BuckCfgCheck;
extern s_LpSpi_msg_t TLFmaster_recv_Window;
extern s_LpSpi_msg_t TLFmaster_recv_Window2;
extern s_LpSpi_msg_t TLFmaster_recv_MONSF0;
extern s_LpSpi_msg_t TLFmaster_recv_MONSF1;
extern s_LpSpi_msg_t TLFmaster_recv_MONSF2;
extern s_LpSpi_msg_t TLFmaster_recv_MONSF3;

extern void TLF_Spi_Init( void );
extern void TLF_Spi_Main( uint8_t index );
extern uint8_t TLF_Spi_GetStartedOK( void );

#endif
