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
#ifndef __DCTABLE_H__
#define __DCTABLE_H__



namespace dc {

    /**
     * A table keeps track of resources that implement a given interface.  Something similar
     * can be achieved by using dcList<dcInterface<I>> but this is more compact and useful, as it
     * provides declarations and methods that naturally arise from this situation.
     *  I - The dcGenericResource-implementing interface type.  This class must have its
     *      implementing classes 'T' linked via a static dcClass<T>::Implements<I> declaration.
     */
    template <typename I> class dcTable : public dcCollection<dcTable<I>>
    {
        public:

            /**
             * An entry in the dcTable collection.
             */
            class Element : public dcResource<Element,dcInterface<I>>
            {
                public:

                    /**
                     * Gets the type name of this class
                     */
                    static std::string staticTypeName()
                    {
                        return "Table Entry " + dcInterface<I>::staticTypeName();
                    }
            };


        public:

            /// The resource interface that is referenced in the table
            typedef I ResourceType;

            /// The dcInterface resource that actually stored in the table
            typedef dcInterface<I> InterfaceType;

            /// Define the type that references an element in this table
            typedef dcReference<typename dcTable<I>::Element> Reference;


        public:

            /**
             * Generates a new resource that implements this table's interface and returns a
             * reference to that resource.
             */
            template <typename C> C* create( std::string entryName )
            {
                // Create a new element in the collection
                Element* element = (Element*)generateElement( entryName );

                // Create an interface of the specified type
                C* resourceInterface = element->implement<C>();

                // Return the interface
                return resourceInterface;
            }


        public:

            /**
             * Determines whether or not this collection allows duplicate entries to
             * be held internally.
             * TODO: this flag is defunct...
             * @return Flag value
             */
            bool allowDuplicates() const
            {
                return true;
            }

            /**
             * Determines whether or not the derived collection type requires that its
             * entries be kept sorted.
             * TODO: this flag is defunct...
             */
            bool keepSorted() const
            {
                return true;
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName() { return std::string("Table of ") + I::staticTypeName(); }
    };

}



#endif