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
//-----------------------------------------------------------------------------
// File:    dcxguaranteedmemorypool.h
// Desc:    A guaranteed memory pool will always provide a returned element, even if doing so
//          requires allocating from a non-contiguous block of memory.  This prevents pools of
//          this type from having iterators.
//-----------------------------------------------------------------------------
#ifndef __DCXGUARANTEEDMEMORYPOOL_H__
#define __DCXGUARANTEEDMEMORYPOOL_H__


namespace dcx {

/**
 * This class is used to allocate a block of memory of objects that are frequently allocated
 * and freed.
 */
template <typename O, const size_t PoolSize> class dcxGuaranteedMemoryPool {
  typedef dcx::dcxMemoryPool<O, PoolSize> CoreMemoryPool;

public:

  /**
   * Allocates memory for this memory pool
   */
  bool create() {
    return pool_.create();
  }

  /**
   * Frees the memory used for this class
   */
  void destroy() {
    pool_.destroy();
  }

  /**
   * Returns true if there are no outstanding allocations
   */
  bool empty() const {
    return pool_.empty();
  }

  /**
   * Gets a new memory segment from the pool
   */
  O* acquire() {
    O* object = pool_.acquire();
    if (!object) object = new O();
    return object;
  }

  /**
   * Gets rid of the given element from the memory pool
   */
  bool release( O* element ) {
    if (pool_.release(element)) return true;
    delete element;
    return true;
  }

  size_t numberOfAllocatedEntries() {
    return pool_.numberOfAllocatedEntries();
  }

  size_t numberOfFreeEntries() {
    return pool_.numberOfFreeEntries();
  }


protected:

  CoreMemoryPool pool_;
};


}



#endif