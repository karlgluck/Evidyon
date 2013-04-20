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
//-----------------------------------------------------------------------------
// File:    dcxstatemachine.h
// Desc:    Provides a simple interface for controlling a state machine based on an enum
//-----------------------------------------------------------------------------
#ifndef __DCXSTATEMACHINE_H__
#define __DCXSTATEMACHINE_H__


namespace dcx {

    /**
     * This class is used to control a finite state machine that can be in a state that is
     * pulled from the enumeration that is passed as the template parameter.
     */
    template <typename T> class dcxStateMachine
    {
        /// The enumeration containing the state-space of this machine
        typedef T State;

        /// List of states to be executed
        typedef std::deque<State> Queue;

        public:

            /**
             * Initializes the state manager.  The advance() method should be called
             * to move into the startup state, otherwise the current state is
             * undefined.
             */
            dcxStateMachine( State startupState )
            {
                myCurrentState = (State)0xFFFFFFFF;
                myNextStates.push_back( startupState );
            }

            /**
             * Gets the state in which 
             */
            State currentState()
            {
                return myCurrentState;
            }

            /**
             * Erases all of the states currently in the machine.  If no state is
             * added to the system, tihs will cause the next call to advance() to
             * return false.
             */
            void clear()
            {
                myNextStates.clear();
            }

            /**
             * Moves the next state in the queue into the current state.  If there are
             * no states in the queue, this method returns 'false' and the current
             * state remains the same.
             */
            bool advance()
            {
                // If there are no available states, exit
                if( myNextStates.empty() ) return false;

                // Grab the next state
                myCurrentState = myNextStates.front();
                myNextStates.pop_front();

                // Success
                return true;
            }

            /**
             * The provided state will be reached after advance() has cycled
             * through all of the states already in the queue.
             */
            void queueState( State state )
            {
                myNextStates.push_back( state );
            }

            /**
             * Causes the state machine to go to the provided state machine on the
             * next call to advance().  Clears the state queue.
             */
            void jumpState( State state )
            {
                myNextStates.clear();
                myNextStates.push_back( state );
            }

            /**
             * Causes the state machine to go to the state that is provided in the
             * parameter the next time advance() is called.  Does not clear the
             * state queue.
             */
            void insertState( State state )
            {
                myNextStates.push_front( state );
            }

            /**
             * Clears the queue, then makes a "procedure call" to the given state.  The
             * advance() method will return the provided state next, then whatever the
             * current state is.
             */
            void branchState( State state )
            {
                myNextStates.clear();
                myNextStates.push_front( myCurrentState );
                myNextStates.push_front( state );
            }

            /**
             * Causes the state machine to move to the given state next, then back to
             * the current state.
             */
            void insertBranchState( State state )
            {
                myNextStates.push_front( myCurrentState );
                myNextStates.push_front( state );
            }

            /**
             * Causes the state machine to move to the given state after all of the
             * other states already in the queue have been processed, then back to
             * the current state.
             */
            void queueBranchState( State state )
            {
                myNextStates.push_back( myCurrentState );
                myNextStates.push_back( state );
            }


        protected:

            // The current state of the state machine
            State myCurrentState;

            // The list of states that come next in the machine
            Queue myNextStates;
    };
}

#endif