//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __TOOLS_VISUALFX_SOURCETOTARGETSLIDER_H__
#define __TOOLS_VISUALFX_SOURCETOTARGETSLIDER_H__

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/texture/tools/texture.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


  

//----[  VisualFX_SourceToTargetSlider  ]--------------------------------------
class VisualFX_SourceToTargetSlider
  : public dc::dcResource<VisualFX_SourceToTargetSlider,VisualFX> {
public:
  static dc::dcClass<VisualFX_SourceToTargetSlider>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_SourceToTargetSlider"; }

public:
  VisualFX_SourceToTargetSlider();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  dc::dcBoolean target_is_sliding_;
  dc::dcFloat arrival_time_;
  dc::dcTable<VisualFX>::Reference sliding_effect_;
};


}
}
}

#endif