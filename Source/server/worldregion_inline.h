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
#ifndef __WORLDREGION_H__
#error must be included by worldregion.h
#else
#ifndef __WORLDREGION_INLINE_H__
#define __WORLDREGION_INLINE_H__
#pragma once


WorldRegion::ActorSet::iterator WorldRegion::localActorsBegin()    { return local_actors_.begin(); }
WorldRegion::ActorSet::iterator WorldRegion::localActorsEnd()      { return local_actors_.end(); }
WorldRegion::ActorSet::iterator WorldRegion::nearbyActorsBegin()   { return nearby_actors_.begin(); }
WorldRegion::ActorSet::iterator WorldRegion::nearbyActorsEnd()     { return nearby_actors_.end(); }
WorldRegion::ClientSet::iterator WorldRegion::localClientsBegin()  { return local_clients_.begin(); }
WorldRegion::ClientSet::iterator WorldRegion::localClientsEnd()    { return local_clients_.end(); }
WorldRegion::ClientSet::iterator WorldRegion::nearbyClientsBegin() { return nearby_clients_.begin(); }
WorldRegion::ClientSet::iterator WorldRegion::nearbyClientsEnd()   { return nearby_clients_.end(); }



//----[  neighboringRegionsHaveNoNearbyClients  ]------------------------------
bool WorldRegion::neighboringRegionsHaveNoNearbyClients() const {
  bool all_empty = true;
  switch (number_of_neighbors_) { // unroll the loop...dunno if it's helpful though
    case 8: all_empty  = neighbors_[7]->nearby_clients_.empty();
    case 7: all_empty &= neighbors_[6]->nearby_clients_.empty();
    case 6: all_empty &= neighbors_[5]->nearby_clients_.empty();
    case 5: all_empty &= neighbors_[4]->nearby_clients_.empty();
    case 4: all_empty &= neighbors_[3]->nearby_clients_.empty();
    case 3: all_empty &= neighbors_[2]->nearby_clients_.empty();
    case 2: all_empty &= neighbors_[1]->nearby_clients_.empty();
    case 1: all_empty &= neighbors_[0]->nearby_clients_.empty();
      return all_empty;
    default: return nearby_clients_.empty();
  }
}

//
//
////----[  containsNoItems  ]----------------------------------------------------
//bool WorldRegion::containsNoItems() const {
//  return local_items_.empty();
//}



//----[  numberOfNearbyClients  ]----------------------------------------------
size_t WorldRegion::numberOfNearbyClients() const { return nearby_clients_.size(); }



//----[  numberOfNearbyActors  ]-----------------------------------------------
size_t WorldRegion::numberOfNearbyActors() const { return nearby_actors_.size(); }


//----[  containsPoint  ]------------------------------------------------------
bool WorldRegion::containsPoint(float x, float y) const {
  float dx = x-center_x_, dy = y-center_y_;
  return dx*dx+dy*dy < Evidyon::World::REGION_RADIUS_SQ;
}



//----[  strictlyContainsPoint  ]----------------------------------------------
inline bool WorldRegion::strictlyContainsPoint(int x, int y) const {
  return (x >= left_) &&
         (y >= top_) &&
         (x < (left_ + World::REGION_SIZE)) &&
         (y < (top_ + World::REGION_SIZE));
}



//----[  addReference  ]-------------------------------------------------------
void WorldRegion::addReference() {
  ++references_;
}



//----[  removeReference  ]----------------------------------------------------
void WorldRegion::removeReference() {
  --references_;
}



//----[  getRegionID  ]--------------------------------------------------------
World::RegionID WorldRegion::getRegionID() const {
  return region_id_;
}



//----[  getRegionX  ]---------------------------------------------------------
int WorldRegion::getRegionX() const { return region_x_; }


//----[  getRegionY  ]---------------------------------------------------------
int WorldRegion::getRegionY() const { return region_y_; }



//----[  uniquelyContainsArea  ]-----------------------------------------------
bool WorldRegion::uniquelyContainsArea(float x, float y, float radius_sq) const { return false; }



//----[  getMap  ]-------------------------------------------------------------
Map* WorldRegion::getMap() { return map_; }

}



#endif
#endif