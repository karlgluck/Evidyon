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
#include "../dc/dcstream.h"
#include "../dc/dcresourcemanager.h"
#include "../dc/dcresource.h"
#include "../dc/dccollection.h"
#include "../dc/dcbufferstream.h"
#include <algorithm>


using namespace dc;


//-----------------------------------------------------------------------------
// Name:  dcGenericCollection
// Desc:  
//-----------------------------------------------------------------------------
dcGenericCollection::dcGenericCollection()
{
}




//-----------------------------------------------------------------------------
// Name:  ~dcGenericCollection
// Desc:  Erases internal variables when the class is destructed
//-----------------------------------------------------------------------------
dcGenericCollection::~dcGenericCollection()
{
    clear();
}



//-----------------------------------------------------------------------------
// Name:  isCollection
// Desc:  Determines whether or not this resource implements dcGenericCollection
//-----------------------------------------------------------------------------
bool dcGenericCollection::isCollection() const
{
    return true;
}


//-----------------------------------------------------------------------------
// Name:  isInterface
// Desc:  Determines whether or not this resource implements dcGenericInterface
//-----------------------------------------------------------------------------
bool dcGenericCollection::isInterface() const
{
    return false;
}



//-----------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//-----------------------------------------------------------------------------
bool dcGenericCollection::save( dcStreamOut* stream )
{
    // Store the number of entries in the list
    size_t entries = myMembers.size();
    if( !stream->write( &entries, sizeof(entries) ) ) return false;

    // Write each entry to the stream
    for( Array::iterator i = myMembers.begin(); i != myMembers.end(); ++i )
    {
        // Get the interface pointer from the iterator
        dcGenericResource* resource = *i;

        // Save the name of this resource
        if( !stream->writeString( resource->getName() ) ) return false;

        // Save the resource
        if( !resource->save( stream ) ) return false;
    }

    // Success
    return true;
}




//-----------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//-----------------------------------------------------------------------------
bool dcGenericCollection::load( dcStreamIn* stream )
{
    // Erase the collection
    clear();

    // Read the number of entries in the collection
    size_t entries;
    if( !stream->read( &entries, sizeof(entries) ) ) return false;

    // Read each entry
    for( size_t i = 0; i < entries; ++i )
    {
        std::string name;
        if( !stream->readString( &name ) ) return false;

        // Create the resource
        dcGenericResource* resource = generateElement( name );

        // Load the resource from the stream
        if( !resource->load( stream ) )
        {
            releaseElement( resource );
            return false;
        }
    }

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name:  sort
// Desc:  Sorts the elements in this collection via the compareTo method
//-----------------------------------------------------------------------------
void dcGenericCollection::sort()
{
    // This structure runs the compareTo operator on each element
    struct resource_pred_compareTo : public std::binary_function<const dcGenericResource*, const dcGenericResource*, bool>
    {
        bool operator()(const dcGenericResource* lhs, const dcGenericResource* rhs) const
        {
            return lhs->compareTo( rhs ) < 0;
        }
    };

    std::sort( myMembers.begin(), myMembers.end(), resource_pred_compareTo() );
}



//-----------------------------------------------------------------------------
// Name:  sortByName
// Desc:  Sorts the elements in this collection by their name
//-----------------------------------------------------------------------------
void dcGenericCollection::sortByName()
{/*
    // Create a temporary storage array for the vector
    size_t resources = myMembers.size();
    dcGenericResource** resourceArray = new dcGenericResource*[resources];
    if( !resourceArray ) { DEBUG_FATAL( "Out of memory during dcGenericCollection::sortByName()!" ); return; }

    // Copy the array
    for( size_t i = 0; i < resources; ++i )
        resourceArray[i] = myMembers.at(i);

    // Clear the source array
    myMembers.clear();

    // Sort the members of the temporary array
    qsort( resourceArray, resources, sizeof(dcGenericResource*), dcGenericResourceSortByNameCallback );

    // Add the members of the temporary array back into the member list
    for( size_t i = 0; i < resources; ++i )
        myMembers.push_back( resourceArray[i] );

    // Free the temporary array
    delete[] resourceArray;*/

    // This structure compares the string names of the resources
    struct resource_pred_name : public std::binary_function<const dcGenericResource*, const dcGenericResource*, bool>
    {
        bool operator()(const dcGenericResource* lhs, const dcGenericResource* rhs) const
        {
            return lhs->getName() < rhs->getName();
        }
    };

    std::sort( myMembers.begin(), myMembers.end(), resource_pred_name() );
}



//-----------------------------------------------------------------------------
// Name:  eraseEqual
// Desc:  Erases all entries in the collection for which entry->compareTo(resource) == 0
//-----------------------------------------------------------------------------
void dcGenericCollection::eraseEqual( dcGenericResource* resource )
{
    // Go through the list and erase all of the equal members
    for( Array::iterator i = myMembers.begin(); i != myMembers.end(); )
    {
        // Get the next entry
        Array::iterator next = i;
        next++;

        // Get this member
        dcGenericResource* currentResource = *i;

        // Check the resource to see if it's equal
        if( currentResource->compareTo( resource ) == 0 )
        {
            // Get rid of this resource's memory
            dcResourceManager::deallocate( currentResource );

            // Erase the resource
            myMembers.erase( i );
        }

        // Set the next element
        i = next;
    }
}




//-----------------------------------------------------------------------------
// Name:  size
// Desc:  Determines how many elements are in this collection
//-----------------------------------------------------------------------------
size_t dcGenericCollection::size()
{
    return myMembers.size();
}




//-----------------------------------------------------------------------------
// Name:  clear
// Desc:  Erases everything in this collection
//-----------------------------------------------------------------------------
void dcGenericCollection::clear()
{
    // Go through the list and erase each of the members
    Array::iterator begin = myMembers.begin();
    Array::iterator end = myMembers.end();
    for( Array::iterator i = begin; i != end; ++i )
        dcResourceManager::deallocate( *i );

    // Get rid of everything in the list
    myMembers.clear();
}



//-----------------------------------------------------------------------------
// Name:  releaseElement
// Desc:  Frees the memory used by an element
//-----------------------------------------------------------------------------
void dcGenericCollection::releaseElement( dcGenericResource* element )
{
    // If the element is invalid or this class does not own the element,
    // this method shouldn't be called
    if( APP_ERROR( !element )( "Invalid parameter to releaseElement" ) ||
        APP_ERROR( element->getOwner() != this )
                ( "releaseElement called for collection %s, which does not own %s",
                  getName().c_str(), element->getName().c_str() ) )
        return;

    // Find this element in the list
    for( Array::iterator i = myMembers.begin(); i != myMembers.end(); )
    {
        // Get the next entry
        Array::iterator next = i;
        next++;

        // Get this member
        dcGenericResource* currentResource = *i;

        // Check the resource to see if it's equal
        if( currentResource == element )
        {
            // Get rid of this resource's memory
            dcResourceManager::deallocate( currentResource );

            // Erase the resource
            myMembers.erase( i );

            // Finished!
            return;
        }

        // Set the next element
        i = next;
    }
}


//-----------------------------------------------------------------------------
// Name:  find
// Desc:  Finds an element of this collection 'e' for which e->compareTo( elementLike ) == 0
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericCollection::find( const dcGenericResource* elementLike, size_t start, size_t* found )
{
    // The end of the list
    size_t end = myMembers.size();

    // Reset the output index
    if( found )
        *found = (size_t)(-1);

    // Look for the element
    for( size_t index = start; index < end; ++index )
    {
        // Get the entry
        dcGenericResource* entry = myMembers.at(index);

        // Check to see if these match
        if( entry->compareTo( elementLike ) == 0 )
        {
            // Save the output index
            if( found ) *found = index;

            // Return the entry
            return entry;
        }
    }

    // Couldn't find the element
    return 0;
}


//-----------------------------------------------------------------------------
// Name:  binarySearch
// Desc:  Finds an element if this is known to be a collection with special properties
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericCollection::binarySearch( const dcGenericResource* elementLike )
{
    // Binary search flags; TODO: use a different type?
    long lowIndex = 0,
         highIndex = myMembers.size() - 1;

    while( lowIndex <= highIndex )
    {
        // Get the midpoint
        long midpointIndex = (highIndex + lowIndex) / 2;
        dcGenericResource* midpointResource = myMembers.at(midpointIndex);

        // Compare the midpoint to the key
        int comparison = midpointResource->compareTo( elementLike );

        // Handle the comparison
        if( comparison > 0 )
            highIndex = midpointIndex - 1;
        else if( comparison < 0 )
            lowIndex = midpointIndex + 1;
        else
            return midpointResource;
    }

    // Couldn't find the element
    return 0;
}




//----[  acquireMember  ]------------------------------------------------------
dcGenericResource* dcGenericCollection::acquireMember(const char* member_name,
                                                      const char* type,
                                                      size_t index_hint) {
  // TODO: collections *could* be made to store more than one kind of element...
  dcGenericResource* element = generateElement(member_name);
  return element;
}



//-----------------------------------------------------------------------------
// Name:  canAdd
// Desc:  Checks to see if this entry type can be added to the collection
//-----------------------------------------------------------------------------
bool dcGenericCollection::canAdd( dcGenericResource* resource )
{
    // If elements in the collection can't be duplicated, search to make sure there
    // isn't an element that matches the parameter already in the list
    if( !allowDuplicates() )
    {
        Array::iterator begin = myMembers.begin();
        Array::iterator end = myMembers.end();
        for( Array::iterator i = begin; i != end; ++i )
            if( (*i)->compareTo( resource ) == 0 ) return false;
    }

    // We can add the resource
    return true;
}




//-----------------------------------------------------------------------------
// Name:  typecast
// Desc:  Casts a generic resource interface into this resource's type.
//-----------------------------------------------------------------------------
dcGenericCollection* dcGenericCollection::typecast( dcGenericResource* variable )
{
    // Null references can be all types
    if( variable == 0 ) return 0;

    // Make sure the types match
    if( getTypeID() != variable->getTypeID() )
        throw "invalid";

    // Re-cast the variable and return it
    return (dcGenericCollection*)variable;
}
