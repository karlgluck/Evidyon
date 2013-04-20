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
#include "../dcx/eventtimer"
#include "../dc/debug"
#include <math.h>


using namespace dcx;

//-----------------------------------------------------------------------------
// Name:  dcxEventTimer
// Desc:  Initializes this timer as an aperiodic timer
//-----------------------------------------------------------------------------
dcxEventTimer::dcxEventTimer( dcxClock* clock )
{
    myClock = clock;
    myPeriod = -1.0;
    myStartTime = -1.0;
    myPausedTime = -1.0;
    myLastQueryEvent = myEvents.end();
    myLastQueryEventTime = 0.0;
    myBwdEnd = myEvents.rend();
    myFwdEnd = myEvents.end();

}




//-----------------------------------------------------------------------------
// Name:  dcxEventTimer
// Desc:  Initializes this timer as a periodic timer
//-----------------------------------------------------------------------------
dcxEventTimer::dcxEventTimer( dcxClock* clock, double period )
{
    myClock = clock;
    myPeriod = fabs( period );
    myStartTime = -1.0;
    myPausedTime = -1.0;
    myLastQueryEvent = myEvents.end();
    myLastQueryEventTime = 0.0;
    myBwdEnd = myEvents.rend();
    myFwdEnd = myEvents.end();
}



//-----------------------------------------------------------------------------
// Name:  addEvent
// Desc:  Inserts an event into the timer with the provided parameters
//-----------------------------------------------------------------------------
void dcxEventTimer::addEvent( int key, double time )
{
    // Reset the timer
    myStartTime = -1.0;
    myPausedTime = -1.0;

    // Make sure the time is within the period
    if( myPeriod > 0.0 ) time = fmod( time, myPeriod );

    // Add to the event list
    myEvents.insert( EventMultimap::value_type( time, key ) );

    // Reset the query event
    myLastQueryEvent = myEvents.begin();
}




//-----------------------------------------------------------------------------
// Name:  removeNextEvent
// Desc:  Gets rid of the next event with the specified key, searching forward in time
//-----------------------------------------------------------------------------
bool dcxEventTimer::removeNextEvent( int key )
{
    // Go through all of the events in the list
    for( EventIteratorFwd i = beginForward(); i != myEvents.end(); i = incrementForward( i ) )
    {
        // Check to see if the keys match
        if( i->second == key )
        {
            // Get rid of the event
            myEvents.erase( i );

            // We removed an event
            return true;
        }
    }

    // Couldn't find an event
    return false;
}




//-----------------------------------------------------------------------------
// Name:  removeLastEvent
// Desc:  Gets rid of the last event with the specified key, searching backward in time
//-----------------------------------------------------------------------------
bool dcxEventTimer::removeLastEvent( int key )
{
    // Go through all of the events in the list, backward
    for( EventIteratorBwd i = beginBackward(); i != myEvents.rend(); i = incrementBackward( i ) )
    {
        // Check to see if the keys match
        if( i->second == key )
        {
            // Get rid of the event
//            myEvents.erase( i );
            DEBUG_ERROR( "Add backward erasing..." );

            // We removed an event
            return true;
        }
    }

    // Couldn't find an event
    return false;
}




//-----------------------------------------------------------------------------
// Name:  removeAllEvents
// Desc:  Erases all of the events with the specified key
//-----------------------------------------------------------------------------
int dcxEventTimer::removeAllEvents( int key )
{
    // Keeps track of the number of events that have been removed
    int removedEvents = 0;

    // Go through all of the events in the event multimap
    for( EventIteratorFwd i = myEvents.begin(); i != myEvents.end(); )
    {
        // Check to see if the keys match
        if( i->second == key )
        {
            // Get the next element in the map
            EventIteratorFwd j = i; ++j;

            // Get rid of the event
            myEvents.erase( i );

            // Add to the number of removed events
            removedEvents++;

            // Advance past the iterator that was removed
            i = j;
        }
        else
        {
            ++i;
        }
    }

    // Return the number of events we removed
    return removedEvents;
}




//-----------------------------------------------------------------------------
// Name:  eventExists
// Desc:  Determines whether or not an event with the given key exists
//-----------------------------------------------------------------------------
bool dcxEventTimer::eventExists( int key )
{
    // Go through all of the events in the list, backward
    for( EventIteratorFwd i = myEvents.begin(); i != myEvents.end(); )
    {
        // Check to see if the keys match
        if( i->second == key ) return true;
    }

    // The event wasn't found
    return false;
}




//-----------------------------------------------------------------------------
// Name:  timeToEvent
// Desc:  Returns the number of milliseconds from the current time to the event
//-----------------------------------------------------------------------------
bool dcxEventTimer::timeToEvent( int key, double* time )
{
    // If the time parameter is invalid, don't do anything
    if( APP_ERROR( !time )( "'time' parameter to timeToEvent() is invalid" ) ) return false;

    // Go through all of the events in the list and find the key
    for( EventIteratorFwd i = myEvents.begin(); i != myEvents.end(); ++i )
    {
        // Check to see if the keys match
        if( i->second == key )
        {
            // Save the time delta
            *time = timeToTime( i->first );

            // Success
            return true;
        }
    }

    // The event wasn't found
    return false;
}


//-----------------------------------------------------------------------------
// Name:  timeToTime
// Desc:  Returns the number of milliseconds from the current time to the given time
//-----------------------------------------------------------------------------
double dcxEventTimer::timeToTime( double time )
{
    double currentTime = elapsedTime();

    // Depending on whether or not this is a periodic timer, get the distance to the event
    if( myPeriod > 0 )
    {
        // Get the time difference in the backward direction by adding a period and subtracting
        double timeDifferenceBwd = fmod( (currentTime + myPeriod) - time, myPeriod);

        // The time difference in the forward direction is just the periodic complement
        double timeDifferenceFwd = myPeriod - timeDifferenceBwd;

        // Return the minimal time, with sign
        return timeDifferenceFwd < timeDifferenceBwd ? +timeDifferenceFwd : -timeDifferenceBwd;
    }
    else
    {
        // Return the distance to the event
        return time - currentTime;
    }
}




//-----------------------------------------------------------------------------
// Name:  nextEvent
// Desc:  Returns the event that is next in the event queue
//-----------------------------------------------------------------------------
int dcxEventTimer::nextEvent()
{
    EventIteratorFwd nextEventIter = beginForward();
    return nextEventIter == myEvents.end() ? EVENT_NONE : nextEventIter->second;
}




//-----------------------------------------------------------------------------
// Name:  lastEvent
// Desc:  Returns the event that is closest in past-time to the current time.  If
//-----------------------------------------------------------------------------
int dcxEventTimer::lastEvent()
{
    EventIteratorBwd lastEventIter = beginBackward();
    return lastEventIter == myEvents.rend() ? EVENT_NONE : lastEventIter->second;
}




//-----------------------------------------------------------------------------
// Name:  closestEvent
// Desc:  Returns the key of the last or next event, whichever is closer in time
//-----------------------------------------------------------------------------
int dcxEventTimer::closestEvent()
{
    EventIteratorFwd nextEventIter = beginForward();
    EventIteratorBwd lastEventIter = beginBackward();

    // Check the next event to see if it's valid
    if( nextEventIter == myEvents.end() )
    {
        // Check the last event to see if it's valid
        if( lastEventIter == myEvents.rend() )
        {
            // Neither direction is valid--there are no events!
            return EVENT_NONE;
        }
        else
        {
            // Only the last event is valid, so it must be the closest event
            return lastEventIter->second;
        }
    }
    else
    {
        // Check the last event to see if it is valid
        if( lastEventIter == myEvents.rend() )
        {
            // Only the next event is valid, so return it
            return nextEventIter->second;
        }
        else
        {
            // Both events are valid, so determine which is closer
            double distLast = timeToTime( lastEventIter->first );
            double distNext = timeToTime( nextEventIter->first );

            // Return the closer of the two events
            return (fabs(distLast) > fabs(distNext)) ? nextEventIter->second : lastEventIter->second;
        }
    }
}




//-----------------------------------------------------------------------------
// Name:  stop
// Desc:  Stops the timer and resets it to the starting point
//-----------------------------------------------------------------------------
void dcxEventTimer::stop()
{
    myStartTime = -1.0;
    myPausedTime = -1.0;
}




//-----------------------------------------------------------------------------
// Name:  pause
// Desc:  Pauses the timer so that it can be resumed
//-----------------------------------------------------------------------------
void dcxEventTimer::pause()
{
    myPausedTime = wallTime() - myStartTime;
}




//-----------------------------------------------------------------------------
// Name:  start
// Desc:  Starts the timer
//-----------------------------------------------------------------------------
void dcxEventTimer::start()
{
    if( myPausedTime > 0.0 )
    {
        // Increment the start time so as to remove the amount of time the timer was paused
        myStartTime += wallTime() - myPausedTime;

        // Reset the paused time to remove pausing
        myPausedTime = -1.0;
    }
    else
        myStartTime = wallTime();
}




//-----------------------------------------------------------------------------
// Name:  query
// Desc:  Asks the timer whether anything has happened
//-----------------------------------------------------------------------------
bool dcxEventTimer::query( int* key )
{
    // Check the parameter
    if( APP_ERROR( !key )( "Invalid key address provided to dcxEventTimer::query()" ) ) return false;

    // If there are no events, exit
    if( myEvents.empty() )
        return false;

    // Get the elapsed time
    double queryTime = elapsedTime();

    // Whether or not we found an event
    bool eventReturned = false;

    // Check to see if we've reached the end of the list
    if( myLastQueryEvent == myEvents.end() )
    {
        // If this is a periodic timer, wrap to the beginning again by setting the iterator
        // to the start when the clock rolls over
        if( myPeriod > 0.0 && queryTime < myLastQueryEventTime )
        {
            // Roll the clock back
            myLastQueryEventTime = 0.0;

            // Reset this event
            myLastQueryEvent = myEvents.begin();
        }
    }
    else
    {
        // If this event time has passed, we need to return this new event.
        if( myLastQueryEvent->first <= queryTime )
        {
            // Store the key from the event
            *key = myLastQueryEvent->second;

            // Advance the event monitor
            myLastQueryEvent++;

            // Success
            eventReturned = true;
        }
    }

    // Save the time of this query
    myLastQueryEventTime = queryTime;

    // Pass back the event flag
    return eventReturned;
}




//-----------------------------------------------------------------------------
// Name:  elapsedTime
// Desc:  How long it has been since the timer was started
//-----------------------------------------------------------------------------
double dcxEventTimer::elapsedTime()
{
    // Get the current time
    double time = myStartTime > 0 ? wallTime() - myStartTime : 0.0;
    if( myPausedTime > 0.0 ) time = myPausedTime;

    // Wrap the time into one period and return
    return (myPeriod > 0.0) ? fmod(time, myPeriod) : time;
}





//-----------------------------------------------------------------------------
// Name:  beginBackward
// Desc:  Gets the first iterator for backward iteration through the event
//-----------------------------------------------------------------------------
dcxEventTimer::EventIteratorBwd dcxEventTimer::beginBackward()
{
    // Make sure the list isn't empty
    if( myEvents.empty() )
        return myEvents.rend();

    EventIteratorBwd begin;
    double time = elapsedTime();

    // Look backward in the list until we find an event lower than the current time
    for( begin = myEvents.rbegin(); begin != myEvents.rend() && begin->first > time; ++begin );

    // If this is a periodic timer, wrap the start iterator and make the iteration end where it
    // begins (the incrementing method will wrap over the end of the list)
    if( myPeriod > 0 )
    {
        if( begin == myEvents.rend() )
            begin = myEvents.rbegin();

        // End at the start
        myBwdEnd = begin;
    }
    else
    {
        // For a nonperiodic timer, the end of the list is just the normal end
        myBwdEnd = myEvents.rend();
    }

    // Return the starting iterator
    return begin;
}




//-----------------------------------------------------------------------------
// Name:  incrementBackward
// Desc:  Returns the next iterator in backward iteration
//-----------------------------------------------------------------------------
dcxEventTimer::EventIteratorBwd dcxEventTimer::incrementBackward( EventIteratorBwd i )
{
    // If the iterator is invalid, this method can't do anything
    if( i == myEvents.rend() )
        return i;

    // Increment the iterator (move it backward in the list)
    i++;

    // Check to see if the iterator matches the backward-end iterator
    if( myBwdEnd == i )
        return myEvents.rend();
    else
    {
        // Wrap the iterator if this is a periodic timer
        if( myPeriod > 0 && (i == myEvents.rend()) )
            return myEvents.rbegin();
        else
            return i;
    }
}




//-----------------------------------------------------------------------------
// Name:  beginForward
// Desc:  Gets the first iterator for forward iteration through the
//-----------------------------------------------------------------------------
dcxEventTimer::EventIteratorFwd dcxEventTimer::beginForward()
{
    // Make sure the list isn't empty
    if( myEvents.empty() )
        return myEvents.end();

    EventIteratorFwd begin;
    double time = elapsedTime();

    // Look backward in the list until we find an event higher than the current time
    for( begin = myEvents.begin(); begin != myEvents.end() && begin->first < time; ++begin );

    // If this is a periodic timer, wrap the begin iterator and make the iteration end where it
    // starts (the incrementing method will wrap over the end of the list)
    if( myPeriod > 0 )
    {
        if( begin == myEvents.end() )
            begin = myEvents.begin();

        // End at the start
        myFwdEnd = begin;
    }
    else
    {
        // For a nonperiodic timer, the end of the list is just the normal end
        myFwdEnd = myEvents.end();
    }

    // Return the starting iterator
    return begin;
}



//-----------------------------------------------------------------------------
// Name:  incrementForward
// Desc:  Returns the next iterator in forward iteration
//-----------------------------------------------------------------------------
dcxEventTimer::EventIteratorFwd dcxEventTimer::incrementForward( EventIteratorFwd i )
{
    // If the iterator is invalid, this method can't do anything
    if( i == myEvents.end() ) return i;

    // Increment the iterator (move it backward in the list)
    i++;

    // Check to see if the iterator matches the backward-end iterator
    if( myFwdEnd == i )
        return myEvents.end();
    else
    {
        // Wrap the iterator if this is a periodic timer
        if( myPeriod > 0 && (i == myEvents.end()) )
            return myEvents.begin();
        else
            return i;
    }
}



//-----------------------------------------------------------------------------
// Name:  wallTime
// Desc:  Gets the current wall time using the internal clock
//-----------------------------------------------------------------------------
double dcxEventTimer::wallTime()
{
    return myClock ? myClock->getWallTime() : dcxClock::getSystemTime() * 1000.0;
}