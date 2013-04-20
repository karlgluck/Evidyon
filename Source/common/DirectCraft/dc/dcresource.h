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
#ifndef __DCRESOURCE_H__
#define __DCRESOURCE_H__

#include <string>
#include <list>
#include <vector>
#include "dcstream.h"
#include "dcdebug.h"
#include "dcresourcemanager.h"

namespace dc {

    class dcGenericInterface;
    struct dcAction;
    class dcStream;
    class dcStreamOut;
    class dcStreamIn;
    class dcResourceStorage2;

    /**
     * All resources derive from this type and implement its methods
     */
    class dcGenericResource
    {
        friend class dcGenericInterface;
        friend class dcResourceStorage2;

        public:

            /// This type is used to list resources
            typedef std::vector<dcGenericResource*> Array;

            /// Defines a location from the root resource (the one with a null parent)
            /// The first value is the string name of the resource in the path, and is
            /// always correct; the second is the index hint of that resource within
            /// its parent.  This can be incorrect for collections, but in cases where
            /// two resources can have the same name (non-collections) the index is
            /// guaranteed to be valid, and must be observed.
            typedef std::list<std::pair<std::string,size_t>> Path;

            /// This character builds the path to a resource and is invalid to use in
            /// the name of a resource.
            static const char PATH_SEPARATOR = '/';

        protected:

            /**
             * Initializes this resource
             */
            dcGenericResource();


        public:

            /**
             * Allow sub-classes to run their destructors
             */
            virtual ~dcGenericResource();

            /**
             * Gets the list of members of this resource.  If this is a collection,
             * it holds all of the entries in that collection.  If it is an interface,
             * the single entry will be the implementing type.  If this is a generic
             * resource, members are properties of this resource.
             * @return The members that this resource contains
             */
            const Array& getMembers() const;

            /**
             * Changes the name of this resource.  This method only works if the resource
             * has no owner, or the owner is a collection.  If the owner is a collection
             * and a resource with the given name already exists, this method will fail.
             * @param name New resource name
             * @param updateReferences Whether or not to update the references to this element
             * @return Whether or not the name could be changed
             */
            bool setName( const std::string& name, bool updateReferences = true );

            /**
             * Finds out what this resource is called
             * @return The name associated with this resource when it was created
             */
            const std::string& getName() const;

            /**
             * Gets the path from the root resource to this resource
             * @param path Destination structure for the constructed path
             */
            void getPath( Path* path ) const;

            /**
             * Gets the path to this resource, starting at the resource with no owner (root)
             * @return The resource path
             */
            std::string getPathString() const;

            // Returns the number of bytes consumed in the buffer
            size_t getPathString(char* buffer, size_t length) const;

            /**
             * Determines the owner of this resource
             * @return The resource's owner
             */
            dcGenericResource* getOwner() const;

            /**
             * Gets the full type name of this resource
             */
            const std::string& getTypeName() const;

            /**
             * Obtains the unique type ID for this resource
             */
            unsigned long getTypeID() const;

            /**
             * Gets the list of actions that can be performed on this resource.  The last
             * entry in this list has both members set to 0 / null.
             * @return dcGenericResource action list
             */
            dcAction* getActions() const;

            /**
             * Returns the index of this resource in its parent.  This
             * method returns -1 if no parent exists.
             */
            int getIndex() const;

            //template <typename T> exactCast() {
            //  assert(); // be sure this conversion is valid by comparing typeID
            //}

        public:

          virtual dcGenericResource* acquireMember(const char* member_name,
                                                   const char* type,
                                                   size_t index_hint);
          virtual void serialize(std::string* string);
          void interpret(const std::string& string);
          virtual void interpret(const char* string);

          dcGenericResource* getMemberByName(const std::string& name);

        public:

            /**
             * All derived classes MUST create their own version of this static method.
             * Unfortunately, because static methods can't be made virtual, this can't
             * be explicitly enforced, but there will be compile errors if this isn't
             * defined.
             */
            static std::string staticTypeName() { return "Generic Resource"; }


        public:

            /**
             * Gets the manager for this resource.  The manager can be used to do
             * general operations for resource instances (those that are not properties)
             */
            virtual dcResourceManager* getManager() const = 0;


        public:

            /**
             * Determines whether or not this resource implements dcGenericCollection.  This
             * will be automatically resolved by dcGenericCollection, so there is no need
             * for a client to override this method.  An incorrect return value can lead to
             * access faults and security vulnerabilities.
             */
            virtual bool isCollection() const;

            /**
             * Determines whether or not this resource implements dcGenericInterface.  This
             * will be automatically resolved by dcGenericInterface, so there is no need
             * for a client to override this method.  An incorrect return value can lead to
             * access faults and security vulnerabilities.
             */
            virtual bool isInterface() const;

            /**
             * Determines whether or not this resource implements dcGenericReference.  This
             * will be automatically resolved by dcGenericReference, so there is no need
             * for a client to override this method.  An incorrect return value can lead to
             * access faults and security vulnerabilities.
             */
            virtual bool isReference() const;

            /**
             * The default implementation of "save" simply goes through all of the
             * members in this resource and saves them.  If a resource has resource
             * members, neither the save nor load methods should not be overloaded
             * @param stream The stream into which to save
             * @return Whether or not the operation succeeded
             */
            virtual bool save( dcStreamOut* stream );

            /**
             * The default implementation of "load" goes through each property
             * and loads it from the stream.  If a resource has resource members,
             * neither the save nor load methods should not be overloaded
             * @param stream The stream from which to load
             * @return Whether or not the operation succeeded
             */
            virtual bool load( dcStreamIn* stream );

            /**
             * Compares this resource with another resource.  The default implementation
             * sorts by resource type ID via getTypeID().  If the resources have the same
             * type ID, the default implementation then compares individual properties
             * to each other.  If the types are incompatible, subclasses should return
             * dcGenericResource::compareTo( other ).
             * @return for a > b (ex. a = 5, b = 2)
             *              a.compareTo(b)  > 0
             *              a.compareTo(a) == 0
             *              b.compareTo(a)  < 0
             */
            virtual int compareTo( const dcGenericResource* other ) const;

            /**
             * Gets a string representation of this class type.  The default implementation
             * calls the same method for all of the members of this resource.
             * @return String value indicating what's in this class
             */
            virtual std::string toString() const;


        protected:

            /**
             * Indicates that the provided resource is a member of this resource and adds
             * it to the internal list.  The method also sets 'resource->myOwner' to 'this'.
             * If the resource already exists in the list of members, this method will change
             * that resource's name, but do nothing else.  If the resource's name exists
             * in the list of members, the name will be appended with random characters
             * until it is unique, and then added.
             */
            void member( const std::string& name, dcGenericResource* resource );

        private:

            // Recursive method used by getPathString
            char* buildPathString(char* buffer, const char* buffer_end) const;

            // Resets myOwner and returns its old value.  This method does not remove
            // this resource from its owner's member list, so it must be used carefully.
            // Its purpose is to temporarily change how the path of this resource behaves.
            // When done, call rejoin with the return value of this method.
            dcGenericResource* leave();

            // Called to restore the owner of this resource.
            void rejoin(dcGenericResource* owner);


        private:

            /// The resource that owns this one (or null if this is the root resource)
            dcGenericResource* myOwner;

            /// The name of this resource
            std::string myName;


        protected:

            /// Members of this resource can be static properties, entries in a collection,
            /// or any other kind of resource that is "contained" by another resource (such
            /// as the resource that a dcTable<>::Reference points to).  The derived class is
            /// responsible for managing this list.
            Array myMembers;
    };

    /**
     * The class from which all resources derive.  No resource should directly implement
     * dcGenericResource unless it also implements a dcResourceManager without using the templated
     * ResourceManagerImplementation helper class.
     * P - the parent for this resource (should inherit from dcGenericResource)
     * D - the derived resource type
     */
    template <typename D, typename P = dcGenericResource> class dcResource : public P
    {
        /**
         * This class masks another class so that it looks like a different resource type.  This
         * way, it can't get confused with the same type used for a completely different purpose.
         * For example, an internal array index is different from a public map coordinate, but
         * they're both integers.  Using this class saves you the time of writing another
         * class for the internal array index type.
         */
        template <typename T> class Alias : public dcResource<Alias<T>,T>
        {
            public:

                /**
                 * The name of this resource type.  An Alias class has no other methods, because
                 * it derives all of its functionality from the parent class for which it
                 * was created.
                 */
                static std::string staticTypeName()
                {
                    return "Alias of " + T::staticTypeName() + " for " + D::staticTypeName();
                }
        };

        public:

            /// This accessor allows the ResourceManagerImplementation class to
            /// statically access this class using only the derived resource type.
            typedef P ParentType;

        public:

            /**
             * This constructor simply passes information up to the main
             * resource class.
             */
            dcResource() : P()
            {
                // This needs to be here, otherwise it's possible that the implementsResource
                // member is never used elsewhere in the program, and the linker will remove
                // all references for it.
                // TODO: in release mode, during more strenuous optimization, we should call
                // some sort of method using this implementation so that it doesn't get removed.
                typename dcClass<D>::Implements<dcGenericResource>* implementation = &implementsGenericResource;
            }

            /**
             * Gets the manager for this resource.  The manager can be used to do
             * general operations for resource instances (those that are not properties)
             */
            dcResourceManager* getManager() const { return dcClass<D>::staticResourceManager(); }

        protected:

            /// This linker does multiple things; first of all, it causes dcInterface<dcGenericResource> to
            /// be able to implement all of the different resource types in the program.  Second,
            /// it forces the manager class (dcClass<D>::Manager) to be implemented.  Any undefined
            /// symbols in there will show up as linker errors to the user, which reminds them to
            /// implement various usage-specific functionality (how resources compile, for example).
            static typename dcClass<D>::Implements<dcGenericResource> implementsGenericResource;
    };

    /// Resolve the linker class that causes all resources to be available to dcInterface
    template <typename D, typename P> typename dcClass<D>::Implements<dcGenericResource> dcResource<D,P>::implementsGenericResource;
}




#endif
