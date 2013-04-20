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
#ifndef __DCLIST_H__
#define __DCLIST_H__



namespace dc {

    /**
     * Stores a list of elements of the given type
     */
    template <typename T> class dcList : public dc::dcCollection<dcList<T>>
    {
        public:

            class Element : public dcResource<Element,T>
            {
                public:

                    /**
                     * Initializes this element
                     */
                    Element()
                    {
                    }


                public:

                    /**
                     * Gets the name of this type of resource
                     */
                    static std::string staticTypeName()
                    {
                        return "Element of " + dcList<T>::staticTypeName();
                    }
            };

            /// Define the type that references an element in this list
            typedef dcReference<typename dcList<T>::Element> Reference;


        public:

            /**
             * Initializes this resource
             */
            dcList()
            {
            }

            /**
             * Determines whether or not this collection allows duplicate entries to
             * be held internally.
             * @return Flag value
             */
            bool allowDuplicates() const
            {
                return true;
            }

            /**
             * Determines whether or not the derived collection type requires that its
             * entries be kept sorted.
             */
            bool keepSorted() const
            {
                return false;
            }

        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName() { return std::string("List of ") + T::staticTypeName(); }
    };

}



#endif