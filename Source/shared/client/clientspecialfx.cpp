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
#include "clientspecialfx.h"
#include "common/null.h"


namespace Evidyon {
namespace Client {


//----[  ClientSpecialFXString  ]----------------------------------------------
const char* ClientSpecialFXString(ClientSpecialFX value) {
  switch (value) {
    case CLIENT_SPECIALFX_MOUSE_OVER_ATTACK_GOOD_ACTOR: return "Mouse Over Attack Good Actor";
    case CLIENT_SPECIALFX_MOUSE_OVER_ATTACK_EVIL_ACTOR: return "Mouse Over Attack Evil Actor";
    case CLIENT_SPECIALFX_MOUSE_OVER_INTERACTIVE_ACTOR: return "Mouse Over Interactive Actor";
    case CLIENT_SPECIALFX_GEONITE_BURST: return "Geonite Burst";
    case CLIENT_SPECIALFX_GEOSID_CHANGEOWNER: return "Geosid Change Owner";
    default: return NULL;
  }
}

}
}

