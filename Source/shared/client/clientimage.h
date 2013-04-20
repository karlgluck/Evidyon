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
#ifndef __SHARED_CLIENT_CLIENTIMAGE_H__
#define __SHARED_CLIENT_CLIENTIMAGE_H__
#pragma once


namespace Evidyon {
namespace Client {


//----[  ClientImage  ]--------------------------------------------------------
// These images are defined for a particular purpose on the client.
enum ClientImage {
  CLIENT_IMAGE_TITLE_TEXT,
  CLIENT_IMAGE_TITLE_BACKGROUND,
  CLIENT_IMAGE_EVIDYON_SYMBOL,
  CLIENT_IMAGE_BLOCK_FONT,
  CLIENT_IMAGE_GUI,
  NUMBER_OF_CLIENT_IMAGES,
  INVALID_CLIENT_IMAGE,
};




//----[  ClientImageString  ]--------------------------------------------------
const char* ClientImageString(ClientImage value);


}
}

#endif