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
#include "slidingtexture.h"
#include <d3dx9.h>


namespace Evidyon {
namespace Texture {
namespace Tools {

/// Resolve this class's implementation linker
dc::dcClass<SlidingTexture>::Implements<Texture> implementsTexture;


//-----------------------------------------------------------------------------
// Name:  SlidingTexture
// Desc:  Initializes this resource with the given name
//-----------------------------------------------------------------------------
SlidingTexture::SlidingTexture()
{
  member("Speed", &myMovementSpeed);
  member("Angle", &myMovementAngle);

  // Initialize the movement speed
  myMovementSpeed.setValue(1.0f);
}




//----[  compile  ]------------------------------------------------------------
void SlidingTexture::compile(TextureDescription* description) {
  description->animation_type = TextureDescription::SLIDING;
  description->animation.sliding.dx
    = cosf(D3DXToRadian(myMovementAngle.getValue())) * myMovementSpeed.getValue();
  description->animation.sliding.dy
    = sinf(D3DXToRadian(myMovementAngle.getValue())) * myMovementSpeed.getValue();
}


//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//-----------------------------------------------------------------------------
std::string SlidingTexture::staticTypeName()
{
  return "Sliding Texture";
}


}
}
}