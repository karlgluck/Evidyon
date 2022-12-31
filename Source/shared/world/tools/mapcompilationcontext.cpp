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
#include "mapcompilationcontext.h"
#include "common/null.h"
#include <assert.h>


namespace Evidyon {
namespace World {
namespace Tools {


  


  
//----[  atOrDefault  ]--------------------------------------------------------
MapLocationData* MapCompilationContext::atOrDefault(int x, int y) {
  accessed = true;
  return (x < 0 || y < 0 || x >= width || y >= height)
    ? &default_location : &locations[y*width+x];
}



//----[  atOrNull  ]-----------------------------------------------------------
MapLocationData* MapCompilationContext::atOrNull(int x, int y) {
  accessed = true;
  return (x < 0 || y < 0 || x >= width || y >= height)
    ? NULL : &locations[y*width+x];
}




//----[  addTrigger  ]---------------------------------------------------------
Trigger::TriggerIndex MapCompilationContext::addTrigger(
    const Trigger::Trigger* trigger) {
  triggers.push_back(*trigger);
  return triggers.size() - 1;
}



//----[  compileTriggers  ]----------------------------------------------------
size_t MapCompilationContext::compileTriggers(Trigger::Trigger* triggers,
                                              size_t buffer_length) {
  assert(this->triggers.size() <= buffer_length);
  size_t index = 0;
  for (Triggers::iterator i = this->triggers.begin();
                          i != this->triggers.end();
                        ++i, ++index) {
    memcpy(&triggers[index], &*i, sizeof(Trigger::Trigger));
  }
  return index;
}


}
}
}