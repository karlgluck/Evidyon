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
#include "actoralignment.h"
#include <assert.h>

// yoinked from d3d9types.h
#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif
#ifndef D3DCOLOR_XRGB
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)
#endif


namespace Evidyon {
namespace Actor {


//----[  ActorAlignmentString  ]-----------------------------------------------
const char* ActorAlignmentString(ActorAlignment value) {
  switch (value) {
    case ALIGNMENT_GUARDIAN:  return "Guardian";
    case ALIGNMENT_CHAMPION:  return "Champion";
    case ALIGNMENT_GOOD:      return "Good";
    default: assert(false);
    case ALIGNMENT_NEUTRAL:   return "Neutral";
    case ALIGNMENT_EVIL:      return "Evil";
    case ALIGNMENT_DESTROYER: return "Destroyer";
    case ALIGNMENT_DREADLORD: return "Dreadlord";
    case ALIGNMENT_ADMIN:     return "Administrator";
  }
}

  
//----[  ActorAlignmentColor  ]------------------------------------------------
D3DCOLOR ActorAlignmentColor(ActorAlignment alignment) {
  switch (alignment) {
    case ALIGNMENT_GUARDIAN:  return D3DCOLOR_XRGB(255,253,204);
    case ALIGNMENT_CHAMPION:  return D3DCOLOR_XRGB(226,224,178);
    case ALIGNMENT_GOOD:      return D3DCOLOR_XRGB(255,255,229);
    default:
    case ALIGNMENT_NEUTRAL:   return D3DCOLOR_XRGB(180,180,180);
    case ALIGNMENT_EVIL:      return D3DCOLOR_XRGB(219,88,88);
    case ALIGNMENT_DESTROYER: return D3DCOLOR_XRGB(230,100,100);
    case ALIGNMENT_DREADLORD: return D3DCOLOR_XRGB(255,124,124);
    case ALIGNMENT_ADMIN:     return D3DCOLOR_XRGB(96,238,99);
  }
}

//----[  ActorAlignmentIsEvil  ]-----------------------------------------------
bool ActorAlignmentIsEvil(ActorAlignment alignment) {
  switch (alignment) {
    case ALIGNMENT_GUARDIAN:
    case ALIGNMENT_CHAMPION:
    case ALIGNMENT_GOOD:
    case ALIGNMENT_ADMIN:
    case ALIGNMENT_NEUTRAL:
      return false;
    default:
      return true;
  }
}


}
}