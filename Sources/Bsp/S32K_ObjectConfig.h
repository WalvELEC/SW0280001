#ifndef S32K_OBJ_CONFIG_H_
#define S32K_OBJ_CONFIG_H_
#include "Cpu.h"
#include "wtypes.h"

typedef enum
{
    STATUS_SAMPLE_OFF          = 0x00U,
    STATUS_SAMPLE_WAIT         = 0x01U,
	STATUS_SAMPLE_DONE         = 0x02U,
	STATUS_SAMPLE_DONE_ISR     = 0x03U,
	STATUS_SAMPLE_TIMEOUT      = 0x04U,
} s_SafetySample_Status;


/**************************************************************************************************
*         SysPin Configuration  - GPIO                                                            *
 **************************************************************************************************/
typedef enum         // Define all pin used as GPIO
{
	GPIO_IN_PWR_INTn,  		// GPIO_0
	GPIO_IN_PWR_ERR, 		// GPIO_1
	GPIO_IN_PWR_SS1, 		// GPIO_2
	GPIO_OUT_PWR_WDI,  		// GPIO_3
	GPIO_OUT_CAN_TERM,   	        // GPIO_4
	GPIO_OUT_TP1,			// GPIO_5
	GPIO_OUT_TP2,			// GPIO_6
	GPIO_OUT_CAN_STB,  		// GPIO_7
	GPIO_OUT_LED_EN0,  		// GPIO_8
	GPIO_OUT_LED_EN1,  		// GPIO_9
	GPIO_OUT_LED_EN2,    	        // GPIO_10
	GPIO_OUT_LED_EN3,    	        // GPIO_11
	GPIO_OUT_VOTING_SYNC,	// GPIO_12
	GPIO_NUM,        		// Leave GPIO_NUM as last definition
} s_S32K_SysPin_GPioID;
#define GPIO_OUT_TLF_CS (s_S32K_SysPin_GPioID)GPIO_NUM
#define GPIO_OUT_MLX_CP (s_S32K_SysPin_GPioID)GPIO_NUM 

#define GPIO_OUT_TP_RX_B    GPIO_IN_PWR_INTn 
#define GPIO_SYS_TICK       GPIO_IN_PWR_INTn 
#define GPIO_OUT_TPTASK     GPIO_OUT_TP2 
#define GPIO_OUT_TP_RX_CAN  GPIO_IN_PWR_INTn 
#define GPIO_OUT_TP_SOSC    GPIO_OUT_TP1


/**************************************************************************************************
 *         Adc Configuration  - Adc_0-Adc_1                                                           *
 **************************************************************************************************/

typedef enum         // Define all pin used as GPIO
{
	ADC_AN_CH00,
	ADC_AN_CH01,
	ADC_AN_CH02,
	ADC_AN_CH03,
	ADC_AN_CH04,
	ADC_AN_CH05,
	ADC_AN_CH06,
	ADC_AN_VREF_ADC0,
	ADC_AN_5V_SENSOR,
	ADC_AN_CH07,
	ADC_AN_CH08,
	ADC_AN_CH09,
	ADC_AN_CH10,
	ADC_AN_CH11,
	ADC_AN_CH12,
	ADC_AN_CH13,
	ADC_AN_VREF_ADC1,
	ADC_AN_24V_SYS,
	ADC_CH_NUM,        		// Leave GPIO_NUM as last definition
} s_S32K_SysAdc_AdcID;


/**************************************************************************************************
 *         Flash Timer Configuration  - MySysTmr                                                           *
 **************************************************************************************************/
// If included this module configure FlexRAM as EEPROM and
//  FlexNVM as EEPROM backup region, as
// - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
// - DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes */
#ifndef MyFlash_H
    #error Module S32_Flash need Object typed flash and named MyFlash
#endif

/**************************************************************************************************
 *         Task Timer Configuration  - MySysTmr                                                           *
 **************************************************************************************************/

#ifndef MySysTmr_H
    #error Module S32_Timer need Object typed lpit and named MySysTmr
#endif

typedef enum          // Object MySysTmr : Definition of task
{
	   TASK_0_ID,
	   TASK_1_ID,
	   TASK_2_ID,
	   TASK_3_ID,
	   TASK_4_ID,
	   TASK_5_ID,
       TASK_6_ID,
       TASK_7_ID,
	   TASK_NUM,      // Leave Task_NUM as last definition
}  s_S32K_TimerTaskId;

/**************************************************************************************************
 *         LpSpi Object Configuration                                                             *
 **************************************************************************************************/
#define LP_SPI_NUM 3U               // Number of object LpSpi
#define LP_SPI_TX_QUEUE_SIZE_0  5U  // Object 0 LpSpi : Transmit Queue length
#define LP_SPI_RX_QUEUE_SIZE_0  5U  // Object 0 LpSpi : Receive Queue length
#define LP_SPI_TRANSFER_SIZE_0  8U  // Object 0 LpSpi : Number of bytes transmitted in a message

#define LP_SPI_TX_QUEUE_SIZE_1  5U  // Object 1 LpSpi : Transmit Queue length
#define LP_SPI_RX_QUEUE_SIZE_1  5U  // Object 1 LpSpi : Receive Queue length
#define LP_SPI_TRANSFER_SIZE_1  8U  // Object 1 LpSpi : Number of bytes transmitted in a message

#define LP_SPI_TX_QUEUE_SIZE_2  5U  // Object 2 LpSpi : Transmit Queue length
#define LP_SPI_RX_QUEUE_SIZE_2  5U  // Object 2 LpSpi : Receive Queue length
#define LP_SPI_TRANSFER_SIZE_2  2U  // Object 2 LpSpi : Number of bytes transmitted in a message

//#define LP_SPI_TX_QUEUE_SIZE_3  5U  // Object 3 LpSpi : Transmit Queue length
//#define LP_SPI_RX_QUEUE_SIZE_3  5U  // Object 3 LpSpi : Receive Queue length
//#define LP_SPI_TRANSFER_SIZE_3  8U  // Object 3 LpSpi : Number of bytes transmitted in a message

/**************************************************************************************************
 *         LpUart Object Configuration                                                             *
 **************************************************************************************************/
#define LP_UART_NUM 0U             // Number of object LpUart

#define LP_UART_TRANSFER_SIZE 12UL // Number of bytes transmitted in a message, common for all Objects

//#define LP_UART_TX_QUEUE_SIZE_0 3U // Object 0 LpUart : Transmit Queue length
//#define LP_UART_RX_QUEUE_SIZE_0 3U // Object 0 LpUart : Receive Queue length

//#define LP_UART_TX_QUEUE_SIZE_1 5U // Object 1 LpUart : Transmit Queue length
//#define LP_UART_RX_QUEUE_SIZE_1 5U // Object 1 LpUart : Receive Queue length

//#define LP_UART_TX_QUEUE_SIZE_2 5U // Object 2 LpUart : Transmit Queue length
//#define LP_UART_RX_QUEUE_SIZE_2 5U // Object 2 LpUart : Receive Queue length

/**************************************************************************************************
 *         FlexSpi Object Configuration                                                             *
 **************************************************************************************************/
#define FLEX_SPI_NUM 1U             // Number of object FlexSpi
#define FLEX_SPI_TRANSFER_SIZE 16U  // Number of bytes transmitted in a message, common for all Objects

#define FLEX_SPI_TX_QUEUE_SIZE_0 0U // Object 0 LpUart : Transmit Queue length
#define FLEX_SPI_RX_QUEUE_SIZE_0 0U // Object 0 LpUart : Receive Queue length

//#define FLEX_SPI_TX_QUEUE_SIZE_1 5U // Object 1 LpUart : Transmit Queue length
//#define FLEX_SPI_RX_QUEUE_SIZE_1 5U // Object 1 LpUart : Receive Queue length

/**************************************************************************************************
 *         Can Object Configuration                                                             *
 **************************************************************************************************/
#define CAN_OBJ_NUM 2U              // Number of object Can
#define CAN_RX_QUEUE_SIZE_0 20U     // Object 0 Can : Receive Queue length
#define CAN_NUM_RX_FILTER_0 24U      // Object 0 Can : Receive Filter Number
//#define CAN_NUM_RX_FILTER_0 24U      // Object 0 Can : Receive Filter Number

#define CAN_RX_QUEUE_SIZE_1 5U      // Object 1 Can : Receive Queue length
#define CAN_NUM_RX_FILTER_1 2U      // Object 1 Can : Receive Filter Number

//#define CAN_RX_QUEUE_SIZE_2 15U     // Object 2 Can : Receive Queue length
//#define CAN_NUM_RX_FILTER_2 2U      // Object 2 Can : Receive Filter Number

#endif




