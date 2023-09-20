#ifndef BOOT_H
#define BOOT_H

#define BOOT_LDR  //se definito BOOT_LDR, viene generato un warning dal linker

#define SA_VAL      ( SAEJ_DEFAULT_SA )	// source address
#define HW_ID       ( 327U )	//W11[3]00[27]
#define SW_ID       ( PRG_APP_ID ) 
#define PRG_VER     (uint32_t)( PRG_VERSION )

/* Selezione invio Address Claim secondo SAEJ1939 da parte del Bootloader 
Valori ammessi:
0xAA -> il Bootloader non invia Address Claim
Altro (<> 0xAA) -> il Bootloader invia Address Claim
*/
#define ACL_VAL ( 0xAA )	

/* Baud Rate applicativo - valori ammessi
1Mbit/s	  -> 0u
800kbit/s -> 1u
500kbit/s -> 2u
250kbit/s -> 3u
125kbit/s -> 4u
 50kbit/s -> 6u
 20kbit/s -> 7u
 10kbit/s -> 8u
*/
#define BAUD_RATE_VAL	( 0x03 )	// 250Kb/s



/*--- indirizzi in flash - NON MODIFICARE!!! ---*/
#define ADDR_DATA_BASE     ( 0x00001430UL )
#define ADDR_APP_ID        ( 0x00001430UL )
#define ADDR_APP_VER       ( 0x00001434UL )	
#define ADDR_ACL_SA_BAUD   ( 0x00001438UL )
#define ADDR_HW_ID         ( 0x0000143CUL )

#endif /*BOOT_H*/
