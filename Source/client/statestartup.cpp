//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include "stdafx.h"
#include <direct.h>




//-----------------------------------------------------------------------------
// Name:  stateStartup
// Desc:  
//-----------------------------------------------------------------------------
void EvidyonClient::stateStartup() {

  // Load the settings file
  APP_INFO(!config_.read("config.ini"))("Config file not found");

  // Do all of the initialization process
  if (APP_FATAL(!acquireGraphics())("Unable to initialize graphics") ||
      APP_FATAL(!acquireInput())("Unable to initialize input") ||
      APP_FATAL(!acquireNetwork())("Unable to initialize network") ||
      APP_FATAL(!acquireSystem())("System couldn't initialize correctly") ||
      APP_FATAL(!acquireResources())("The game couldn't load its resource file")) {
    // Display error message
    MessageBox(NULL, "Failed to initialize; check debug log for more info", "Error", MB_OK);

    // Exit
    myStateMachine.jumpState(VCS_SHUTDOWN);
  } else {
    // Move to the next state (determined by the calling method!)
    //myStateMachine.jumpState(VCS_CONNECT);
    //myStateMachine.jumpState(VCS_SHUTDOWN);
    //myStateMachine.jumpState(VCS_DEBUGSTATESANDBOX);
  }
}