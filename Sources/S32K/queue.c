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
 **************************************************************************************************
 * \file queue.c
 * History:
 *  - 14 mar 2018  2.0 dian - Inserita gestione del NULL, e dei risultati wres_t
 * queue.c Description
 **************************************************************************************************/

/**************************************************************************************************
 * 	1.	PRIVATE INCLUDE FILES
 **************************************************************************************************/
#include "queue.h"
#include <string.h>
/**************************************************************************************************
 * 	2.	PRIVATE MACROS
 **************************************************************************************************/

/**************************************************************************************************
 * 	3.	PRIVATE TYPE DEFINITIONS
 **************************************************************************************************/

/**************************************************************************************************
 * 	4.	PRIVATE ENUMERATIONS
 **************************************************************************************************/

/**************************************************************************************************
 * 	5.	PRIVATE STRUCTURES
 **************************************************************************************************/

/**************************************************************************************************
 * 	6.	IMPORTED (EXTERN) VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 * 	7.	PRIVATE VARIABLES 
 **************************************************************************************************/

/**************************************************************************************************
 * 	8.	PRIVATE (STATIC) FUNCTION PROTOTYPES
 **************************************************************************************************/

/**************************************************************************************************
 * 	9.	FUNCTION DEFINITIONS 
 **************************************************************************************************/
/*! \defgroup queue_func Funzioni
 * \ingroup queue
 * @{
 */

/*!
 * \brief Funzione di inizializzazione (costruttore)
 *
 * Inizializza l'oggetto coda. Deve essere chiamata prima di qualsiasi altra funzione. Chiamarla su
 * un oggetto gia' istanziato provoca la reinizializzazione
 *
 * \note Per motivi di performance il controllo sugli argomenti e' demandato ad ASSERT, pertanto
 *       ritorna sempre TRUE o solleva un'eccezione da ASSERT
 *
 * \param[in]   q        = puntatore all'oggetto coda
 * \param[in]   buffer   = puntatore all'area di dati allocata dall'utente
 * \param[in]   sizeItem = dimensione di un elemento della coda in byte
 * \param[in]   nItems   = numero di elementi della coda
 *
 * \retval      TRUE     = Coda inizializzata correttamente.
 *
 */
wres_t Queue_Init(Queue_t* q, void* itemArray, size_t itemSize, size_t nItems)
{
    wres_t res;
    if ((NULL == q) || (NULL == itemArray) || (itemSize == 0U) || (nItems == 0U))
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else
    {
        (void) memset(q, 0, sizeof(Queue_t));
        q->buffer = (uint8_t*) itemArray;
        q->itemSize = itemSize;
        q->size = nItems;
        res = WRES_OK;
    }
    return res;
}

/*!
 * \brief Funzione di de-inizialzzazione (distruttore)
 *
 * Elimina l'oggetto coda, in modo che non possa essere piu' utilizzato, se non dopo una chiamata a
 * Queue_Init()
 *
 * \param[in]   q        = puntatore all'oggetto coda
 *
 */
wres_t Queue_DeInit(Queue_t* q)
{
    wres_t res;
    if (NULL == q)
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else
    {
        q->size = 0U;
        res = WRES_OK;
    }
    return res;
}

/*!
 * \brief Funzione di inserimento elemento
 *
 * Inserisce un elemento in testa alla coda, se non e' piena.
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in]   q        = puntatore all'oggetto coda
 * \param[in]   item     = puntatore all'elemento da inserire
 *
 * \retval    TRUE     = elemento inserito
 * \retval    FALSE    = elemento non inserito, coda piena
 */
wres_t Queue_Enqueue(Queue_t* q, const void* item)
{
   wres_t res;
   if ((NULL == q) || (NULL == item))
   {
       res = WRES_ERROR_INVALID_PARAMETER;
   }
   else if (0u == q->size)
   {
       res = WRES_ERROR_UNINITIALIZED;
   }
   else if (q->count == q->size)
   {
       res = WRES_BUFFER_FULL;
   }
   else
   {
       (void)memcpy(&q->buffer[q->itemSize * q->in], item, q->itemSize);
       q->in = (size_t)((q->in + 1U) % q->size);
       ++q->count;
       res = WRES_OK;
   }
   return res;
}

/*!
 * \brief Funzione di estrazione elemento
 *
 * Estrae un elemento dalla coda, se non e' vuota.
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in,out]   q     = puntatore all'oggetto coda
 * \param[out]   item     = puntatore all'elemento da estrarre
 *
 * \retval    TRUE     = elemento estratto correttamente
 * \retval    FALSE    = elemento non estratto, coda vuota, item non e' stato modificato
 */
wres_t Queue_Dequeue(Queue_t* q, void* item)
{
    wres_t res;
    if ((NULL == q) || (NULL == item))
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else if (0u == q->size)
    {
        res = WRES_ERROR_UNINITIALIZED;
    }
    else if (q->count == 0u)
    {
        res = WRES_BUFFER_EMPTY;
    }
    else
    {
        (void)memcpy(item, &q->buffer[q->itemSize * q->out], q->itemSize);
        q->out = ((uint16_t)(q->out + 1U) % q->size);
        --q->count;
        res = WRES_OK;
    }
    return res;
}

/*!
 * \brief Funzione di letture ultimo elemento
 *
 * Legge un elemento dalla coda, se non e' vuota.
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in,out]   q     = puntatore all'oggetto coda
 * \param[out]   item     = puntatore all'elemento da Leggere
 *
 * \retval    TRUE     = elemento letto correttamente
 * \retval    FALSE    = elemento non letto, coda vuota, item non e' stato modificato
 */
wres_t Queue_GetElement(Queue_t* q, void* item)
{
    wres_t res;
    if ((NULL == q) || (NULL == item))
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else if (0u == q->size)
    {
        res = WRES_ERROR_UNINITIALIZED;
    }
    else if (q->count == 0u)
    {
        res = WRES_BUFFER_EMPTY;
    }
    else
    {
        (void)memcpy(item, &q->buffer[q->itemSize * q->out], q->itemSize);
        //q->out = ((uint16_t)(q->out + 1U) % q->size);
        //--q->count;
        res = WRES_OK;
    }
    return res;
}
wres_t Queue_MultiEnqueue(Queue_t* q, const void* item, size_t nItem)
{
   wres_t res;
   if ((NULL == q) || (NULL == item))
   {
       res = WRES_ERROR_INVALID_PARAMETER;
   }
   else if (0u == q->size)
   {
       res = WRES_ERROR_UNINITIALIZED;
   }
   else if (nItem > q->size)
   {
       res = WRES_ERROR_INVALID_PARAMETER;
   }
   else if (q->count + nItem > q->size)
   {
       res = WRES_BUFFER_FULL;
   }
   else
   {
       size_t itemsToCopy = nItem;
       const uint8_t* srcBuff = (const uint8_t*)item;
       if (q->size - q->in < itemsToCopy)
       {
           (void)memcpy(&q->buffer[q->itemSize * q->in], srcBuff, q->itemSize* (q->size - q->in));
           srcBuff += (q->itemSize* (q->size - q->in));
           itemsToCopy = itemsToCopy - (q->size - q->in);
           q->in = 0U;
       }
       (void)memcpy(&q->buffer[q->itemSize * q->in], srcBuff, q->itemSize * itemsToCopy);
       q->in = itemsToCopy;
       q->count += nItem;
       res = WRES_OK;
   }
   return res;
}

wres_t Queue_MultiDequeue(Queue_t* q, void* item, size_t nItem)
{
    wres_t res;
    if ((NULL == q) || (NULL == item))
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else if (0u == q->size)
    {
        res = WRES_ERROR_UNINITIALIZED;
    }
    else if (nItem > q->size)
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else if (nItem > q->count)
    {
        res = WRES_BUFFER_EMPTY;
    }
    else
    {
        size_t itemsToCopy = nItem;
        uint8_t* srcBuff = (uint8_t*)item;
        if (q->size - q->out < itemsToCopy)
        {
            (void)memcpy(srcBuff, &q->buffer[q->itemSize * q->out], q->itemSize* (q->size - q->out));
            srcBuff += (q->itemSize* (q->size - q->out));
            itemsToCopy = itemsToCopy - (q->size - q->out);
            q->out = 0U;
        }
        (void)memcpy( srcBuff, &q->buffer[q->itemSize * q->out], q->itemSize * itemsToCopy);
        q->in = itemsToCopy;
        q->count -= nItem;
        res = WRES_OK;
    }
    return res;
}

/*!
 * \brief Funzione getter stato della coda piena
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in]   q        = puntatore all'oggetto coda
 *
 * \retval    TRUE     = coda piena
 * \retval    FALSE    = coda non piena
 */
bool Queue_IsFull(const Queue_t* q)
{
    bool result;
    if ((NULL != q) && (q->size != 0u))
    {
        if (q->count == q->size)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }
    return result;
}

/*!
 * \brief Funzione getter stato della coda vuota
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in]   q        = puntatore all'oggetto coda
 *
 * \retval    TRUE     = coda vuota
 * \retval    FALSE    = coda non vuota
 */
bool Queue_IsEmpty(const Queue_t* q)
{
    bool result;
    if ((NULL != q) && (q->size != 0u))
    {
        if (q->count == 0U)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else
    {
        result = false;
    }
    return result;
}

/*!
 * \brief Funzione getter del numero degli elementi nella coda
 *
 * Estrae un elemento dalla coda, se non e' vuota.
 *
 * \note Per motivi di performance il controllo di coerenza sugli argomenti e' demandato ad ASSERT,
 *       pertanto l'utilizzo di argomenti non corretti o su code non inizializzate solleva
 *       un'eccezione
 *
 * \param[in]  q     = puntatore all'oggetto coda
 *
 * \return     queueCount = numero di elementi presenti nella coda
 */
wres_t Queue_GetItemCount(const Queue_t* q, size_t* itemCount)
{
    wres_t res;
    if ((NULL == q) || (NULL == itemCount))
    {
        res = WRES_ERROR_INVALID_PARAMETER;
    }
    else if (0u == q->size)
    {
        res = WRES_ERROR_UNINITIALIZED;
    }
    else
    {
        *itemCount = q->count;
        res = WRES_OK;
    }
    return res;
}
/**************************************************************************************************
 * 	10.	PRIVATE (STATIC) FUNCTION DEFINITIONS
 **************************************************************************************************/

/*! @} */
/**************************************************************************************************
 * 	EOF
 **************************************************************************************************/
