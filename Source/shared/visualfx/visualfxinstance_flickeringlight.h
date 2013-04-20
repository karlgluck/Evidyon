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
#ifndef __VISUALFXINSTANCE_FLICKERINGLIGHT_H__
#define __VISUALFXINSTANCE_FLICKERINGLIGHT_H__
#pragma once



#include "visualfxinstance.h"
#include "common/d3dcolor.h"


namespace Evidyon {
struct PointLight;

namespace VisualFX {
struct VisualFX_FlickeringLight;


//----[  VisualFXInstance_FlickeringLight  ]-----------------------------------
class VisualFXInstance_FlickeringLight : public VisualFXInstance {
public:
  VisualFXInstance_FlickeringLight(const VisualFX_FlickeringLight* parameters);
  virtual void destroy();
  virtual bool update(double time,
                      double time_since_last_update);

private:
  const Evidyon::VisualFX::VisualFX_FlickeringLight* parameters_;
  D3DCOLOR current_color_;
  float flicker_position_;
  double flicker_sine_speed_base_time_;
  PointLight* light_;
};

}
}

#endif