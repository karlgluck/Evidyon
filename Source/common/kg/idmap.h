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
#ifndef __COMMON_KG_KG_H__
#error "must be included by kg.h"
#endif

#include <hash_map>

namespace kg {

//----[  IDMap  ]--------------------------------------------------------------
template <typename _ID, typename _Object> class CLASSNAME(IDMap) {
public:
  typedef _ID ID;
  typedef _Object Object;
  typedef stdext::hash_map<ID,Object*> Map;

public:
  CLASSNAME(IDMap)() : next_id_(0) {}

  void clear() {
    map_.clear();
  }

  // calls either a function pointer or operator () on a class for each
  // element in the list before clearing the map
  template <typename _C> void clear(_C* c) {
    for (Map::iterator i = map_.begin(); i != map_.end(); ++i) (*c)(i->second);
    map_.clear();
  }

  // invokes a callback on each element before clearing the list
  template <typename _C> void clear(_C* _c, void (_C::*callback)(Object*)) {
    for (Map::iterator i = map_.begin(); i != map_.end(); ++i) {
      (((*_c).*callback)(i->second));
    }
    map_.clear();
  }

  void setNextID(ID id) {
    next_id_ = id;
  }

  ID add(Object* object) {
    ID id = ++next_id_;
    confirm(map_.find(id) == map_.end()) else {
      while (map_.find(++id) != map_.end());
      next_id_ = id + 1;
    }
    Map::_Pairib result = map_.insert(Map::value_type(id, object));
    return id;
  }

  bool doesNotContain(ID id) const {
    return map_.find(id) == map_.end();
  }
  
  void erase(ID id) { map_.erase(id); }
  void erase(Object* object) {
    erase(idOf(object));
  }

  ID idOf(Object* object) {
    for (Map::const_iterator i = map_.begin(); i != map_.end(); ++i) {
      if (i->second == object) return i->first;
    }
    return 0; // 0 is never generated
  }

  //Object* operator [](ID id) {
  //  Map::iterator i = map_.find(id);
  //  assert(i != map_.end());
  //  return i->second;
  //}

  Object* get(ID id) {
    Map::iterator i = map_.find(id);
    return i == map_.end() ? 0 : i->second;
  }

  const Object* get(ID id) const {
    Map::const_iterator i = map_.find(id);
    return i == map_.end() ? 0 : i->second;
  }

  Object* insert(ID id, Object* object) {
    confirm(map_.find(id) == map_.end()) else return 0;
    map_.insert(Map::value_type(id,object));
    return object;
  }

  //// returns 'true' if all callbacks returned 'true'; false will break the loop
  //template <typename _C> bool forEachObjectUntilFalse(_C* _c, bool (_C::*callback)(ID,Object*)) {
  //  for (Map::iterator i = map_.begin(); i != map_.end(); ++i) {
  //    if (!(((*_c).*callback)(i->first, i->second))) return false;
  //  }
  //  return true;
  //}

  // returns 'true' if all callbacks returned 'true'; false will not break the loop
  template <typename _C> bool forAllObjects(_C* _c, bool (_C::*callback)(ID,Object*)) {
    bool r = true;
    for (Map::iterator i = map_.begin(); i != map_.end(); ++i) {
      r = (((*_c).*callback)(i->first, i->second)) && r;
    }
    return r;
  }

private:
  Map map_;
  ID next_id_;
};


}