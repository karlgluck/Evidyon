//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __SHARED_SPECIALFX_TOOLS_SPECIALFX_H__
#define __SHARED_SPECIALFX_TOOLS_SPECIALFX_H__
#pragma once

#include <dc/dcresource.h>
#include "shared/specialfx/specialfxindex.h"

namespace Evidyon {
namespace SpecialFX {
namespace Tools {
class SpecialFXCompiler;
}
}
}

namespace Evidyon {
namespace SpecialFX {
namespace Tools {

//----[  SpecialFX  ]----------------------------------------------------------
class SpecialFX : public dc::dcGenericResource {
public:
  static std::string staticTypeName() { return "SpecialFX"; }
public:

  // The defined index should be set only for root effects (those in the
  // primary special FX table, to which references are made).  Otherwise,
  // it should be set to INVALID_SPECIALFX_INDEX.  A valid index is always
  // returned if compilation succeeds.
  virtual SpecialFXIndex compile(SpecialFXIndex index,
                                 SpecialFXCompiler* compiler) = 0;
};

}
}
}

#include <dc/table>
#include <dc/interface>

namespace Evidyon {
namespace Tools {
typedef dc::dcTable<Evidyon::SpecialFX::Tools::SpecialFX> SpecialFXTable;
typedef dc::dcTable<Evidyon::SpecialFX::Tools::SpecialFX>::Element SpecialFXElement;
typedef dc::dcTable<Evidyon::SpecialFX::Tools::SpecialFX>::Reference SpecialFXReference;
}
namespace SpecialFX {
namespace Tools {
typedef dc::dcInterface<Evidyon::SpecialFX::Tools::SpecialFX> SpecialFXInterface;
}
}
}


#endif