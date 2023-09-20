#include "string.h"
#include "application.h"
#include "S32K_Flash.h"
//#include "bsp.h"

// ------------------------------------------------------------------------------------------------------ 
//                         Flash Drivers
// ------------------------------------------------------------------------------------------------------ 
// Declare a FLASH config struct which initialized by FlashInit, and will be used by all flash operations 
// extern flash_ssd_config_t flashSSDConfig;
// Function declarations
//void CCIF_Handler(void);
// If target is flash, insert this macro to locate callback function into RAM
//START_FUNCTION_DECLARATION_RAMSECTION
//void CCIF_Callback(void)
//END_FUNCTION_DECLARATION_RAMSECTION

//status_t Initret;
//uint32_t Initaddress;
//uint32_t Initsize;
static uint32_t ABS_address;
static uint8_t WriteResult;
//static void HardwareFlash_Init( void );
// ------------------------------------------------------------------------------------------------------ 
//#define COMMAND_WRITE_FLASH             ( 0x01U )
//#define COMMAND_READ_FLASH              ( 0x02U )
//#define COMMAND_WRITE_RAM               ( 0x03U )
//#define COMMAND_READ_RAM                ( 0x04U )

#define FLASH_POINTER_EE2               ( 0x01000000UL )
#define FLASH_POINTER_RAM               ( 0x04000000UL )

#define FLASH_COMMAND_READ              ( 0x01U )
#define FLASH_COMMAND_WRITE             ( 0x02U )
#define FLASH_COMMAND_STATUS_REQ	( 0x03U )
#define FLASH_COMMAND_COMPLETED         ( 0x04U )
#define FLASH_COMMAND_FAIL              ( 0x05U )

#define FLASH_STATUS_PROCEED            ( 0x00U )
#define FLASH_STATUS_BUSY               ( 0x01U )
#define FLASH_STATUS_COMPLETED          ( 0x04U )
#define FLASH_STATUS_FAIL               ( 0x05U )

#define FLASH_KEY_READ_RAM              ( 0x1525U )
#define FLASH_KEY_WRITE_RAM             ( 0x2030U )
#define FLASH_KEY_READ_EE2              ( 0x5565U )
#define FLASH_KEY_WRITE_EE2             ( 0x6070U )

#define FLASH_NOT_VALID_CMD             ( 0x02U )
#define FLASH_VALID_CMD                 ( 0x01U )
#define FLASH_NO_CMD                    ( 0x00U )

#define FLASH_NOT_VALID_DATA            ( 0x02U )
#define FLASH_VALID_DATA                ( 0x01U )
#define FLASH_NO_DATA                   ( 0x00U )

#define FLASH_CHECK_PAGE_OK             ( 0x00U )

#define FLASH_OP_OK                     ( 0x00U )
#define FLASH_OP_FAIL                   ( 0x01U )

#define FLASH_STATUS_OFF                ( 0U )
#define FLASH_STATUS_WDT_WAIT           ( 1U )
#define FLASH_STATUS_ACCESS_ACK         ( 2U )
#define FLASH_STATUS_WRITE_FLASH        ( 3U )
#define FLASH_STATUS_READ_FLASH         ( 4U )
#define FLASH_STATUS_WRITE_RAM          ( 5U )
#define FLASH_STATUS_READ_RAM           ( 6U )
#define FLASH_STATUS_READ_ACK           ( 7U )
#define FLASH_STATUS_OP_EXIT            ( 8U )
#define FLASH_STATUS_WAIT_TX_COMPLETED  ( 9U )

#define FLASH_INTERNAL_STATUS_OFF          ( 0U )
#define FLASH_INTERNAL_STATUS_WDT_WAIT     ( 1U )
#define FLASH_INTERNAL_STATUS_READ_FLASH   ( 2U )
#define FLASH_INTERNAL_STATUS_WRITE_FLASH  ( 3U )
#define FLASH_INTERNAL_STATUS_OP_ERROR     ( 4U )
#define FLASH_INTERNAL_STATUS_OP_EXIT      ( 5U )
#define FLASH_INTERNAL_STATUS_OP_COMPLETED ( 6U )

//static uint32_t Flash_parameters[FLASH_PAGE_NUM * FLASH_PAGE_WORD_NUM];
static uint8_t Flash_newCmd;
static uint8_t Flash_newData;
static uint8_t Flash_cmd;
//static uint16_t Flash_addressBase;
static uint8_t Flash_status;
static uint8_t Flash_opResult;
static uint8_t Flash_msgTxStatus;
static uint8_t Flash_bufferTx[FLASH_MSG_BYTE_NUM];
//static uint32_t *Flash_address;

static s_memAccessReq Flash_accessRx;
static s_memAccessResp Flash_accessTx;
static s_memAccessData Flash_dataRx;
static uint32_t Flash_dataTx;

static uint32_t Flash_internalAccessAddress;
static uint32_t Flash_internalAccessData;
static uint8_t Flash_internalAccessRequest;
static uint8_t Flash_internalStatus;
static uint8_t Flash_error;


//uint8_t Flash_TPData[MEM_SIZE*4U + 1U];
static void Flash_BufferTxDM15Update( void );
//static void HardwareFlash_Init( void );
static uint8_t NXP_WriteDWord( uint32_t DW_address, uint32_t data );
static uint8_t NXP_ReadDWord( uint32_t DW_address, uint32_t *data );
static void Flash_BufferTxDM16Update( void );
static void Flash_InternalAccessMain( void );
static void Flash_ExternalAccessMain( void );

//------------------------------------------------------------------------------
uint8_t Flash_ParameterRead( uint32_t address, uint32_t *data )
{
//    Flash_Read( FLASH_PARAMETER_SECTOR_BASE, address, data );
	return NXP_ReadDWord( address, data );
}

//------------------------------------------------------------------------------
uint8_t Flash_ParameterUpdate( uint32_t address, uint32_t data )
{
    return (NXP_WriteDWord(address,data));
}

//------------------------------------------------------------------------------
uint32_t Flash_ParameterGet( uint32_t index )
{
    uint32_t data = 0U;
    //return Flash_parameters[index];
    NXP_ReadDWord( index, &data );
    return data;
}

////------------------------------------------------------------------------------
static void Flash_BufferTxDM15Update( void )
{
    Flash_bufferTx[0U] = (uint8_t)Flash_accessTx.length;
    Flash_bufferTx[1U] = (uint8_t)( ( Flash_accessTx.length >> 3U ) & 0xE0U ) + (uint8_t)( Flash_accessTx.status << 1U );
    Flash_bufferTx[2U] = 0xFFU;
    Flash_bufferTx[3U] = 0xFFU;
    Flash_bufferTx[4U] = 0xFFU;
    Flash_bufferTx[5U] = 0xFFU;
    Flash_bufferTx[6U] = (uint8_t)( Flash_accessTx.seed );
    Flash_bufferTx[7U] = (uint8_t)( Flash_accessTx.seed >> 8U );
    
    return;
}

//------------------------------------------------------------------------------
static void Flash_BufferTxDM16Update( void )
{
    Flash_bufferTx[0U] = (uint8_t)Flash_accessTx.length;
    Flash_bufferTx[1U] = (uint8_t)( Flash_dataTx );
    if ( 1U < Flash_accessTx.length )
    {
        Flash_bufferTx[2U] = (uint8_t)( Flash_dataTx >> 8U );
    }
    else
    {
        Flash_bufferTx[2U] = 0xFFU;
    }
    if ( 2U < Flash_accessTx.length )
    {
        Flash_bufferTx[3U] = (uint8_t)( Flash_dataTx >> 16U );
    }
    else
    {
        Flash_bufferTx[3U] = 0xFFU;
    }    
    if ( 3U < Flash_accessTx.length )
    {
        Flash_bufferTx[4U] = (uint8_t)( Flash_dataTx >> 24U );
    }
    else
    {
        Flash_bufferTx[4U] = 0xFFU;
    }    
    Flash_bufferTx[5U] = 0xFFU;
    Flash_bufferTx[6U] = 0xFFU;
    Flash_bufferTx[7U] = 0xFFU;
    
    return;
}

//------------------------------------------------------------------------------
uint8_t Flash_CmdUpdate( uint8_t buf[8U], uint8_t toolSA )
{
    uint8_t retVal;  
  
    retVal = FLASH_ACCESS_REQUEST_FAIL;    

    Flash_accessRx.toolSA = toolSA; 
    Flash_accessRx.length = ( ( (uint16_t)buf[1U] & 0xE0u ) << 3U ) + buf[0U];
    Flash_accessRx.pointerType = (uint8_t)( ( buf[1U] & 0x10U ) >> 4U );
    Flash_accessRx.command = (uint8_t)( ( buf[1U] & 0x0EU ) >> 1U );
    Flash_accessRx.pointer = (uint32_t)( buf[2U] ) + (uint32_t)( (uint32_t)( buf[3U] ) << 8U ) + (uint32_t)( (uint32_t)( buf[4U] ) << 16U ) + (uint32_t)( (uint32_t)( buf[5U] ) << 24U );
    Flash_accessRx.key = (uint16_t)( buf[6U] ) + (uint16_t)( buf[7U] << 8U );
    if ( 0U == Flash_accessRx.pointerType ) 
    {
        if ( FLASH_POINTER_RAM == ( Flash_accessRx.pointer & 0xFF000000U ) ) 
        {
	    if ( ( FLASH_COMMAND_READ == Flash_accessRx.command ) && ( FLASH_KEY_READ_RAM == Flash_accessRx.key ) && ( 4U >= Flash_accessRx.length ) ) 
            {
                Flash_cmd = COMMAND_READ_RAM;
                Flash_newCmd = FLASH_VALID_CMD;
                retVal = FLASH_ACCESS_REQUEST_OK;
                Flash_accessTx.status = FLASH_STATUS_PROCEED;
	        Flash_accessTx.length = Flash_accessRx.length;
	        Flash_accessTx.seed = 0xFFFFU;
            } 
            else if ( ( FLASH_COMMAND_WRITE == Flash_accessRx.command ) && ( FLASH_KEY_WRITE_RAM == Flash_accessRx.key ) && ( 4U >= Flash_accessRx.length ) ) 
            {
                Flash_cmd = COMMAND_WRITE_RAM;
                Flash_newCmd = FLASH_VALID_CMD;
                retVal = FLASH_ACCESS_REQUEST_OK;              
                Flash_accessTx.status = FLASH_STATUS_PROCEED;
	        Flash_accessTx.length = Flash_accessRx.length;
	        Flash_accessTx.seed = 0xFFFFU;
	    } 
            else
            {
                Flash_newCmd = FLASH_NOT_VALID_CMD;
                Flash_accessTx.status = FLASH_STATUS_BUSY;
	        Flash_accessTx.length = 0U;
	        Flash_accessTx.seed = 0xFFFFU;                
            }
        }
	else if ( FLASH_POINTER_EE2 == ( Flash_accessRx.pointer & 0xFF000000U ) )
        {
	    if ( ( FLASH_COMMAND_READ == Flash_accessRx.command ) && ( FLASH_KEY_READ_EE2 == Flash_accessRx.key ) && ( 4U >= Flash_accessRx.length ) ) 
            {
                Flash_cmd = COMMAND_READ_FLASH;
                Flash_newCmd = FLASH_VALID_CMD;
                retVal = FLASH_ACCESS_REQUEST_OK;   
                Flash_accessTx.status = FLASH_STATUS_PROCEED;
	        Flash_accessTx.length = Flash_accessRx.length;
	        Flash_accessTx.seed = 0xFFFFU;                
            } 
            else if ( ( FLASH_COMMAND_WRITE == Flash_accessRx.command ) && ( FLASH_KEY_WRITE_EE2 == Flash_accessRx.key ) && ( 4U >= Flash_accessRx.length ) )
            {
                Flash_cmd = COMMAND_WRITE_FLASH;
                Flash_newCmd = FLASH_VALID_CMD;
                retVal = FLASH_ACCESS_REQUEST_OK;   
                Flash_accessTx.status = FLASH_STATUS_PROCEED;
	        Flash_accessTx.length = Flash_accessRx.length;
	        Flash_accessTx.seed = 0xFFFFU;                
            } 
            else 
            {
                Flash_newCmd = FLASH_NOT_VALID_CMD;
                Flash_accessTx.status = FLASH_STATUS_BUSY;
	        Flash_accessTx.length = 0U;
	        Flash_accessTx.seed = 0xFFFFU;                
	    }
        }
        else
        {
            Flash_newCmd = FLASH_NOT_VALID_CMD;
        }
    }
    else
    {
        Flash_newCmd = FLASH_NOT_VALID_CMD;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
uint8_t Flash_DataUpdate( uint8_t buf[8U] )
{ 
    uint8_t retVal;
    
    retVal = FLASH_ACCESS_REQUEST_FAIL;
    
    if ( 4U >= buf[0U] )
    {
        Flash_dataRx.length = buf[0U];
        Flash_dataRx.value = (uint32_t)( buf[1U] );
        if ( 1U < Flash_dataRx.length )
        {
            Flash_dataRx.value = Flash_dataRx.value + ( (uint32_t)buf[2U] << 8U );
        }
        if ( 2U < Flash_dataRx.length )
        {
            Flash_dataRx.value = Flash_dataRx.value + ( (uint32_t)buf[3U] << 16U );
        }    
        if ( 3U < Flash_dataRx.length )
        {
            Flash_dataRx.value = Flash_dataRx.value + ( (uint32_t)buf[4U] << 24U );
        }    
        retVal = FLASH_ACCESS_REQUEST_OK;
        Flash_newData = FLASH_VALID_DATA;
    }
    else
    {
        Flash_newData = FLASH_NOT_VALID_DATA;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
uint8_t *Flash_GetTxData( uint8_t byteNum )
{
//    uint8_t *buffer;
//
//    buffer = (uint8_t *)malloc( byteNum * sizeof( uint8_t ) );
//
//    buffer = Flash_bufferTx;
//
//    return buffer;
	(void)byteNum;
    return (uint8_t *)(Flash_bufferTx);
}

//------------------------------------------------------------------------------
uint8_t Flash_MsgTxStatusGet( void )
{
    return Flash_msgTxStatus;
}

//------------------------------------------------------------------------------
void Flash_MsgTxStatusClear( void )
{
    Flash_msgTxStatus = FLASH_MSG_TX_STATUS_NO;
    
    return;
}

//------------------------------------------------------------------------------
void Flash_Write( uint8_t *result )
{
//    uint8_t i;
//    
//    Flash_EraseSector( FLASH_PARAMETER_SECTOR_BASE );
//    for ( i=0U; i<FLASH_PAGE_NUM; i++ )
//    {
//        Flash_WritePage( FLASH_PARAMETER_SECTOR_BASE + ( (uint32_t)( i ) * FLASH_SIZE_OF_DATA * FLASH_PAGE_WORD_NUM ), &Flash_parameters[(uint32_t)( i ) * FLASH_PAGE_WORD_NUM ] );
//    }
//    Flash_CheckPage( FLASH_PARAMETER_SECTOR_BASE, &Flash_parameters[0U], result );
    *result = WriteResult;
    return;
}

//------------------------------------------------------------------------------
void Flash_InternalAccessSet( uint8_t accessReq, uint32_t address, uint32_t data )
{
    Flash_internalAccessRequest = accessReq;
    Flash_internalAccessAddress = address;
    Flash_internalAccessData = data;
    
    return;
}    

//------------------------------------------------------------------------------
void Flash_InternalAccessReset( void )
{
    Flash_internalAccessRequest = FLASH_INTERNAL_ACCESS_REQUEST_OFF;
    Flash_internalAccessAddress = 0xFFUL;
    Flash_internalAccessData = 0xFFFFFFFFUL;
    
    return;
}

//------------------------------------------------------------------------------
bool Flash_InternalAccessCompleted( void )
{
    bool retVal = false;
    
    if ( FLASH_INTERNAL_STATUS_OP_COMPLETED == Flash_internalStatus )
    {
        retVal = true;
    }
    
    return retVal;
}

//------------------------------------------------------------------------------
static void Flash_InternalAccessMain( void )
{
    switch ( Flash_internalStatus )
    {
    case FLASH_INTERNAL_STATUS_OFF:
        if ( ( FLASH_INTERNAL_ACCESS_REQUEST_OFF != Flash_internalAccessRequest ) && ( FLASH_STATUS_OFF == Flash_status ) )
        {
//#ifndef DP_DEBUG_EN           
//            TLE_WDTRequestStopSet();
//#endif
            Flash_internalStatus = FLASH_INTERNAL_STATUS_WDT_WAIT;
        }
        break;
    case FLASH_INTERNAL_STATUS_WDT_WAIT:
//#ifndef DP_DEBUG_EN       
//        if ( TLE_WDTCompletedStopGet() )
//        {
//#endif           
            if ( FLASH_INTERNAL_ACCESS_REQUEST_READ == Flash_internalAccessRequest )
            {
                Flash_internalStatus = FLASH_INTERNAL_STATUS_READ_FLASH;
            }
            else if ( FLASH_INTERNAL_ACCESS_REQUEST_WRITE == Flash_internalAccessRequest )
            {
                Flash_internalStatus = FLASH_INTERNAL_STATUS_WRITE_FLASH;
            }
            else
            {
                Flash_internalStatus = FLASH_INTERNAL_STATUS_OP_ERROR;      
            }
//#ifndef DP_DEBUG_EN             
//            TLE_WDTCompletedStopClear();
//        }      
//#endif         
        break;
    case FLASH_INTERNAL_STATUS_READ_FLASH:
        Flash_opResult = MemData_GetEepromItem( Flash_internalAccessAddress, &Flash_internalAccessData );
        if ( FLASH_OP_OK != Flash_opResult )
        {
            Flash_error |= FLASH_ERROR_READ;
        }     
//#ifndef DP_DEBUG_EN         
//        TLE_WDTRequestStartSet();
//#endif         
        Flash_internalStatus = FLASH_INTERNAL_STATUS_OP_EXIT;      
        break;
    case FLASH_INTERNAL_STATUS_WRITE_FLASH:
        Flash_opResult = MemData_SetEepromItem( Flash_internalAccessAddress, Flash_internalAccessData );
        if ( FLASH_OP_OK != Flash_opResult )
        {
            Flash_error |= FLASH_ERROR_WRITE;
        }
//#ifndef DP_DEBUG_EN         
//        TLE_WDTRequestStartSet();
//#endif         
        if ( ADDR_BOOT_SET_DATA_01 == Flash_internalAccessAddress )
        {
            Flash_internalAccessAddress = ADDR_BOOT_SET_DATA_02;
        }
        else
        {
            Flash_internalStatus = FLASH_INTERNAL_STATUS_OP_EXIT;      
        }
        break;   
    case FLASH_INTERNAL_STATUS_OP_ERROR:
        Flash_opResult = FLASH_OP_FAIL;
//#ifndef DP_DEBUG_EN         
//        TLE_WDTRequestStartSet();
//#endif        
        Flash_internalStatus = FLASH_INTERNAL_STATUS_OP_EXIT;
        break;        
    case FLASH_INTERNAL_STATUS_OP_EXIT:
//#ifndef DP_DEBUG_EN       
//        if ( TLE_WDTCompletedStartGet() )
//        {
//#endif          
            //TLE_WDTCompletedStartClear();
            Flash_internalStatus = FLASH_INTERNAL_STATUS_OP_COMPLETED;            
//#ifndef DP_DEBUG_EN             
//        }
//#endif
        break;
    case FLASH_INTERNAL_STATUS_OP_COMPLETED:
        if ( FLASH_INTERNAL_ACCESS_REQUEST_OFF == Flash_internalAccessRequest )
        {
            Flash_internalStatus = FLASH_INTERNAL_STATUS_OFF;
        }
        break;
    default:
        break;
    }    
      
    return;
}

//------------------------------------------------------------------------------
static void Flash_ExternalAccessMain( void )
{
    static uint16_t Flash_addressError = 0xFFFFU;
//    static uint16_t Flash_addressOffset = 0U;
//    static uint8_t tpIndex = 0xFFU;
    static uint16_t Flash_address;
    static uint32_t Flash_data;
//    static uint8_t Flash_TPData[MEM_SIZE*4U + 1U];
//    uint8_t i;
//    uint16_t numPackets;
      
    switch ( Flash_status )
    {
    case FLASH_STATUS_OFF:
        if ( FLASH_VALID_CMD == Flash_newCmd )
        {
            if ( FLASH_INTERNAL_ACCESS_REQUEST_OFF == Flash_internalAccessRequest )
            {
                Flash_newCmd = FLASH_NO_CMD;
//#ifndef DP_DEBUG_EN
//                TLE_WDTRequestStopSet();
//#endif
                Flash_status = FLASH_STATUS_WDT_WAIT;
            }
            else
            {
                Flash_newCmd = FLASH_NO_CMD;
                Flash_accessTx.status = FLASH_STATUS_BUSY;
                Flash_accessTx.length = 0U;
                Flash_accessTx.seed = 0xFFFFU;
                Flash_BufferTxDM15Update();
                Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;
                Flash_status = FLASH_STATUS_WAIT_TX_COMPLETED;                    
            }
        }
        else if ( FLASH_NOT_VALID_CMD == Flash_newCmd )
        {
            Flash_newCmd = FLASH_NO_CMD;
            Flash_BufferTxDM15Update();
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;
            Flash_status = FLASH_STATUS_WAIT_TX_COMPLETED;
        }
        else
        {
          // Do Nothing
        }
        break;
    case FLASH_STATUS_WDT_WAIT:
//#ifndef DP_DEBUG_EN      
//        if ( TLE_WDTCompletedStopGet() )
//        {
//#endif
            Flash_BufferTxDM15Update();
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;
            Flash_status = FLASH_STATUS_ACCESS_ACK;
//#ifndef DP_DEBUG_EN            
//            TLE_WDTCompletedStopClear();
//        }
//#endif                    
        break;
    case FLASH_STATUS_ACCESS_ACK:
        if ( FLASH_MSG_TX_STATUS_NO == Flash_msgTxStatus )
        {
            if ( COMMAND_WRITE_FLASH == Flash_cmd )
            {
                Flash_status = FLASH_STATUS_WRITE_FLASH;
            }
            else if ( COMMAND_READ_FLASH == Flash_cmd )
            {
                Flash_status = FLASH_STATUS_READ_FLASH;
//                Flash_addressOffset = 0U;
            }
            else if ( COMMAND_WRITE_RAM == Flash_cmd )
            {
                Flash_status = FLASH_STATUS_WRITE_RAM;
            }
            else if ( COMMAND_READ_RAM == Flash_cmd )
            {
                Flash_status = FLASH_STATUS_READ_RAM;
//                Flash_addressOffset = 0U;
            }
            else 
            {
              // Do Nothing
            }
        }
        break;
    case FLASH_STATUS_WRITE_FLASH:
        if ( FLASH_VALID_DATA == Flash_newData )
        {
            Flash_opResult = FLASH_OP_FAIL;
            Flash_newData = FLASH_NO_DATA;
            if ( 4U >= Flash_dataRx.length ) 
            {
                Flash_address = (uint16_t)( Flash_accessRx.pointer & 0x0000FFFFU );
                Flash_data = Flash_dataRx.value;
                if ( ( 0x03FFU < Flash_address ) /*|| ( ADDR_SAEJ_DEFAULT_ADDRESS == Flash_address )*/ ) //il soruce address si cambia col commanded address
                {
                    Flash_accessTx.status = FLASH_STATUS_FAIL;
                    Flash_accessTx.length = 0U;
                    Flash_accessTx.seed = 0xFFFFU;                  
                }
                else
                {
                    Flash_opResult = MemData_SetEepromItem( (uint32_t)Flash_address, Flash_data );
                    if ( FLASH_OP_OK == Flash_opResult )
                    {
                        Flash_accessTx.status = FLASH_STATUS_COMPLETED;
                        Flash_accessTx.length = 0U;
                        Flash_accessTx.seed = 0xFFFFU;   
                        if ( Flash_address == Flash_addressError )
                        {
                            Flash_error &= ~FLASH_ERROR_WRITE;
                            Flash_addressError = 0xFFFFU;
                        }
                    }
                    else
                    {
                        Flash_accessTx.status = FLASH_STATUS_FAIL;
                        Flash_accessTx.length = 0U;
                        Flash_accessTx.seed = 0xFFFFU;
                        Flash_addressError = Flash_address;
                        Flash_error |= FLASH_ERROR_WRITE;
                    }
                }
                Flash_BufferTxDM15Update();     
                Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;  
                Flash_status = FLASH_STATUS_OP_EXIT;            
            }
            else
            {
                Flash_accessTx.status = FLASH_STATUS_FAIL;
                Flash_accessTx.length = 0U;
                Flash_accessTx.seed = 0xFFFFU;   
                Flash_addressError = Flash_address;
                Flash_error |= FLASH_ERROR_WRITE;
            Flash_BufferTxDM15Update();     
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;    
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Flash_status = FLASH_STATUS_OP_EXIT;            
            }  
        } 
        else if ( FLASH_NOT_VALID_DATA == Flash_newData )
        {
            Flash_newData = FLASH_NO_DATA;
            Flash_accessTx.status = FLASH_STATUS_FAIL;
            Flash_accessTx.length = 0U;
            Flash_accessTx.seed = 0xFFFFU;            
            Flash_BufferTxDM15Update();
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;  
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Flash_status = FLASH_STATUS_OP_EXIT;
        }
        else
        {
          // Do Nothing
        }
        break;
    case FLASH_STATUS_READ_FLASH:
        Flash_data = 0xFFFFFFFFU;
        Flash_opResult = FLASH_OP_FAIL;
        Flash_address = (uint16_t)( Flash_accessRx.pointer & 0x0000FFFFU );
        Flash_opResult = MemData_GetEepromItem( (uint32_t)Flash_address, &Flash_data );
        if ( FLASH_OP_OK == Flash_opResult )
        {
            Flash_dataTx = Flash_data;
        }
        else
        {
            Flash_dataTx = 0xFFFFFFFFUL;
        }
        Flash_BufferTxDM16Update();
        Flash_msgTxStatus = FLASH_MSG_TX_DM16_STATUS_OK;
        //TLE_WDTRequestStartSet();
        Flash_status = FLASH_STATUS_READ_ACK;
        break;
    case FLASH_STATUS_WRITE_RAM:
        if ( FLASH_VALID_DATA == Flash_newData )
        {
            Flash_opResult = FLASH_OP_FAIL;
            Flash_newData = FLASH_NO_DATA;
            if ( 4U >= Flash_dataRx.length ) 
            {
                Flash_address = (uint16_t)( Flash_accessRx.pointer & 0x0000FFFFU );
                Flash_data = Flash_dataRx.value;
                Flash_opResult = MemData_SetRAMItem( (uint32_t)Flash_address, Flash_data );
                if ( FLASH_OP_OK == Flash_opResult )
                {
                    Flash_accessTx.status = FLASH_STATUS_COMPLETED;
                    Flash_accessTx.length = 0U;
                    Flash_accessTx.seed = 0xFFFFU;   
                }
                else
                {
                    Flash_accessTx.status = FLASH_STATUS_FAIL;
                    Flash_accessTx.length = 0U;
                    Flash_accessTx.seed = 0xFFFFU;
                }
            }
            else
            {
                Flash_accessTx.status = FLASH_STATUS_FAIL;
                Flash_accessTx.length = 0U;
                Flash_accessTx.seed = 0xFFFFU;   
            }   
            Flash_BufferTxDM15Update();     
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;   
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Flash_status = FLASH_STATUS_OP_EXIT;            
        } 
        else if ( FLASH_NOT_VALID_DATA == Flash_newData )
        {
            Flash_newData = FLASH_NO_DATA;
            Flash_accessTx.status = FLASH_STATUS_FAIL;
            Flash_accessTx.length = 0U;
            Flash_accessTx.seed = 0xFFFFU;            
            Flash_BufferTxDM15Update();
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK; 
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Flash_status = FLASH_STATUS_OP_EXIT;
        }
        else
        {
          // Do Nothing
        }
        break;
    case FLASH_STATUS_READ_RAM:
        Flash_data = 0xFFFFFFFFU;
        Flash_opResult = FLASH_OP_FAIL;
        Flash_address = (uint16_t)( Flash_accessRx.pointer & 0x0000FFFFU );
        Flash_opResult = MemData_GetRAMItem( (uint32_t)Flash_address, &Flash_data );
        if ( FLASH_OP_OK == Flash_opResult )
        {
            Flash_dataTx = Flash_data;
        }  
        else
        {
            Flash_dataTx = 0xFFFFFFFFUL;
        }
        Flash_BufferTxDM16Update();
        Flash_msgTxStatus = FLASH_MSG_TX_DM16_STATUS_OK;
        //TLE_WDTRequestStartSet();
        Flash_status = FLASH_STATUS_READ_ACK;      
        break;
    case FLASH_STATUS_READ_ACK:
        if ( FLASH_MSG_TX_STATUS_NO == Flash_msgTxStatus )
        {
            Flash_accessTx.status = FLASH_STATUS_COMPLETED;
            Flash_accessTx.length = 0U;
            Flash_accessTx.seed = 0xFFFFU;          
            Flash_BufferTxDM15Update();
            Flash_msgTxStatus = FLASH_MSG_TX_DM15_STATUS_OK;      
//#ifndef DP_DEBUG_EN            
//            TLE_WDTRequestStartSet();
//#endif            
            Flash_status = FLASH_STATUS_OP_EXIT;
        }            
        break;
    case FLASH_STATUS_OP_EXIT:
//#ifndef DP_DEBUG_EN      
//        if ( TLE_WDTCompletedStartGet() )
//        {
//            TLE_WDTCompletedStartClear();
//#endif            
            if ( FLASH_MSG_TX_STATUS_NO == Flash_msgTxStatus )
            {
                Flash_status = FLASH_STATUS_OFF;            
            }
//#ifndef DP_DEBUG_EN            
//        }
//#endif        
        break;
    case FLASH_STATUS_WAIT_TX_COMPLETED:
        if ( FLASH_MSG_TX_STATUS_NO == Flash_msgTxStatus )
        {
            Flash_status = FLASH_STATUS_OFF;
        }
        break;
    default:
        break;
    }
    return;
}

//------------------------------------------------------------------------------
uint8_t Flash_ErrorGet( void )
{
    return Flash_error;
}

//------------------------------------------------------------------------------
void Flash_Init( void )
{
    //HardwareFlash_Init();
    //memset( Flash_parameters, 0xFFFFFFFF, FLASH_PAGE_WORD_NUM );
    memset( Flash_bufferTx, 0x00, FLASH_MSG_BYTE_NUM );
    Flash_internalStatus = FLASH_INTERNAL_STATUS_OFF;
    Flash_status = FLASH_STATUS_OFF;
    Flash_opResult = FLASH_OP_OK;
    Flash_msgTxStatus = FLASH_MSG_TX_STATUS_NO;
    //Flash_ReadPage( FLASH_PARAMETER_SECTOR_BASE, &Flash_parameters[0U] );
    Flash_newCmd = FLASH_NO_CMD;
    Flash_newData = FLASH_NO_DATA;
    Flash_internalAccessRequest = FLASH_INTERNAL_ACCESS_REQUEST_OFF;
    Flash_internalStatus = FLASH_INTERNAL_STATUS_OFF;
    Flash_internalAccessAddress = 0xFFUL;
    Flash_internalAccessData = 0xFFFFFFFFUL;
    Flash_error = FLASH_ERROR_OK;
    WriteResult  = FLASH_ERROR_OK;
    return;
}

//------------------------------------------------------------------------------
void Flash_Main( uint8_t index )
{
    (void)index;
	Flash_InternalAccessMain();
    Flash_ExternalAccessMain();
    return;
}

// ------------------------------------------------------------------------------------------------------ 
//                         Flash Drivers
// ------------------------------------------------------------------------------------------------------ 

static uint8_t NXP_ReadDWord( uint32_t DW_address, uint32_t *data )
{
        uint8_t res;
	ABS_address =  (DW_address << 2u);
	if (WRES_OK == S32K_Flash_ReadByteNum( ABS_address, (uint8_t *)data, 4))
	{
		res = FLASH_OP_OK;
	}
	else
	{
                res = FLASH_OP_FAIL;
	}
        return res;
}
static uint8_t NXP_WriteDWord( uint32_t DW_address, uint32_t data )
{
        uint32_t ABS_Data;
        
        uint8_t res;
	 ABS_address =  (DW_address << 2u);
	 ABS_Data =data;
	 uint8_t *pData =( uint8_t *)&ABS_Data;
	if (WRES_OK ==  S32K_Flash_WriteByteNum( ABS_address, pData, 4U))
	{
		res = FLASH_OP_OK;
	}
	else
	{
                res = FLASH_OP_FAIL;
	}
        return res;
}
//static void HardwareFlash_Init( void )
//{
//	wres_t Res;
//	Res = S32K_Flash_Init();
//	DEV_ASSERT(WRES_OK == Res);
//}
//static uint8_t NXP_ReadDWord( uint32_t DW_address, uint32_t *data )
//{
//  uint32_t *flash_address;
//  uint32_t ABS_address;
//
//  if (flashSSDConfig.EEESize != 0u){
//  ABS_address =  ((uint8_t)(DW_address) << 2u);
//  ABS_address = flashSSDConfig.EERAMBase + ABS_address;
//  flash_address = (uint32_t *)( ABS_address );
//
//    *data = *flash_address;
//    return FLASH_OP_OK;
//  }else{
//    return FLASH_OP_FAIL;
//  }
//}

//typedef union{
//  unsigned long DWORD[1];
//  unsigned int  WORD[2];
//  unsigned char BYTE[4];
//}tData4bytes1;
//
//tData4bytes1 DW_Data1;
//static uint8_t NXP_WriteDWord( uint32_t DW_address, uint32_t data )
//{
//  uint32_t ABS_address;
//  uint8_t result;
//  uint8_t size;
//
//  if (flashSSDConfig.EEESize != 0u){
//        ABS_address =  ((uint8_t)(DW_address) << 2u);
//        ABS_address = flashSSDConfig.EERAMBase + ABS_address;
//        size = 4U;
//        DW_Data1.DWORD[0] = data;
//        result = FLASH_DRV_EEEWrite(&flashSSDConfig, ABS_address, size, &DW_Data1.BYTE[0]);
//        if(STATUS_SUCCESS == result){
//          return FLASH_OP_OK;
//        }else{
//          return FLASH_OP_FAIL;
//        }
//  }else{
//    return FLASH_OP_FAIL;
//  }
//}
