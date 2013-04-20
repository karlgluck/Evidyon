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
#include "circlingtexture.h"



namespace Evidyon {
namespace Texture {
namespace Tools {


/// Resolve this class's implementation linker
dc::dcClass<CirclingTexture>::Implements<Texture> implementsTexture;


//-----------------------------------------------------------------------------
// Name:  CirclingTexture
// Desc:  Initializes this resource with the given name
//-----------------------------------------------------------------------------
CirclingTexture::CirclingTexture()
{
  member("Speed", &myCirclingSpeed);
  member("Magnitude", &myCirclingMagnitude);

  // Set up default values
  myCirclingSpeed.setValue(0.5f);
  myCirclingMagnitude.setValue(0.5f);
}




//----[  compile  ]------------------------------------------------------------
void CirclingTexture::compile(TextureDescription* description) {
  description->animation_type = TextureDescription::CIRCLING;
  description->animation.circling.radius = myCirclingMagnitude.getValue();
  description->animation.circling.speed = myCirclingSpeed.getValue();
}


//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Gets the name of this class
//-----------------------------------------------------------------------------
std::string CirclingTexture::staticTypeName()
{
  return "Circling Texture";
}


}
}
}