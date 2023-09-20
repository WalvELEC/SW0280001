/*
 * hjwConfig.h
 *
 *  Created on: 30 aug 2018
 *      Author: PoliD
 */

#ifndef _DJWCONFIG_H
#define _DJWCONFIG_H

//macro da abilitare/disabilitare
//#define DJW_MEM_DISABLED
#define ISOBUS_DJW
#define DM1_ENABLED
//#define ISOBUS_DEBUG
#define ISOBUS_CAN_RX_COUNTER

//definizioni ingressi analogici per App Translate
#define ADC_IN_CH0           (  0U ) //PORT PTA0 -->JP1.1
#define ADC_IN_CH1           (  1U ) //PORT PTA1 -->JP1.3
#define ADC_IN_CH2           (  2U ) //PORT PTA6 -->JP2.1
#define ADC_IN_CH3           (  3U ) //PORT PTA7 -->JP2.3
#define ADC_IN_CH4           (  4U ) //PORT PTB0 -->JP3.1
#define ADC_IN_CH5           (  5U ) //PORT PTB1 -->JP3.3
#define ADC_IN_CH6           (  6U ) //PORT PTB2 -->JP4.1
#define ADC_IN_VREF_ADC0     (  7U ) //PORT PTB3 -->VREF_ADC0_M4
#define ADC_IN_5V_SENSOR     (  8U ) //5V_SENSOR
#define ADC_IN_CH7           (  9U ) //PORT PTA2 -->JP1.2
#define ADC_IN_CH8           ( 10U ) //PORT PTA3 -->JP1.4
#define ADC_IN_CH9           ( 11U ) //PORT PTD2 -->JP2.2
#define ADC_IN_CH10          ( 12U ) //PORT PTD3 -->JP2.4
#define ADC_IN_CH11          ( 13U ) //PORT PTC6 -->JP3.2
#define ADC_IN_CH12          ( 14U ) //PORT PTC7 -->JP3.4
#define ADC_IN_CH13          ( 15U ) //PORT PTD4 -->JP4.2
#define ADC_IN_VREF_ADC1     ( 16U ) //PORT PTB12 ->VREF_ADC1_M4
#define ADC_IN_24V_SYS       ( 17U ) //24V_SYS

//parametri identificativi dell'applicativo
#define PRG_APP_ID           ( 280001U ) //SW[0280001]
#define PRG_VERSION          ( 0x0002U ) //progressivo
#define PRG_CHR_APP_ID_D0    ( 0x30U )   //Asc(0)  SW0280[0]01   //si usa la 'E' per indicare una versione engineering o di test, in alternativa si mette lo '0' per produzione
#define PRG_CHR_APP_ID_D1    ( 0x30U )   //Asc(0)  SW02800[0]1
#define PRG_CHR_APP_ID_D2    ( 0x31U )   //Asc(1)  SW028000[1]
#define PRG_CHR_APP_ID_D3    ( 0x45U )   //Asc(.)  //0x2E = '.' per versione rilasciata; 0x45 = 'E' per versione engineering
#define PRG_CHR_APP_ID_D4    ( 0x30U )   //Asc(0)  PRG_VERSION = [0]1
#define PRG_CHR_APP_ID_D5    ( 0x32U )   //Asc(1)  PRG_VERSION = 0[1]

#define ASC_DUT_CODE_1       ( 0x31U )
#define ASC_DUT_CODE_2       ( 0x38U )
#define ASC_DUT_CODE_3       ( 0x33U )
#define ASC_DUT_CODE_4       ( 0x35U )
#define ASC_DUT_CODE_5       ( 0x36U )
#define ASC_DUT_CODE_6       ( 0x30U )
#define ASC_DUT_CODE_7       ( 0x30U )
#define ASC_DUT_CODE_8       ( 0x30U )
#define ASC_DUT_CODE_9       ( 0x31U )

#define ASC_MANUFACTURER_CODE_1    'W'
#define ASC_MANUFACTURER_CODE_2    'A'
#define ASC_MANUFACTURER_CODE_3    'L'
#define ASC_MANUFACTURER_CODE_4    'V'
#define ASC_MANUFACTURER_CODE_5    'O'
#define ASC_MANUFACTURER_CODE_6    'I'
#define ASC_MANUFACTURER_CODE_7    'L'

#define MANUFACTURER_CODE   ( 0x0261U )   //Assegnato da AEF il 30/07/2014 - è il dato che viene visualizzato nell'ISO Name

#define ASC_DEV_TYPE_CODE_1        'D'
#define ASC_DEV_TYPE_CODE_2        'J'
#define ASC_DEV_TYPE_CODE_3        'W'

#define HWID_0               ( 0x31U )   //W1[1]30021 M4
#define HWID_1               ( 0x33U )   //W11[3]0021 M4
#define HWID_2               ( 0x30U )   //W1130[0]21 M4
#define HWID_3               ( 0x32U )   //W11300[2]1 M4
#define HWID_4               ( 0x31U )   //W113002[1] M4
#define HWID_5               ( 0x34U )   //W1130021 M[4]

#define INDUSTRY_GROUP      ( 0x02U   )   //Agricultural and forestry
#define FUNCTION            ( 0x81U   )   //Specific Operator Input
#define VEHICLE_SYSTEM      ( 0x00u   )   //General
#define VEHICLE_INSTANCE    ( 0x00U   )   //1 VEHICLE
#define ECU_INSTANCE        ( 0x00U   )   //1 ECU
#define FUNCTION_INSTANCE   ( 0x00U   )   //1 Function
#define ADDRESS_CAPABLE     ( 0x01U   )   //Arbitrary Address capable

#define ECU_LOCATION_1       ( 0x49U )  //ascii 'I'
#define ECU_LOCATION_2       ( 0x4EU )  //ascii 'N'
#define ECU_LOCATION_3       ( 0x20U )  //ascii 'space'
#define ECU_LOCATION_4       ( 0x43U )  //ascii 'C'
#define ECU_LOCATION_5       ( 0x41U )  //ascii 'A'
#define ECU_LOCATION_6       ( 0x42U )  //ascii 'B'

#define ECU_TYPE_00          ( 0x41U )  //ascii [A]UX
#define ECU_TYPE_01          ( 0x55U )  //ascii A[U]X
#define ECU_TYPE_02          ( 0x58U )  //ascii AU[X]

// numero di serie del prodotto
// come inizializzazione viene tutto settato a 0, poiché il numero di serie deve essere definito in fase di collaudo,
// diventando univoco per ciascun articolo
// il formato è il seguente:
// SERIAL_NUM_0 : anno di produzione ( 1 byte - si mettono le ultime due cifre dell'anno, quindi 2020 diventa 20 )
// SERIAL_NUM_1 : 0 (reserved)
// SERIAL_NUM_2 : 0 (reserved)
// SERIAL_NUM_3 : 0 (reserved)
// SERIAL_NUM_4 : LSB dell'ODL senza anno (quindi ad es. se l'ODL è PX2002412, si prende l'LSB di 02412 )
// SERIAL_NUM_5 : MSB dell'ODL senza anno (quindi ad es. se l'ODL è PX2002412, si prende l'MSB di 02412 )
// SERIAL_NUM_6 : LSB del progressivo di produzione
// SERIAL_NUM_7 : MSB del progressivo di produzione

#define SERIAL_NUM_0         ( 0xFFU )
#define SERIAL_NUM_1         ( 0xFFU )
#define SERIAL_NUM_2         ( 0xFFU )
#define SERIAL_NUM_3         ( 0xFFU )
#define SERIAL_NUM_4         ( 0xFFU )
#define SERIAL_NUM_5         ( 0xFFU )
#define SERIAL_NUM_6         ( 0xFFU )
#define SERIAL_NUM_7         ( 0xFFU )
#define SERIAL_NUM_8         ( 0xFFU )
#define SERIAL_NUM_9         ( 0xFFU )

////configurazione CAN dell'applicativo in bps
#define CAN0_BAUD_RATE_BPS   ( CAN_BIT_RATE_250K )
#define DEFAULT_CAN_RES_TERM ( CAN_RES_TERM_NOT_ENABLED )

// source address nodo SAEJ
#define SAEJ_DEFAULT_SA            ( 0x31U )
#define SAEJ_SELF_CONFIGURABLE_EN   SAEJ_IS_SELF_CONFIGURABLE  //SAEJ_IS_SELF_CONFIGURABLE:      il source address del device è self configurable
                                                               //SAEJ_IS_NOT_SELF_CONFIGURABLE:  il source address del device è fisso, in caso quindi di mancato claim, diventa non operativo

// tempistiche di trasmissione dei messaggi del joystick
#define SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US              ( 20000U )    //periodo in uS di trasmissione BJM e EJM in caso di trasmissione free-run
#define SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_BASE_TIME_OUT_US    ( 100000U )   //periodo in uS di trasmissione BJM e EJM in caso di trasmissione on-change
#define SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_REFRESH_TIME_OUT_US ( 20000U )    //rate massimo in uS di rinfresco dei dati in caso di trasmissione on-change
#define SAEJ_JOY_TX_ON_CHANGE_EN                          false         //true:  il joystick trasmette BJM e EJM con periodo SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_BASE_TIME_OUT_US
                                                                        //       oppure se cambia un dato, con rate massimo SAEJ_CAN_MSG_JOY_TX_ON_CHANGE_REFRESH_TIME_OUT_US
                                                                        //false: il joystick trasmette BJM e EJM con periodo SAEJ_CAN_MSG_JOY_TX_FREE_TIME_OUT_US, sia che
                                                                        //       cambi un dato, sia che il messaggio rimanga costante (eccezione rispetto alla norma SAEJ)

// configurazione assi e roller abilitati
/*
   per ciascun asse/roller sono definiti i parametri:
   SAEJ_AXIS_n_ENABLED                      - indica se l'asse n è abilitato
   SAEJ_AXIS_n_INVERTED                     - indica se il verso dell'asse n è invertito (per la pubblicazione delle direzioni POS e NEG sul messaggio SAEJ
   SAEJ_AXIS_n_NOT_WAIT_NEUTRAL_AT_STARTUP  - indica se per l'asse n posso iniziare a pilotare subito all'accensione senza attesa in neutro

   n = X, Y, W, Z, J, Q, V
*/
//#define SAEJ_DEFAULT_CFG_AXES     ( SAEJ_AXIS_X_ENABLED | SAEJ_AXIS_X_INVERTED | SAEJ_AXIS_Y_ENABLED )

#define SAEJ_DEFAULT_CFG_AXES     ( SAEJ_AXIS_X_ENABLED | SAEJ_AXIS_Y_ENABLED )


//configurazione detent su assi/roller
/*
   per cascun asse/roller sono definiti i parametri:

   SAEJ_AXIS_n_DETENT_EN
       Tale parametro è composto dall'OR tra i valori:
       SAEJ_DETENT_EN_OFF      - indica che il detent per l'asse non è abilitato
       SAEJ_DETENT_EN_POS      - indica che il detent per l'asse è attivo in direzione POS
       SAEJ_DETENT_EN_NEG      - indica che il detent per l'asse è attivo in direzione NEG

   SAEJ_AXIS_n_DETENT_TH_POS   - indica la soglia ( scalata su 1000 punti ) di innesco del detent in direzione POS
   SAEJ_AXIS_n_DETENT_TH_NEG   - indica la soglia ( scalata su 1000 punti ) di innesco del detent in direzione NEG

   n = X, Y, W, Z, J, Q, V
*/
#define SAEJ_AXIS_X_DETENT_EN     SAEJ_DETENT_EN_OFF //( SAEJ_DETENT_EN_POS | SAEJ_DETENT_EN_NEG )
#define SAEJ_AXIS_X_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_X_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_Y_DETENT_EN     SAEJ_DETENT_EN_OFF //( SAEJ_DETENT_EN_POS | SAEJ_DETENT_EN_NEG )
#define SAEJ_AXIS_Y_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_Y_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_W_DETENT_EN     SAEJ_DETENT_EN_OFF
#define SAEJ_AXIS_W_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_W_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_Z_DETENT_EN     SAEJ_DETENT_EN_OFF
#define SAEJ_AXIS_Z_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_Z_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_J_DETENT_EN     SAEJ_DETENT_EN_OFF
#define SAEJ_AXIS_J_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_J_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_Q_DETENT_EN     SAEJ_DETENT_EN_OFF
#define SAEJ_AXIS_Q_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_Q_DETENT_TH_NEG ( 1000U )
#define SAEJ_AXIS_V_DETENT_EN     SAEJ_DETENT_EN_OFF
#define SAEJ_AXIS_V_DETENT_TH_POS ( 1000U )
#define SAEJ_AXIS_V_DETENT_TH_NEG ( 1000U )

// configurazione pulsanti abilitati
/*
   si possono mappare fino a 13 pulsanti. Per ciascun pulsante, se abilitato, bisogna
   fare l'OR con il parametro SAEJ_BUTTON_n_ENABLED, con n = 1, 2, ..., 13

   Se non sono previsti pulsanti, settare il valore al parametro SAEJ_BUTTON_ALL_DISABLED
*/
//#define SAEJ_DEFAULT_CFG_PUSH_BUTTON SAEJ_BUTTON_ALL_DISABLED //nessun pulsante abilitato

#define SAEJ_DEFAULT_CFG_PUSH_BUTTON   ( SAEJ_BUTTON_1_ENABLED  | SAEJ_BUTTON_2_ENABLED  | SAEJ_BUTTON_3_ENABLED | \
                                         SAEJ_BUTTON_4_ENABLED  | SAEJ_BUTTON_5_ENABLED  | SAEJ_BUTTON_6_ENABLED | \
                                         SAEJ_BUTTON_7_ENABLED  | SAEJ_BUTTON_8_ENABLED  | SAEJ_BUTTON_9_ENABLED | \
                                         SAEJ_BUTTON_10_ENABLED )

//assegnazione ingressi fisici pulsanti
/*
    il joystick può interpretare fino a 14 input digitali, che sono associati ad altrettanti I/O fisici sulla scheda, secondo la seguente numerazione logica:
    ADC_IN_CH0           (  0U ) //PORT PTA0 -->JP1.1  //24V_SYS fisso, alimentazione esterna verso impugnatura
    ADC_IN_CH1           (  1U ) //PORT PTA1 -->JP1.3
    ADC_IN_CH2           (  2U ) //PORT PTA6 -->JP2.1
    ADC_IN_CH3           (  3U ) //PORT PTA7 -->JP2.3
    ADC_IN_CH4           (  4U ) //PORT PTB0 -->JP3.1
    ADC_IN_CH5           (  5U ) //PORT PTB1 -->JP3.3
    ADC_IN_CH6           (  6U ) //PORT PTB2 -->JP4.1
    ADC_IN_CH7           (  9U ) //PORT PTA2 -->JP1.2
    ADC_IN_CH8           ( 10U ) //PORT PTA3 -->JP1.4
    ADC_IN_CH9           ( 11U ) //PORT PTD2 -->JP2.2
    ADC_IN_CH10          ( 12U ) //PORT PTD3 -->JP2.4
    ADC_IN_CH11          ( 13U ) //PORT PTC6 -->JP3.2
    ADC_IN_CH12          ( 14U ) //PORT PTC7 -->JP3.4
    ADC_IN_CH13          ( 15U ) //PORT PTD4 -->JP4.2
    l'ingresso fisico è indicato come ADC_AN_CHx ( x= 00, 01, 02, 03, ...13), secondo la denominazione
    riferita nello schema elettrico dell'hardware

    per ciascun ingresso digitale logico DIGITAL_IN_n, con n = 1, 2, ..., 13, sono definiti:
    DIGITAL_IN_n_INPUT_MAIN  - ingresso dritto (o unico ingresso, se non c'è ridondanza), deve essere definito come uno degli ADC_IN_CHx di cui sopra
    DIGITAL_IN_n_INPUT_RED   - ingresso ridondato (se c'è ridondanza), deve essere definito come uno degli ADC_IN_CHx di cui sopra
    DIGITAL_IN_n_RED_TYPE    - tipo di ridondanza sul pulsante, può assumere i valori:   
                               DIGITAL_RED_TYPE_DISABLED     - ridondanza non attiva (l'ingresso logico sarà quindi gestito dal solo DIGITAL_IN_n_INPUT_MAIN)
                               DIGITAL_RED_TYPE_NOT_INVERTED - coerenza se ingresso dritto e ridondato variano nello stesso verso (entrambi alti o entrambi bassi, LO/LO e HI/HI)
                               DIGITAL_RED_TYPE_INVERTED     - coerenza se ingresso dritto e ridondato variano in verso opposto   (uno alto e uno basso, LO/HI e HI/LO)
    
    se DIGITAL_IN_n_INPUT_MAIN o DIGITAL_IN_n_INPUT_RED non sono collegati, devono essere definti come DIGITAL_IN_NOT_CONNECTED    
*/


#define DIGITAL_IN_0_INPUT_MAIN      ( ADC_IN_CH0 )
#define DIGITAL_IN_0_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_0_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_1_INPUT_MAIN      ( ADC_IN_CH1 )
#define DIGITAL_IN_1_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_1_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_2_INPUT_MAIN      ( ADC_IN_CH2 )
#define DIGITAL_IN_2_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_2_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_3_INPUT_MAIN      ( ADC_IN_CH3 )
#define DIGITAL_IN_3_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_3_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_4_INPUT_MAIN      ( ADC_IN_CH4 )
#define DIGITAL_IN_4_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_4_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_5_INPUT_MAIN      ( ADC_IN_CH5 )
#define DIGITAL_IN_5_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_5_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_6_INPUT_MAIN      ( ADC_IN_CH6 )
#define DIGITAL_IN_6_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_6_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_7_INPUT_MAIN      ( ADC_IN_CH7 )
#define DIGITAL_IN_7_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_7_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_8_INPUT_MAIN      ( ADC_IN_CH8 )
#define DIGITAL_IN_8_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_8_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_9_INPUT_MAIN      ( ADC_IN_CH9 )
#define DIGITAL_IN_9_INPUT_RED       ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_9_RED_TYPE        ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_10_INPUT_MAIN     ( ADC_IN_CH10 )
#define DIGITAL_IN_10_INPUT_RED      ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_10_RED_TYPE       ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_11_INPUT_MAIN     ( ADC_IN_CH11 )
#define DIGITAL_IN_11_INPUT_RED      ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_11_RED_TYPE       ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_12_INPUT_MAIN     ( ADC_IN_CH12 )
#define DIGITAL_IN_12_INPUT_RED      ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_12_RED_TYPE       ( DIGITAL_RED_TYPE_DISABLED )
#define DIGITAL_IN_13_INPUT_MAIN     ( ADC_IN_CH13 )
#define DIGITAL_IN_13_INPUT_RED      ( DIGITAL_IN_NOT_CONNECTED )
#define DIGITAL_IN_13_RED_TYPE       ( DIGITAL_RED_TYPE_DISABLED )

//#define SAEJ_BUTTON_1_INDEX          ( DIGITAL_IN_4  ) //( SAEJ_IN_NOT_CONNECTED ) //ingresso non collegato
//#define SAEJ_BUTTON_2_INDEX          ( DIGITAL_IN_5  )   
//#define SAEJ_BUTTON_3_INDEX          ( DIGITAL_IN_13 ) 
//#define SAEJ_BUTTON_4_INDEX          ( DIGITAL_IN_10 ) 
//#define SAEJ_BUTTON_5_INDEX          ( DIGITAL_IN_11 )
//#define SAEJ_BUTTON_6_INDEX          ( DIGITAL_IN_12 )
//#define SAEJ_BUTTON_7_INDEX          ( SAEJ_IN_NOT_CONNECTED )
//#define SAEJ_BUTTON_8_INDEX          ( SAEJ_IN_NOT_CONNECTED )
//#define SAEJ_BUTTON_9_INDEX          ( SAEJ_IN_NOT_CONNECTED )
//#define SAEJ_BUTTON_10_INDEX         ( SAEJ_IN_NOT_CONNECTED ) 
//#define SAEJ_BUTTON_11_INDEX         ( SAEJ_IN_NOT_CONNECTED )
//#define SAEJ_BUTTON_12_INDEX         ( SAEJ_IN_NOT_CONNECTED ) 
//#define SAEJ_BUTTON_13_INDEX         ( SAEJ_IN_NOT_CONNECTED )

/*
    per ciascun pulsante, se questo è associato a un ingresso fisico, viene definito nel parametro
    SAEJ_BUTTON_n_INDEX, con n = 1, 2, ..., 13
    
    se il pulsante n è collegato, deve essere associato a uno degli ingressi DIGITAL_IN_j, con j = 0, 1, ..., 13    

    se il pulsante n non è collegato, oppure ha solo un valore logico non associato a ingresso fisico (ad es. l'N di un FNR software),
    SAEJ_BUTTON_n_INDEX deve essere settato al valore SAEJ_IN_NOT_CONNECTED
*/
#define SAEJ_BUTTON_1_INDEX          ( DIGITAL_IN_2 ) //( SAEJ_IN_NOT_CONNECTED ) //ingresso non collegato
#define SAEJ_BUTTON_2_INDEX          ( DIGITAL_IN_9 )
#define SAEJ_BUTTON_3_INDEX          ( DIGITAL_IN_3 )
#define SAEJ_BUTTON_4_INDEX          ( DIGITAL_IN_10 )
#define SAEJ_BUTTON_5_INDEX          ( DIGITAL_IN_4 )
#define SAEJ_BUTTON_6_INDEX          ( DIGITAL_IN_8 )
#define SAEJ_BUTTON_7_INDEX          ( DIGITAL_IN_5 )
#define SAEJ_BUTTON_8_INDEX          ( DIGITAL_IN_1 )
#define SAEJ_BUTTON_9_INDEX          ( DIGITAL_IN_6 )
#define SAEJ_BUTTON_10_INDEX         ( DIGITAL_IN_7 )
#define SAEJ_BUTTON_11_INDEX         ( SAEJ_IN_NOT_CONNECTED )
#define SAEJ_BUTTON_12_INDEX         ( SAEJ_IN_NOT_CONNECTED )
#define SAEJ_BUTTON_13_INDEX         ( SAEJ_IN_NOT_CONNECTED )

//indici pulsanti per il WST
// indicare SAEJ_WST_INDEX_NOT_CONNECTED, se non connesso, oppure SAEJ_BUTTON_n, n = 1, 2, 3... a seconda se il pulsante è connesso o meno sul WST
#define SAEJ_WST_BUTTON_1_INDEX      ( SAEJ_BUTTON_1 ) //( SAEJ_IN_NOT_CONNECTED ) //ingresso non collegato
#define SAEJ_WST_BUTTON_2_INDEX      ( SAEJ_BUTTON_2 )
#define SAEJ_WST_BUTTON_3_INDEX      ( SAEJ_BUTTON_3 )
#define SAEJ_WST_BUTTON_4_INDEX      ( SAEJ_BUTTON_4 )
#define SAEJ_WST_BUTTON_5_INDEX      ( SAEJ_BUTTON_5 )
#define SAEJ_WST_BUTTON_6_INDEX      ( SAEJ_BUTTON_6 )
#define SAEJ_WST_BUTTON_7_INDEX      ( SAEJ_BUTTON_7 )
#define SAEJ_WST_BUTTON_8_INDEX      ( SAEJ_BUTTON_8 )
#define SAEJ_WST_BUTTON_9_INDEX      ( SAEJ_BUTTON_9 )
#define SAEJ_WST_BUTTON_10_INDEX     ( SAEJ_BUTTON_10 )
#define SAEJ_WST_BUTTON_11_INDEX     ( SAEJ_WST_INDEX_NOT_CONNECTED )
#define SAEJ_WST_BUTTON_12_INDEX     ( SAEJ_WST_INDEX_NOT_CONNECTED )
#define SAEJ_WST_BUTTON_13_INDEX     ( SAEJ_WST_INDEX_NOT_CONNECTED )

//configurazione parametri FNR software
/*
   qualora il joystick preveda un FNR software (nel quale F e R sono pulsanti fisici mentre N è dato dalla logica derivante dalle loro posizioni),
   devono essere definiti i parametri:
   SAEJ_FNR_DEFAULT_EN - true, se è presente un FNR software; false, se non è presente un FNR software
   SAEJ_F_INDEX        - SAEJ_BUTTON_n, per indicare che la posizione F dell'FNR software è associata al pulsante n sul messaggio SAEJ ( n = 1, 2, ..., 13 )
                         SAEJ_IN_NOT_CONNECTED, se non presente
   SAEJ_R_INDEX        - SAEJ_BUTTON_n, per indicare che la posizione R dell'FNR software è associata al pulsante n sul messaggio SAEJ ( n = 1, 2, ..., 13 )
                         SAEJ_IN_NOT_CONNECTED, se non presente
   SAEJ_N_INDEX        - SAEJ_BUTTON_n, per indicare che la posizione N dell'FNR software è associata al pulsante n sul messaggio SAEJ ( n = 1, 2, ..., 13 )
                         SAEJ_IN_NOT_CONNECTED, se non presente
*/

#define SAEJ_FNR_DEFAULT_EN  false
#define SAEJ_F_INDEX         ( SAEJ_IN_NOT_CONNECTED )
#define SAEJ_R_INDEX         ( SAEJ_IN_NOT_CONNECTED )
#define SAEJ_N_INDEX         ( SAEJ_IN_NOT_CONNECTED )

////Doosan
//#define SAEJ_FNR_DEFAULT_EN  true
//#define SAEJ_F_INDEX         ( SAEJ_BUTTON_1 )
//#define SAEJ_R_INDEX         ( SAEJ_BUTTON_2 )
//#define SAEJ_N_INDEX         ( SAEJ_BUTTON_12 )

//configurazione logica azionamento pulsanti
#define DEFAULT_CFG_INPUT_TYPE ( 0x00003FFFUL )  //bit da 0 a DIGITAL_INPUT_NUM-1 (DIGITAL_INPUT_NUM = 14): se 1 -> input di tipo DIGITAL_ON_HI, cioè attivo alto, se 0 -> input di tipo DIGITAL_ON_LO, cioè attivo basso
#define DEFAULT_CFG_WAIT_NEED  ( 0x00003F7FUL )  //bit da 0 a DIGITAL_INPUT_NUM-1 (DIGITAL_INPUT_NUM = 14): se 1 -> input con attesa di un valore prima di iniziare a validare l'ingresso, se 0 -> input senza richiesta di attesa valore iniziale
#define DEFAULT_CFG_WAIT_VALUE ( 0x00000000UL )  //bit da 0 a DIGITAL_INPUT_NUM-1 (DIGITAL_INPUT_NUM = 14): se 0 -> input con valore di attesa DIGITAL_IN_LO, cioè basso, prima di validare l'ingresso, se 1 -> input con valore di attesa DIGITAL_IN_HI, cioè alto, prima di validare l'ingresso

//configurazione led
#define SAEJ_DEFAULT_LED_CMD_EN  SAEJ_LED_EN_OFF // diverso da 0U: i led vengono pilotati via CAN mediante il messaggio normato SAEJ. 
                                                 // I led abilitati sono corrispondenti ai bit diversi da 0, quindi: 
                                                 // bit0 = led0 -> usare macro SAEJ_LED_0_EN
                                                 // bit1 = led1 -> usare macro SAEJ_LED_1_EN
                                                 // bit2 = led2 -> usare macro SAEJ_LED_2_EN
                                                 // bit3 = led3 -> usare macro SAEJ_LED_3_EN
                                                 // se uguale a 0U: i led non vengono pilotati via CAN. Si può usare macro SAEJ_LED_EN_OFF
                                                 
#define SAEJ_DEFAULT_LED_EN      true        //true: i led vengono pilotati autonomamente dall'hardware per segnalazioni interne (es. in caso di errore); false: i led non vengono pilotati
#define SAEJ_LED_CMD_TIMEOUT_US  ( 1000000U ) //se il pilotaggio dei led via CAN è attivo, questo parametro indica il timeout in uS scaduto il quale il joystick
                                              //segnala errore per mancata ricezione del messaggio di pilotaggio

#define SAEJ_LED_ERROR_INDEX     ( SAEJ_LED_3_INDEX )  //specifica quale delle 4 uscite led deve essere associata alla segnalazione di errore
                                                       //n = 0, 1, 2, 3 
                                                       //SAEJ_LED_0_INDEX -> JP2.4
                                                       //SAEJ_LED_1_INDEX -> JP3.2
                                                       //SAEJ_LED_2_INDEX -> JP3.4
                                                       //SAEJ_LED_3_INDEX -> JP4.2 

#define DEFAULT_ISOBUS_LED_DUTY_SUPPLY_ON  ( 0x01U )   //0x00 = 25%, 0x01 = 50%, 0x02 = 75%
#define LED_COUNTER_25                     ( 3U )      //se vale 3 -> il 25% di Duty corrisponde al 25% effettivo. Se vale 2 corrisponde al 10% effettivo

   //parametrizzazione sensori
/*
   per ciascun sensore n = X, Y sono definiti:
   MLX_SIGNAL_n_ALPHA_MIN_A               per il segnale dritto, valore in punti di uscita da banda morta verso GND
   MLX_SIGNAL_n_ALPHA_MAX_A               per il segnale dritto, valore in punti di saturazione del segnale logico verso GND
   MLX_SIGNAL_n_ALPHA_ERR_TH_A            per il segnale dritto, valore in punti oltre il quale, andando verso GND, viene segnalato errore
   MLX_SIGNAL_n_ALPHA_MIN_B               per il segnale dritto, valore in punti di uscita da banda morta verso Vcc
   MLX_SIGNAL_n_ALPHA_MAX_B               per il segnale dritto, valore in punti di saturazione del segnale logico verso Vcc
   MLX_SIGNAL_n_ALPHA_ERR_TH_B            per il segnale dritto, valore in punti oltre il quale, andando verso Vcc, viene segnalato errore
   MLX_SIGNAL_n_ALPHA_SAFE_DB             per il segnale dritto, valore in punti della banda di guardia, in uscita da banda morta e a fine corsa, per tenere conto delle tolleranze meccaniche
   MLX_REDUNDANCE_n_ALPHA_MIN_A           per il segnale ridondato, valore in punti di uscita da banda morta verso GNDper il segnale ridondato, valore in mV di uscita da banda morta verso GND
   MLX_REDUNDANCE_n_ALPHA_MAX_A           per il segnale ridondato, valore in punti di saturazione del segnale logico verso GND    per il segnale ridondato, valore in mV di saturazione del segnale logico verso GND
   MLX_REDUNDANCE_n_ALPHA_ERR_TH_A        per il segnale ridondato, valore in punti oltre il quale, andando verso GND, viene segnalato errore    per il segnale ridondato, valore in mV oltre il quale, andando verso GND, viene segnalato errore
   MLX_REDUNDANCE_n_ALPHA_MIN_B           per il segnale ridondato, valore in punti di uscita da banda morta verso Vcc    per il segnale ridondato, valore in mV di uscita da banda morta verso Vcc
   MLX_REDUNDANCE_n_ALPHA_MAX_B           per il segnale ridondato, valore in punti di saturazione del segnale logico verso Vcc    per il segnale ridondato, valore in mV di saturazione del segnale logico verso Vcc
   MLX_REDUNDANCE_n_ALPHA_ERR_TH_B        per il segnale ridondato, valore in punti oltre il quale, andando verso Vcc, viene segnalato errore    per il segnale ridondato, valore in mV oltre il quale, andando verso Vcc, viene segnalato errore
   MLX_REDUNDANCE_n_ALPHA_SAFE_DB         per il segnale ridondato, valore in punti della banda di guardia, in uscita da banda morta e a fine corsa, per tenere conto delle tolleranze meccaniche

   Questi valori vengono calibrati una volta sola e NON ANDREBBERO MAI MODIFICATI

   Si definisce inoltre se abilitare o meno il controllo di ridondanza del sensore mediante la macro MLX_REDUNDANCE_TYPE:
   MLX_REDUNDANCE_ON   -> controllo di ridondanza abilitato
   MLX_REDUNDANCE_OFF  -> controllo di ridondanza disabilitato
*/

#define MLX_REDUNDANCE_TYPE      MLX_REDUNDANCE_OFF

//sensore 0
#define MLX_0_ALPHA_MIN_A        (  3000U )
#define MLX_0_ALPHA_MAX_A        (  1500U )
#define MLX_0_ALPHA_MIN_B        (  5000U )
#define MLX_0_ALPHA_MAX_B        (  6500U )
#define MLX_0_ALPHA_ERR_TH_A     (  1000U )
#define MLX_0_ALPHA_ERR_TH_B     (  7000U )
#define MLX_0_ALPHA_SAFE_DB      (     0U )
#define MLX_0_BETA_MIN_A         (  3000U )
#define MLX_0_BETA_MAX_A         (  1500U )
#define MLX_0_BETA_MIN_B         (  5000U )
#define MLX_0_BETA_MAX_B         (  6500U )
#define MLX_0_BETA_ERR_TH_A      (  1000U )
#define MLX_0_BETA_ERR_TH_B      (  7000U )
#define MLX_0_BETA_SAFE_DB       (     0U )

//sensore 1
#define MLX_1_ALPHA_MIN_A        (  3000U )
#define MLX_1_ALPHA_MAX_A        (  1500U )
#define MLX_1_ALPHA_MIN_B        (  5000U )
#define MLX_1_ALPHA_MAX_B        (  6500U )
#define MLX_1_ALPHA_ERR_TH_A     (  1000U )
#define MLX_1_ALPHA_ERR_TH_B     (  7000U )
#define MLX_1_ALPHA_SAFE_DB      (     0U )
#define MLX_1_BETA_MIN_A         (  3000U )
#define MLX_1_BETA_MAX_A         (  1500U )
#define MLX_1_BETA_MIN_B         (  5000U )
#define MLX_1_BETA_MAX_B         (  6500U )
#define MLX_1_BETA_ERR_TH_A      (  1000U )
#define MLX_1_BETA_ERR_TH_B      (  7000U )
#define MLX_1_BETA_SAFE_DB       (     0U )


//condigurazione roller impugnatura
/*
    per ciascun roller n = W, Z, J, Q, V sono definiti i parametri:
    ROLLER_n_SGN_EN                       - associato alla presenza del segnale principale
    ROLLER_n_RED_EN                       - associato alla presenza del segnale ridondato

    nella configurazione bisogna fare l'OR con i parametri richiesti
    se non sono previsti gli assi, si assegna il valore ROLLER_ALL_DISABLED
*/
//#define ROLLER_CFG_DEFAULT        ROLLER_ALL_DISABLED //nessun roller presente
//#define ROLLER_CFG_DEFAULT          ( ROLLER_W_SGN_EN | ROLLER_Z_SGN_EN | ROLLER_J_SGN_EN | ROLLER_Q_SGN_EN | ROLLER_V_SGN_EN )
//#define ROLLER_CFG_DEFAULT          ( ROLLER_Z_SGN_EN )
#define ROLLER_CFG_DEFAULT          ROLLER_ALL_DISABLED


//#define ADC_IN_CH0           (  0U ) //PORT PTA0 -->JP1.1
//#define ADC_IN_CH1           (  1U ) //PORT PTA1 -->JP1.3
//#define ADC_IN_CH2           (  2U ) //PORT PTA6 -->JP2.1
//#define ADC_IN_CH3           (  3U ) //PORT PTA7 -->JP2.3
//#define ADC_IN_CH4           (  4U ) //PORT PTB0 -->JP3.1
//#define ADC_IN_CH5           (  5U ) //PORT PTB1 -->JP3.3
//#define ADC_IN_CH6           (  6U ) //PORT PTB2 -->JP4.1
//#define ADC_IN_VREF_ADC0     (  7U ) //PORT PTB3 -->VREF_ADC0_M4
//#define ADC_IN_5V_SENSOR     (  8U ) //5V_SENSOR
//#define ADC_IN_CH7           (  9U ) //PORT PTA2 -->JP1.2
//#define ADC_IN_CH8           ( 10U ) //PORT PTA3 -->JP1.4
//#define ADC_IN_CH9           ( 11U ) //PORT PTD2 -->JP2.2
//#define ADC_IN_CH10          ( 12U ) //PORT PTD3 -->JP2.4
//#define ADC_IN_CH11          ( 13U ) //PORT PTC6 -->JP3.2
//#define ADC_IN_CH12          ( 14U ) //PORT PTC7 -->JP3.4
//#define ADC_IN_CH13          ( 15U ) //PORT PTD4 -->JP4.2
//#define ADC_IN_VREF_ADC1     ( 16U ) //PORT PTB12 ->VREF_ADC1_M4
//#define ADC_IN_24V_SYS       ( 17U ) //24V_SYS


//abilitazione errori dei roller
/*
    per ciascun roller n = W, Z, J, Q, V si può decidere se, in caso di errore
    sul segnale (roller attivo ma non collegato, errore di ridondanda, segnale fuori dai limit, etc.),
    venga pubblicato sul DM1 il corrispondente DTC. A tale scopo è definita la macro
    ROLLER_n_ERROR_SHOW_EN                - in caso di errore sul roller n, il corrispondente DTC sul DM1 viene pubblicato
    
    nella configurazione bisogna fare l'OR con i parametri richiesti
    se è previsto che nessun DTC sia pubblicato per alcun roller, si assegna il valoe ROLLER_ALL_ERROR_SHOW_DISABLED

    indipendentemente dalla configurazione stabilita, se il roller n è abilitato, in caso di errore il suo setpoint
    viene comunque settato al valore SETPOINT_OUT_ERROR sul messaggio EJM, anche se il DTC sul DM1 è disabilitato
*/
#define ROLLER_ERROR_CFG_DEFAULT    ( ROLLER_ALL_ERROR_SHOW_DISABLED )

//assegnazione ingressi fisici roller
/*
   per ciascun roller vengono definiti i parametri
   ROLLER_n_IN_SGN
   ROLLER_n_IN_RED
   con n = W, Z, J, Q, V
   tali parametri associano al roller n il corrispondente ingresso fisico, sia per il segnale dritto che per il ridondato (qualora presente)

   l'ingresso fisico è indicato come ADC_INPUT_Gx_CHy ( x= 0, 1, 2, 3, y = 0, 1, 2...), secondo la denominazione
   riferita nello schema elettrico dell'hardware

   in caso di segnale non collegato, si assegna il valore ROLLER_INPUT_NOT_CONNECTED
*/

#define ROLLER_W_IN_SGN             ( ROLLER_INPUT_NOT_CONNECTED ) //(ROLLER_INPUT_NOT_CONNECTED) //segnale non collegato
#define ROLLER_Z_IN_SGN             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_J_IN_SGN             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_Q_IN_SGN             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_V_IN_SGN             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_W_IN_RED             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_Z_IN_RED             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_J_IN_RED             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_Q_IN_RED             ( ROLLER_INPUT_NOT_CONNECTED )
#define ROLLER_V_IN_RED             ( ROLLER_INPUT_NOT_CONNECTED )

//parametrizzazione roller
/*
   per ciascun roller n = W, Z, J, Q, V sono definiti:
   ROLLER_n_SGN_MIN_A               per il segnale dritto, valore in mV di uscita da banda morta verso GND
   ROLLER_n_SGN_MAX_A               per il segnale dritto, valore in mV di saturazione del segnale logico verso GND
   ROLLER_n_SGN_ERR_TH_A            per il segnale dritto, valore in mV oltre il quale, andando verso GND, viene segnalato errore
   ROLLER_n_SGN_MIN_B               per il segnale dritto, valore in mV di uscita da banda morta verso Vcc
   ROLLER_n_SGN_MAX_B               per il segnale dritto, valore in mV di saturazione del segnale logico verso Vcc
   ROLLER_n_SGN_ERR_TH_B            per il segnale dritto, valore in mV oltre il quale, andando verso Vcc, viene segnalato errore
   ROLLER_n_RED_MIN_A               per il segnale ridondato, valore in mV di uscita da banda morta verso GND
   ROLLER_n_RED_MAX_A               per il segnale ridondato, valore in mV di saturazione del segnale logico verso GND
   ROLLER_n_RED_ERR_TH_A            per il segnale ridondato, valore in mV oltre il quale, andando verso GND, viene segnalato errore
   ROLLER_n_RED_MIN_B               per il segnale ridondato, valore in mV di uscita da banda morta verso Vcc
   ROLLER_n_RED_MAX_B               per il segnale ridondato, valore in mV di saturazione del segnale logico verso Vcc
   ROLLER_n_RED_ERR_TH_B            per il segnale ridondato, valore in mV oltre il quale, andando verso Vcc, viene segnalato errore
*/

//roller W
#define ROLLER_W_SGN_MIN_A            ( 2250U )
#define ROLLER_W_SGN_MAX_A            ( 750U  )
#define ROLLER_W_SGN_ERR_TH_A         ( 250U  )
#define ROLLER_W_SGN_MIN_B            ( 2750U )
#define ROLLER_W_SGN_MAX_B            ( 4250U )
#define ROLLER_W_SGN_ERR_TH_B         ( 4750U )
#define ROLLER_W_RED_MIN_A            ( 2250U )
#define ROLLER_W_RED_MAX_A            ( 750U  )
#define ROLLER_W_RED_ERR_TH_A         ( 250U  )
#define ROLLER_W_RED_MIN_B            ( 2750U )
#define ROLLER_W_RED_MAX_B            ( 4250U )
#define ROLLER_W_RED_ERR_TH_B         ( 4750U )

//roller Z
#define ROLLER_Z_SGN_MIN_A            ( 2250U )
#define ROLLER_Z_SGN_MAX_A            ( 750U  )
#define ROLLER_Z_SGN_ERR_TH_A         ( 250U  )
#define ROLLER_Z_SGN_MIN_B            ( 2750U )
#define ROLLER_Z_SGN_MAX_B            ( 4250U )
#define ROLLER_Z_SGN_ERR_TH_B         ( 4750U )
#define ROLLER_Z_RED_MIN_A            ( 2250U )
#define ROLLER_Z_RED_MAX_A            ( 750U  )
#define ROLLER_Z_RED_ERR_TH_A         ( 250U  )
#define ROLLER_Z_RED_MIN_B            ( 2750U )
#define ROLLER_Z_RED_MAX_B            ( 4250U )
#define ROLLER_Z_RED_ERR_TH_B         ( 4750U )

//roller J
#define ROLLER_J_SGN_MIN_A            ( 2250U )
#define ROLLER_J_SGN_MAX_A            ( 750U  )
#define ROLLER_J_SGN_ERR_TH_A         ( 250U  )
#define ROLLER_J_SGN_MIN_B            ( 2750U )
#define ROLLER_J_SGN_MAX_B            ( 4250U )
#define ROLLER_J_SGN_ERR_TH_B         ( 4750U )
#define ROLLER_J_RED_MIN_A            ( 2250U )
#define ROLLER_J_RED_MAX_A            ( 750U  )
#define ROLLER_J_RED_ERR_TH_A         ( 250U  )
#define ROLLER_J_RED_MIN_B            ( 2750U )
#define ROLLER_J_RED_MAX_B            ( 4250U )
#define ROLLER_J_RED_ERR_TH_B         ( 4750U )

//roller Q
#define ROLLER_Q_SGN_MIN_A            ( 2250U )
#define ROLLER_Q_SGN_MAX_A            ( 750U  )
#define ROLLER_Q_SGN_ERR_TH_A         ( 250U  )
#define ROLLER_Q_SGN_MIN_B            ( 2750U )
#define ROLLER_Q_SGN_MAX_B            ( 4250U )
#define ROLLER_Q_SGN_ERR_TH_B         ( 4750U )
#define ROLLER_Q_RED_MIN_A            ( 2250U )
#define ROLLER_Q_RED_MAX_A            ( 750U  )
#define ROLLER_Q_RED_ERR_TH_A         ( 250U  )
#define ROLLER_Q_RED_MIN_B            ( 2750U )
#define ROLLER_Q_RED_MAX_B            ( 4250U )
#define ROLLER_Q_RED_ERR_TH_B         ( 4750U )

//roller V
#define ROLLER_V_SGN_MIN_A            ( 2250U )
#define ROLLER_V_SGN_MAX_A            ( 750U  )
#define ROLLER_V_SGN_ERR_TH_A         ( 250U  )
#define ROLLER_V_SGN_MIN_B            ( 2750U )
#define ROLLER_V_SGN_MAX_B            ( 4250U )
#define ROLLER_V_SGN_ERR_TH_B         ( 4750U )
#define ROLLER_V_RED_MIN_A            ( 2250U )
#define ROLLER_V_RED_MAX_A            ( 750U  )
#define ROLLER_V_RED_ERR_TH_A         ( 250U  )
#define ROLLER_V_RED_MIN_B            ( 2750U )
#define ROLLER_V_RED_MAX_B            ( 4250U )
#define ROLLER_V_RED_ERR_TH_B         ( 4750U )

#endif