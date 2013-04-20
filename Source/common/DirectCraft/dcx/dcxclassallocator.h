//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
// Provides a simple class which, when instantiated, allocates a new instance of another
// class and, when destructed, frees the class it created.
#ifndef __DCXCLASSALLOCATOR_H__
#define __DCXCLASSALLOCATOR_H__

namespace dcx {

    /**
     * Allocates a class when constructed, deallocates it when destructed.  This common pattern
     * is used in functions that get global objects.  The object of type T can be defined as
     * a static entity using the dcxClassAllocator, and it is guaranteed to be freed.  This uses
     * a much smaller executable memory footprint then would a solid static class.
     *
     * Template parameters:
     *      T - class type to allocate
     *
     * Example:
     *
     *      class Foo { ... };
     *
     *      Foo* getGlobalFoo()
     *      {
     *        static dcxClassAllocator<Foo> allocator;
     *        return allocator.getClass();
     *      }
     *
     */
    template <typename T> class dcxClassAllocator
    {
        public:

            /**
             * Initializes the class allocator by creating a new instance of the templated type
             */
            dcxClassAllocator()
            {
                myClass = new T();
            }

            /**
             * Frees the memory that is managed by this class
             */
            ~dcxClassAllocator()
            {
                delete myClass;
            }

            /**
             * Obtains the class this allocator handles
             */
            T* getClass()
            {
                return myClass;
            }

            /**
             * Calls a method of the allocated class
             */
            T& operator -> ()
            {
                return *myClass;
            }

        protected:

            /// The memory for the class this allocator manages
            T* myClass;
    };

}



#endif