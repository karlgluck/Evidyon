//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#ifndef __TOOLS_VISUALFX_SCENERY_H__
#define __TOOLS_VISUALFX_SCENERY_H__
#pragma once

#include "visualfx.h"
#include <dc/boolean>
#include <dc/float>
#include "shared/scenery/tools/scenery.h" // ???


namespace Evidyon {
namespace VisualFX {
namespace Tools {



  
//----[  VisualFX_Scenery  ]---------------------------------------------------
class VisualFX_Scenery
   : public dc::dcResource<VisualFX_Scenery, VisualFX> {
public:
  static dc::dcClass<VisualFX_Scenery>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_Scenery"; }

public:
  VisualFX_Scenery();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  dc::dcList<Scenery::Tools::Scenery>::Reference scenery_;
  dc::dcBoolean turn_to_follow_path_;
};

}
}
}

#endif