//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __DCXEVENTTIMER_H__
#define __DCXEVENTTIMER_H__

namespace dcx {

    /**
     * This class manages a start/stop timer that can keep track of scheduled events and, optionally,
     * repeat those events over a period.
     */
    class dcxEventTimer
    {
        /// This value is returned in place of an event key when an event could not be found
        static const int EVENT_NONE = 0xFFFFFFFF;

        /// The data container for the event queue
        typedef std::multimap<double,int> EventMultimap;

        /// Forward event multimap iterator
        typedef EventMultimap::iterator EventIteratorFwd;

        /// Reverse event multimap iterator
        typedef EventMultimap::reverse_iterator EventIteratorBwd;

        public:

            /**
             * Initializes this timer as an aperiodic timer.  Passing a null clock is
             * allowed; the default low-resolution timer will be used instead.
             */
            dcxEventTimer( dcxClock* clock );

            /**
             * Initializes this timer as a periodic timer.  Passing a null clock is
             * allowed; the default low-resolution timer will be used instead.
             */
            dcxEventTimer( dcxClock* clock, double period );

            /**
             * Inserts an event into the timer with the provided parameters.  This can
             * only be done while the timer is stopped; calling this method while the
             * timer is running will stop the timer automatically.  Multiple events
             * can be inserted with the same key, so use eventExists() to determine if
             * a key is already in use if this is not the desired behaviour.
             * If this is a periodic timer, the 'time' value is reduced so that it is
             * within the period.
             * @param key The key of the event to insert
             * @param time The time (in seconds-from-now) at which to insert the event
             */
            void addEvent( int key, double time );

            /**
             * Gets rid of the next event with the specified key, searching forward in time
             */
            bool removeNextEvent( int key );

            /**
             * Gets rid of the last event with the specified key, searching backward in time
             */
            bool removeLastEvent( int key );

            /**
             * Erases all of the events with the specified key
             * @return The number of events removed
             */
            int removeAllEvents( int key );
            
            /**
             * Determines whether or not an event with the given key exists
             * @param key The event key to look for
             * @return Whether or not an event exists with the provided key
             */
            bool eventExists( int key );

            /**
             * Returns the number of seconds from the current time to the event
             * with the given key.  If the event has passed, the return value is
             * negative.  If this is a periodic timer, the distance with the lowest
             * absolute value is returned (including sign).  Calling this method when
             * multiple events have the same key will return the time to the event with
             * the lowest time index.
             * @param key The event key to look for
             * @param time Destination for the difference in times
             * @return Whether or not an event with the given key was found
             */
            bool timeToEvent( int key, double* time );

            /**
             * Returns the number of seconds from the current time to the given
             * time.  If this is a periodic timer, the provided time will be wrapped
             * into a single period, and the returned time distance will be the shorter
             * of the forward and reverse times.
             * All times are returned with a sign indicating the direction.
             */
            double timeToTime( double time );

            /**
             * Returns the event that is next in the event queue.  If there is no event
             * after the current time, or the timer has not been started, this method
             * returns TIMER_EVENT_NONE.
             */
            int nextEvent();

            /**
             * Returns the event that is closest in past-time to the current time.  If
             * there is no event before the current time or the timer has not been
             * started, this method returns TIMER_EVENT_NONE.
             */
            int lastEvent();

            /**
             * Returns the key of the last or next event, whichever is closer in time
             */
            int closestEvent();

            /**
             * Stops the timer and resets it to the starting point
             */
            void stop();

            /**
             * Pauses the timer so that it can be resumed
             */
            void pause();

            /**
             * Starts the timer
             */
            void start();

            /**
             * Asks the timer whether anything has happened.  If an event has been
             * passed since the last call to query() this method returns 'true' and
             * returns the key of the event.  This method should be called multiple
             * times until the return value is 'false' in order to acquire the keys
             * of all of the events triggered.<br/>
             * If this is a periodic timer, query() will trigger whenever the timer
             * passes a given key--even multiple times if query() is called less
             * often than once per period
             */
            bool query( int* key );

            /**
             * How long time timer has been running.  If the timer has not been
             * started, this method returns zero.  If the timer is paused, this
             * this method will return the same value until it is started
             * again.  If this is a periodic timer, the elapsed time value
             * will be wrapped to within a period.
             * @return Time in seconds
             */
            double elapsedTime();


        protected:

            /**
             * Gets the first iterator for backward iteration through the event
             * queue, starting at the current time.
             */
            EventIteratorBwd beginBackward();
            
            /**
             * Returns the next iterator in backward iteration.  When the iteration
             * completes, this returns myEvents.rend();
             */
            EventIteratorBwd incrementBackward( EventIteratorBwd i );

            /**
             * Gets the first iterator for forward iteration through the
             * event queue, starting at the current time.
             */
            EventIteratorFwd beginForward();
            
            /**
             * Returns the next iterator in forward iteration.  If the iteration
             * completes, this returns myEvents.end()
             */
            EventIteratorFwd incrementForward( EventIteratorFwd i );

            /**
             * Gets the current wall time using the internal clock (in seconds)
             */
            double wallTime();


        protected:

            /// The clock this timer uses to keep time
            dcxClock* myClock;
        
            /// If this is positive, the value is the period of the timer.  Otherwise,
            /// this designates that the timer is not periodic.
            double myPeriod;

            /// The time at which the timer was started (if this is not positive, the
            /// timer has not been started)
            double myStartTime;
            
            /// The time at which the timer was paused (if this is not positive, the timer
            /// has not been paused)
            double myPausedTime;

            /// The last event returned
            EventIteratorFwd myLastQueryEvent;

            /// The last time at which an event was returned
            double myLastQueryEventTime;

            /// The events in this timer.  Maps the time at which an event
            /// occurs to its event key.
            EventMultimap myEvents;

            /// Temporary iterators used while searching
            EventIteratorBwd myBwdEnd;
            EventIteratorFwd myFwdEnd;
    };

}


#endif