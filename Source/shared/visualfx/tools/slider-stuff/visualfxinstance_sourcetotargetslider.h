//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __VISUALFXINSTANCE_SOURCETOTARGETSLIDER_H__
#define __VISUALFXINSTANCE_SOURCETOTARGETSLIDER_H__

#include "visualfxinstance.h"
#include "visualfx_sourcetotargetslider.h"


namespace Evidyon {
namespace VisualFX {
class VisualFXBinding_SourceToTargetSlider;

//----[  VisualFXInstance_TrailingParticles  ]---------------------------------
class VisualFXInstance_SourceToTargetSlider : public VisualFXInstance  {
public:
  VisualFXInstance_SourceToTargetSlider(
      const VisualFX_SourceToTargetSlider* parameters);
  virtual void destroy();
  virtual bool update(double time,
                      double time_since_last_update);
  virtual void render(Texture::TextureIndex texture);
private:
  const VisualFX_SourceToTargetSlider* parameters_;
  VisualFXBinding_SourceToTargetSlider* sliding_binding_;
  double arrival_time_;
  float vx_, vz_;
};


}
}

#endif