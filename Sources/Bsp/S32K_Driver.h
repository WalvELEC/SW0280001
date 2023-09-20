/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Can.h
 * Brief description
 */
#ifndef S32K_DRIVER_H_
#define S32K_DRIVER_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include "wtypes.h"
#include "S32K_CanDma.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/
#define TASK_DRIVER_INDEX  TASK_0_ID
#define TASK_DRIVER_PERIOD 10000
#define TASK_DRIVER_DELAY  0000

#define TASK_WDT_INDEX  TASK_1_ID
#define TASK_WDT_PERIOD 10000
#define TASK_WDT_DELAY  2000

#define CAN_JOY_OUT   S32K_CAN_ID_0
#define CAN_HAD_OUT   S32K_CAN_ID_1

#define SPI_MLX_1 S32K_LP_SPI_ID_0
#define SPI_MLX_2 S32K_LP_SPI_ID_1
#define SPI_TLF   S32K_LP_SPI_ID_2

#define FLEXIO_SPI_SLAVE 	S32K_FLEXSPI_ID_0




/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/


/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/

/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/
extern void S32K_Driver_Task( void );
extern wres_t S32K_Driver_Init( void );

/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/

#endif /* S32K_DRIVER_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/