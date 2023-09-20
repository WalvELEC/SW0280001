/**************************************************************************************************
 * (C) 2018 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file wtypes.h
 * Brief description
 */
#ifndef WTYPES_H_
#define WTYPES_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/
#ifndef TRUE
#define TRUE    ( 1U )
#endif
#ifndef FALSE
#define FALSE   ( 0U )
#endif
#define WTYPES_VERSION_H               ( 1U )
#define WTYPES_VERSION_M               ( 1U )
#define WTYPES_VERSION_L               ( 1U )
#define W_INIT                         ( 0U )
#define W_MAIN                         ( 1U )

//#define TEST_DEBUG_CODE_SPY
//#define ISOBUS_MEM_TEST_DEBUG

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/
#define GetBit(var, bit)                 ((var & (1u<<bit))>>bit)
   
/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/
typedef union{
  unsigned long DWORD[1];
  unsigned int  WORD[2];
  unsigned char BYTE[4];
}tData4bytes;

/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/
typedef float  float32_t;
typedef double float64_t;

/** E.S.T.E. Function results */
typedef enum
{
    // Note errors
    WRES_ERROR_CLEARED               = 10,  /// Error Input cleared

    WRES_OPERATION_COMPLETED          = 5,  ///< Indicator for operation completed (used to triggere an event before ready for a new operation)
    WRES_BUFFER_NOTFULL               = 4,  ///< Circular buffer not full, some operation could be not correct
    WRES_BUFFER_EMPTY                 = 3,  ///< Circular buffer can not be read because the buffer is empty
    WRES_BUFFER_FULL                  = 2,  ///< Circular buffer can not be written because the buffer is full
    WRES_OPERATION_IN_PROGRESS        = 1,  ///< Indicator for operation in progress (e.g. ADC conversion not finished, DMA channel used, etc.)

    //WRES_OK                       = 1,  ///< WSL No error
    WRES_OK                           = 0,  ///< No error
    // Counter Errors
    WRES_ERROR                        = -1,  ///< Non-specific error code
    WRES_ERROR_NOT_READY              = -2,  ///< A requested final state is not reached
    WRES_ERROR_ACCESS_RIGHTS          = -3,  ///< Wrong mode (e.g. user/system) mode is set
    WRES_ERROR_INVALID_PARAMETER      = -4,  ///< Provided parameter is not valid
    WRES_ERROR_INVALID_MODE           = -5,  ///< Operation not allowed in current mode
    WRES_ERROR_UNINITIALIZED          = -6,  ///< Module (or part of it) was not initialized properly
    WRES_ERROR_TIMEOUT                = -7,  ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    WRES_ERROR_ADDRESS_ALIGNMENT      = -8,  ///< Address alignment does not match
    WRES_ERROR_NOT_IMPLEMENTED        = -9,  ///< L'operazione richiesta non e' ancora stata implementata, controllare la configurazione
    WRES_ERROR_MEMORYCORRUPTION       = -10, ///< A memory corruption has been identified
    WRES_ERROR_OUTOFMEMORY            = -11, ///< Operation cannot be performed because a memory allocation failed
    WRES_ERROR_MEMORYVIRGIN           = -12, ///<
    WRES_ERROR_OUTOFRANGE             = -13, ///<
} wres_t;

typedef struct w_error_entry_e
{
    uint32_t cnt;
    uint32_t cntTh;
    uint32_t value;
    uint8_t occurrence : 7;
    bool en;
    bool active;
} w_error_entry_t;

typedef struct w_status_e
{
    uint8_t value;
    uint8_t previous;
    uint32_t cnt;
    uint32_t cntTh;
} w_status_t;

/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/

#endif /* WTYPES_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
 
