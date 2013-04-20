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
#include "visualfxinstance_flickeringlight.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/evidyond3dlightmanager.h"
#include "visualfx_flickeringlight.h"
#include <math.h> // sin
#include <d3dx9.h>
#include "common/randf.h"

namespace Evidyon {
namespace VisualFX {





  
//----[  VisualFXInstance_FlickeringLight  ]-----------------------------------
VisualFXInstance_FlickeringLight::VisualFXInstance_FlickeringLight(
    const Evidyon::VisualFX::VisualFX_FlickeringLight* parameters) {
  parameters_ = parameters;
  light_ = NULL;
  flicker_position_ = 0.0f;
}




//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_FlickeringLight::destroy() {
  if (light_) {
    renderer()->lightManager()->freeLight(light_);
    light_ = NULL;
  }
}




//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_FlickeringLight::update(double time,
                                              double time_since_last_update) {
  float x, y, z;
  if (getSourceLocation(&x, &y, &z) == false || sourceExpired()) return false;
  if (!light_) {
    flicker_sine_speed_base_time_ = (randf()*4.0) + time;
    light_ = renderer()
              ->lightManager()
              ->allocateBigLight(parameters_->colors[0]);
    light_->h = x;
    light_->v = z;
  }
  if (light_) {
    double f = sin(time - flicker_sine_speed_base_time_);
    flicker_position_ +=
      (20.0 + 10.0*sin(time)) * time_since_last_update * (1.0f - f*f);
    f = fmod((double)flicker_position_, 4.0);
    D3DXCOLOR c;
    if (f < 1.0) {
      D3DXCOLOR a(parameters_->colors[0]);
      D3DXCOLOR b(parameters_->colors[1]);
      D3DXColorLerp(&c, &a, &b, f);
    } else if (f < 2.0) { 
      D3DXCOLOR a(parameters_->colors[1]);
      D3DXCOLOR b(parameters_->colors[2]);
      D3DXColorLerp(&c, &a, &b, f - 1.0);
    } else if (f < 3.0) {
      D3DXCOLOR a(parameters_->colors[2]);
      D3DXCOLOR b(parameters_->colors[3]);
      D3DXColorLerp(&c, &a, &b, f - 2.0);
    } else {
      D3DXCOLOR a(parameters_->colors[3]);
      D3DXCOLOR b(parameters_->colors[0]);
      D3DXColorLerp(&c, &a, &b, f - 3.0);
    }
    light_->d3d_light.Diffuse = c;
    light_->h = x;
    light_->v = z;
    light_->d3d_light.Position.y = y;
  }
  return true;
}


}
}