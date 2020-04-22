/******************************************************************************

    @file    IntrOS: ostimer.h
    @author  Rajmund Szymanski
    @date    22.04.2020
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

#ifndef __INTROS_TMR_H
#define __INTROS_TMR_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : timer
 *
 ******************************************************************************/

struct __tmr
{
	hdr_t    hdr;   // timer / task header

	fun_t  * state; // callback procedure
#if OS_FUNCTIONAL
	FUN_t    fun;   // function<void(void)> for internal use in c++ functions
#ifdef     __cplusplus
#define     _FUN_INIT(_state) (fun_t*)_state, { FUN_t() }
#else
#define     _FUN_INIT(_state) (fun_t*)_state, { NULL }
#endif
#else
#define     _FUN_INIT(_state) (fun_t*)_state
#endif
	cnt_t    start;
	cnt_t    delay;
	cnt_t    period;

	unsigned signal;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _TMR_INIT( _state ) { _HDR_INIT(), _FUN_INIT(_state), 0, 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_TMR
 *
 * Description       : define and initialize a timer object
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define             OS_TMR( tmr, state )                     \
                       tmr_t tmr##__tmr = _TMR_INIT( state ); \
                       tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : OS_TMR_DEF
 *
 * Description       : define and initialize complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define             OS_TMR_DEF( tmr )                            \
                       void tmr##__fun( void );                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                       tmr_id tmr = & tmr##__tmr;                   \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_START
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_START( tmr, delay, period )                          \
                       void tmr##__fun( void );                                  \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                       tmr_id tmr = & tmr##__tmr;                                  \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_UNTIL
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_UNTIL( tmr, time )                               \
                       void tmr##__fun( void );                              \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                       tmr_id tmr = & tmr##__tmr;                              \
         __CONSTRUCTOR void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define         static_TMR( tmr, state )                     \
                static tmr_t tmr##__tmr = _TMR_INIT( state ); \
                static tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : static_TMR_DEF
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define         static_TMR_DEF( tmr )                            \
                static void tmr##__fun( void );                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                static tmr_id tmr = & tmr##__tmr;                   \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_START
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_START( tmr, delay, period )                          \
                static void tmr##__fun( void );                                  \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                \
                static tmr_id tmr = & tmr##__tmr;                                  \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_start(tmr, delay, period); } \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_UNTIL
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_UNTIL( tmr, time )                               \
                static void tmr##__fun( void );                              \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );            \
                static tmr_id tmr = & tmr##__tmr;                              \
  __CONSTRUCTOR static void tmr##__start( void ) { tmr_startUntil(tmr, time); } \
                static  void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_INIT( state ) \
                      _TMR_INIT( state )
#endif

/******************************************************************************
 *
 * Name              : TMR_CREATE
 * Alias             : TMR_NEW
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : pointer to timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE( state ) \
           (tmr_t[]) { TMR_INIT  ( state ) }
#define                TMR_NEW \
                       TMR_CREATE
#endif

/******************************************************************************
 *
 * Name              : tmr_this
 *
 * Description       : return current timer object
 *
 * Parameters        : none
 *
 * Return            : current timer object
 *
 ******************************************************************************/

__STATIC_INLINE
tmr_t *tmr_this( void ) { return (tmr_t *) System.cur; }

/******************************************************************************
 *
 * Name              : tmr_init
 *
 * Description       : initialize a timer object
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_init( tmr_t *tmr, fun_t *state );

/******************************************************************************
 *
 * Name              : tmr_start
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period );

/******************************************************************************
 *
 * Name              : tmr_startFor
 *
 * Description       : start/restart one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startFor( tmr_t *tmr, cnt_t delay ) { tmr_start(tmr, delay, 0); }

/******************************************************************************
 *
 * Name              : tmr_startPeriodic
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startPeriodic( tmr_t *tmr, cnt_t period ) { tmr_start(tmr, period, period); }

/******************************************************************************
 *
 * Name              : tmr_startFrom
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   proc            : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc );

/******************************************************************************
 *
 * Name              : tmr_startNext
 *
 * Description       : restart the timer for given duration of time from the end of the previous countdown
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startNext( tmr_t *tmr, cnt_t delay );

/******************************************************************************
 *
 * Name              : tmr_startUntil
 *
 * Description       : start/restart one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   time            : timepoint value
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_startUntil( tmr_t *tmr, cnt_t time );

/******************************************************************************
 *
 * Name              : tmr_take
 * Alias             : tmr_tryWait
 *
 * Description       : check if the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer object has not yet completed counting
 *
 ******************************************************************************/

unsigned tmr_take( tmr_t *tmr );

__STATIC_INLINE
unsigned tmr_tryWait( tmr_t *tmr ) { return tmr_take(tmr); }

/******************************************************************************
 *
 * Name              : tmr_wait
 *
 * Description       : wait indefinitely until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return            : none
 *
 ******************************************************************************/

void tmr_wait( tmr_t *tmr );

/******************************************************************************
 *
 * Name              : tmr_flip
 *
 * Description       : change callback procedure for current periodic timer (available in next period)
 *
 * Parameters
 *   proc            : new callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 * Note              : use only in timer's callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_flip( fun_t *proc ) { tmr_this()->state = proc; }

/******************************************************************************
 *
 * Name              : tmr_delay
 *
 * Description       : change delay time for current timer
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     0:         current timer is stopped even if it was periodic
 *                     otherwise: current timer is restarted even if it was one-shot
 *
 * Return            : none
 *
 * Note              : use only in timer's callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_delay( cnt_t delay ) { tmr_this()->delay = delay; }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Timer
 *
 * Description       : create and initialize a timer object
 *
 * Constructor parameters
 *   state           : callback procedure
 *
 ******************************************************************************/

struct Timer : public __tmr
{
	Timer( void ): __tmr _TMR_INIT(NULL) {}
#if OS_FUNCTIONAL
	Timer( FUN_t _state ): __tmr _TMR_INIT(fun_) { __tmr::fun = _state; }
#else
	Timer( FUN_t _state ): __tmr _TMR_INIT(_state) {}
#endif
	Timer( Timer&& ) = default;
	Timer( const Timer& ) = delete;
	Timer& operator=( Timer&& ) = delete;
	Timer& operator=( const Timer& ) = delete;

	~Timer( void ) { assert(__tmr::hdr.id == ID_STOPPED); }

	void start        ( cnt_t _delay, cnt_t _period )               {        tmr_start        (this, _delay, _period);         }
	void startFor     ( cnt_t _delay )                              {        tmr_startFor     (this, _delay);                  }
	void startPeriodic( cnt_t _period )                             {        tmr_startPeriodic(this,         _period);         }
#if OS_FUNCTIONAL
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        __tmr::fun = _state;
	                                                                         tmr_startFrom    (this, _delay, _period, fun_);   }
#else
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        tmr_startFrom    (this, _delay, _period, _state); }
#endif
	void startNext    ( cnt_t _delay )                              {        tmr_startNext    (this, _delay);                  }
	void startUntil   ( cnt_t _time )                               {        tmr_startUntil   (this, _time);                   }

	unsigned take     ( void )                                      { return tmr_take         (this);                          }
	unsigned tryWait  ( void )                                      { return tmr_tryWait      (this);                          }
	void     wait     ( void )                                      {        tmr_wait         (this);                          }

	bool     operator!( void )                                      { return __tmr::hdr.id == ID_STOPPED;                      }
#if OS_FUNCTIONAL
	static
	void     fun_     ( void )                                      {        tmr_this()->fun();                                }
#endif
};

/******************************************************************************
 *
 * Class             : startTimer
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimer : public Timer
{
	startTimer( const cnt_t _delay, const cnt_t _period ):               Timer()       { tmr_start(this, _delay, _period); }
	startTimer( const cnt_t _delay, const cnt_t _period, FUN_t _state ): Timer(_state) { tmr_start(this, _delay, _period); }
};

/******************************************************************************
 *
 * Class             : startTimerFor
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerFor : public startTimer
{
	startTimerFor( const cnt_t _delay ):               startTimer(_delay, 0)         {}
	startTimerFor( const cnt_t _delay, FUN_t _state ): startTimer(_delay, 0, _state) {}
};

/******************************************************************************
 *
 * Class             : startTimerPeriodic
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Constructor parameters
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerPeriodic : public startTimer
{
	startTimerPeriodic( const cnt_t _period ):               startTimer(_period, _period)         {}
	startTimerPeriodic( const cnt_t _period, FUN_t _state ): startTimer(_period, _period, _state) {}
};

/******************************************************************************
 *
 * Class             : startTimerUntil
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Constructor parameters
 *   time            : timepoint value
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerUntil : public Timer
{
	startTimerUntil( const cnt_t _time ):               Timer()       { tmr_startUntil(this, _time); }
	startTimerUntil( const cnt_t _time, FUN_t _state ): Timer(_state) { tmr_startUntil(this, _time); }
};

/******************************************************************************
 *
 * Namespace         : ThisTimer
 *
 * Description       : provide set of functions for current timer
 *
 ******************************************************************************/

namespace ThisTimer
{
#if OS_FUNCTIONAL
	static inline void flip ( FUN_t _state ) { tmr_this()->fun = _state;
	                                           tmr_flip (Timer::fun_); }
#else
	static inline void flip ( FUN_t _state ) { tmr_flip (_state); }
#endif
	static inline void delay( cnt_t _delay ) { tmr_delay(_delay); }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__INTROS_TMR_H
