/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Adc.c
 * History:
 *  - 05 feb 2020 1.0 Davoli - First release
 * S32K_can.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_Adc.h"
#include "string.h"
#include "S32K_SysPin.h"

/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#define GROUP_NUM_MAX ADC_GROUP_NUM

/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/
typedef struct
{
	uint8_t Index;
	bool ConvDone;
	bool Enable;
	float AdcMax;
	adc_pal_instance_t Instance;
	uint8_t numChannels;
	uint8_t FirstChIndex;
	uint16_t * resultBuffer;
	uint32_t BusyCnt;

}s_LpAdc_Group;

static uint16_t Adc_In_pt[ADC_CH_NUM];
static s_LpAdc_Group GroupList[GROUP_NUM_MAX];

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/

/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/
extern void ADC_callback(const adc_callback_info_t * const callbackInfo, void * userData)
{
       uint16_t * resultBuffer;
       resultBuffer=(uint16_t *)(userData);
       uint32_t groupIndex = callbackInfo->groupIndex;
       uint8_t Group;
       if (0x00 == userData)
       {
    	   DEV_ASSERT(false);  // Set Callback user data = Name of conversion result array
       }
	    for (Group = 0U; Group < (uint8_t)GROUP_NUM_MAX; Group++)
	    {
			if ((true == GroupList[Group].Enable ) && (GroupList[Group].resultBuffer == resultBuffer)){
				if (GroupList[Group].Index == groupIndex)
				{
					GroupList[Group].ConvDone =true;

					break;
				}
			}
	    }
}
/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/
//extern wres_t S32K_Adc_GetValue_pt( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value )
extern wres_t S32K_Adc_GetValue_pt( s_S32K_SysAdc_AdcID AdcID, uint16_t *Value )
{
	wres_t res;
	uint8_t FirstCh;
	s_GroupList GroupListIndex;
	uint8_t channel;
	if(ADC_CH_NUM > AdcID)
	{
		GroupListIndex = AnList[AdcID].Group;
		channel = AnList[AdcID].ChNum;
		FirstCh = GroupList[GroupListIndex].FirstChIndex;
		*Value =Adc_In_pt[FirstCh + channel];
		res = WRES_OK;

	}
	else
	{
		*Value =0u;
		res = WRES_ERROR_INVALID_PARAMETER;
	}
	return res;
}
//----------------------------------------------------------------------------------------------------
//extern wres_t S32K_Adc_GetValue_mV( s_GroupList GroupListIndex, uint8_t channel, uint16_t *Value )
extern wres_t S32K_Adc_GetValue_mV( s_S32K_SysAdc_AdcID AdcID, uint16_t *Value )
{
	wres_t res;
	uint8_t FirstCh;
	s_GroupList GroupListIndex;
	uint8_t channel;
	if(ADC_CH_NUM > AdcID)
	{
		GroupListIndex = AnList[AdcID].Group;
		channel = AnList[AdcID].ChNum;
		FirstCh = GroupList[GroupListIndex].FirstChIndex;
		*Value = (uint16_t)(((float)(Adc_In_pt[FirstCh + channel])* AnList[FirstCh + channel].AdcK) / GroupList[GroupListIndex].AdcMax);
		res = WRES_OK;
	}
	else
	{
		*Value =0u;
		res = WRES_ERROR_INVALID_PARAMETER;
	}
	return res;
}

//----------------------------------------------------------------------------------------------------
extern wres_t S32K_Adc_Task( s_GroupList GroupListIndex )
{
	wres_t res;
	uint8_t i;
	uint8_t FirstCh;
	if (GROUP_NUM_MAX > GroupListIndex)
        {
		if (true == GroupList[GroupListIndex].Enable)
		{
			if (true == GroupList[GroupListIndex].ConvDone)
			{
				FirstCh = GroupList[GroupListIndex].FirstChIndex;
				for (i = 0U; i < GroupList[GroupListIndex].numChannels; i++)
				{
				Adc_In_pt[FirstCh+i] = GroupList[GroupListIndex].resultBuffer[i];
				}
				GroupList[GroupListIndex].ConvDone =false;
				res = WRES_OK;
			}
			else
			{
				GroupList[GroupListIndex].BusyCnt++;
				res = WRES_ERROR_NOT_READY; //WRES_OPERATION_IN_PROGRESS;
			}
		}
		else
		{
			res = WRES_ERROR_UNINITIALIZED;
		}
        }
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
	return res;
}

//----------------------------------------------------------------------------------------------------
extern wres_t S32K_Adc_Start(s_GroupList GroupListIndex)
{
// Start all Group for one ADC
        status_t status;
	wres_t res;
	if (GROUP_NUM_MAX > GroupListIndex)
        {
		if (true == GroupList[(uint8_t)GroupListIndex].Enable)
		{
			status = ADC_StartGroupConversion((GroupList[GroupListIndex].Instance), GroupList[GroupListIndex].Index);
			if(STATUS_SUCCESS == status )
			{
				res = WRES_OK;
				//break;
			}
			else
			{
				GroupList[GroupListIndex].BusyCnt++;
				res = WRES_OPERATION_IN_PROGRESS;
				//break;
			}
		}
		else
		{
			res = WRES_ERROR_UNINITIALIZED;
		}
        }
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
	return res;

}

//----------------------------------------------------------------------------------------------------
extern wres_t S32K_Adc_Init( adc_pal_instance_t instanceNum,const adc_config_t *AdcInitConfigPtr)
{
    wres_t res = WRES_OK;
 	uint8_t Group;
	float AdcMax;
	adc_resolution_t Resolution;
        bool break_stat = true;
        static uint32_t EnabledGroup=0u;
        static uint8_t FirstChIndex=0u;
	if (NUMBER_OF_ADC_PAL_INSTANCES > (uint8_t)instanceNum)
	{
		if (STATUS_SUCCESS == ADC_Init(instanceNum, AdcInitConfigPtr))
		{
			Resolution = ((extension_adc_s32k1xx_t *)(AdcInitConfigPtr->extension))->resolution;
			if (Resolution == ADC_RESOLUTION_8BIT){
				AdcMax = (float) (1u << 8u);
			}else if (Resolution == ADC_RESOLUTION_10BIT){
				AdcMax = (float) (1u << 10u);
			}else{
				AdcMax = (float) (1u << 12u);
			}
			for (Group = 0U; Group < AdcInitConfigPtr->numGroups; Group++)
			{
				GroupList[EnabledGroup].Index =Group;
				GroupList[EnabledGroup].ConvDone = false;
				GroupList[EnabledGroup].Instance = instanceNum;
				GroupList[EnabledGroup].resultBuffer = AdcInitConfigPtr->groupConfigArray[Group].resultBuffer;
				GroupList[EnabledGroup].numChannels = AdcInitConfigPtr->groupConfigArray[Group].numChannels;
				GroupList[EnabledGroup].FirstChIndex =FirstChIndex;
				GroupList[EnabledGroup].AdcMax = AdcMax;
                                GroupList[EnabledGroup].BusyCnt =0u;

				FirstChIndex = FirstChIndex + GroupList[EnabledGroup].numChannels;
				if ( (FirstChIndex > (uint8_t)ADC_CH_NUM) && break_stat )
				{
					res = WRES_ERROR_OUTOFMEMORY;
//					DEV_ASSERT(FirstChIndex <= ADC_CH_NUM);
					break_stat = false;
				}
				if ( (EnabledGroup >= (uint8_t)GROUP_NUM_MAX) && break_stat )
				{
					res = WRES_ERROR_OUTOFMEMORY;
//					DEV_ASSERT(EnabledGroup < GROUP_NUM_MAX);
					break_stat = false;

				}
                                if (break_stat)
                                {
                                  GroupList[EnabledGroup].Enable =true;
                                  EnabledGroup++;
                                }
			}
		}
		else
		{
			res = WRES_ERROR_INVALID_MODE;
		}
	}
	else
	{
		res = WRES_ERROR_INVALID_PARAMETER;
	}
	return res;
}

/**************************************************************************************************
 * EOF                                                                                            *
 **************************************************************************************************/
