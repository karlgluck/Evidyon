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
#ifndef __SHARED_GEOSID_TOOLS_GEOSIDCOMPILER_H__
#define __SHARED_GEOSID_TOOLS_GEOSIDCOMPILER_H__
#pragma once

#include "common/kg/gid/structcollector.h"
#include "shared/geosid/geosidindex.h"

namespace Evidyon {
namespace Geosid {
struct GeosidServerDescription;
struct GeosidClientDescription;
}
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
namespace LifeformAI {
namespace Tools {
class LifeformAICompiler;
}
}
}


namespace Evidyon {
namespace Geosid {
namespace Tools {


  
//----[  GeosidCompiler  ]-----------------------------------------------------
class GeosidCompiler {
public:
  GeosidCompiler(Magic::Tools::MagicCompiler* magic_compiler,
                 LifeformAI::Tools::LifeformAICompiler* lifeform_compiler);

  void clear();

  // Puts the given geosid in the map.  This will automatically set the
  // descriptions' internal 'index' members.
  Evidyon::Geosid::GeosidIndex add(
    const GeosidServerDescription& server,
    const GeosidClientDescription& client);

  size_t numberOfGeosids() const;
  void compileServer(size_t index, GeosidServerDescription* desc);
  void compileClient(size_t index, GeosidClientDescription* desc);

  Magic::Tools::MagicCompiler* magicCompiler();
  LifeformAI::Tools::LifeformAICompiler* lifeformCompiler();

private:
  Magic::Tools::MagicCompiler* magic_compiler_;
  LifeformAI::Tools::LifeformAICompiler* lifeform_compiler_;
  kg::gid::StructCollector<GeosidServerDescription> server_;
  kg::gid::StructCollector<GeosidClientDescription> client_;
};

}
}
}

#endif