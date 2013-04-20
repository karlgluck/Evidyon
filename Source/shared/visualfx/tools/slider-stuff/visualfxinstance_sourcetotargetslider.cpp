//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "visualfxinstance_sourcetotargetslider.h"
#include "visualfxbinding.h"
#include "common/safemacros.h"
#include "visualfxrenderer.h"
#include <assert.h>
#include <math.h> // sqrtf


namespace Evidyon {
namespace VisualFX {

class VisualFXBinding_SourceToTargetSlider : public VisualFXBinding {
public:
  VisualFXBinding_SourceToTargetSlider() {}
};


//----[  VisualFXInstance_SourceToTargetSlider  ]---------------------------------
VisualFXInstance_SourceToTargetSlider::VisualFXInstance_SourceToTargetSlider(
    const VisualFX_SourceToTargetSlider* parameters) {
  parameters_ = parameters;
  arrival_time_ = -1.0;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_SourceToTargetSlider::destroy() {
  if (sliding_binding_) {
    sliding_binding_->releaseReference();
    assert(sliding_binding_->hasNoReferences());
    SAFE_DELETE(sliding_binding_);
  }
}





//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_SourceToTargetSlider::update(
    double time,
    double time_since_last_update) {

  bool expired = false;
  float x, y, z;
  if (!sliding_binding_) {
    arrival_time_ = time + parameters_->arrival_time;
    sliding_binding_ = new VisualFXBinding_SourceToTargetSlider();
    sliding_binding_->acquireReference();
    const bool target_is_sliding = parameters_->target_is_sliding;
    renderer()->acquireVisualFX(
        parameters_->sliding_effect,
        intensity(),
        target_is_sliding ? sourceBinding() : sliding_binding_,
        target_is_sliding ? sliding_binding_ : targetBinding());
    expired = getSourceLocation(&x, &y, &z);
  } else {
    expired = sliding_binding_->getBindingLocation(&x, &y, &z);
  }

  if (!sliding_binding_->hasExpired()) {
    float target_x, target_y, target_z;
    if (getTargetLocation(&target_x, &target_y, &target_z)) {
      double dt = arrival_time_ - time;
      if (dt < time_since_last_update) {
        sliding_binding_->expired();
      } else {
        float dx = target_x - x,
              dz = target_z - z;
        float dd = sqrtf(dx*dx+dz*dz);
        float nx = dx / dd,
              nz = dz / dd;
        double nt = dt / parameters_->arrival_time;
        vx_ = nx * nt;
        vz_ = nz * nt;
      }
    } else {
      sliding_binding_->expired();
    }
  }

  x += time_since_last_update * vx_;
  z += time_since_last_update * vz_;

  sliding_binding_->updateBindingLocation(x, 0.0f, z);

  return true;
}





//----[  render  ]-------------------------------------------------------------
void VisualFXInstance_SourceToTargetSlider::render(
    Texture::TextureIndex texture) {

}

}
}
