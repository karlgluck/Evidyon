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
#ifndef __SHARED_TRIGGER_TOOLS_TRIGGER_MAPLINK_H__
#define __SHARED_TRIGGER_TOOLS_TRIGGER_MAPLINK_H__
#pragma once


#include "trigger.h"
#include "shared/world/tools/map.h"


namespace Evidyon {
namespace Trigger {
namespace Tools {



//----[  Trigger_MapLink  ]----------------------------------------------------
class Trigger_MapLink : public dc::dcResource<Trigger_MapLink,Trigger> {
public:
  static std::string staticTypeName() { return "Trigger_MapLink"; }
  static dc::dcClass<Trigger_MapLink>::Implements<Trigger> implements_trigger_;
public:
  Trigger_MapLink();
  virtual bool compile(Evidyon::Trigger::Trigger* map_link);
private:
  Evidyon::Tools::MapReference destination_map_;
  dc::dcInteger x_, y_;
};



}
}
}

#endif