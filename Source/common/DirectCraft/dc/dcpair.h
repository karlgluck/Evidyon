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
#ifndef __PAIR_H__
#define __PAIR_H__


namespace dc {

    /**
     * Associates two values with each other
     */
    template <typename A, typename B> class dcPair : public dcResource<dcPair<A,B>>
    {
        public:

            /// Key resource type
            typedef A First;

            /// Value resource type
            typedef B Second;

            /// The basic resource type

        public:

            /**
             * Initializes this resource
                 */
            dcPair()
            {
                member( A::staticTypeName(), &myFirst );
                member( B::staticTypeName(), &mySecond );
            }

            /**
             * 
             */
            First* getFirst()
            {
                return &myFirst;
            }

            /**
             * 
             */
            Second* getSecond()
            {
                return &mySecond;
            }

            /** 
             * Compares this pair to another resource
             */
            int compareTo( const dcGenericResource* other ) const
            {
                // If the types don't match, use the default type comparator
                if( !other || getTypeID() != other->getTypeID() )
                    return dcGenericResource::compareTo( other );

                // Cast the parameter
                dcPair<A,B>* pair = (dcPair<A,B>*)other;

                // Check the first elements
                int firstCompare = myFirst.compareTo( &pair->myFirst );
                if( firstCompare != 0 ) return firstCompare;

                // The first elements were equal, so compare the second ones
                return mySecond.compareTo( &pair->mySecond );
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName()
            {
                return std::string("Pair of ") + A::staticTypeName() + std::string(" to ") + B::staticTypeName();
            }


        protected:

            /// The first value
            First myFirst;

            /// The second value
            Second mySecond;
    };
}


#endif