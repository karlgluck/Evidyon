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
#ifndef __DCCOLLECTION_H__
#define __DCCOLLECTION_H__

#include <list>

namespace dc {

    /**
     * A resource that contains a group of other resources.  Set, dcMap, dcList, Stack, Queue and
     * all other grouping-types drive from this class.  Generics cannot be instantiated.
     * TODO: idea!  create a GenericElement in dcGenericCollection that has the setName method
     * instead of implementing it in dcGenericResource.
     */
    class dcGenericCollection : public dcGenericResource
    {
        protected:

            /**
             * Initializes this resource
             */
            dcGenericCollection();

            /**
             * Erases internal variables when the class is destructed
             */
            virtual ~dcGenericCollection();


        public:

            /**
             * Returns 'true' because all subclasses will implement dcGenericCollection
             */
            bool isCollection() const;

            /**
             * Returns 'false' because no subclass will be an interface.   This
             * delcaration is not strictly necessary, but it prevents subclasses from
             * overriding this method erroneously.
             */
            bool isInterface() const;

            /**
             * Saves this resource
             * @param stream The stream into which to save
             * @return Whether or not the operation succeeded
             */
            virtual bool save( dcStreamOut* stream );

            /**
             * Loads this resource.  If this doesn't succeed, the collection will be empty.
             * @param stream The stream from which to load
             * @return Whether or not the operation succeeded
             */
            virtual bool load( dcStreamIn* stream );


        public:

            /**
             * Sorts the elements in this collection via the compareTo method
             */
            void sort();

            /**
             * Sorts the elements in this collection by name
             */
            void sortByName();

            /**
             * Erases all entries in the collection for which entry->compareTo(resource) == 0.  Note
             * that if a resource finds itself (entry == resource), it will NOT erase the memory for
             * the resource.  It will, however, remove itself from the collection.
             */
            void eraseEqual( dcGenericResource* resource );

            /**
             * Determines how many elements are in this collection
             */
            size_t size();

            /**
             * Erases everything in this collection
             */
            void clear();

            /**
             * Creates a new resource of the type this collection contains
             * @param name The name of the new resource to create
             * @return A new instance of the resource
             */
            virtual dcGenericResource* generateElement( std::string name ) = 0;

            /**
             * Frees the memory used by an element.  This element will be
             * removed if it exists in the list.  This method cannot be virtual
             * because it is used in the destructor to free memory; therefore, all
             * resources should be allocated using the "new" operator, because this
             * method deallocates with "delete".
             */
            void releaseElement( dcGenericResource* element );

            /**
             * Finds an element of this collection 'e' for which e->compareTo( elementLike ) == 0.
             * The definition of this comparison depends on what's actually in the collection; for
             * example, this method can search for a key value if the collection contains key-
             * value pairs.
             * @param elementLike The element to use for searching
             * @param start The index at which to begin searching
             * @param found The index at which the result was found (optional)
             */
            dcGenericResource* find( const dcGenericResource* elementLike, size_t start, size_t* found );

            /**
             * Finds an element if this is known to be a collection with special properties.  The
             * elements must be sorted via the sort() function and must be unique, such that for
             * only one or zero elements is compareTo( elementLike ) == 0.
             */
            dcGenericResource*  binarySearch( const dcGenericResource* elementLike );

            /**
             * Determines whether or not this collection allows duplicate entries to
             * be held internally.
             * @return Flag value
             */
            virtual bool allowDuplicates() const = 0;

            virtual dcGenericResource* acquireMember(const char* member_name,
                                                     const char* type,
                                                     size_t index_hint);

        protected:

            /**
             * Checks to see if this entry type can be added to the collection
             * @return Whether or not add() will succeed
             */
            bool canAdd( dcGenericResource* resource );

            /**
             * Casts a generic resource interface into this resource's type.
             * This method will throw an exception if the variable is of a
             * different type.
             */
            dcGenericCollection* typecast( dcGenericResource* variable );

            /**
             * Determines whether or not the derived collection type requires that its
             * entries be kept sorted.
             */
            virtual bool keepSorted() const = 0;
    };







    /**
     * A resource that contains a group of other resources.  Set, dcMap, dcList, dcStack, dcQueue and
     * all other grouping-types drive from this class.  Generics cannot be instantiated.
     * D - the derived collection type
     * P - the parent type (should 
     */
    template <typename D, typename P = dcGenericCollection> class dcCollection : public dcResource<D,P>
    {
        protected:

            /**
             * Initializes this resource
             */
            dcCollection()
            {
            }


        public:

            /**
             * Creates a new resource as a member of this list and returns an editable reference
             */
            dcGenericResource* generateElement( std::string name )
            {
                // Return the reference
                D::Element* element = new D::Element();
                member( name, element );

                // Return the element that was created
                return element;
            }
    };

}


#endif