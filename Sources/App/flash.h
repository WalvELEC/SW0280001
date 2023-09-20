#ifndef _FLASH_H
#define _FLASH_H

#include "wtypes.h"
#include "Cpu.h"

//#define FLASH_PARAMETER_SECTOR_BASE ( XMC_FLASH_UNCACHED_BASE + 0x04000UL )
#define FLASH_PAGE_WORD_NUM         64U //XMC_FLASH_WORDS_PER_PAGE
//#define FLASH_SIZE_OF_DATA          ( 4UL )
#define FLASH_PAGE_NUM              ( 2U )

#define FLASH_ACCESS_REQUEST_OK     ( 1U )
#define FLASH_ACCESS_REQUEST_FAIL   ( 0U )

#define FLASH_MSG_TX_DM16_STATUS_OK ( 2U )
#define FLASH_MSG_TX_DM15_STATUS_OK ( 1U )
#define FLASH_MSG_TX_STATUS_NO      ( 0U )

#define FLASH_MSG_BYTE_NUM          ( 8U )

#define FLASH_INTERNAL_ACCESS_REQUEST_OFF   ( 0U )
#define FLASH_INTERNAL_ACCESS_REQUEST_WRITE ( 1U )
#define FLASH_INTERNAL_ACCESS_REQUEST_READ  ( 2U )

#define FLASH_ERROR_OK       ( 0U )
#define FLASH_ERROR_WRITE    ( 1U )
#define FLASH_ERROR_READ     ( 2U )

#define COMMAND_WRITE_FLASH             ( 0x01U )
#define COMMAND_READ_FLASH              ( 0x02U )
#define COMMAND_WRITE_RAM               ( 0x03U )
#define COMMAND_READ_RAM                ( 0x04U )

typedef struct 
{
    uint16_t length;
    uint8_t pointerType;
    uint8_t command;
    uint32_t pointer;
    uint16_t key;
    uint8_t toolSA;
} s_memAccessReq;

typedef struct 
{
    uint16_t length;
    uint8_t status;
    uint16_t seed;
} s_memAccessResp;

typedef struct {
    uint16_t length;
    uint32_t value;
} s_memAccessData;

extern uint8_t *Flash_GetTxData( uint8_t byteNum );
extern uint8_t Flash_CmdUpdate( uint8_t buf[8U], uint8_t toolSA );
extern uint8_t Flash_DataUpdate( uint8_t buf[8U] );
extern uint8_t Flash_MsgTxStatusGet( void );
extern void Flash_MsgTxStatusClear( void );
extern uint32_t Flash_ParameterGet( uint32_t index );
extern uint8_t Flash_ParameterUpdate( uint32_t address, uint32_t data );
extern uint8_t Flash_ParameterRead( uint32_t address, uint32_t *data );
extern void Flash_Write( uint8_t *result );
extern void Flash_InternalAccessSet( uint8_t accessReq, uint32_t address, uint32_t data );
extern void Flash_InternalAccessReset( void );
extern bool Flash_InternalAccessCompleted( void );
extern uint8_t Flash_ErrorGet( void );
extern void Flash_Init( void );
extern void Flash_Main( uint8_t index );

#endif