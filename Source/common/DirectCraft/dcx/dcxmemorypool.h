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
#ifndef __DCXMEMORYPOOL_H__
#define __DCXMEMORYPOOL_H__

#include <assert.h>

namespace dcx {

/**
 * This class is used to allocate a block of memory of objects that are frequently allocated
 * and freed.
 */
template <typename O, const size_t PoolSize> class dcxMemoryPool
{
    public:

        /**
         * This class is used to iterate through elements in the memory pool.  Since valid
         * elements are not necessarily sequential, it skips those that are invalid.
         */
        class Iterator
        {
            public:

                /**
                 * Initializes this class
                 */
                Iterator( dcxMemoryPool* pool )
                {
                    myMemoryPool = pool;
                    myCurrentEntry = 0;

                    // If location 0 of the pool doesn't have a valid entry,
                    // advance to the first valid entry
                    if( !myMemoryPool->myAvailabilityMask.get( 0 ) )
                        advance();
                }

                /**
                 * Obtains a pointer to the element that is currently referenced
                 */
                const O* get() const
                {
                    return myMemoryPool->get( myCurrentEntry );
                }

                /**
                 * Obtains a pointer to the element that is currently referenced
                 */
                O* get()
                {
                    return myMemoryPool->get( myCurrentEntry );
                }

                /**
                 * Determines whether or not there is another element in the pool
                 */
                bool hasNext() const
                {
                    return myMemoryPool->isValid() && myCurrentEntry < PoolSize;
                }

                /**
                 * Moves this iterator to the next element in the pool
                 */
                void advance()
                {
                    while( myCurrentEntry < PoolSize && !myMemoryPool->myAvailabilityMask.get( ++myCurrentEntry ) );
                }

                /**
                 * Erases the entry at the current memory pool location
                 */
                void remove()
                {
                    myMemoryPool->release( get() );
                }


            protected:

                /// The iterator's source memory pool
                dcxMemoryPool* myMemoryPool;

                /// The location of this iterator
                size_t myCurrentEntry;
        };
        
    public:

        /**
         * Initializes valids in this class
         */
        dcxMemoryPool()
        {
            myAllocatedMemory = 0;
            myAllocatedMemoryEnd = 0;
            myAvailabilityMask.setAll();
        }

        /**
         * Makes sure this pool's memory is freed
         */
        ~dcxMemoryPool()
        {
            destroy();
        }

        /**
         * Allocates memory for this memory pool
         */
        bool create()
        {
            if( isValid() ) return true;

            myAllocatedMemory = new O[PoolSize];
            if( !myAllocatedMemory ) return false;
            myAllocatedMemoryEnd = myAllocatedMemory + sizeof(O) * PoolSize;

            myAvailabilityMask.clearAll();

            return true;
        }

        /**
         * Frees the memory used for this class
         */
        void destroy()
        {
            if( myAllocatedMemory ) delete[] myAllocatedMemory;
            myAllocatedMemory = 0;
            myAllocatedMemoryEnd = 0;
            myAvailabilityMask.setAll();
        }

        /**
         * Returns true if there are no outstanding allocations
         */
        bool empty() const {
            if (!isValid()) return true;
            return myAvailabilityMask.allClear();
        }

        /**
         * Gets a new memory segment from the pool
         */
        O* acquire()
        {
            // Make sure that this class is allocated correctly
            if( !isValid() && !create() ) return 0;

            // The structure to return
            O* returnValue = 0;

            // Find an available entry
            for( size_t i = 0; i < PoolSize; ++i )
            {
                if( myAvailabilityMask.get(i) == 0 )
                {
                    myAvailabilityMask.toggle(i);
                    return &myAllocatedMemory[i];
                }
            }

            // Couldn't find an entry
            return 0;
        }

        /**
         * Gets rid of the given element from the memory pool
         */
        bool release( O* element )
        {
            // Make sure that this pool is valid
            if( !isValid() ) return false;

            // Validate the element pointer
            size_t elementIndex = indexOf( element );
            if( !myAvailabilityMask.get( elementIndex ) ) return false;

            // Make this element availabile for use.
            myAvailabilityMask.clear( elementIndex );
            return true;
        }

        // Forces the memory pool to release the lock on all elements.  This
        // method can be abused, so be absolutely sure that there are no
        // outstanding references to objects in the pool before calling it.
        void releaseAll() {
          myAvailabilityMask.clearAll();
          // In debug mode only, reallocate so that if the user accidentally
          // accesses dangling pointers, an exception is thrown.
#if defined(DEBUG)||defined(_DEBUG)
          destroy();
          assert(create());
#endif
        }

        /**
         * Gets the element at the given index, if it is valid.
         */
        O* get( size_t index )
        {
            if( !isValid() || !myAvailabilityMask.get( index ) ) return 0;
            return &myAllocatedMemory[index];
        }

        /**
         * Gets the element at the given index, if it is valid.
         */
        const O* get( size_t index ) const
        {
            if( !isValid() || !myAvailabilityMask.get( index ) ) return 0;
            return &myAllocatedMemory[index];
        }

        /**
         * Gets the index of the given element, if it exists in this memory pool.  Unless
         * the return value of this method is (unsigned)(-1), element == get( indexOf( element ) )
         * will always be true.
         */
        size_t indexOf( O* element )
        {
            if( !isValid() || element < myAllocatedMemory || element >= myAllocatedMemoryEnd ) return (size_t)(-1);
            size_t differenceIndex = (size_t)(((unsigned char*)element) - ((unsigned char*)myAllocatedMemory));
            if( differenceIndex % sizeof(O) ) return (size_t)(-1);
            return differenceIndex / sizeof(O);
        }

        size_t numberOfAllocatedEntries() {
          return myAvailabilityMask.countBitsSet();
        }

        size_t numberOfFreeEntries() {
          return PoolSize - myAvailabilityMask.countBitsSet();
        }


    protected:

        /**
         * Make sure that the memory pool is valid
         */
        bool isValid() const
        {
            return myAllocatedMemory && myAllocatedMemoryEnd;
        }


    protected:

        /// List of the memory used in this pool
        O* myAllocatedMemory;
        O* myAllocatedMemoryEnd;

        /// The availability of various entries
        dcxBitMask<PoolSize> myAvailabilityMask;
};


}



#endif