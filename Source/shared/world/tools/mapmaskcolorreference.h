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
#ifndef __MAPMASKCOLORREFERENCE_H__
#define __MAPMASKCOLORREFERENCE_H__
#pragma once



#include "mapmask.h"
#include "win32-rect.h"
#include <dc/list>
#include <dcx/d3dcolor>


namespace Evidyon {
namespace World {
namespace Tools {

struct MapCompilationContext;
struct MapLocationData;

//----[  MapMaskLocationFunctionoid  ]-----------------------------------------
// Each map layer that uses a MapMaskColorReference implements this class to
// apply its effect to the map during compilation.
//
// For more on functionoids, see:
//  http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.10
//
class MapMaskLocationFunctionoid {
public:
  virtual ~MapMaskLocationFunctionoid() = 0;

  // Apply some effect to the location.  If the location has changed, this
  // method returns 'true'.
  virtual bool process(MapLocationData* location) = 0;
};
inline MapMaskLocationFunctionoid::~MapMaskLocationFunctionoid() {}


//----[  MapMaskColorReference  ]----------------------------------------------
// References a specific color-set in the map layer mask stack.
class MapMaskColorReference : public dc::dcResource<MapMaskColorReference> {
public:
  static std::string staticTypeName() { return "MapMaskColorReference"; }
public:
  MapMaskColorReference();

  // Calls functionoid->process for each of the locations in the referenced
  // mask that lie within 'area'.  The 'area' pointer should not ever be NULL.
  // The referenced map mask should have been loaded prior to calling this
  // method.
  // If a location was changed, its last-modified-by property is changed to
  // reference 'map_layer_index'
  void processMaskLocations(
      size_t map_layer_index,
      const RECT* area,
      MapCompilationContext* compilation_context,
      MapMaskLocationFunctionoid* functionoid);

  // Returns a string containing the defined name of the specified color
  // in the referenced mask.
  std::string toString();

  // Obtains the mask to which this element points
  dc::dcList<MapMask>::Element* getReferencedMaskListElement();

  // Gets the color referenced by this element
  D3DCOLOR getD3DColorValue() const;

  // Returns whether or not this reference points to the given mask or
  // color-subset of the given mask
  bool references(MapMask* mask) const;
  bool references(MapMask* mask, D3DCOLOR color) const;

  // Sets this reference's properties
  void set(dc::dcList<MapMask>::Element* map_mask, D3DCOLOR color);

private:
  dc::dcList<MapMask>::Reference mask_;
  dcx::dcxD3DColor color_;
};

}
}
}

#endif