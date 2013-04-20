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
#include "visualfxinstance_grounddecal.h"
#include "shared/mesh/meshgeometry.h"
#include <d3dx9.h>
#include "shared/visualfx/visualfxrenderer.h"
#include <assert.h>
#include "visualfx_grounddecal.h"
#include "common/randf.h"


namespace Evidyon {
namespace VisualFX {


  
//----[  VisualFXInstance_GroundDecal  ]----------------------------------------------
VisualFXInstance_GroundDecal::VisualFXInstance_GroundDecal(
    const VisualFX_GroundDecal* parameters) {
  parameters_ = parameters;
  number_of_decals_ = 0;
  next_generate_decal_time_ = 0.0;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_GroundDecal::destroy() {

}





//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_GroundDecal::update(double time,
                                   double time_since_last_update) {
  float x, y, z;
  bool alive = getTargetLocation(&x, &y, &z) && !targetExpired();

  if (alive &&
      time > next_generate_decal_time_ &&
      number_of_decals_ < MAX_NUMBER_OF_DECALS) {
    struct {
      float x, y, z;
    } coordinates[4] = {
      { -0.5f, 0.0f, -0.5f },
      {  0.5f, 0.0f, -0.5f },
      { -0.5f, 0.0f, +0.5f },
      {  0.5f, 0.0f, +0.5f },
    };

    // transform the decal
    float size = parameters_->random_size_base 
      + randf() * parameters_->random_size_range;
    D3DXMATRIXA16 mx;
    D3DXVECTOR3 scaling(size,
                        1.0f,
                        size);
    D3DXQUATERNION rotation;
    D3DXQuaternionRotationYawPitchRoll(&rotation,
                                       randfAngleRadians(),
                                       0.0f,
                                       0.0f);
    D3DXMatrixTransformation(&mx,
                             NULL,
                             NULL,
                             &scaling,
                             NULL,
                             &rotation,
                             &D3DXVECTOR3(x, 0.0f, z));
    D3DXVec3TransformCoordArray((D3DXVECTOR3*)(coordinates),
                                sizeof(D3DXVECTOR3),
                                (D3DXVECTOR3*)(coordinates),
                                sizeof(D3DXVECTOR3),
                               &mx, 4);

    // save the coordinates
    Decal* decal = &decals_[number_of_decals_++];
    decal->dying_time = time + parameters_->decal_lifetime;
    for (int i = 0; i < 4; ++i) {
      decal->x[i] = coordinates[i].x;
      decal->z[i] = coordinates[i].z;
    }
    decal->color = parameters_->colors[rand()%3];
    next_generate_decal_time_ = time + parameters_->generate_period;
  } else {
    if (number_of_decals_ == 0) return false; // finished
  }

  // update all decals
  for (size_t i = 0; i < number_of_decals_; /*++i in loop*/) {
    Decal* decal = &decals_[i];
    double lifetime_left = decal->dying_time - time;
    bool decal_alive = alive && (lifetime_left > 0.0);
    if (!decal_alive) {
      decal->color &= 0x00FFFFFF;
      int alpha = int(255.0f + lifetime_left * parameters_->decal_fade_rate);

      // Erase invisible decals.
      if (alpha < 0x08) {
        size_t last = number_of_decals_ - 1;
        if (i < last) {
          memcpy(decal, &decals_[last], sizeof(Decal));
        }
        --number_of_decals_;
      } else {
        alpha = min(alpha, 255);
        decal->color |= (unsigned int(alpha)) << 24;
        ++i;
      }
    } else {
      ++i;
    }
  }

  return true;
}



//----[  render  ]-------------------------------------------------------------
void VisualFXInstance_GroundDecal::render(Evidyon::Texture::TextureIndex texture) {
  assert(texture == parameters_->texture);
  if (hidden()) return;
  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer = renderer();

  for (size_t i = 0; i < number_of_decals_; ++i) {
    Decal* decal = &decals_[i];
    const D3DCOLOR color = decal->color;
    Mesh::GeometryVertex vertices[] = {
      // (x,     y,       z)    (nx,   ny,   nz)    (diffuse)    (u,    v)
      { decal->x[0], 0.0f, decal->z[0],   0.0f, 1.0f, 0.0f,   color,   0.0f, 0.0f },
      { decal->x[1], 0.0f, decal->z[1],   0.0f, 1.0f, 0.0f,   color,   1.0f, 0.0f },
      { decal->x[2], 0.0f, decal->z[2],   0.0f, 1.0f, 0.0f,   color,   0.0f, 1.0f },
      { decal->x[3], 0.0f, decal->z[3],   0.0f, 1.0f, 0.0f,   color,   1.0f, 1.0f },
    };

    const Mesh::GeometryIndex indices[] = {
      0,  1,  2,  2,  1,  3,
    };


    visual_fx_renderer->renderDynamicGeometry(vertices,
                                              1 * 4,
                                              indices,
                                              1 * 6);
  }
}



}
}