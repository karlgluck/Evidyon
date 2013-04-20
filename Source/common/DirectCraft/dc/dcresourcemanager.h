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
// TODO: an interesting idea is to keep track of inheritance graphs so that action
// tables can be inherited.  It's going to take some thinking and I can't do it now...[2007-12-29]
#ifndef __DCRESOURCEMANAGER_H__
#define __DCRESOURCEMANAGER_H__


namespace dc {

    class dcGenericResource;
    class dcResourceManager;

    /// An Action Function is stored by a resource manager so that the client can keep track of
    /// tasks that can be performed on a resource.  The method's return code, and what is
    /// provided to userData, is psecified by the user; DirectCraft imposes no restrictions.
    typedef int (*dcActionFunction)(dc::dcGenericResource* resource, void* userData);

    /**
     * This action structure specifies a name for a function (and the function!) that does
     * something with a resource.
     */
    struct dcAction
    {
        /// The null-terminated name of this action
        const char* name;

        /// What function to call when this action should be performed
        dcActionFunction function;
    };



    /**
     * Classes deriving from this type provide general control over a certain type of
     * resource.
     */
    class dcResourceManager
    {
        friend class dcGenericResource;

        public:

            /**
             * Gets the actions that this class can perform
             */
            virtual dcAction* getActions() const = 0;

        public:

            /**
             * Creates a new resource of whatever type this manager is responsible for.  This
             * method is used when loading resources of an unknown type by the an dcInterface.
             * @param name The name of the resource
             */
            virtual dcGenericResource* generate() const = 0;

            /**
             * Deallocates the given resource.  This resource should be of the type managed
             * by this resource manager.
             */
            virtual void release(dcGenericResource* resource) const = 0;

            /**
             * Generates this resource's unique numeric identifier.  This number is created
             * by running a short hashing algorithm on the string returned from typeName().
             * @return Number that identifies the type of resource this authority is responsible for
             */
            virtual unsigned long uniqueID() const;

            /**
             * The name of the resource type that is referenced by this authority
             * @return Name of this resource type
             */
            virtual const std::string& typeName() const = 0;


        public:

            /**
             * Global method to free the memory for a resource
             * @param resource The resource whose memory needs to be freed
             */
            static void deallocate(dcGenericResource* resource);

            /**
             * Converts a type name into its unique ID number
             */
            static unsigned long uniqueIDOfTypeName(const std::string& type_name);
    };


    // Pre-declare the interface type so that the linker can add it as a friend
    template <typename T> class dcInterface;

    /**
     * The interface linker provides a medium between which the 'dcInterface<>' class and
     * a dcClass<T>::Implements<I> class can communicate.  Each 'Implements<>' class creates
     * a new InterfaceLinker for its parent object type so that the dcInterface for which
     * that class refers can "see" the managers for all of its subclasses.
     *
     * For example, the InterfaceLinker<dcGenericResource> class is automatically set up (by the
     * dcResource class) to contain managers for all of the resource classes
     * in the DirectCraft library, including those created by the end aplication.
     *
     * Furthermore, this linking makes dcClass<T>::Manager compile for every type/combination
     * of resources; this forces the user to write action lists for all of the classes.
     */
    template <typename T> class InterfaceLinker
    {
        // Make sure that the interface can look inside this class; nobody else should
        // be poking around inside here.
        friend class dcInterface<T>;

        public:

            /**
             * Adds a new linker to the chain.  This class should never be used except
             * by the internal library.
             * @param manager The manager to associate with this linker
             */
            InterfaceLinker(dcResourceManager* manager)
            {
                // Save the incoming parameter
                myManager = manager;

                // Push this linker into the chain
                myNextLinker = InterfaceLinker<T>::firstLinker;
                InterfaceLinker<T>::firstLinker = this;
            }


        protected:

            /// The next entry in the linked list of managers
            InterfaceLinker<T>* myNextLinker;

            /// The implementing authority linked with this manager
            dcResourceManager* myManager;

            /// The first linker in this manager's chain
            static InterfaceLinker<T>* firstLinker;
    };

    // Resolve the linker's "firstLinker" member for all InterfaceLinker types.
    template <typename T> typename InterfaceLinker<T>* InterfaceLinker<T>::firstLinker = 0;



    /**
     * This class is used to define a class via which resource interface types it implements.  The
     * most general type is implemented for all resources by dcResource:  for any
     * resource of type T, dcClass<T>::Implements<dcGenericResource> should be resolved (and it is, auto-
     * matically!).  To extend this definition, we need an example.
     * If there were a generic class Number (there isn't, but let's pretend there is) and both
     * dcFloat and dcInteger were a subclass of Number, dcFloat should contain a static entry of the type
     * dcClass<dcFloat>::Implements<Number> and dcInteger should contain the similar static entry
     * of type dcClass<dcInteger>::Implements<Number>.  This would allow dcInterface<Number> to generate
     * classes of either subtype when it creates resources automatically.
     */
    template <typename T> class dcClass : public dcResourceManager
    {
        typedef T ResourceType;

        public:

            /**
             *
             */
            class Manager : public dcResourceManager
            {
                public:

                    Manager()
                    {
                    }

                    /**
                     * Gets the actions that this class can perform.  This array is terminated by a NULL
                     * entry.
                     */
                    dcAction* getActions() const
                    {
                        return dcClass<T>::actions;
                    }

                    /**
                     * Generates a new instance of this resource with the specified values, but does
                     * not add it to any global lists.  This method is chiefly used during the loading
                     * procedure for the dcInterface class.
                     */
                    dcGenericResource* generate() const
                    {
                        // Create the new resource
                        return (dcGenericResource*)new ResourceType();
                    }

                    /**
                     * Deallocates the given resource.  This resource should be of the type managed
                     * by this resource manager.  This is put here instead of inside the library so
                     * that we can be sure the new/delete functions are linked in the same way.
                     */
                    void release(dcGenericResource* resource) const
                    {
                        delete (ResourceType*)resource;
                    }

                    /**
                     * Generates the unique ID for the resource type managed by this authority
                     * @return Number that identifies the resource this authority is responsible for
                     */
                    unsigned long uniqueID() const
                    {
                        // Use the default implementation to generate the ID, but store its value statically
                        // because generating the ID involves a fairly long algorithm.
                        static unsigned long id = dcResourceManager::uniqueID();

                        // Return the ID number we generated
                        return id;
                    }

                    /**
                     * Gets the fully-defined name of the class managed by this resource administrator.
                     * This method invokes the class's static type defining method.
                     * @return Name of the managed class
                     */
                    const std::string& typeName() const
                    {
                        // Get the type name by using the static member function of the type that this
                        // manager is responsible for handling.  The result is stored as a static
                        // variable because the process for generating the name can be time-consuming.
                        static std::string name = ResourceType::staticTypeName();

                        // Return the name obtained from the resource type
                        return name;
                    }
            };

        public:

            /**
             * Used to define an interface that the template parameter to the dcClass declaration
             * containing this definition can instantiate.
             */
            template <typename I> class Implements
            {
                public:

                    /**
                     * Constructor to set up the linker class for the interface to point to this
                     * particular resource type.
                     */
                    Implements() : myLinker(&dcClass<T>::manager)
                    {
                    }

                protected:

                    /// Generate the linker for this implementation type
                    typename InterfaceLinker<I> myLinker;
            };

            /**
             * Get the manager type for this class.  This is used explicitly by
             * dcResource.
             */
            static dcResourceManager* staticResourceManager() { return &manager; }

        protected:

            /// The auto-generated manager for this class.  Don't mess with it.
            static Manager manager;

            /// The list of actions that can be performed on this resource.  This will come
            /// up as an unresolved external symbol if it isn't implemented by the user.
            static dcAction actions[];
    };

    /// Resolve the manager for all class types
    template <typename T> typename dcClass<T>::Manager dcClass<T>::manager;
}






#endif