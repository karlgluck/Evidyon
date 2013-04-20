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


namespace kg {

//----[  Array, SafeArray  ]---------------------------------------------------
template <typename _Element, const size_t _SIZE> struct CLASSNAME(Array) {
  typedef _Element Element;

  // safe version has one extra element that is returned on invalid index access
  static const size_t SIZE = _SIZE IFSAFE(+ 1);

  CLASSNAME(Array)() : count(0) {}
  Element elements[SIZE];
  size_t count;

  inline Element& operator[](size_t i) {
    SELECTSAFE(confirm(i < count) else { return elements[SIZE-1]; }, assert(i<count));
    return elements[i];
  }
  inline Element* operator()() { return elements; }
  inline const Element& operator[](size_t i) const {
    SELECTSAFE(confirm(i < count) else { return elements[SIZE-1]; }, assert(i<count));
    return elements[i];
  }
  inline const Element* operator()() const { return elements; }

  inline bool canHoldMore() const { return count < SIZE; }
  inline bool isFull() const { confirm(count <= SIZE); return count == SIZE; }
  inline bool isEmpty() const { return count == 0; }
  inline void ensureEmpty() { confirm(count == 0) else { count = 0; } }

#if SAFE
  inline bool add(Element& element, size_t* index) {
    confirm(index) else return false;
    if (!canHoldMore()) return false;
    elements[count] = element;
    *index = count;
    ++count;
    return true;
  }
  inline bool add(size_t* index) {
    confirm(index) else return false;
    if (!canHoldMore()) return false;
    *index = count++;
    return true;
  }
#endif
#if NOTSAFE
  inline size_t add(Element& element) {
    confirm(canHoldMore());
    elements[count] = element;
    return count++;
  }
  inline Element& add() {
    confirm(canHoldMore());
    return elements[count++];
  }
  inline Element& add(size_t* index) {
    confirm(canHoldMore());
    confirm(index);
    *index = count;
    return elements[count++];
  }
#endif

  inline void swapRemove(size_t index) {
    confirm(index < count) else return;
    elements[index] = elements[--count];
  }

  inline void swapErase(size_t index, const Element& invalid) {
    confirm(index < count) else return;
    elements[index] = elements[--count];
    elements[count] = invalid;
  }

  inline void swapRemoveMemcpy(size_t index) {
    confirm(index < count) else return;
    memcpy(&elements[index], &elements[--count], sizeof(Element));
  }

  inline size_t indexOf(const Element* element) const {
    confirm(element >= elements && element < elements + count) else return SIZE;
    return element - elements;
  }

  inline bool validIndex(size_t index) const { return index < count; }

  inline bool containsByEquality(const Element& element) const {
    return linearFindByEquality(element) != SIZE;
  }

  inline size_t swapRemoveAllByLinearFindByEquality(const Element& element) {
    size_t index;
    size_t removed = 0;
    while (count > (index = linearFindByEquality(element))) {
      swapRemove(index);
      ++removed;
    }
    return removed;
  }

  inline size_t linearFindByEquality(const Element& element) const {
    size_t index = count;
    while (index > 0) {
      if (element == elements[--index]) return index;
    }
    return SIZE;
  }

  // returns 'true' if all callbacks returned 'true'; false will not break the loop
  template <typename _C> bool forAllElements(_C* _c, bool (_C::*callback)(Element*)) {
    bool r = true;
    for (size_t i = count; i > 0;) {
      r = (((*_c).*callback)(&elements[--i])) && r;
    }
    return r;
  }

  // returns 'true' when the first callback returns 'true'.  Returns 'false'
  // otherwise.
  template <typename _C> bool forEachUntilTrue(_C* _c, bool (_C::*callback)(Element*)) {
    for (size_t i = count; i > 0;) {
      if (((*_c).*callback)(&elements[--i])) return true;
    }
    return false;
  }
};



}