//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __VISUALFX_SOURCETOTARGETSLIDER_H__
#define __VISUALFX_SOURCETOTARGETSLIDER_H__

#include "shared/visualfx/visualfxindex.h"

namespace Evidyon {
namespace VisualFX {

struct VisualFX_SourceToTargetSlider {

  // true: effect being slid is between [original source, sliding target]
  // false: effecing being slid is between [sliding target, original target]
  bool target_is_sliding;

  // How fast the binding moves
  float arrival_time;

  // This is the effect type for which this slider generates a binding
  VisualFXIndex sliding_effect;
};


}
}

#endif