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
#ifndef __VISUALFXINSTANCE_BEAM_H__
#define __VISUALFXINSTANCE_BEAM_H__
#pragma once



#include "visualfxinstance.h"


namespace Evidyon {
namespace VisualFX {
struct VisualFX_Beam;

//----[  VisualFXInstance_Beam  ]----------------------------------------------
class VisualFXInstance_Beam : public VisualFXInstance {
public:
  VisualFXInstance_Beam(const VisualFX_Beam* parameters);
  virtual void destroy();
  virtual bool update(double time,
                      double time_since_last_update);
  virtual void render(Evidyon::Texture::TextureIndex texture);
private:
  const Evidyon::VisualFX::VisualFX_Beam* parameters_;
  float source_x_, source_y_, source_z_;
  float target_x_, target_y_, target_z_;
};

}
}

#endif