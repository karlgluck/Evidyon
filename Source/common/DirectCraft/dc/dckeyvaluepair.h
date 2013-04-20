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
#ifndef __KEYVALUEPAIR_H__
#define __KEYVALUEPAIR_H__


// todo: add comments

namespace dc {


    /**
     * Associates two values with each other
     */
    template <typename K, typename V> class dcKeyValuePair : public dcResource<dcGenericResource,dcKeyValuePair<K,V>>
    {
        public:

            /// Key resource type
            typedef K Key;

            /// Value resource type
            typedef V Value;

        public:


            /**
             * Initializes this resource
                 */
            dcKeyValuePair()
            {
                member( Key::staticTypeName(), &myKey );
                member( Value::staticTypeName(), &myValue );
            }

            /**
             * 
             */
            Key* getKey()
            {
                return &myKey;
            }

            /**
             * 
             */
            Value* getValue()
            {
                return &myValue;
            }

            /**
             * Compares this pair with the given key
             */
            int compareKey( const Key* key ) const
            {
                return myKey.compareTo( key );
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName()
            {
                return std::string("Key-Value Pair of ") + Key::staticTypeName() + std::string(" to ") + Value::staticTypeName();
            }

        protected:

            /// The key in this pair
            Key myKey;

            /// The value to which the key maps
            Value myValue;
    };

}


#endif