/**************************************************************************************************
 * (C) 2020 Walvoil S.p.a.                                                                            *
 * All rights reserved.                                                                           *
 **************************************************************************************************/
/* @file S32K_Flash.c
 * History:
 *  - 05 feb 2020 1.0 Davoli - First release
 * S32K_Timer.c description
 */

/**************************************************************************************************
 *  1. PRIVATE INCLUDE FILES                                                                      *
 **************************************************************************************************/
#include "Cpu.h"
#include "S32K_Flash.h"
//#include "MyFlash.h"
#include "S32K_ObjectConfig.h"
/**************************************************************************************************
 *  2. PRIVATE DEFINES                                                                            *
 **************************************************************************************************/

/**************************************************************************************************
 *  3. PRIVATE MACROS                                                                             *
 **************************************************************************************************/
#ifndef MyFlash_H
    #error Module S32_Flash need Object typed flash and named MyFlash
#endif
// Settings Link from this module and Object flash named MyFlash
#define Init_Config			MyFlash_InitConfig0
/**************************************************************************************************
 *  4. PRIVATE ENUMERATIONS                                                                       *
 **************************************************************************************************/

/**************************************************************************************************
 *  5. PRIVATE STRUCTURES                                                                         *
 **************************************************************************************************/

/**************************************************************************************************
 *  6. PRIVATE TYPES                                                                              *
 **************************************************************************************************/

/**************************************************************************************************
 *  7. PRIVATE (STATIC) FUNCTION PROTOTYPES                                                       *
 **************************************************************************************************/
static void CCIF_Handler(void);
START_FUNCTION_DECLARATION_RAMSECTION
void CCIF_Callback(void)
END_FUNCTION_DECLARATION_RAMSECTION

/**************************************************************************************************
 *  8. PRIVATE (STATIC) VARIABLES                                                                 *
 **************************************************************************************************/
static flash_ssd_config_t flashSSDConfig;

/**************************************************************************************************
 *  9. PUBLIC VARIABLES                                                                           *
 **************************************************************************************************/

/**************************************************************************************************
 * 10. FUNCTION DEFINITIONS                                                                       *
 **************************************************************************************************/
// -----------------------------------------------------------------------------------------------------
static void CCIF_Handler(void)  // brief Interrupt handler for Flash Command Complete event
{
    // Disable Flash Command Complete interrupt
    FTFx_FCNFG &= (~FTFx_FCNFG_CCIE_MASK);
    return;
}

START_FUNCTION_DEFINITION_RAMSECTION
// -----------------------------------------------------------------------------------------------------
void CCIF_Callback(void)  //  brief Callback function for Flash operations
{
    /* Enable interrupt for Flash Command Complete */
    if ((FTFx_FCNFG & FTFx_FCNFG_CCIE_MASK) == 0u)
    {
        FTFx_FCNFG |= FTFx_FCNFG_CCIE_MASK;
    }
}
END_FUNCTION_DEFINITION_RAMSECTION

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Flash_Init( void )
{
	wres_t res = WRES_OK;
        status_t Initret;
        //static uint32_t Initaddress;
        //uint32_t Initsize;

        INT_SYS_DisableIRQGlobal();
	//#define FLASH_RESTORE_FACTOY_SETTINGS // If defined delete FlexRAM Partitions
//	#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
//	uint8_t unsecure_key[FTFx_PHRASE_SIZE] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu};
//	#else
//	uint8_t unsecure_key[FTFx_LONGWORD_SIZE] = {0xFEu, 0xFFu, 0xFFu, 0xFFu};
//	#endif
	// Disable cache to ensure that all flash operations will take effect instantly,
	// this is device dependent
	#ifdef S32K144_SERIES
//	MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
//	MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
//	MSCM->OCMDR[2u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
//	MSCM->OCMDR[3u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
	#endif
	// Install interrupt for Flash Command Complete event
	INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t*) 0);
	INT_SYS_EnableIRQ(FTFC_IRQn);
	// Enable global interrupt //
	INT_SYS_EnableIRQGlobal();

	// Always initialize the driver before calling other functions //

	Initret = FLASH_DRV_Init(&Init_Config, &flashSSDConfig);
	if (STATUS_SUCCESS != Initret ) 
        {
          res = WRES_ERROR_UNINITIALIZED;
        }
        
	if (flashSSDConfig.EEESize == 0u) // Config FlexRAM as EEPROM if it is currently used as traditional RAM //
	{
//		// First, erase all Flash blocks if code is placed in RAM to ensure
//		// the IFR region is blank before partitioning FLexNVM and FlexRAM
//		// ret = FLASH_DRV_EraseAllBlock(&flashSSDConfig);
//		//DEV_ASSERT(STATUS_SUCCESS == ret);
//
//		// Verify the erase operation at margin level value of 1
//		// ret = FLASH_DRV_VerifyAllBlock(&flashSSDConfig, 1u);
//		//DEV_ASSERT(STATUS_SUCCESS == ret);
//
//		// First, erase all FlexRAM block; if code is placed in RAM to ensure
//		// the IFR region is blank before partitioning FLexNVM and FlexRAM
//
//		Initsize = FEATURE_FLS_PF_BLOCK_SECTOR_SIZE;
//		if (WRES_OK == res)
//		{
//			Initret = FLASH_DRV_EraseSector(&flashSSDConfig, Initaddress, Initsize);
//			if (STATUS_SUCCESS != Initret )
//                        {
//                          res = WRES_ERROR_MEMORYCORRUPTION;
//                        }
//		}
//		// Reprogram secure byte in Flash configuration field
//		#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
//		Initaddress = 0x408u;
//		Initsize = FTFx_PHRASE_SIZE;
//		#else
//		Initaddress = 0x40Cu;
//		Initsize = FTFx_LONGWORD_SIZE;
//		#endif
//		if (WRES_OK == res)
//		{
//			Initret = FLASH_DRV_Program(&flashSSDConfig, Initaddress, Initsize, unsecure_key);
//			if (STATUS_SUCCESS != Initret ) 
//                        {
//                          res = WRES_ERROR_UNINITIALIZED;
//                        }
//		}
//		// Configure FlexRAM as EEPROM and FlexNVM as EEPROM backup region,
//		// DEFlashPartition will be failed if the IFR region isn't blank.
//		// Refer to the device document for valid EEPROM Data Size Code
//		// and FlexNVM Partition Code. For example on S32K144:
//		// - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
//		// - DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes */
//		if (WRES_OK == res)
//		{
//			Initret = FLASH_DRV_DEFlashPartition(&flashSSDConfig, 0x02u, 0x08u, 0x0u, false, true);
//			if (STATUS_SUCCESS != Initret ) 
//                        {
//                          res = WRES_ERROR_OUTOFMEMORY;
//                        }
//		}
//
//		// Re-initialize the driver to update the new EEPROM configuration
//		if (WRES_OK == res)
//		{
//			Initret = FLASH_DRV_Init(&Init_Config, &flashSSDConfig);
//			if (STATUS_SUCCESS != Initret ) 
//                        {
                          res = WRES_ERROR_UNINITIALIZED;
//                        }
//		}
//
//		// Make FlexRAM available for EEPROM
//		if (WRES_OK == res)
//		{
//			Initret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
//			if (STATUS_SUCCESS != Initret ) 
//                        {
//                          res = WRES_ERROR_INVALID_MODE;
//                        }
//		}
	}
	else    // FLexRAM is already configured as EEPROM
	{
		#ifdef FLASH_RESTORE_FACTOY_SETTINGS
		Initaddress = 0;
		Initsize = FEATURE_FLS_PF_BLOCK_SECTOR_SIZE;
		if (WRES_OK == res)
		{
			Initret = FLASH_DRV_EraseSector(&flashSSDConfig, Initaddress, Initsize);
			if (STATUS_SUCCESS != Initret ) res = WRES_ERROR_MEMORYCORRUPTION;
		}
		// Re-initialize the driver to update the new EEPROM configuration
		if (WRES_OK == res)
		{
			Initret = FLASH_DRV_Init(&Init_Config, &flashSSDConfig);
			if (STATUS_SUCCESS != Initret ) res = WRES_ERROR_UNINITIALIZED;
		}
		#else
		// Make FlexRAM available for EEPROM, make sure that FlexNVM and FlexRAM
		// are already partitioned successfully before
		if (WRES_OK == res)
		{
			Initret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
			if (STATUS_SUCCESS != Initret ) 
                        {
                          res = WRES_ERROR_INVALID_MODE;
                        }
		}
		#endif
	}
	return res;
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Flash_ReadByteNum( uint32_t StartByteAddress, uint8_t *pData, uint8_t size )
{
	wres_t res;
	uint8_t *flash_Byte;
	uint32_t ABS_address;
    uint8_t i;

	if (flashSSDConfig.EEESize != 0u){
	    if ( flashSSDConfig.EEESize<( StartByteAddress + size))
	    {
	    	res = WRES_ERROR_INVALID_PARAMETER;
	    }
		//ABS_address =  ((uint8_t)(DW_address) << 2u);
		ABS_address = flashSSDConfig.EERAMBase + StartByteAddress;
		for (i = 0U; i < size; i++)
		{
			flash_Byte = (uint8_t *)( ABS_address );
			*pData = *flash_Byte;
			ABS_address ++;
			pData++;
		}
		res = WRES_OK;
	}
	else
	{
		res = WRES_ERROR_UNINITIALIZED;
	}
	return res;
}

// -----------------------------------------------------------------------------------------------------
extern wres_t S32K_Flash_WriteByteNum( uint32_t StartByteAddress, uint8_t *pData, uint8_t size )
{
	wres_t res;
	uint32_t ABS_address;
	status_t result;
	if (flashSSDConfig.EEESize != 0u){
	    if ( flashSSDConfig.EEESize<( StartByteAddress + size))
	    {
	    	res = WRES_ERROR_INVALID_PARAMETER;
	    }

		//ABS_address =  ((uint8_t)(DW_address) << 2u);
		ABS_address = flashSSDConfig.EERAMBase + StartByteAddress;
		//size = 4U;
		//DW_Data.DWORD[0] = data;
		//result = FLASH_DRV_EEEWrite(&flashSSDConfig, ABS_address, size, &DW_Data.BYTE[0]);
		result = FLASH_DRV_EEEWrite(&flashSSDConfig, ABS_address, size, pData);
		if(STATUS_SUCCESS == result)
                {
			res = WRES_OK;
		}
                else
                {
			res = WRES_ERROR;
		}
	}
	else
	{
		res = WRES_ERROR_UNINITIALIZED;
	}

	return res;
}

