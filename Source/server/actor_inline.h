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
#ifndef __ACTOR_INLINE_H__
#define __ACTOR_INLINE_H__
#pragma once



//----[  objectsCanMoveThrough  ]----------------------------------------------
bool ActorInstance::objectsCanMoveThrough() const {
  return isDead();
}



//----[  getController  ]------------------------------------------------------
ActorController* ActorInstance::getController() {
  return controller_;
}

void ActorInstance::setPosition(float x, float y) { update_mask_.position = 1; x_ = x; y_ = y; }
void ActorInstance::setDirection(float direction) { direction_ = direction; }
void ActorInstance::teleportTo(float x, float y) {
  setTeleportFlag();
  update_mask_.position = 1;
  x_ = x;
  y_ = y;
  for (int i = 0; i < Evidyon::ClientConfig::SYNC_GROUPS+1; ++i) {
    //sync_flags_[i].teleport = 1;
  }
}




//----[  setMemberOfFaction  ]-------------------------------------------------
inline void ActorInstance::setMemberOfFaction(Faction faction) {
  faction_ = faction;
}





//----[  setAction  ]----------------------------------------------------------
inline void ActorInstance::setAction(Actor::ActorAction action,
                                     Time::Milliseconds duration) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].action = action;
  sync_flags_info_[1].action = action;
  sync_flags_info_[2].action = action;
  sync_flags_info_[3].action = action;
  sync_flags_info_[4].action = action;
  sync_flags_info_[5].action = action;
  if (action != Actor::ACTORACTION_LEAP) {
    // the leap duration should not be limited
    duration = duration < MAX_ACTION_DURATION ? duration : MAX_ACTION_DURATION;
  }
  sync_flags_info_[0].action_duration = duration;
  sync_flags_info_[1].action_duration = duration;
  sync_flags_info_[2].action_duration = duration;
  sync_flags_info_[3].action_duration = duration;
  sync_flags_info_[4].action_duration = duration;
  sync_flags_info_[5].action_duration = duration;
}



//----[  changeMap  ]----------------------------------------------------------
inline void ActorInstance::changeMap(Map* map, float x, float y) {
  map_ = map;
  update_mask_.position = 1;
  x_ = x;
  y_ = y;
  setTeleportFlag();
  setMovingFlag(false);
}



//----[  changeMap  ]----------------------------------------------------------
inline void ActorInstance::changeMap(Map* map, Number x, Number z) {
  map_ = map;
  x_ = x.toFloat();
  y_ = z.toFloat();
  update_mask_.position = 1;
  setTeleportFlag();
  setMovingFlag(false);
}

bool ActorInstance::isAlive() const { return alive_; }
bool ActorInstance::isDead() const { return !alive_; }
bool ActorInstance::isInvisible() const { return false; }

WorldRegion* ActorInstance::getRegion() { return local_region_; }
WorldRegion* ActorInstance::getRegion() const { return local_region_; }
ActorGroup* ActorInstance::getGroup() { return group_; }
void ActorInstance::getPosition(float* x, float* y) const { *x = x_; *y = y_; }
void ActorInstance::getPosition(Number* x, Number* y) const { *x = x_; *y = y_; }
float ActorInstance::getDirection() const { return direction_; }


//----[  distanceToSq  ]-------------------------------------------------------
float ActorInstance::distanceToSq(const ActorInstance* other) const {
  float dx = other->x_-x_, dy = other->y_-y_;
  return dx*dx+dy*dy;
}



//----[  distanceToSq  ]-------------------------------------------------------
inline Number ActorInstance::distanceToSq(Number x, Number z) const {
  x -= Number(x_);
  z -= Number(y_);
  return (x * x) + (z * z);
}


//----[  distanceToSq  ]-------------------------------------------------------
float ActorInstance::distanceToSq(float x, float y) const {
  float dx = x-x_, dy = y-y_;
  return dx*dx+dy*dy;
}


//----[  getIDNumber  ]--------------------------------------------------------
Actor::ActorID ActorInstance::getIDNumber() const { return id_; }






//----[  withinViewOf  ]-------------------------------------------------------
bool ActorInstance::withinViewOf(const ActorInstance* client_actor) const {
  static const int SHIFT_SIGN = sizeof(int) * 8 - 1; // 31
  static const int VIEW_RANGE = 10 << 5;//<< Evidyon::Network::FLOAT_PACK_DECIMAL_BITS;
  todo("karl","subtract off for this actor's size");
  {
    const int delta = sync_data_.x_packed - client_actor->sync_data_.x_packed;
    int abs_diff = delta - (((delta) & (delta >> SHIFT_SIGN)) << 1);
    // abs_diff -= actor_size_in_squares_;
    if (abs_diff > VIEW_RANGE) return false;
  }
  {
    const int delta = sync_data_.y_packed - client_actor->sync_data_.y_packed;
    int abs_diff = delta - (((delta) & (delta >> SHIFT_SIGN)) << 1);
    // abs_diff -= actor_size_in_squares_;
    if (abs_diff > VIEW_RANGE) return false;
  }

  return true;
}





//----[  getName  ]------------------------------------------------------------
inline void ActorInstance::tellName(Client* client) { if (controller_) controller_->tellName(client); }



//----[  inSameGroupAs  ]------------------------------------------------------
inline bool ActorInstance::inSameGroupAs(ActorInstance* actor) const {
  return (actor == this) || ((group_ != 0) && (group_ == actor->getGroup()));
}



//----[  getSize  ]------------------------------------------------------------
inline float ActorInstance::getSize() const {
  return size_;
}



//----[  setSize  ]------------------------------------------------------------
inline void ActorInstance::setSize(float size) {
  size_ = size;
}




//----[  checkContactWith  ]---------------------------------------------------
inline float ActorInstance::checkContactWith(float x, float y, float radius) {
  float rhs = radius + size_;
  return distanceToSq(x, y) - rhs*rhs;
}



//----[  checkContactWith  ]---------------------------------------------------
inline float ActorInstance::checkContactWith(const ActorInstance* other_actor) {
  float rhs = (size_ + other_actor->size_);
  return distanceToSq(other_actor) - rhs*rhs;
}



//----[  scaleWithinRangeOf  ]-------------------------------------------------
inline float ActorInstance::scaleWithinRangeOf(float x, float y, float radius, float range) {
  float rhs = radius + size_ + range;
  return distanceToSq(x, y) - rhs*rhs;
}




//----[  setLevel  ]-----------------------------------------------------------
inline void ActorInstance::setLevel(Number level) { level_ = level; }


//----[  getLevel  ]-----------------------------------------------------------
inline Number ActorInstance::getLevel() const { return level_; }



//----[  changeHP  ]-----------------------------------------------------------
inline Number ActorInstance::changeHP(ActorInstance* source,
                                    bool magic,
                                    Number amount) {
  return controller_->changeHP(source, magic, amount);
}



//----[  actorIsOutOfRange  ]--------------------------------------------------
inline bool ActorInstance::actorIsOutOfRange(ActorInstance* actor) const {
  assert(actor);
  return (actor->map_ != map_) || locationIsOutOfRange(actor->x_, actor->y_);
}



//----[  locationIsOutOfRange  ]-----------------------------------------------
inline bool ActorInstance::locationIsOutOfRange(Number x, Number z) const {
  static const Number max_range = 11.0 * 11.0;
  return (FixedPoint::square<16>(Number(x_) - x) +
          FixedPoint::square<16>(Number(y_) - z)) > max_range;
}



//----[  isOnSameMapAs  ]------------------------------------------------------
inline bool ActorInstance::isOnSameMapAs(ActorInstance* other) {
  return other->getMap() == getMap();
}



//----[  separationDistanceTo  ]-----------------------------------------------
float ActorInstance::separationDistanceTo(float x, float y, float radius) {
// the distance approximation is:
//  dx - x coordinate difference
//  dy - y coordinate difference
//  a, b - radii
//  t - the separation distance
// dx*dx + dy*dy = (a+b+t)*(a+b+t)
// dx*dx + dy*dy = (a+b)*(a+b) + 2*(a+b)*t + t*t
// (dx*dx + dy*dy - (a+b)*(a+b)) / (2*(a+b)) <=> t
// (dx*dx + dy*dy) / (a+b) - (a+b) / (2) <=> t
// t*t should be small relative to 2*(a+b)*t for best results

// the actual distance is:
// (-2*(a+b) +- sqrt(4*(a+b)*(a+b)-4*((a+b)*(a+b)-(dx*dx+dy*dy)))) / 2
// -(a+b) +- sqrt((a+b)*(a+b)-((a+b)*(a+b)-(dx*dx+dy*dy)))
// -(a+b) +- (a+b)*sqrt(1-(1-(dx*dx+dy*dy)/((a+b)*(a+b))))
// -(a+b) +- sqrt( dx*dx+dy*dy )
  float dd = distanceToSq(x, y);// / 2.0f;
  float combined_radii = size_ + radius;
  return dd / combined_radii - combined_radii / 2.0f;
}



//----[  separationDistanceTo  ]-----------------------------------------------
float ActorInstance::separationDistanceTo(const ActorInstance* other_actor) {
  float dd = distanceToSq(other_actor);// / 2.0f;
  float combined_radii = size_ + other_actor->size_;
  return dd / combined_radii - combined_radii / 2.0f;
}




//----[  scaleWithinRangeOf  ]-------------------------------------------------
inline float ActorInstance::scaleWithinRangeOf(const ActorInstance* other_actor, float range) {
  // a = actor 1 radius
  // b = actor 2 radius
  // d = distance between actors
  // t = threashold distance
  // d - (a + b) < t
  // d < t + a + b
  // d*d < (t + a + b)^2
  // d*d = t*t + 2*t*(a+b) + (a+b)*(a+b)
  // 0 = t*t + 2*t*(a+b) * (a+b)*(a+b) - d*d
  // t1,t2 = (-2*(a+b) +- sqrt(4*(a+b)*(a+b) - 4*1*((a+b)*(a+b) - d*d)))/2
  // t1,t2 = (-2*(a+b) +- sqrt(4*(a+b)*(a+b)*(1 - 1) + 4*1*d*d))/2
  // t1,t2 = (-2*(a+b) +- sqrt(4*d*d))/2
  // t1,t2 = (-(a+b) +- d)
  // t1 = d - (a+b)
  float rhs = (range + size_ + other_actor->size_);
  return distanceToSq(other_actor) - rhs*rhs;
}


//----[  setLivingFlag  ]------------------------------------------------------
inline void ActorInstance::setLivingFlag(bool living) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].living = living;
  sync_flags_info_[1].living = living;
  sync_flags_info_[2].living = living;
  sync_flags_info_[3].living = living;
  sync_flags_info_[4].living = living;
  sync_flags_info_[5].living = living;
}


//----[  setMovingFlag  ]------------------------------------------------------
inline void ActorInstance::setMovingFlag(bool moving) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].moving = moving;
  sync_flags_info_[1].moving = moving;
  sync_flags_info_[2].moving = moving;
  sync_flags_info_[3].moving = moving;
  sync_flags_info_[4].moving = moving;
  sync_flags_info_[5].moving = moving;
}


//----[  setCombatFlag  ]------------------------------------------------------
inline void ActorInstance::setCombatFlag(bool combat) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].combat = combat;
  sync_flags_info_[1].combat = combat;
  sync_flags_info_[2].combat = combat;
  sync_flags_info_[3].combat = combat;
  sync_flags_info_[4].combat = combat;
  sync_flags_info_[5].combat = combat;
}


//----[  setTeleportFlag  ]----------------------------------------------------
inline void ActorInstance::setTeleportFlag() {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].teleport = true;
  sync_flags_info_[1].teleport = true;
  sync_flags_info_[2].teleport = true;
  sync_flags_info_[3].teleport = true;
  sync_flags_info_[4].teleport = true;
  sync_flags_info_[5].teleport = true;
}


//----[  setWadingFlag  ]------------------------------------------------------
inline void ActorInstance::setWadingFlag(bool wading) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].wading = wading;
  sync_flags_info_[1].wading = wading;
  sync_flags_info_[2].wading = wading;
  sync_flags_info_[3].wading = wading;
  sync_flags_info_[4].wading = wading;
  sync_flags_info_[5].wading = wading;
}


//----[  setFallingFlag  ]-----------------------------------------------------
inline void ActorInstance::setFallingFlag(bool falling) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].falling = falling;
  sync_flags_info_[1].falling = falling;
  sync_flags_info_[2].falling = falling;
  sync_flags_info_[3].falling = falling;
  sync_flags_info_[4].falling = falling;
  sync_flags_info_[5].falling = falling;
}


//----[  setSwimmingFlag  ]----------------------------------------------------
inline void ActorInstance::setSwimmingFlag(bool swimming) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].swimming = swimming;
  sync_flags_info_[1].swimming = swimming;
  sync_flags_info_[2].swimming = swimming;
  sync_flags_info_[3].swimming = swimming;
  sync_flags_info_[4].swimming = swimming;
  sync_flags_info_[5].swimming = swimming;
}


//----[  setSwimmingFlag  ]----------------------------------------------------
inline void ActorInstance::setFlyingLevitatingFlag(bool flying_levitating) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].flying_levitating = flying_levitating;
  sync_flags_info_[1].flying_levitating = flying_levitating;
  sync_flags_info_[2].flying_levitating = flying_levitating;
  sync_flags_info_[3].flying_levitating = flying_levitating;
  sync_flags_info_[4].flying_levitating = flying_levitating;
  sync_flags_info_[5].flying_levitating = flying_levitating;
}


//----[  setLeapingFlag  ]-----------------------------------------------------
inline void ActorInstance::setLeapingFlag(bool leaping) {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].leaping = leaping;
  sync_flags_info_[1].leaping = leaping;
  sync_flags_info_[2].leaping = leaping;
  sync_flags_info_[3].leaping = leaping;
  sync_flags_info_[4].leaping = leaping;
  sync_flags_info_[5].leaping = leaping;
}

//----[  setBloodFlag  ]-------------------------------------------------------
inline void ActorInstance::setBloodFlag() {
  assert(Evidyon::ClientConfig::SYNC_GROUPS == 5);
  sync_flags_info_[0].blood = true;
  sync_flags_info_[1].blood = true;
  sync_flags_info_[2].blood = true;
  sync_flags_info_[3].blood = true;
  sync_flags_info_[4].blood = true;
  sync_flags_info_[5].blood = true;
}


#endif