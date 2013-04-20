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
#ifndef __DCXVECTOR3_H__
#define __DCXVECTOR3_H__



namespace dcx {

    // Use the DirectCraft basic namespace
    using namespace dc;

    /**
     * Stores a set of three variables of the templated type
     */
    template <typename T> class dcxVector3 : public dcResource<dcxVector3<T>>
    {
        public:

            /**
             * Initializes this resource
             */
            dcxVector3()
            {
                member( "x", &myX );
                member( "y", &myY );
                member( "z", &myZ );
            }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            T* getX() { return &myX; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            T* getY() { return &myY; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            T* getZ() { return &myZ; }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            const T* getX() const { return &myX; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            const T* getY() const { return &myY; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            const T* getZ() const { return &myZ; }

            /**
             * Obtains the X, Y, or Z member of the vector by its index (0, 1 or 2)
             * @return The specified member, or null (0) if the index is out of range.
             */
            T* getByIndex( size_t index )
            {
                switch( index )
                {
                    case 0:  return &myX;
                    case 1:  return &myY;
                    case 2:  return &myZ;
                    default: return 0;
                }
            }

        public:

            /**
             * Returns the name of this class type
             */
            static std::string staticTypeName()
            {
                return "Vector3<" + T::staticTypeName() + ">";
            }


        protected:

            /// First component of the vector
            T myX;

            /// Second component of the vector
            T myY;

            /// Third component of the vector
            T myZ;
    };

}



#endif
