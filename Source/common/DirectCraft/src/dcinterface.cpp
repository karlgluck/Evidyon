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
#include "../dc/dcinternal_begin.h"
#include "../dc/interface"
#include "../dc/dcbufferstream.h"



using namespace dc;


//-----------------------------------------------------------------------------
// Name:  dcGenericInterface
// Desc:  Initializes this resource
//-----------------------------------------------------------------------------
dcGenericInterface::dcGenericInterface()
{
    //myImplementation = 0;
}




//-----------------------------------------------------------------------------
// Name:  ~dcGenericInterface
// Desc:  Cleans up when the class is being deconstructed
//-----------------------------------------------------------------------------
dcGenericInterface::~dcGenericInterface()
{
    release();
}




//-----------------------------------------------------------------------------
// Name:  release
// Desc:  Erases the implementation inside of the interface, if it exists
//-----------------------------------------------------------------------------
void dcGenericInterface::release()
{
    dcGenericResource* implementation = getGenericImplementation();
    if( implementation ) dcResourceManager::deallocate( implementation );
    //myImplementation = 0;
    myMembers.clear();
}


//-----------------------------------------------------------------------------
// Name:  setImplementationByType
// Desc:  Changes the implementation of this resource
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericInterface::implementByType( unsigned long id )
{
    // Erase the current implementation
    release();

    // Get the manager for the subclass type
    dcResourceManager* manager = getSubclassManager( id );

    // The location where the implementation will be stored (if it can be created)
    dcGenericResource* implementation = 0;

    // If the manager could be found, use it to create the implementation
    if( manager )
    {
        // Allocate the implementing type using this manager
        implementation = manager->generate();

        // Set this as a child implementation
        myMembers.clear();
        member( "<" + manager->typeName() + ">", implementation );
    }

    // Return the created interface
    return getGenericImplementation();
}


//-----------------------------------------------------------------------------
// Name:  isImplemented
// Desc:  Determines whether or not this interface has an implementation
//-----------------------------------------------------------------------------
bool dcGenericInterface::isImplemented() const
{
    return getGenericImplementation() != 0;
}


//-----------------------------------------------------------------------------
// Name:  getImplementationType
// Desc:  Gets the type of this resource's implementation
//-----------------------------------------------------------------------------
unsigned long dcGenericInterface::getImplementationType() const
{
    dcGenericResource* implementation = getGenericImplementation();
    if( implementation ) return implementation->getTypeID();
    else return 0;
}


//-----------------------------------------------------------------------------
// Name:  getGenericImplementation
// Desc:  Gets the generic implementation of this interface
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericInterface::getGenericImplementation() const
{
    if( myMembers.empty() )
        return 0;
    else
        return myMembers.front();
    //return myImplementation;
}



//-----------------------------------------------------------------------------
// Name:  isCollection
// Desc:  Determines whether or not this resource implements dcGenericCollection
//-----------------------------------------------------------------------------
bool dcGenericInterface::isCollection() const
{
    return false;
}



//-----------------------------------------------------------------------------
// Name:  isInterface
// Desc:  Determines whether or not this resource implements dcGenericInterface
//-----------------------------------------------------------------------------
bool dcGenericInterface::isInterface() const
{
    return true;
}



//-----------------------------------------------------------------------------
// Name:  save
// Desc:  Saves whatever kind of interface is referenced by this one.
//-----------------------------------------------------------------------------
bool dcGenericInterface::save( dcStreamOut* stream )
{
    // Make sure there are no type collisions
    //if( typeCollisionsExist() ) return false;

    // This interface's implementation
    dcGenericResource* implementation = getGenericImplementation();

    // Check to see whether or not this interface is implemented at all
    bool implementationExists = implementation != 0;
    if( !stream->write( &implementationExists, sizeof(implementationExists) ) ) return false;

    // If there is no implementation, just succeed
    if( !implementationExists ) return true;

    // Get this resource's manager object
    dcResourceManager* manager = implementation->getManager();

    // Save this resource's type ID
    unsigned long type = manager->uniqueID();
    if( !stream->write( &type, sizeof(type) ) ) return false;

    // Make sure there is room for the byte length by saving a temporary value
    size_t byteLength = 0;
    if( !stream->write( &byteLength, sizeof(byteLength) ) ) return false;

    // Figure out where we are in the file before writing this resource into the stream
    size_t begin = stream->tell();

    // Write this resource into the stream
    if( !implementation->save( stream ) ) return false;

    // Save the current location
    size_t end = stream->tell();

    // Write the byte length of the entry so that it can be skipped if the type isn't
    // recognized during a load.
    byteLength = (size_t)(end - begin);
    if( !stream->seek( begin - sizeof(byteLength) ) ) return false;
    if( !stream->write( &byteLength, sizeof(byteLength) ) ) return false;

    // Move the stream back to the current write location
    if( !stream->seek( end ) ) return false;

    // Success
    return true;
}




//-----------------------------------------------------------------------------
// Name:  load
// Desc:  Loads an object implementing the interface that this class references from
//-----------------------------------------------------------------------------
bool dcGenericInterface::load( dcStreamIn* stream )
{
    // Make sure there are no type collisions
    //if( typeCollisionsExist() ) return false;

    // Get rid of the resource that this interface currently holds
    release();

    // Check to see whether or not this interface is implemented at all
    bool implementationExists = false;
    if( !stream->read( &implementationExists, sizeof(implementationExists) ) ) return false;

    // If there is no implementation, just succeed
    if( !implementationExists ) return true;

    // Determine the type of whatever's in the stream
    unsigned long type;
    if( !stream->read( &type, sizeof(type) ) ) return false;

    // Create a new resource of this type
    dcResourceManager* manager = getSubclassManager( type );

    // Generate a resource
    dcGenericResource* loadingResource;

    // If the manager could be found, use it to create the resource
    if( manager )
    {
        // Allocate the implementing type using this manager
        loadingResource = manager->generate();
    }
    else
        loadingResource = 0;

    // Read the size of this resource
    size_t byteLength;
    if( !stream->read( &byteLength, sizeof(byteLength) ) ) return false;

    // Read the resource from the file if we could obtain one of the correct type
    if( loadingResource )
    {
        // Determine where we are in the file
        size_t begin = stream->tell();

        // Load the new resource
        if( !loadingResource->load( stream ) ) { dcResourceManager::deallocate( loadingResource ); return false; }

        // Make sure the size is correct
        size_t end = stream->tell();
        if( end - begin != byteLength ) { dcResourceManager::deallocate( loadingResource ); return false; }

        // Own the resource that we just loaded
        member( "<" + manager->typeName() + ">", loadingResource );
    }
    else
    {
        // Skip this resource in the stream.  We don't have to free anything if this
        // fails because it is implied that loadingResource is null.
        if( !stream->move( (int)byteLength ) ) { return true; }
    }

    // Save the implementing resource
    //myImplementation = loadingResource;

    // Success
    return true;
}




//-----------------------------------------------------------------------------
// Name:  compareTo
// Desc:  Compares this resource with another resource
//-----------------------------------------------------------------------------
int dcGenericInterface::compareTo( const dcGenericResource* other ) const
{
    dcGenericResource* implementation = getGenericImplementation();
    if( other == 0 || getTypeID() != other->getTypeID() )
        return dcGenericResource::compareTo( other );
    else if( implementation != 0 )
        return implementation->compareTo( typecastGeneric(other)->getGenericImplementation() );
    else if( typecastGeneric(other)->isImplemented() )
        return +1;
    else
        return 0;
}





//----[  acquireMember  ]------------------------------------------------------
dcGenericResource* dcGenericInterface::acquireMember(const char* member_name,
                                                     const char* type,
                                                     size_t index_hint) {
  implementByType(dcResourceManager::uniqueIDOfTypeName(type));
  return getGenericImplementation();
}




//-----------------------------------------------------------------------------
// Name:  typecastGeneric
// Desc:  Typecasts a resource variable into a generic interface
//-----------------------------------------------------------------------------
const dcGenericInterface* dcGenericInterface::typecastGeneric( const dcGenericResource* variable ) const
{
    // Null references can be all types
    if( variable == 0 ) return 0;

    // Make sure the types match
    if( getTypeID() != variable->getTypeID() )
        throw "invalid";

    // Re-cast the variable and return it
    return (dcGenericInterface*)variable;
}