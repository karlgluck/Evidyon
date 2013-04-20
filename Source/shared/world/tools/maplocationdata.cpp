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
#include "maplocationdata.h"


namespace Evidyon {
namespace World {
namespace Tools {


  
//----[  getLayerBlendingType  ]-----------------------------------------------
MapLayerBlendingType MapLocationData::getLayerBlendingType(
    size_t inner_map_layer_index,
    size_t outer_map_layer_index) {

#define IS_INNER(ptr)  \
  (ptr->last_layer.last_modified_by.map_layer_index == inner_map_layer_index)
#define IS_OUTER(ptr)  \
  (ptr->last_layer.last_modified_by.map_layer_index == outer_map_layer_index)

  if (!IS_INNER(this)) return INVALID_MAP_LAYER_BLENDING_TYPE;

#define  N(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[0]->neighbors_nesw[3])
#define NE(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[0]->neighbors_nesw[1])
#define  E(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[1])
#define SE(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[2]->neighbors_nesw[1])
#define  S(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[2])
#define SW(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[2]->neighbors_nesw[3])
#define  W(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[3])
#define NW(INNER_or_OUTER)  \
  IS_##INNER_or_OUTER(neighbors_nesw[0]->neighbors_nesw[3])

#define INNER2(i1,i2) (i1(INNER) && i2(INNER))
#define OUTER2(o1,o2) (o1(OUTER) && o2(OUTER))

  if (INNER2(E,W)) {
    if (N(OUTER)) return NORTH;
    if (S(OUTER)) return SOUTH;
  }
  if (INNER2(N,S)) {
    if (E(OUTER)) return EAST;
    if (W(OUTER)) return WEST;
  }

  if (N(OUTER)) {
    if (E(OUTER)) return NORTHEAST_OUTER_CORNER;
    if (W(OUTER)) return NORTHWEST_OUTER_CORNER;
  }
  if (S(OUTER)) {
    if (E(OUTER)) return SOUTHEAST_OUTER_CORNER;
    if (W(OUTER)) return SOUTHWEST_OUTER_CORNER;
  }

  if (INNER2(N,E) && NE(OUTER)) return NORTHEAST_INNER_CORNER;
  if (INNER2(S,E) && SE(OUTER)) return SOUTHEAST_INNER_CORNER;
  if (INNER2(S,W) && SW(OUTER)) return SOUTHWEST_INNER_CORNER;
  if (INNER2(N,W) && NW(OUTER)) return NORTHWEST_INNER_CORNER;

  return INVALID_MAP_LAYER_BLENDING_TYPE;
}


}
}
}