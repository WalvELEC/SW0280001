#include "Cpu.h"
#include "S32K_SysPin.h"
#include "S32K_ObjectConfig.h"


/**************************************************************************************************
 *         SysPin Configuration  - GPIO                                                           *
 **************************************************************************************************/
// Define all pin used as GPIO
const s_S32K_SysPin_GPioProp GpioLIst[GPIO_NUM]=
{
	//Port,	PinNumber,	direction
	    {PTA,	12,		GPIO_INPUT_DIRECTION  },  // GPIO_IN_PWR_INTn
	    {PTA,	13,		GPIO_INPUT_DIRECTION  },  // GPIO_IN_PWR_ERR
	    {PTB,	8,		GPIO_INPUT_DIRECTION  },  // GPIO_IN_PWR_SS1
	    {PTA,	17,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_PWR_WDI
	    {PTC,	5,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_CAN_TERM
	    {PTC,	8,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_TPTASK
	    {PTC,	9,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_TP2
	    {PTD,	8,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_CAN_STB
	    {PTD,	11,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_LED_EN0
	    {PTD,	12,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_LED_EN1
	    {PTD,	13,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_LED_EN2
	    {PTD,	14,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_LED_EN3
	    {PTE,	3,		GPIO_OUTPUT_DIRECTION },  // GPIO_OUT_VOTING_SYNC
};


/**************************************************************************************************
 *         Adc Configuration  - Adc_0-Adc_1                                                       *
 **************************************************************************************************/

#define ADC_K_5V    5049.9f
#define ADC_K_3V    3300.0f
#define ADC_K_VREF  5019.6f
#define ADC_K_5V0S 10000.0f // Da testare
#define ADC_K_24V  50410.0f
#define ADC_K_IN_VBB  33615.1f

// Conversion factor used in S32K_Adc_GetValue_mV()
/*
const float Adc_In_K1[ADC_CH_NUM]={	    //Adc.Group.ChannelIndex
							ADC_K_5V, 		// 0.0.0
							ADC_K_5V, 		// 0.0.1
							ADC_K_5V, 		// 0.0.2
							ADC_K_5V, 		// 0.0.3
							ADC_K_5V,		// 0.0.4
							ADC_K_5V, 		// 0.0.5
							ADC_K_5V, 		// 1.0.0
							ADC_K_5V, 		// 1.0.1
							ADC_K_5V, 		// 1.0.2
							ADC_K_5V, 		// 1.0.3
							ADC_K_5V, 		// 1.0.4
							ADC_K_5V, 		// 1.0.5
							ADC_K_5V, 		// 1.0.6
							ADC_K_5V, 		// 1.0.7
							ADC_K_5V, 		// 1.1.0
							ADC_K_5V, 		// 1.1.1
							ADC_K_5V, 		// 1.1.2
							ADC_K_5V, 		// 1.1.3
};
*/
#include "S32K_Adc.h"
const s_S32K_Adc_AnGroup AnList[ADC_CH_NUM]=
{
	//Group,	ChNumber,	ChIndex
		{ADC0_GROUP_0,	Ch_0,	ADC_K_5V,	ADC_AN_CH00     },
		{ADC0_GROUP_0,	Ch_1,	ADC_K_5V,	ADC_AN_CH01	  	},
		{ADC0_GROUP_0,	Ch_2, 	ADC_K_IN_VBB    /*ADC_K_5V*/,	ADC_AN_CH02	  	},
		{ADC0_GROUP_0,	Ch_3, 	ADC_K_5V,	ADC_AN_CH03	  	},
		{ADC0_GROUP_0,	Ch_4, 	ADC_K_5V,	ADC_AN_CH04	  	},
		{ADC0_GROUP_1,	Ch_0, 	ADC_K_5V,	ADC_AN_CH05	 	},
		{ADC0_GROUP_1,	Ch_1, 	ADC_K_5V,	ADC_AN_CH06	  	},
		{ADC0_GROUP_1,	Ch_2, 	ADC_K_VREF,	ADC_AN_VREF_ADC0},
		{ADC0_GROUP_1,	Ch_3, 	ADC_K_5V0S,	ADC_AN_5V_SENSOR},
		{ADC1_GROUP_0,	Ch_0, 	ADC_K_5V,	ADC_AN_CH07	  	},
		{ADC1_GROUP_0,	Ch_1, 	ADC_K_IN_VBB    /*ADC_K_5V*/,	ADC_AN_CH08	  	},
		{ADC1_GROUP_0,	Ch_2, 	ADC_K_5V,	ADC_AN_CH09	 	},
		{ADC1_GROUP_0,	Ch_3, 	ADC_K_5V,	ADC_AN_CH10	  	},
		{ADC1_GROUP_0,	Ch_4, 	ADC_K_5V,	ADC_AN_CH11	  	},
		{ADC1_GROUP_1,	Ch_0, 	ADC_K_5V,	ADC_AN_CH12	  	},
		{ADC1_GROUP_1,	Ch_1, 	ADC_K_5V,	ADC_AN_CH13	  	},
		{ADC1_GROUP_1,	Ch_2, 	ADC_K_VREF,	ADC_AN_VREF_ADC1},
		{ADC1_GROUP_1,	Ch_3, 	ADC_K_24V,	ADC_AN_24V_SYS	},
};
