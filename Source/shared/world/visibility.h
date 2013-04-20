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
#ifndef __VISIBILITY_H__
#define __VISIBILITY_H__
#pragma once



namespace Evidyon {
namespace World {

//----[  Visibility  ]---------------------------------------------------------
enum Visibility {

  // This tile can't be seen at all because it's in a room behind a wall.  The
  // player should be completely unaware that this tile is even present.
  VISIBILITY_INVISIBLE,

  // This tile was made invisible, but it should be rendered as black instead
  // of not rendered.  This is useful if terrain is occluding, for example.
  VISIBILITY_HIDDEN,

  // This tile is behind something that occludes the direct view of the player.
  // These squares are colored slightly darker to show that there might be
  // something hiding there!
  VISIBILITY_OCCLUDED,

  // This tile is completely visible.
  VISIBILITY_NORMAL,
};



//----[  VisibilityString  ]---------------------------------------------------
const char* VisibilityString(Visibility visibility);

}
}


#endif