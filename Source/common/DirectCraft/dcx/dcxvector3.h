//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
