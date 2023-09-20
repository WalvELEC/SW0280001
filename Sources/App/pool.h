/*
 *  pool.h
 *
 *  Created on: 06 sep 2018
 *      Author: PoliD
 */

#ifndef _POOL_H
#define _POOL_H

//------------------------------------------------------------------------------
//etichetta pool
#define ISO_VERSION_LABEL             "DJW01E1"
#define ISO_VERSION_LABEL_CHAR_0            'D'
#define ISO_VERSION_LABEL_CHAR_1            'J'
#define ISO_VERSION_LABEL_CHAR_2            'W'
#define ISO_VERSION_LABEL_CHAR_3            '0'
#define ISO_VERSION_LABEL_CHAR_4            '1'
#define ISO_VERSION_LABEL_CHAR_5            'E'
#define ISO_VERSION_LABEL_CHAR_6            '1'

#define DESIGNATOR_WIDTH   72
#define DESIGNATOR_HEIGHT  72

#define SQUARE_SIZE    24
#define ELLIPSE_SIZE   24
#define MODE_MASK_SIZE 24
#define MODE_MASK_SIZE_DESIGNATOR 18
#define BUTTON_MASK_SIZE 36
#define AXIS_MASK_SIZE 60

#define STRING_WIDTH   90
#define STRING_HEIGHT  32


#define REF_DM_PIC_JOY_X   0
#define REF_DM_PIC_JOY_Y   0

//X - ok
#define REF_DM_OP_33000_X	282
#define REF_DM_OP_33000_Y	2 

//Y - ok
#define REF_DM_OP_33001_X	2
#define REF_DM_OP_33001_Y	2

//Z (condiviso con P7) - ok
#define REF_DM_OP_33003_X	402
#define REF_DM_OP_33003_Y	242

//J (condiviso con P9) - ok
#define REF_DM_OP_33004_X	402
#define REF_DM_OP_33004_Y	82

//W (condiviso con P5) - ok
#define REF_DM_OP_33002_X	322
#define REF_DM_OP_33002_Y	402

//P1 - ok
#define REF_DM_OP_33005_X	2
#define REF_DM_OP_33005_Y	402

//P2 - ok
#define REF_DM_OP_33006_X	82
#define REF_DM_OP_33006_Y	402 

//P3 - ok
#define REF_DM_OP_33007_X	162
#define REF_DM_OP_33007_Y	402

//P4 - ok
#define REF_DM_OP_33008_X	242
#define REF_DM_OP_33008_Y	402

//P5 - ok
#define REF_DM_OP_33009_X	322
#define REF_DM_OP_33009_Y	402

//P6 - ok
#define REF_DM_OP_33010_X	402
#define REF_DM_OP_33010_Y	322

//P7 - ok
#define REF_DM_OP_33011_X	402
#define REF_DM_OP_33011_Y	242

//P8 - ok
#define REF_DM_OP_33012_X	402
#define REF_DM_OP_33012_Y	162

//P9 - ok
#define REF_DM_OP_33013_X	402
#define REF_DM_OP_33013_Y	82

//P0 - ok
#define REF_DM_OP_33014_X	402
#define REF_DM_OP_33014_Y	2

#define REF_P0_MASK_X           368
#define REF_P0_MASK_Y           22

#define REF_AN_Y_MASK_X         76
#define REF_AN_Y_MASK_Y         22

#define REF_AN_X_MASK_X         246
#define REF_AN_X_MASK_Y         22

//#define REF_DM_OP_33014_X	DM_WIDTH - DESIGNATOR_WIDTH
//#define REF_DM_OP_33014_Y	(DM_HEIGHT/2)

//JOY X SX - ok
#define REF_DM_OP_33015_X	2
#define REF_DM_OP_33015_Y	162

//JOY X DX (condiviso con X) - ok
#define REF_DM_OP_33016_X	282
#define REF_DM_OP_33016_Y	82

//JOY Y BW - ok
#define REF_DM_OP_33017_X	282
#define REF_DM_OP_33017_Y	162

//JOY Y FW (condiviso con Y)
#define REF_DM_OP_33018_X	2
#define REF_DM_OP_33018_Y	82

//P1 marker
#define REF_RECT_P1_X  178
//#define REF_RECT_P1_X  177 per BN e 16
#define REF_RECT_P1_X_BN_16  177
#define REF_RECT_P1_Y  257

//P2 marker
#define REF_RECT_P2_X  201
//#define REF_RECT_P2_X  200
#define REF_RECT_P2_X_BN_16  200
#define REF_RECT_P2_Y  267

//P3 marker
//#define REF_RECT_P3_X  215
#define REF_RECT_P3_X  216
#define REF_RECT_P3_X_BN_16  215
#define REF_RECT_P3_Y  287

//P4 marker
#define REF_RECT_P4_X  190
//#define REF_RECT_P4_X  189
#define REF_RECT_P4_X_BN_16  189
#define REF_RECT_P4_Y  293

//P5 marker
//#define REF_RECT_P5_X  205
#define REF_RECT_P5_X  206
#define REF_RECT_P5_X_BN_16  205
#define REF_RECT_P5_Y  314

//#define REF_RECT_P5_ROLLER_X  205
#define REF_RECT_P5_ROLLER_X  206
#define REF_RECT_P5_ROLLER_X_BN_16  205
#define REF_RECT_P5_ROLLER_Y  314

//P6 marker
#define REF_RECT_P6_X  119
//#define REF_RECT_P6_X  118
#define REF_RECT_P6_X_BN_16  118
#define REF_RECT_P6_Y  267

//P7 marker
#define REF_RECT_P7_X  131
//#define REF_RECT_P7_X  130
#define REF_RECT_P7_X_BN_16  130
#define REF_RECT_P7_Y  293

#define REF_RECT_P7_ROLLER_X  121
//#define REF_RECT_P7_ROLLER_X  120
#define REF_RECT_P7_ROLLER_X_BN_16  120
#define REF_RECT_P7_ROLLER_Y  301

//P8 marker
#define REF_RECT_P8_X  146
//#define REF_RECT_P8_X  145
#define REF_RECT_P8_X_BN_16  145
#define REF_RECT_P8_Y  267

//P9 marker
#define REF_RECT_P9_X  158
//#define REF_RECT_P9_X  157
#define REF_RECT_P9_X_BN_16  157
#define REF_RECT_P9_Y  293

#define REF_RECT_P9_ROLLER_X  158
//#define REF_RECT_P9_ROLLER_X  157
#define REF_RECT_P9_ROLLER_X_BN_16  157
#define REF_RECT_P9_ROLLER_Y  293

//P0 marker
#define REF_RECT_P0_X  180
#define REF_RECT_P0_Y  100

//rect mode
#define REF_RECT_MODE_1_X 82
#define REF_RECT_MODE_1_Y 350
#define REF_RECT_MODE_2_X 262
#define REF_RECT_MODE_2_Y 350

//labels
#define REF_UP_X  162
#define REF_UP_Y  2
#define REF_LATCH_X_X 2
#define REF_LATCH_X_Y 242
#define REF_LATCH_Y_X 282
#define REF_LATCH_Y_Y 242
#define REF_LATCH_W_X 282
#define REF_LATCH_W_Y 278
#define REF_LATCH_Z_X 2
#define REF_LATCH_Z_Y 278
#define REF_LATCH_J_X 2
#define REF_LATCH_J_Y 314

// Object Pool
#define ISO_DESIGNATOR_WIDTH                160
#define ISO_DESIGNATOR_HEIGHT                80
#define ISO_MASK_SIZE                       480
#define MASK_WIDTH                          200
#define MASK_HEIGHT                         200
#define WorkingSet_0                          0
#define DataMask_1000                      1000
#define DataMask_1001                      1001
#define Container_3000                     3000
#define Container_3001                     3001
#define Container_3002                     3002
#define Container_3003                     3003
#define Container_3004                     3004
#define Container_3005                     3005
#define Container_3006                     3006
#define Container_3007                     3007
#define Container_3008                     3008
#define Container_3009                     3009
#define Container_3010                     3010
#define Container_3011                     3011
#define Container_3012                     3012
#define Container_3013                     3013
#define Container_3014                     3014
#define Container_3015                     3015
#define Container_3016                     3016
#define Container_3017                     3017
#define Container_3018                     3018
#define Container_3019                     3019
#define Container_3020                     3020
#define Container_3021                     3021
#define Container_3022                     3022
#define Container_3023                     3023
#define Container_3024                     3024
#define Container_3025                     3025
#define Container_3026                     3026
#define Container_3027                     3027
#define Container_3028                     3028
#define Container_3029                     3029
#define Container_3030                     3030
#define Container_3031                     3031
#define Container_3032                     3032
#define Container_3033                     3033
#define Container_3034                     3034
#define Container_3035                     3035
#define Container_3036                     3036

#define OutputString_11000                11000
#define OutputString_11001                11001
#define OutputString_11002                11002
#define OutputString_11003                11003
#define OutputString_11004                11004
#define OutputString_11005                11005
#define OutputString_11006                11006
#define OutputString_11007                11007
#define OutputString_11008                11008
#define OutputString_11009                11009
#define OutputString_11010                11010
#define OutputString_11011                11011
#define OutputString_11012                11012
#define OutputString_11013                11013
#define OutputString_11014                11014
#define OutputString_11015                11015
#define OutputString_11016                11016
#define OutputString_11017                11017
#define OutputString_11018                11018
#define OutputString_11019                11019
#define OutputString_11020                11020
#define OutputNumber_Header_12000         12000
#define Imp_Fronte_20x20_mono_20001       20001
#define Imp_Lato_20x20_mono_20002         20002
#define JoyHandle_20000                   20000
#define Logo_20x20_mono_20004             20004
#define Pattern_20005                     20005
#define Pattern_20006                     20006
#define FontAttributes_23000              23000
#define FontAttributes_23001              23001
#define LineAttributes_24000              24000
#define LineAttributes_24001              24001
#define LineAttributes_24002              24002
#define LineAttributes_24003              24003
#define LineAttributes_24004              24004
#define FillAttributes_25000              25000
#define FillAttributes_25001              25001
#define Rect_14000                        14000
#define Rect_14001                        14001
#define Rect_14002                        14002
#define Rect_14003                        14003
#define Rect_14004                        14004
#define Rect_14005                        14005
#define Rect_14006                        14006
#define Rect_14007                        14007
#define Rect_14008                        14008
#define Rect_14009                        14009
#define Rect_14010                        14010
#define Rect_14011	                  14011
#define Rect_14012	                  14012
#define Rect_14013	                  14013
#define Rect_14014	                  14014
#define Rect_14015	                  14015
#define Rect_14016	                  14016
#define Rect_14017	                  14017
#define Ellipse_15000                     15000
#define Ellipse_15001                     15001
#define Ellipse_15002                     15002
#define Ellipse_15003                     15003
#define Ellipse_15004                     15004
#define Ellipse_15005                     15005
#define Ellipse_15006                     15006
#define Ellipse_15007                     15007
#define Ellipse_15008                     15008
#define Ellipse_15009                     15009
#define Ellipse_15010                     15010
#define Ellipse_15011                     15011
#define Ellipse_15012                     15012
#define Ellipse_15013                     15013
#define Ellipse_15014                     15014
#define Ellipse_15015                     15015
#define Ellipse_15016                     15016
#define Ellipse_15017                     15017
#define Ellipse_15018                     15018
#define Ellipse_15019                     15019
#define Ellipse_15020                     15020
#define AuxJoyX_28000                     28000
#define AuxJoyY_28001                     28001
#define AuxJoyW_28002                     28002
#define AuxJoyZ_28003                     28003
#define AuxJoyJ_28004                     28004
#define AuxInputButton1_28005             28005
#define AuxInputButton2_28006             28006
#define AuxInputButton3_28007             28007
#define AuxInputButton4_28008             28008
#define AuxInputButton5_28009             28009
#define AuxInputButton6_28010             28010
#define AuxInputButton7_28011             28011
#define AuxInputButton8_28012             28012
#define AuxInputButton9_28013             28013
#define AuxInputButton1_28014             28014
#define AuxInputButton2_28015             28015
#define AuxInputButton3_28016             28016
#define AuxInputButton4_28017             28017
#define AuxInputButton5_28018             28018
#define AuxInputButton6_28019             28019
#define AuxInputButton7_28020             28020
#define AuxInputButton8_28021             28021
#define AuxInputButton9_28022             28022
#define AuxJoyXSx_28023                   28023
#define AuxJoyXDx_28024                   28024
#define AuxJoyYFw_28025                   28025
#define AuxJoyYBw_28026                   28026
#define AuxInputButton10_28027            28027
#define ObjPointer_33000                  33000
#define ObjPointer_33001                  33001
#define ObjPointer_33002                  33002
#define ObjPointer_33003                  33003
#define ObjPointer_33004                  33004
#define ObjPointer_33005                  33005
#define ObjPointer_33006                  33006
#define ObjPointer_33007                  33007
#define ObjPointer_33008                  33008
#define ObjPointer_33009                  33009
#define ObjPointer_33010                  33010
#define ObjPointer_33011                  33011
#define ObjPointer_33012                  33012

#define LineBar_18000                     18000
#define LineBar_18001                     18001
#define LineBar_18002                     18002
#define LineBar_18003                     18003

#define ID_NULL               0xFFFFUL

#define TYPEID_WORKSET        0
#define TYPEID_DATAMASK       1
#define TYPEID_ALARMMASK      2
#define TYPEID_CONTAINER      3
#define TYPEID_SKEYMASK       4
#define TYPEID_SOFTKEY        5
#define TYPEID_BUTTON         6
#define TYPEID_INBOOL         7
#define TYPEID_INSTR          8
#define TYPEID_INNUM          9
#define TYPEID_INLIST        10
#define TYPEID_OUTSTR        11
#define TYPEID_OUTNUM        12
#define TYPEID_OUTLINE       13
#define TYPEID_OUTRECT       14
#define TYPEID_OUTELLIPSE    15
#define TYPEID_OUTPOLY       16
#define TYPEID_OUTMETER      17
#define TYPEID_OUTLINBAR     18
#define TYPEID_OUTARCBAR     19
#define TYPEID_OUTPICT       20
#define TYPEID_VARNUM        21
#define TYPEID_VARSTR        22
#define TYPEID_FONTATTR      23
#define TYPEID_LINEATTR      24
#define TYPEID_FILLATTR      25
#define TYPEID_INPATTR       26
#define TYPEID_OBJPTR        27
#define TYPEID_MACRO         28
#define TYPEID_AUXFUNC       29
#define TYPEID_AUXINP        30
#define TYPEID_AUXFUNC2	     31
#define TYPEID_AUXINP2       32
#define TYPEID_AUXPOINTER    33

#define EV_REFRESH            0
#define EV_ACT                1
#define EV_DEACT              2
#define EV_SHOW               3
#define EV_HIDE               4
#define EV_ENABLE             5
#define EV_DISABLE            6
#define EV_CHGACTMASK         7
#define EV_CHGSKEYMASK        8
#define EV_CHGATTR            9
#define EV_CHGBKCOLOR        10
#define EV_CHGFONTATTR       11
#define EV_CHGLINEATTR       12
#define EV_CHGFILLATTR       13
#define EV_CHGCHILDLOC       14
#define EV_CHGSIZE           15
#define EV_CHGVAL            16
#define EV_CHGPRIOR          17
#define EV_CHGENDPNT         18
#define EV_SELINPUT          19
#define EV_DESELINPUT        20
#define EV_ESC               21
#define EV_ENTERVAL          22
#define EV_ENTERCHGVAL       23
#define EV_KEYPRESS          24
#define EV_KEYRELEASE        25
#define EV_CHGCHILDPOS       26

#define CMD_HIDE_SHOW               160
#define CMD_ENABLE_DISABLE          161
#define CMD_SELECT_INPUT_OBJECT     162
#define CMD_CONTROL_AUDIO_DEVICE    163
#define CMD_SET_AUDIO_VOLUME        164
#define CMD_CHANGE_CHILD_LOCATION   165
#define CMD_CHANGE_SIZE             166
#define CMD_CHANGE_BACKGROUND_COLOR 167
#define CMD_CHANGE_NUMERIC_VALUE    168
#define CMD_CHANGE_END_POINT        169
#define CMD_CHANGE_FONT_ATTRIBUTES  170
#define CMD_CHANGE_LINE_ATTRIBUTES  171
#define CMD_CHANGE_FILL_ATTRIBUTES  172
#define CMD_CHANGE_ACTIVE_MASK      173
#define CMD_CHANGE_SOFT_KEY_MASK    174
#define CMD_CHANGE_ATTRIBUTE        175
#define CMD_CHANGE_PRIORITY         176
#define CMD_CHANGE_LIST_ITEM        177
#define CMD_CHANGE_STRING_VALUE     179
#define CMD_CHANGE_CHILD_POSITION   180
#define CMD_SET_OBJECT_LABEL        181
#define CMD_CHANGE_POLYGON_POINT    182
#define CMD_CHANGE_POLYGON_SCALE    183
#define CMD_GRAPHICS_CONTEXT        184
#define CMD_LOCK_UNLOCK_MASK        189
#define CMD_EXECUTE_MACRO           190

#define COLOR_BLACK       0
#define COLOR_WHITE       1
#define COLOR_GREEN       2
#define COLOR_TEAL        3
#define COLOR_MAROON      4
#define COLOR_PURPLE      5
#define COLOR_OLIVE       6
#define COLOR_SILVER      7
#define COLOR_GREY        8
#define COLOR_BLUE        9
#define COLOR_LIME       10
#define COLOR_CYAN       11
#define COLOR_RED        12
#define COLOR_MAGENTA    13
#define COLOR_YELLOW     14
#define COLOR_NAVY       15
//---
#define COLOR_ORANGE     214

#define FLOAT_1      0x00, 0x00, 0x80, 0x3F
#define FLOAT_10     0x00, 0x00, 0x20, 0x41
#define FLOAT_100    0x00, 0x00, 0xC8, 0x42
#define FLOAT_1000   0x00, 0x00, 0x7A, 0x44
#define FLOAT_0_1    0xCD, 0xCC, 0xCC, 0x3D
#define FLOAT_0_01   0x0A, 0xD7, 0x23, 0x3C
#define FLOAT_0_001  0x6F, 0x12, 0x83, 0x3A

//nuove definizioni 12 sept 2018
#define DM_SIZE	       ( 480U )
#define DM_WIDTH       ( 480U )
#define DM_HEIGHT      ( 480U )

#define AUX_IN_AN_NUM_MAX           (  5U )
#define AUX_IN_BOOL_NUM_MAX         ( 23U )
#define AUX_IN_BOOL_SHIFT_NUM       (  9U )
#define AUX_IN_BOOL_SINGLE_NUM      (  1U ) 
#define AUX_IN_BOOL_DIRECTION_NUM   (  4U )

#define SIZE_OF_OP_FAIL                  ( 96U )
//#define SIZE_OF_WS_DM                    ( 255U ) //( 254u ) //27 + 6*numero di oggetti
#define SIZE_OF_WS_DM                    ( 407U ) ////41 + 6*numero di oggetti - ho aggiunto 6 + 8 byte in più per considerare l'oggetto associato all'header mask nel WS per Muller (6 byte) e l'intestazione della Header DM (8 byte)
#define SIZE_OF_CONTAINER                ( 706U ) //10*numero container + 6*numero totale di oggetti
#define SIZE_OF_OUT_STRINGS              ( 353U ) //17*numero stringhe + numero totale di caratteri 
#define SIZE_OF_OUT_RECTS                ( 104U  ) //13*numero rettangoli
#define SIZE_OF_OUT_ELLIPSE              ( 180U ) //15*numero ellissi
//#define SIZE_OF_OUT_HEAD_ELLIPSE         ( 135U ) //15*numero ellissi
#define SIZE_OF_OUT_HEAD_ELLIPSE         ( 117U ) //13*numero rettangoli
#define SIZE_OF_PATTERNS                 ( 0U   ) //25*numero pattern
#define SIZE_OF_GRAPH_ATTRIBUTES         ( 112U ) //8*numero attributi
#define SIZE_OF_AUX_INPUT                ( 336U ) //( 444U ) //6*numero aux + 6*numero oggetti totali
#define SIZE_OF_OBJ_POINTER              ( 114U ) //6*numero pointer
#define SIZE_OF_OUT_NUM                  ( 29U  ) //29*numero output numbers  
#define SIZE_OF_OUT_LINEBAR              ( 96U  ) //24*numero di linebar   

#define ISOBUS_OP_STEP_OUT_STRINGS       (  0U )
#define ISOBUS_OP_STEP_OUT_RECTS         (  1U )
#define ISOBUS_OP_STEP_OUT_ELLIPSE       (  2U )
#define ISOBUS_OP_STEP_OUT_HEAD_ELLIPSE  (  3U )
#define ISOBUS_OP_STEP_OUT_NUM           (  4U )
#define ISOBUS_OP_STEP_OUT_LINEBAR       (  5U )
//#define STEP_PATTERNS                    (  3u )
//#define ISOBUS_OP_LAST_STEP_BEFORE_PICS  ( ISOBUS_OP_STEP_OUT_ELLIPSE )
#define ISOBUS_OP_LAST_STEP_BEFORE_PICS  ( ISOBUS_OP_STEP_OUT_LINEBAR )
//#define STEP_GRAPH_ATTRIBUTES            (  8u )
//#define STEP_AUX_INPUT                   (  9u )
//#define STEP_OBJ_POINTER                 ( 10u )
//#define STEP_CONTAINER                   ( 11u )
#define ISOBUS_OP_STEP_FIRST             ( 0U )
#define AUX_INPUT_NUM_MAX                ( 28U )

#define NUM_PICTURE_OBJ                  ( 30U )
#define PIC_DATA_OFFSET                  ( 1U )
//#define SIZE_OF_PICTURE_256              ( 52256U )  //size del picture DM a 256 colori + size del picture designatore a 256 colori + 20386U (somma delle parti fisse relative ai designatori e al resto della grafica)
#define SIZE_OF_PICTURE_256              ( 73032U ) //( 77054U ) //( 57778U ) //( 66484U ) //( 66034U ) //( 63660U ) //( 60358U )  //( 57142U )  //( 56952U )  //size del picture DM a 256 colori + size del picture designatore a 256 colori + size del picture Header DM a 256 + 20386U (somma delle parti fisse relative ai designatori e al resto della grafica)
#define SIZE_OF_PICTURE_16               ( 53602U ) //( 53790U ) //( 41852U ) //( 43220U ) //( 42870U ) //( 40334U ) //( 44192U )  //( 40976U )  //( 40786U )  //size del picture DM a 16 colori + size del picture designatore a 16 colori + 20386U (somma delle parti fisse relative ai designatori e al resto della grafica)
#define SIZE_OF_PICTURE_MONO             ( 30866U ) //( 31140U ) //( 24948U ) //( 23154U )  //( 21588U )  //( 21392U )  //size del picture DM monocromatico + size del picture designatore monocromatico + 10868U (somma delle parti fisse relative ai designatori e al resto della grafica)

extern const uint8_t Isobus_poolWorkingSetDataMask[SIZE_OF_WS_DM];
extern const uint8_t Isobus_poolContainer[SIZE_OF_CONTAINER + 1U];
extern const uint8_t Isobus_poolString[SIZE_OF_OUT_STRINGS + 1U];
extern const uint8_t Isobus_poolRectangle[SIZE_OF_OUT_RECTS + 1U];
extern const uint8_t Isobus_poolEllipse[SIZE_OF_OUT_ELLIPSE + 1U];
extern const uint8_t Isobus_poolHeadEllipse[SIZE_OF_OUT_HEAD_ELLIPSE + 1U];
extern const uint8_t Isobus_poolGraphAttribute[SIZE_OF_GRAPH_ATTRIBUTES + 1U];
extern const uint8_t Isobus_poolAuxInput[SIZE_OF_AUX_INPUT + 1U];
extern const uint8_t Isobus_poolObjPointer[SIZE_OF_OBJ_POINTER + 1U];
extern const uint8_t Isobus_poolPicture256[SIZE_OF_PICTURE_256];
extern const uint8_t Isobus_poolPicture16[SIZE_OF_PICTURE_16];
extern const uint8_t Isobus_poolPictureMono[SIZE_OF_PICTURE_MONO];
extern const uint8_t Isobus_poolOutputNumber[SIZE_OF_OUT_NUM + 1U];
extern const uint8_t Isobus_poolLineBar[SIZE_OF_OUT_LINEBAR + 1U];
extern const uint8_t Isobus_poolHeaderDataMask[120];

extern const uint16_t AUX_INPUT_ID[AUX_INPUT_NUM_MAX];
extern const uint8_t Isobus_poolFail[SIZE_OF_OP_FAIL];

#endif

