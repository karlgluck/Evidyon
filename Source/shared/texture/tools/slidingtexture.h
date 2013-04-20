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