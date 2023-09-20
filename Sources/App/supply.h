/*
 * supply.h
 *
 *  Created on: 05 jul 2018
 *      Author: PoliD
 */

#ifndef _SUPPLY_H
#define _SUPPLY_H

#include "application.h"

#define SUPPLY_VBB_PROT_ERROR          ( 0x00U )
#define SUPPLY_VBB_RIT_ERROR           ( 0x01U )
#define SUPPLY_VCC_5VDC_ERROR          ( 0x02U )
#define SUPPLY_VCC_5VDC_EXT_ERROR      ( 0x03U )

#define SUPPLY_VBB_PROT_ERR_NO         ( 0x00U )
#define SUPPLY_VBB_PROT_ERR_LO         ( 0x01U )
#define SUPPLY_VBB_PROT_ERR_HI         ( 0x02U )
#define SUPPLY_VBB_PROT_ERR_CAN_OFF    ( 0x04U )

#define SUPPLY_VBB_RIT_ERR_NO         ( 0x00U )
#define SUPPLY_VBB_RIT_ERR_LO         ( 0x01U )
#define SUPPLY_VBB_RIT_ERR_HI         ( 0x02U )

#define SUPPLY_VCC_5VDC_ERR_NO         ( 0x00U )
#define SUPPLY_VCC_5VDC_ERR_LO         ( 0x01U )
#define SUPPLY_VCC_5VDC_ERR_HI         ( 0x02U )

#define SUPPLY_VCC_5VDC_EXT_ERR_NO     ( 0x00U )
#define SUPPLY_VCC_5VDC_EXT_ERR_LO     ( 0x01U )
#define SUPPLY_VCC_5VDC_EXT_ERR_HI     ( 0x02U )

extern uint8_t Supply_ErrorGet( uint8_t index );
extern uint16_t Supply_VbbProtValueGet( void );
extern uint16_t Supply_VccValueGet( void );
extern uint16_t Supply_VccExtValueGet( void );
extern wres_t Supply_Init( void );
extern wres_t Supply_Main( void );
#endif
