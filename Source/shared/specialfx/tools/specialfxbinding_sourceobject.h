//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __SHARED_SPECIALFX_TOOLS_SPECIALFXBINDING_SOURCEOBJECT_H__
#define __SHARED_SPECIALFX_TOOLS_SPECIALFXBINDING_SOURCEOBJECT_H__
#pragma once

#include "specialfxbinding.h"


namespace Evidyon {
namespace SpecialFX {
namespace Tools {

//----[  SpecialFXBinding_SourceObject  ]--------------------------------------
class SpecialFXBinding_SourceObject
  : public dc::dcResource<SpecialFXBinding_SourceObject, SpecialFXBinding> {
  static dc::dcClass<SpecialFXBinding_SourceObject>
           ::Implements<SpecialFXBinding> implementsSpecialFXBinding;
public:
  static std::string staticTypeName() { return "SpecialFXBinding_SourceObject"; }
public:
  SpecialFXBinding_SourceObject();
  virtual SpecialFXBindingIndex compile(SpecialFXCompiler* compiler);
};

}
}
}

#endif