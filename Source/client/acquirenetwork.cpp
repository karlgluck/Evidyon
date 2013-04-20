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





//-----------------------------------------------------------------------------
// Name: acquireNetwork
// Desc: 
//-----------------------------------------------------------------------------
bool EvidyonClient::acquireNetwork()
{
  // Load the ENet library
  if (APP_FATAL(0 > enet_initialize())("Unable to initialize the network library"))
    return false;

  // This creates a client "host" object that has one outgoing connection and no static
  // downstream/upstream bandwidth limitaion.  This causes the ENet library to rely on
  // its dynamic throttling algorithm alone.
  enet_client_ = enet_host_create(NULL, 1, 0, 0);

  // Success
  return true;
}