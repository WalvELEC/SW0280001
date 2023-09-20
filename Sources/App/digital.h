/*
 * digital.h
 *
 *  Created on: 20 jun 2018
 *      Author: PoliD
 */

#ifndef _DIGITAL_H
#define _DIGITAL_H
#include "application.h"

#define DIGITAL_TH_ON_US  (  60000UL ) // in us
#define DIGITAL_TH_OFF_US (  60000UL ) // in us
#define DIGITAL_TH_RED_US ( 500000UL ) // in us

/*
#define DIGITAL_IN_ON_TH  ( 2804u )		//3,5V 
#define DIGITAL_IN_OFF_TH ( 1202u )		//1,5V
*/

#define DIGITAL_IN_ON_TH_MV_VBB  ( 7500UL )
#define DIGITAL_IN_OFF_TH_MV_VBB  ( 4500UL )

#define DIGITAL_IN_ON_TH_MV     ( 3500UL )
#define DIGITAL_IN_OFF_TH_MV    ( 1500UL )

#define DIGITAL_INPUT_NUM  ( 14U )

#define DIGITAL_ON_LEVEL_HI ( 1U )
#define DIGITAL_ON_LEVEL_LO ( 0U )

#define DIGITAL_IN_LEVEL_HI ( 1U )
#define DIGITAL_IN_LEVEL_LO ( 0U )

#define DIGITAL_IN_ON      ( 1U )
#define DIGITAL_IN_OFF     ( 0U )
#define DIGITAL_IN_ERROR   ( 2U )

#define DIGITAL_RED_TYPE_DISABLED     ( 0u )
#define DIGITAL_RED_TYPE_NOT_INVERTED ( 1u )
#define DIGITAL_RED_TYPE_INVERTED     ( 2u )
#define DIGITAL_IN_NOT_CONNECTED ( 0x1FU )
/*
#define DIGITAL_IN_ADC_REF_0   ( ADC_INPUT_G0_CH0 )
#define DIGITAL_IN_ADC_REF_1   ( ADC_INPUT_G0_CH1 )
#define DIGITAL_IN_ADC_REF_2   ( ADC_INPUT_G0_CH2 )
#define DIGITAL_IN_ADC_REF_3   ( ADC_INPUT_G0_CH3 )
#define DIGITAL_IN_ADC_REF_4   ( ADC_INPUT_G0_CH4 )
#define DIGITAL_IN_ADC_REF_5   ( ADC_INPUT_G0_CH5 )
#define DIGITAL_IN_ADC_REF_6   ( ADC_INPUT_G0_CH6 )
#define DIGITAL_IN_ADC_REF_7   ( ADC_INPUT_G0_CH7 )
#define DIGITAL_IN_ADC_REF_8   ( ADC_INPUT_G1_CH4 )
#define DIGITAL_IN_ADC_REF_9   ( ADC_INPUT_G1_CH5 )
#define DIGITAL_IN_ADC_REF_10  ( ADC_INPUT_G1_CH6 )
#define DIGITAL_IN_ADC_REF_11  ( ADC_INPUT_G1_CH7 )
#define DIGITAL_IN_ADC_REF_12  ( ADC_INPUT_G2_CH3 )
#define DIGITAL_IN_ADC_REF_13  ( ADC_INPUT_G3_CH0 )
#define DIGITAL_IN_ADC_REF_14  ( ADC_INPUT_G3_CH1 )
#define DIGITAL_IN_ADC_REF_15  ( ADC_INPUT_G3_CH2 )
#define DIGITAL_IN_ADC_REF_16  ( ADC_INPUT_G3_CH3 )
#define DIGITAL_IN_ADC_REF_17  ( ADC_INPUT_MUX_1 )//( ADC_INPUT_G2_CH2 ) //mux
*/
#define DIGITAL_IN_ADC_REF_0   ( ADC_AN_CH00 )
#define DIGITAL_IN_ADC_REF_1   ( ADC_AN_CH01 )
#define DIGITAL_IN_ADC_REF_2   ( ADC_AN_CH02 )
#define DIGITAL_IN_ADC_REF_3   ( ADC_AN_CH03 )
#define DIGITAL_IN_ADC_REF_4   ( ADC_AN_CH04 )
#define DIGITAL_IN_ADC_REF_5   ( ADC_AN_CH05 )
#define DIGITAL_IN_ADC_REF_6   ( ADC_AN_CH06 )
#define DIGITAL_IN_ADC_REF_7   ( ADC_AN_CH07 )
#define DIGITAL_IN_ADC_REF_8   ( ADC_AN_CH08 )
#define DIGITAL_IN_ADC_REF_9   ( ADC_AN_CH09 )
#define DIGITAL_IN_ADC_REF_10  ( ADC_AN_CH10 )
#define DIGITAL_IN_ADC_REF_11  ( ADC_AN_CH11 )
#define DIGITAL_IN_ADC_REF_12  ( ADC_AN_CH12 )
#define DIGITAL_IN_ADC_REF_13  ( ADC_AN_CH13 )


#define DIGITAL_IN_0           ( 0U )
#define DIGITAL_IN_1           ( 1U )
#define DIGITAL_IN_2           ( 2U )
#define DIGITAL_IN_3           ( 3U )
#define DIGITAL_IN_4           ( 4U )
#define DIGITAL_IN_5           ( 5U )
#define DIGITAL_IN_6           ( 6U )
#define DIGITAL_IN_7           ( 7U )
#define DIGITAL_IN_8           ( 8U )
#define DIGITAL_IN_9           ( 9U )
#define DIGITAL_IN_10          ( 10U )
#define DIGITAL_IN_11          ( 11U )
#define DIGITAL_IN_12          ( 12U )
#define DIGITAL_IN_13          ( 13U )

//#define DEFAULT_CFG_INPUT_TYPE ( 0x0003FFFFUL )  //bit da 0 a DIGITAL_INPUT_NUM-1 tutti 1 -> tutti input di tipo DIGITAL_ON_HI, cioè attivi alti
//#define DEFAULT_CFG_WAIT_NEED  ( 0x0003FFFFUL )  //bit da 0 a DIGITAL_INPUT_NUM-1 tutti 1 -> tutti input con attesa di un valore prima di iniziare a validare l'ingresso
//#define DEFAULT_CFG_WAIT_VALUE ( 0x00000000UL )  //bit da 0 a DIGITAL_INPUT_NUM-1 tutti 0 -> tutti input hanno valore di attesa DIGITAL_IN_LO, cioè basso, prima di validare l'ingresso

extern uint8_t Digital_GetValue( uint8_t index );
extern bool Digital_GetExternalError( void );
extern void Digital_Init( void );
extern void Digital_Main( void );

#endif
