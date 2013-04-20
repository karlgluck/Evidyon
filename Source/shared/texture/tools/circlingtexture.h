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
#ifndef __CIRCLINGTEXTURE_H__
#define __CIRCLINGTEXTURE_H__
#pragma once


#include "texture.h"
#include <dc/float>



namespace Evidyon {
namespace Texture {
namespace Tools {







  
//----[  CirclingTexture  ]----------------------------------------------------
class CirclingTexture : public dc::dcResource<CirclingTexture,Texture> {
public:
  CirclingTexture();

public:
  virtual void compile(TextureDescription* description);

  /**
   * Gets the value of the speed at which this texture circles
   */
  float getCirclingSpeedValue() const { return myCirclingSpeed.getValue(); }

  /**
   * Gets the value of the magnitude at which this texture circles
   */
  float getCirclingMangitudeValue() const { return myCirclingMagnitude.getValue(); }


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  /// How fast this texture moves.  If this is zero, all other movement parameters are
  /// ignored.  If this is negative, the parameter defines
  dc::dcFloat myCirclingSpeed;

  /// Defines the size of the circling
  dc::dcFloat myCirclingMagnitude;


protected:

  /// This class implements the Texture interface
  static dc::dcClass<CirclingTexture>::Implements<Texture> implementsTexture;
};

}
}
}



#endif