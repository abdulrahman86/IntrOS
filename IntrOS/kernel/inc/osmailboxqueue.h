/******************************************************************************

    @file    IntrOS: osmailboxqueue.h
    @author  Rajmund Szymanski
    @date    09.06.2020
    @brief   This file contains definitions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __INTROS_BOX_H
#define __INTROS_BOX_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : mailbox queue
 *
 ******************************************************************************/

typedef struct __box box_t, * const box_id;

struct __box
{
	size_t   count; // size of used memory in the mailbox buffer (in bytes)
	size_t   limit; // size of the mailbox buffer (in bytes)
	unsigned size;  // size of a single mail (in bytes)

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	char   * data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _BOX_INIT
 *
 * Description       : create and initialize a mailbox queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *   data            : mailbox queue data buffer
 *
 * Return            : mailbox queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _BOX_INIT( _limit, _size, _data ) { 0, _limit * _size, _size, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _BOX_DATA
 *
 * Description       : create a mailbox queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 * Return            : mailbox queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _BOX_DATA( _limit, _size ) (char[_limit * _size]){ 0 }
#endif

/******************************************************************************
 *
 * Name              : OS_BOX
 *
 * Description       : define and initialize a mailbox queue object
 *
 * Parameters
 *   box             : name of a pointer to mailbox queue object
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 ******************************************************************************/

#define             OS_BOX( box, limit, size )                                \
                       char box##__buf[limit * size];                          \
                       box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                       box_id box = & box##__box

/******************************************************************************
 *
 * Name              : static_BOX
 *
 * Description       : define and initialize a static mailbox queue object
 *
 * Parameters
 *   box             : name of a pointer to mailbox queue object
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 ******************************************************************************/

#define         static_BOX( box, limit, size )                                \
                static char box##__buf[limit * size];                          \
                static box_t box##__box = _BOX_INIT( limit, size, box##__buf ); \
                static box_id box = & box##__box

/******************************************************************************
 *
 * Name              : BOX_INIT
 *
 * Description       : create and initialize a mailbox queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 * Return            : mailbox queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                BOX_INIT( limit, size ) \
                      _BOX_INIT( limit, size, _BOX_DATA( limit, size ) )
#endif

/******************************************************************************
 *
 * Name              : BOX_CREATE
 * Alias             : BOX_NEW
 *
 * Description       : create and initialize a mailbox queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 * Return            : pointer to mailbox queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                BOX_CREATE( limit, size ) \
           (box_t[]) { BOX_INIT  ( limit, size ) }
#define                BOX_NEW \
                       BOX_CREATE
#endif

/******************************************************************************
 *
 * Name              : box_init
 *
 * Description       : initialize a mailbox queue object
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   size            : size of a single mail (in bytes)
 *   data            : mailbox queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 ******************************************************************************/

void box_init( box_t *box, unsigned size, void *data, size_t bufsize );

/******************************************************************************
 *
 * Name              : box_take
 * Alias             : box_tryWait
 *
 * Description       : try to transfer mailbox data from the mailbox queue object,
 *                     don't wait if the mailbox queue object is empty
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   data            : pointer to store mailbox data
 *
 * Return
 *   E_SUCCESS       : mailbox data was successfully transferred from the mailbox queue object
 *   E_FAILURE       : mailbox queue object is empty
 *
 ******************************************************************************/

unsigned box_take( box_t *box, void *data );

__STATIC_INLINE
unsigned box_tryWait( box_t *box, void *data ) { return box_take(box, data); }

/******************************************************************************
 *
 * Name              : box_wait
 *
 * Description       : try to transfer mailbox data from the mailbox queue object,
 *                     wait indefinitely while the mailbox queue object is empty
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   data            : pointer to store mailbox data
 *
 * Return            : none
 *
 ******************************************************************************/

void box_wait( box_t *box, void *data );

/******************************************************************************
 *
 * Name              : box_give
 *
 * Description       : try to transfer mailbox data to the mailbox queue object,
 *                     don't wait if the mailbox queue object is full
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   data            : pointer to mailbox data
 *
 * Return
 *   E_SUCCESS       : mailbox data was successfully transferred to the mailbox queue object
 *   E_FAILURE       : mailbox queue object is full
 *
 ******************************************************************************/

unsigned box_give( box_t *box, const void *data );

/******************************************************************************
 *
 * Name              : box_send
 *
 * Description       : try to transfer mailbox data to the mailbox queue object,
 *                     wait indefinitely while the mailbox queue object is full
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   data            : pointer to mailbox data
 *
 * Return            : none
 *
 ******************************************************************************/

void box_send( box_t *box, const void *data );

/******************************************************************************
 *
 * Name              : box_push
 *
 * Description       : transfer mailbox data to the mailbox queue object,
 *                     remove the oldest mailbox data if the mailbox queue object is full
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *   data            : pointer to mailbox data
 *
 * Return            : none
 *
 ******************************************************************************/

void box_push( box_t *box, const void *data );

/******************************************************************************
 *
 * Name              : box_count
 *
 * Description       : return the amount of data contained in the mailbox queue
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *
 * Return            : amount of data contained in the mailbox queue
 *
 ******************************************************************************/

unsigned box_count( box_t *box );

/******************************************************************************
 *
 * Name              : box_space
 *
 * Description       : return the amount of free space in the mailbox queue
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *
 * Return            : amount of free space in the mailbox queue
 *
 ******************************************************************************/

unsigned box_space( box_t *box );

/******************************************************************************
 *
 * Name              : box_limit
 *
 * Description       : return the size of the mailbox queue
 *
 * Parameters
 *   box             : pointer to mailbox queue object
 *
 * Return            : size of the mailbox queue
 *
 ******************************************************************************/

unsigned box_limit( box_t *box );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : MailBoxQueueT<>
 *
 * Description       : create and initialize a mailbox queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored mails)
 *   size            : size of a single mail (in bytes)
 *
 ******************************************************************************/

template<size_t limit_, unsigned size_>
struct MailBoxQueueT : public __box
{
	constexpr
	MailBoxQueueT( void ): __box _BOX_INIT(limit_, size_, data_) {}

	MailBoxQueueT( MailBoxQueueT&& ) = default;
	MailBoxQueueT( const MailBoxQueueT& ) = delete;
	MailBoxQueueT& operator=( MailBoxQueueT&& ) = delete;
	MailBoxQueueT& operator=( const MailBoxQueueT& ) = delete;

	uint take   (       void *_data ) { return box_take   (this, _data); }
	uint tryWait(       void *_data ) { return box_tryWait(this, _data); }
	void wait   (       void *_data ) {        box_wait   (this, _data); }
	uint give   ( const void *_data ) { return box_give   (this, _data); }
	void send   ( const void *_data ) {        box_send   (this, _data); }
	void push   ( const void *_data ) {        box_push   (this, _data); }
	uint count  (       void )        { return box_count  (this); }
	uint space  (       void )        { return box_space  (this); }
	uint limit  (       void )        { return box_limit  (this); }

	private:
	char data_[limit_ * size_];
};

/******************************************************************************
 *
 * Class             : MailBoxQueueTT<>
 *
 * Description       : create and initialize a mailbox queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored mails)
 *   C               : class of a single mail
 *
 ******************************************************************************/

template<size_t limit_, class C>
using MailBoxQueueTT = MailBoxQueueT<limit_, sizeof(C)>;

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_BOX_H
