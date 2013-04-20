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
#ifndef __TOOLS_VISUALFX_GROUNDDECAL_H__
#define __TOOLS_VISUALFX_GROUNDDECAL_H__
#pragma once

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/texture/tools/texture.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


  
  
//----[  VisualFX_GroundDecal  ]-----------------------------------------------
class VisualFX_GroundDecal
  : public dc::dcResource<VisualFX_GroundDecal,VisualFX> {
public:
  static dc::dcClass<VisualFX_GroundDecal>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_GroundDecal"; }

public:
  VisualFX_GroundDecal();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  Evidyon::Tools::TextureReference texture_;
  dcx::dcxD3DColor colors_[3];  // Colors
  dc::dcFloat min_size_, max_size_;
  dc::dcFloat generate_period_;
  dc::dcFloat total_visible_time_;
  dc::dcFloat fade_time_;
};


}
}
}

#endif