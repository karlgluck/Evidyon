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
#ifndef __SLIDINGTEXTURE_H__
#define __SLIDINGTEXTURE_H__
#pragma once


#include "texture.h"
#include <dc/float>


namespace Evidyon {
namespace Texture {
namespace Tools {






  
//----[  SlidingTexture  ]-----------------------------------------------------
class SlidingTexture : public dc::dcResource<SlidingTexture,Texture> {
public:
  SlidingTexture();

public:
  virtual void compile(TextureDescription* description);

  /**
   * Gets the value of the speed at which this texture slides
   */
  float getMovementSpeedValue() const { return myMovementSpeed.getValue(); }

  /**
   * Gets the value of the angle (in radians) at which this texture slides
   */
  float getMovementAngleValue() const { return myMovementAngle.getValue(); }

public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  /// How fast this texture moves.
  dc::dcFloat myMovementSpeed;

  /// What angle the texture moves in.  This will just perform a straight
  /// transform of the texture coordinates, so wrapping has to be enabled.
  dc::dcFloat myMovementAngle;


protected:

  /// This class implements the Texture interface
  static dc::dcClass<SlidingTexture>::Implements<Texture> implementsTexture;
};

}
}
}



#endif