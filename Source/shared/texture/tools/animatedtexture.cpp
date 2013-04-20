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
#include "animatedtexture.h"




namespace Evidyon {
namespace Texture {
namespace Tools {

/// Resolve this class's implementation linker
dc::dcClass<AnimatedTexture>::Implements<Texture> implementsTexture;



//-----------------------------------------------------------------------------
// Name:  AnimatedTexture
// Desc:  Initializes this class
//-----------------------------------------------------------------------------
AnimatedTexture::AnimatedTexture()
{
  member("Horizontal Frame Divisions", &myFramesWide);
  member("Vertical Frame Divisions", &myFramesHigh);
  member("Speed", &myAnimationSpeed);
}




//----[  compile  ]------------------------------------------------------------
void AnimatedTexture::compile(TextureDescription* description) {
  description->animation_type = TextureDescription::FRAME_SEQUENCE;
  description->animation.frame_sequence.frames_wide = myFramesWide.getValue();
  description->animation.frame_sequence.frames_high = myFramesHigh.getValue();
  description->animation.frame_sequence.speed = myAnimationSpeed.getValue();
}




//-----------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//-----------------------------------------------------------------------------
std::string AnimatedTexture::staticTypeName()
{
  return "Animated Texture";
}


}
}
}