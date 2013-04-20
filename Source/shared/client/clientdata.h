//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#ifndef __SHARED_CLIENT_CLIENTDATA_H__
#define __SHARED_CLIENT_CLIENTDATA_H__
#pragma once


#include "shared/world/navigability.h"
#include "clientimage.h"
#include "clientsound.h"
#include "clientspecialfx.h"
#include "shared/image/imageindex.h"
#include "shared/sound/soundindex.h"
#include "shared/specialfx/specialfxindex.h"


namespace Evidyon {
namespace Client {

  
//----[  ClientData  ]---------------------------------------------------------
class ClientData {
public:

  // Gets the index of a defined image type
  virtual Image::ImageIndex clientImage(ClientImage image);

  // Obtains the sound from its definition
  virtual Sound::SoundIndex clientSound(ClientSound sound);

  // Obtains the special effect from its definition
  virtual SpecialFX::SpecialFXIndex clientSpecialFX(ClientSpecialFX special_fx);

  // Determines whether or not the given location is visible on the current
  // map, based on the user's location.  This interfaces with the map renderer
  // at some level, but this abstraction allows that to happen somewhere else.
  virtual bool locationIsVisible(int x, int z);

  // Obtains the navigability of the location on the current map.
  virtual World::Navigability navigability(int x, int z);

  // Obtains the sound that an actor at (x,y) should make when it puts a foot
  // down.  This allows walking in water or swimming to make a different sound
  // from being on land.
  //return movement_sounds_[(navigability(x,y))]; // wading, swimming, walking, ...
  virtual Sound::SoundIndex footstepSound(int x, int z);
};


}
}

#endif