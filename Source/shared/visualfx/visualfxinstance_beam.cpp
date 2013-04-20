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
#include "visualfxinstance_beam.h"
#include "shared/mesh/meshgeometry.h"
#include <d3dx9.h>
#include "shared/visualfx/visualfxrenderer.h"
#include "visualfx_beam.h"


namespace Evidyon {
namespace VisualFX {


  
//----[  VisualFXInstance_Beam  ]----------------------------------------------
VisualFXInstance_Beam::VisualFXInstance_Beam(const VisualFX_Beam* parameters) {
  parameters_ = parameters;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_Beam::destroy() {

}





//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_Beam::update(double time,
                                   double time_since_last_update) {
  if (!getSourceLocation(&source_x_, &source_y_, &source_z_) ||
      !getTargetLocation(&target_x_, &target_y_, &target_z_) ||
      expired()) return false;
  return true;
}



//----[  render  ]-------------------------------------------------------------
void VisualFXInstance_Beam::render(Evidyon::Texture::TextureIndex texture) {
  if (hidden()) return;

  Evidyon::VisualFX::VisualFXRenderer* visual_fx_renderer = renderer();

  if (texture == parameters_->end_particles_texture) {
    Evidyon::VisualFX::VisualFXParticle particle;
    particle.color = parameters_->end_particles_color;
    particle.point_size = parameters_->end_particle_base_size
        + intensity() * parameters_->end_particle_intensity_size;

    particle.x = source_x_;
    particle.y = source_y_;
    particle.z = source_z_;
    visual_fx_renderer->renderParticle(&particle);

    particle.x = target_x_;
    particle.y = target_y_;
    particle.z = target_z_;
    visual_fx_renderer->renderParticle(&particle);

  } else {
    const D3DCOLOR beam_color = parameters_->beam_color;

    Mesh::GeometryVertex beam_vertices[] = {
      // (x,     y,       z)    (nx,   ny,   nz)    (diffuse)    (u,    v)
      {  0.0f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   beam_color,   0.0f, 0.0f },
      {  1.0f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   beam_color,   1.0f, 0.0f },
      {  0.0f, 0.0f, +0.5f,   0.0f, 1.0f, 0.0f,   beam_color,   0.0f, 1.0f },
      {  1.0f, 0.0f, +0.5f,   0.0f, 1.0f, 0.0f,   beam_color,   1.0f, 1.0f },
    };

    const Mesh::GeometryIndex beam_indices[] = {
      0,  1,  2,  2,  1,  3,
      4,  5,  6,  6,  5,  7,
      8,  9, 10,  9, 10, 11,
    };

    float dx = target_x_ - source_x_,
          dy = target_y_ - source_y_,
          dz = target_z_ - source_z_;

    // transform the beam
    D3DXMATRIXA16 beam_mx;
    D3DXVECTOR3 scaling(sqrtf(dx*dx + dy*dy + dz*dz), // beam length
                        1.0f,
                        parameters_->beam_base_width
                          + intensity() * parameters_->beam_intensity_width);
    D3DXQUATERNION rotation;
    D3DXQuaternionRotationYawPitchRoll(&rotation,
                                       -atan2f(dz, dx),
                                       0.0f,
                                       0.0f);
    D3DXMatrixTransformation(&beam_mx,
                             NULL,
                             NULL,
                             &scaling,
                             NULL,
                             &rotation,
                             &D3DXVECTOR3(source_x_, source_y_, source_z_));
    D3DXVec3TransformCoordArray((D3DXVECTOR3*)(beam_vertices),
                                sizeof(Mesh::GeometryVertex),
                                (D3DXVECTOR3*)(beam_vertices),
                                sizeof(Mesh::GeometryVertex),
                               &beam_mx, 4);

    visual_fx_renderer->renderDynamicGeometry(beam_vertices,
                                              1 * 4,
                                              beam_indices,
                                              1 * 6);
  }
}



}
}
