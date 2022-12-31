//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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