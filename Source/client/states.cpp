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
#include "stdafx.h"



//-----------------------------------------------------------------------------
// Name:  stateName
// Desc:  Gets the name of the address of a particular state
//-----------------------------------------------------------------------------
const char* EvidyonClient::stateName(State state)
{
    switch(state)
    {
        case VCS_STARTUP:       return "Startup";
        case VCS_CONNECT:       return "Connect";
        case VCS_ACCOUNT_LOGIN: return "Account Login";
//        case VCS_CREATE_ACCOUNT:return "Create Account";
        case VCS_LOGGED_INTO_ACCOUNT:   return "Logged Into Account";
        case VCS_WORLD_MAIN:    return "World - Main";
        case VCS_NO_CONNECTION: return "No Connection";
        case VCS_DISCONNECT:    return "Disconnect";
        case VCS_SHUTDOWN:      return "Shutdown";
        case VCS_CREATE_CHARACTER: return "Create Character";
        case VCS_LOST_D3DDEVICE: return "Lost D3D Device";
        case VCS_CREDITS: return "Credits";

        case VCS_DEBUGSTATESANDBOX: return "DEBUG: sandbox";
    }

    // Who knows what happened!
    return "Unknown";
}

//-----------------------------------------------------------------------------
// Name:  run
// Desc:  Executes the state with the given address
//-----------------------------------------------------------------------------
void EvidyonClient::run(State state) {
  switch(state) {
    case VCS_STARTUP:       stateStartup(); break;
    case VCS_CONNECT:       stateConnect(); break;
    case VCS_ACCOUNT_LOGIN: stateAccountLogin(); break;
//        case VCS_CREATE_ACCOUNT:stateCreateAccount(); break;
    case VCS_LOGGED_INTO_ACCOUNT:   stateLoggedIntoAccount(); break;
    case VCS_WORLD_MAIN:    stateWorldMain(); break;
    case VCS_NO_CONNECTION: stateNoConnection(); break;
    case VCS_DISCONNECT:    stateDisconnect(); break;
    case VCS_SHUTDOWN:      stateShutdown(); break;
    case VCS_CREATE_CHARACTER: stateCreateCharacter(); break;
    case VCS_LOST_D3DDEVICE: stateLostD3DDevice(); break;
    case VCS_CREDITS: stateCredits(); break;

    // Testing-only states
    case VCS_DEBUGSTATESANDBOX:   debugStateSandbox(); break;

    // Output a warning if we don't know what the state is
    default:
        DEBUG_ERROR("Tried to execute unknown state:  %i (%s)", (int)state, stateName(state));
        break;
  }
}