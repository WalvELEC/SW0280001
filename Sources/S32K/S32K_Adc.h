/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Adc.h
 * Brief description
 */
#ifndef S32K_ADC_H_
#define S32K_ADC_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES                                                                             *
 **************************************************************************************************/
#include "wtypes.h"
#include "adc_pal.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2.  PUBLIC DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  PUBLIC MACROS                                                                             *
 **************************************************************************************************/
#define Ch_0 0U
#define Ch_1 1U
#define Ch_2 2U
#define Ch_3 3U
#define Ch_4 4U
#define Ch_5 5U
#define Ch_6 6U
#define Ch_7 7U

/**************************************************************************************************
 *  4.  PUBLIC ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5.  PUBLIC STRUCTURES                                                                         *
 **************************************************************************************************/


/**************************************************************************************************
 *  6.  PUBLIC TYPES                                                                              *
 **************************************************************************************************/
typedef enum
{
	  ADC0_GROUP_0,       // Add first group referred to ADC_0
	  ADC0_GROUP_1,
	  ADC1_GROUP_0,
	  ADC1_GROUP_1,
	  ADC_GROUP_NUM,          // Leave GROUP_NUM as last definition
}  s_GroupList;

typedef struct
{
	s_GroupList Group;               // Group Number.
	uint8_t ChNum;                   // Ch number.
	float AdcK;                    // Ch number.
	s_S32K_SysAdc_AdcID	ChIndex;     // ChIndex
} s_S32K_Adc_AnGroup;

	/**************************************************************************************************
 *  7.  EXPORTED (PUBLIC) VARIABLES                                                               *
 **************************************************************************************************/
extern const s_S32K_Adc_AnGroup AnList[ADC_CH_NUM];
/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES                                                                       *
 **************************************************************************************************/
extern wres_t S32K_Adc_Task( s_GroupList GroupListIndex );
extern wres_t S32K_Adc_Start(s_GroupList GroupListIndex);
extern wres_t S32K_Adc_Init( adc_pal_instance_t instanceNum,const adc_config_t *AdcInitConfigPtr);
//extern wres_t S32K_Adc_GetValue_pt( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value );
//extern wres_t S32K_Adc_GetValue_mV( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value );
extern wres_t S32K_Adc_GetValue_pt( s_S32K_SysAdc_AdcID AdcID, uint16_t *Value );
extern wres_t S32K_Adc_GetValue_mV( s_S32K_SysAdc_AdcID AdcID, uint16_t *Value );


//extern wres_t S32K_Adc_GetSafetyValue_pt( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value ,s_SafetySample_Status *Status);
//extern wres_t S32K_Adc_GetSafetyValue_mV( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value,s_SafetySample_Status *Status);
extern wres_t S32K_Adc_GetSafetyValue_pt( s_S32K_SysAdc_AdcID AdcID, uint16_t *Value ,s_SafetySample_Status *Status);
extern wres_t S32K_Adc_GetSafetyValue_mV( s_S32K_SysAdc_AdcID AdcID,  uint16_t *Value,s_SafetySample_Status *Status);

/**************************************************************************************************
 *  8.  FUNCTION CALL TUTORIAL
 **************************************************************************************************/
// 	Init functions
//		Res = S32K_Adc_Init( INST_ADC_0, &ADC_0_InitConfig0);
//		Res = S32K_Adc_Init( INST_ADC_1, &ADC_1_InitConfig0);

// 	Start functions
//		S32K_Adc_Start(ADC0_GROUP_0);
//		S32K_Adc_Start( ADC1_GROUP_0);
//		S32K_Adc_Start( ADC1_GROUP_1);
// 	Task
//		S32K_Adc_Task(ADC0_GROUP_0);
//		S32K_Adc_Task(ADC1_GROUP_0);
//		S32K_Adc_Task(ADC1_GROUP_1);

// When conversion is finish ADC module stopped, for an other measure need to restart
//  --------------------- Configuration Settings ---------------------
// 		- use adc_pal object for ADC
// 		- Enable PDB0 and PDB1 in clock manager
// 		- For any channel group set Callback = ADC_callback
// 		- For any channel group set Callback user data = Name of conversion result array
//		- define in s_GroupList all group defined (sequentially)(leave GROUP_NUM at least )
//		- Define ADC_CHANNEL_TOT equal ad total number of define channel

//	--- Example for define Channel number for GetValue function ---
//		#define ADC0_GROUP_0_Ch_0 0U
//		#define ADC0_GROUP_0_Ch_1 1U
//		#define ADC0_GROUP_0_Ch_2 2U
//		#define ADC0_GROUP_0_Ch_3 3U
//		#define ADC0_GROUP_0_Ch_4 4U
//		#define ADC0_GROUP_0_Ch_5 5U

//		#define ADC1_GROUP_0_Ch_0 0U
//		#define ADC1_GROUP_0_Ch_1 1U
//		#define ADC1_GROUP_0_Ch_2 2U
//		#define ADC1_GROUP_0_Ch_3 3U
//		#define ADC1_GROUP_0_Ch_4 4U
//		#define ADC1_GROUP_0_Ch_5 5U
//		#define ADC1_GROUP_0_Ch_6 6U
//		#define ADC1_GROUP_0_Ch_7 7U

//		#define ADC1_GROUP_1_Ch_0 0U
//		#define ADC1_GROUP_1_Ch_1 1U
//		#define ADC1_GROUP_1_Ch_2 2U
//		#define ADC1_GROUP_1_Ch_3 3U

#endif /* S32K_ADC_H_ */
/**************************************************************************************************
 *  EOF                                                                                           *
 **************************************************************************************************/
