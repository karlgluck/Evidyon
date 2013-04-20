//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#include "visualfx_sourcetotargetslider.h"
#include <assert.h>
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_sourcetotargetslider.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {



dc::dcClass<VisualFX_SourceToTargetSlider>::Implements<VisualFX>
  VisualFX_SourceToTargetSlider::implementsVisualFX;



//----[  VisualFX_SourceToTargetSlider  ]-----------------------------------------------
VisualFX_SourceToTargetSlider::VisualFX_SourceToTargetSlider() {
  member("Target is Sliding", &target_is_sliding_);
  member("Arrival Time", &arrival_time_);
  member("Sliding Effect", &sliding_effect_);
  target_is_sliding_.setValue(true);
  arrival_time_.setValue(0.5f);
}




//----[  compile  ]------------------------------------------------------------
void VisualFX_SourceToTargetSlider::compile(Evidyon::VisualFX::VisualFX* description) {
  Evidyon::VisualFX::VisualFX_SourceToTargetSlider* this_description
      = &description->source_to_target_slider;
  description->type = Evidyon::VisualFX::VisualFX::SOURCE_TO_TARGET_SLIDER;
  this_description->target_is_sliding = target_is_sliding_.getValue();
  this_description->arrival_time = arrival_time_.getValue();
  this_description->sliding_effect = sliding_effect_.getReferencedResourceIndex();
}


}
}
}
