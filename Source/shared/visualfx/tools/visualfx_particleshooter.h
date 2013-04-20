//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __TOOLS_VISUALFX_PARTICLESHOOTER_H__
#define __TOOLS_VISUALFX_PARTICLESHOOTER_H__

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/texture/tools/texture.h" // for TextureReference


namespace Evidyon {
namespace VisualFX {
namespace Tools {

  
//----[  VisualFX_ParticleShooter  ]-------------------------------------------
class VisualFX_ParticleShooter
  : public dc::dcResource<VisualFX_ParticleShooter,VisualFX> {
public:
  static dc::dcClass<VisualFX_ParticleShooter>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_ParticleShooter"; }

public:
  VisualFX_ParticleShooter();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  Evidyon::Tools::TextureReference particle_texture_;
  dc::dcFloat particle_size_;   // The size of the particles before they start decaying
  dcx::dcxD3DColor colors_[3];  // Colors
  dc::dcFloat visible_time_;  // Total time a particle is visible
  dc::dcFloat decay_time_;
  dc::dcFloat generation_period_;
  dc::dcInteger particles_per_period_;     // number of particles that are generated simultaneously
  dc::dcFloat angle_around_vertical_;
  dc::dcFloat initial_distance_;
  dc::dcFloat initial_height_;    // The initial Y-location of the shooter
  dc::dcFloat radial_velocity_;
  dc::dcFloat gravity_, drag_percent_;
  dc::dcBoolean attach_to_target_; // Either attach to target (true) or source
};

}
}
}

#endif