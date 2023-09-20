/**************************************************************************************************
 * Last revision ID:   $Rev$ 
 * Last change date:   $Date$
 * Last change author: $Author$
 **************************************************************************************************
 *               (C) 2018 E.S.T.E. S.r.l.                                                         *
 *               All rights reserved.                                                             *
 * NOTICE:  All information contained herein is, and remains the property of E.S.T.E. S.r.l. and  *
 * its customers, if any.  The intellectual and technical concepts contained herein are           *
 * proprietary to E.S.T.E. S.r.l. and its customers and may be covered by Italy and Foreign       *
 * Patents, patents in process, and are protected by trade secret or copyright law.               *
 * Dissemination of this information or reproduction of this material is strictly forbidden       *
 * unless prior written permission is obtained from E.S.T.E. S.r.l. or its customers              *
 **************************************************************************************************/
/* @file queue.h
 */
#ifndef QUEUE_H_
#define QUEUE_H_

/**************************************************************************************************
 *  1.  INCLUDE FILES
 **************************************************************************************************/
#include "wtypes.h"

/**************************************************************************************************
 *  2.  MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *  3.  TYPE DEFINITIONS
 **************************************************************************************************/
/*! \defgroup queue_types Tipi
 * \ingroup queue
 * @{
 */

/*!
 * Questa struttura rappresenta l'oggetto FIFO, che viene utilizzato da tutte le funzioni del modulo
 */
 typedef struct
{
    uint8_t     *buffer;    /*!< puntatore all'array di dati fornito dall'utente */
    size_t      out;        /*!< indice della coda, cioe' del primo elemento inserito */
    size_t      in;         /*!< indice della testa, cioe' dell'ultimo elemento inserito*/
    size_t      count;      /*!< numero di elementi nella coda, per vedere se e' piena*/
    size_t      size;       /*!< numero di elementi dell'array di dati*/
    size_t      itemSize;   /*!< dimensione di un elemento dell'array di dati*/
} Queue_t;

/**************************************************************************************************
 *  4.  ENUMERATIONS
 **************************************************************************************************/


/**************************************************************************************************
 *  5.  STRUCTURES
 **************************************************************************************************/


/*! @} */ /* queue_types */
/**************************************************************************************************
 *  6.  CONSTANTS 
 **************************************************************************************************/

/**************************************************************************************************
 *  7.  EXPORTED (EXTERN) VARIABLES
 **************************************************************************************************/

/*! @} */ /* queue */
/**************************************************************************************************
 *  8.  FUNCTION PROTOTYPES
 **************************************************************************************************/
wres_t   Queue_Init(Queue_t* q, void* itemArray, size_t itemSize, size_t nItems);
wres_t   Queue_DeInit(Queue_t* q);
wres_t   Queue_Enqueue(Queue_t* q, const void* item);
wres_t   Queue_Dequeue(Queue_t* q, void* item);

wres_t   Queue_GetElement(Queue_t* q, void* item);

wres_t   Queue_MultiEnqueue(Queue_t* q, const void* item, size_t nItem);
wres_t   Queue_MultiDequeue(Queue_t* q, void* item, size_t nItem);
bool     Queue_IsFull(const Queue_t* q);
bool     Queue_IsEmpty(const Queue_t* q);
wres_t   Queue_GetItemCount(const Queue_t* q, size_t* itemCount);

#endif /* QUEUE_H_ */
/**************************************************************************************************
 *  EOF
 **************************************************************************************************/
