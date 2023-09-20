#include <wtypes.h>
#include <djwConfig.h>
//#include <application.h>
#include "boot.h"

#pragma location=ADDR_APP_ID
__root static const uint32_t BOOT_SW_ID = SW_ID;

#pragma location=ADDR_APP_VER
__root static const uint32_t BOOT_PRG_VER = PRG_VER;

#pragma location=ADDR_ACL_SA_BAUD
__root static const uint32_t BOOT_ACL_SA_BAUD = (uint32_t)( SA_VAL ) + (uint32_t)( ACL_VAL * 256U ) + (uint32_t)( BAUD_RATE_VAL * 65536U );

#pragma location=ADDR_HW_ID
__root static const uint32_t BOOT_HW_ID = (uint32_t)( HW_ID );

#ifdef BOOT_LDR
	#warning Bootloader active
#endif

