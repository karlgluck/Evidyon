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
#ifndef __SHARED_GEOSID_TOOLS_GEOSID_H__
#define __SHARED_GEOSID_TOOLS_GEOSID_H__
#pragma once


#include "shared/world/tools/location.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/geosid/geosidindex.h"
#include "shared/geosid/geosidtype.h"
#include "shared/world/mapindex.h"
#include <dc/string>
#include <dc/enum>
#include "shared/magic/tools/magic.h"
#include "shared/item/tools/item.h"
#include "shared/specialfx/tools/specialfx.h"
//#include "shared/world/tools/maplocationreference.h"
#include "shared/lifeformai/tools/lifeformai_geosidguardian.h"

namespace Evidyon {
namespace World {
namespace Tools {
struct MapCompilationContext;
// note: actually #including maplocationreference.h here will
// break the build since:
//  map -> geosid -> map location reference -> map = explode
class MapLocationReference;
}
}
}

namespace Evidyon {
namespace Geosid {
namespace Tools {


class GeosidCompiler;


//----[  Geosid  ]-------------------------------------------------------------
// A list of these is created inside each World::Map
class Geosid : public dc::dcResource<Geosid> {
public:
  static std::string staticTypeName() { return "Geosid"; }

public:
  Geosid();
  ~Geosid();

  // Writes the geosid scenery into the map's context
  void compile(World::Tools::MapCompilationContext* context) const;

  // Puts the geosid into the compiler and writes the geosid's
  // guardian into the context.
  Evidyon::Geosid::GeosidIndex compile(
    Evidyon::World::MapIndex map,
    GeosidCompiler* compiler,
    World::Tools::MapCompilationContext* context);

  void getLocation(float* x, float* y);
  const char* getDisplayedName();

private:
  dc::dcString displayed_name_;
  dc::dcEnum<GeosidType> type_;
  Evidyon::World::Tools::Location location_;
  Evidyon::Tools::SceneryReference scenery_;

  dc::dcInteger burst_magic_geonite_cost_;
  Evidyon::Tools::MagicInterface burst_magic_;
  dc::dcInteger geonite_to_activate_;
  Evidyon::Tools::SpecialFXReference activation_special_fx_;
  dc::dcFloat activation_special_fx_duration_;
  dc::dcList<Evidyon::Tools::ItemReference> geonite_generated_items_;

  LifeformAI::Tools::LifeformAI_GeosidGuardian guardian_;

  // these can't actually be instantiated here because doing so
  // causes an #include loop...
  World::Tools::MapLocationReference* portals_;
};


}
}
}

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Geosid::Tools::Geosid> GeosidList;
typedef dc::dcList<Evidyon::Geosid::Tools::Geosid>::Element GeosidElement;
typedef dc::dcList<Evidyon::Geosid::Tools::Geosid>::Reference GeosidReference;
}
}


#endif