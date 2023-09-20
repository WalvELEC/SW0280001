/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Timer.h
 * Brief description
 */
#ifndef S32K_SYSPIN_H_
#define S32K_SYSPIN_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include "wtypes.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/
#define CM_CONFIG_NUM			CLOCK_MANAGER_CONFIG_CNT
#define CM_ConfigsArr			g_clockManConfigsArr
#define CM_CallbacksArr			g_clockManCallbacksArr
#define CM_CALLBACK_CNT			CLOCK_MANAGER_CALLBACK_CNT
#define PM_InitConfigArr		g_pin_mux_InitConfigArr
#define PM_NUM_OF_CONF_PINS		NUM_OF_CONFIGURED_PINS

/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/
typedef enum {
    GPIO_PORT_A =0,
    GPIO_PORT_B =1,
    GPIO_PORT_C =2,
    GPIO_PORT_D =3,
    GPIO_PORT_E =4,
} s_GPIO_Port;

/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
     const GPIO_Type * const GpioPort;          // GPIO base address.
     uint32_t                PinNumber;         // Port pin number.
     port_data_direction_t   direction;         // Configures the port data direction.
} s_S32K_SysPin_GPioProp;

extern const s_S32K_SysPin_GPioProp GpioLIst[GPIO_NUM];
/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/
extern uint8_t edgeCounter;
extern uint8_t tlfIntFlag;
extern uint8_t tlfStatusCheckWIP;
extern uint8_t tlfGoToRecovery;
extern uint8_t tlfMONSFValToCAN;

/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/
extern wres_t S32K_SysPin_Init( void );
extern wres_t S32K_SysPin_InstallInterrupt( s_S32K_SysPin_GPioID PinId,port_interrupt_config_t Event );
extern wres_t S32K_SysPin_SetOutputPins(s_S32K_SysPin_GPioID PinId);
extern wres_t S32K_SysPin_ClearOutputPins(s_S32K_SysPin_GPioID PinId);
extern wres_t S32K_SysPin_ToggleOutputPins(s_S32K_SysPin_GPioID PinId);
extern wres_t S32K_SysPin_GetInputPins(s_S32K_SysPin_GPioID PinId, uint8_t *Value);
extern wres_t S32K_SysPin_GetOutputPins(s_S32K_SysPin_GPioID PinId, uint8_t *Value);
extern void S32K_SysPin_EnableGpioIRQ(void );
extern void S32K_SysPin_DisableGpioIRQ(void );
extern void S32K_SysPin_WaitForSync( bool SkipNext );
extern void CodeSpy_DEBUG_Set(s_S32K_SysPin_GPioID PinId);
extern void CodeSpy_DEBUG_Clear(s_S32K_SysPin_GPioID PinId);

/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/

#endif /* S32K_SYSPIN_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
