/*
 * error.c
 *
 *  Created on: 22 jun 2018
 *      Author: PoliD
 */

#include "application.h"

//------------------------------------------------------------------------------
void Error_Reset( w_error_entry_t *error )
{
    error->cnt = 0U;
    error->active = false;

    return;
}

//------------------------------------------------------------------------------
void Error_Update( w_error_entry_t *error, bool error_condition, bool errorResetType )
{
    if ( true == error->en )
    {
        if ( ( true == error_condition ) && ( error->cntTh > 0U ) )
        {
            ++error->cnt;
            if ( error->cnt >= error->cntTh )
            {
                error->cnt = error->cntTh;
                if ( false == error->active )
                {
                    ++error->occurrence;
                    if ( error->occurrence > 0x7FU )
                    {
                        error->occurrence = 0x7FU;
                    }
                }
                error->active = true;
            }
        }
        else
        {
            if ( ERROR_RESET_WITH_CNT == errorResetType )
            {
                if ( error->cnt > 0U )
                {
                    error->cnt--;
                }
            }
            else
            {
                error->cnt = 0U;
            }
            if ( 0U == error->cnt )
            {
                error->active = false;
            }
        }
    }
    else
    {
        error->cnt = 0U;
        error->active = false;
    }
    
    return;
}

//------------------------------------------------------------------------------
bool Error_IsActive( w_error_entry_t error )
{
    return error.active;
}

//------------------------------------------------------------------------------
uint8_t Error_OccurrenceGet( w_error_entry_t error )
{
    return error.occurrence;
}

