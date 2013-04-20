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
#ifndef __GUIMOUSE_H__
#define __GUIMOUSE_H__
#pragma once


/**
 * Defines different types of mouse button actions
 */
enum GUIMouseButtonAction
{
  /// States for the primary mouse button
  GUIMOUSE_DOWN,
  GUIMOUSE_UP,
  GUIMOUSE_CLICK_SINGLE,
  GUIMOUSE_CLICK_DOUBLE,

  /// States for the alternate mouse button
  GUIMOUSE_ALT_DOWN,
  GUIMOUSE_ALT_UP,
  GUIMOUSE_CLICK_ALT_SINGLE,
  GUIMOUSE_CLICK_ALT_DOUBLE,
};


// Pre-declare these classes, since they reference each other
class GUIMouseSentinel;
class GUIMouseInputSource;

// Include the sentinel and input source
#include "guimouseinputsource.h"
#include "guimousesentinel.h"



#endif