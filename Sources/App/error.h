/*
 * error.h
 *
 *  Created on: 22 jun 2018
 *      Author: PoliD
 */

#ifndef ERROR_H
#define ERROR_H

#define ERROR_RESET_IMMEDIATE  true
#define ERROR_RESET_WITH_CNT   false

extern void Error_Update( w_error_entry_t *error, bool error_condition, bool errorResetType );
extern void Error_Reset( w_error_entry_t *error );
extern bool Error_IsActive( w_error_entry_t error );
extern uint8_t Error_OccurrenceGet( w_error_entry_t error );

#endif /*ERROR_H*/
