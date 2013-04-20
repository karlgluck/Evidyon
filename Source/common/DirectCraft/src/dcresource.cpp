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
#include "../dc/dcfilestream.h"
#include "../dc/dcreference.h"

using namespace dc;


//-----------------------------------------------------------------------------
// Name: dcGenericResource
// Desc: Initializes this resource
//-----------------------------------------------------------------------------
dcGenericResource::dcGenericResource()
{
    // This resource has no default name
    myName = "";

    // Reset the resource's owner
    myOwner = 0;
}



//-----------------------------------------------------------------------------
// Name: dcGenericResource
// Desc: Allow sub-classes to run their destructors
//-----------------------------------------------------------------------------
dcGenericResource::~dcGenericResource()
{

}


//-----------------------------------------------------------------------------
// Name: getMembers
// Desc: Gets the list of members this resource has
//-----------------------------------------------------------------------------
const dcGenericResource::Array& dcGenericResource::getMembers() const
{
    return myMembers;
}


//-----------------------------------------------------------------------------
// Name: setName
// Desc: Changes the name of this resource
//-----------------------------------------------------------------------------
bool dcGenericResource::setName( const std::string& name, bool updateReferences )
{
    // On July 14 2009 I relaxed this requirement so that dcResourceStorage could be used by the
    // Evidyon complete-editor to import and export arbitrary resources.  it requires that the
  // resource be named the same for loading and saving, so i name all resources ##import_export##
  // temporarily to meet this requirement.  Hacky.  Better soln' would involve modifying
  // dcresourcestorage which I dont have time to do right now.

    //// Fail unless the owner is a collection or there is no parent resource
    //if( APP_WARNING( myOwner && myOwner->isCollection() == false )
    //    ( "Couldn't change the name of resource %s to %s because it isn't a collection member", 
    //      myName.c_str(), name.c_str() ) )
    //    return false;

    if( APP_WARNING( name.find(PATH_SEPARATOR) != name.npos )
        ( "Couldn't change name of resource %s to %s because the new name contains a reserved character",
          myName.c_str(), name.c_str() ) )
        return false;

    // If this resource has an owner, make sure that there are no other
    // resources with the targeted name
    if( myOwner && myOwner->isCollection() )
    {
        // Get the collection siblings of this resource and the iterators
        const dcGenericResource::Array& siblings = myOwner->getMembers();
        dcGenericResource::Array::const_iterator i;
        dcGenericResource::Array::const_iterator end = siblings.end();

        // Make sure the parent collection doesn't already have a resource with the name
        for( i = siblings.begin(); i != siblings.end(); ++i )
        {
            // Check the name
            if( APP_WARNING( name.compare( (*i)->getName() ) == 0 )
                ( "Couldn't change the name of resource %s to %s because another resource already exists with this name",
                  myName.c_str(), name.c_str() ) )
              return false;
        }
    }

    // These paths are used to update references
    Path oldPath, newPath;

    // If necessary, grab the current path
    if( updateReferences ) getPath( &oldPath );

    // Change this resource's name
    myName.assign( name );

    // Get the new path and update references, if we need to
    if( updateReferences )
    {
        // Obtain the new path here
        getPath( &newPath );

        // Find the root resources
        dcGenericResource* rootResource = this;
        while( rootResource->getOwner() != 0 ) rootResource = rootResource->getOwner();

        // Add the root resource to the update array.  We reserve lots of space here
        // to try and prevent frequent reallocations.
        dcGenericResource::Array resources;
        size_t rootChildren = rootResource->getMembers().size();
        resources.reserve( rootChildren * rootChildren );
        resources.push_back( rootResource );

        // Go through the list
        while( !resources.empty() )
        {
            // Obtain the last entry in the list
            dcGenericResource* resource = resources.back();

            // Add any of this element's children to the start of the list
            const dcGenericResource::Array& members = resource->getMembers();
            if( !members.empty() )
                resources.insert( resources.begin(), members.begin(), members.end() );

            // If this resource is a reference, update it
            if( resource->isReference() )
            {
                // Get the reference interface
                dcGenericReference* reference = (dcGenericReference*)resource;

                // Update the path
                reference->updateReferencedPath( oldPath, newPath );
            }

            // Remove this resource from the list, since we've processed it
            resources.pop_back(); 
        }
    }

    // Success
    return true;
}


//-----------------------------------------------------------------------------
// Name: getName
// Desc: Finds out what this resource is called
//-----------------------------------------------------------------------------
const std::string& dcGenericResource::getName() const
{
    return myName;
}


//-----------------------------------------------------------------------------
// Name: getPath
// Desc: Gets the path from the root resource to this resource
//-----------------------------------------------------------------------------
void dcGenericResource::getPath( Path* path ) const
{
    // Reset the path
    path->clear();

    // The current resource in the iteration
    const dcGenericResource* resource = this;
    const dcGenericResource* owner = getOwner();

    // Search upwards and build the path in reverse
    while( owner != 0 )
    {
        // This resource's index in its owner
        size_t indexInOwner = (size_t)(-1);

        // Find the this resource's place in this parent object
        const dcGenericResource::Array& ownerMembers = owner->getMembers();
        dcGenericResource::Array::const_iterator i = ownerMembers.begin();
        dcGenericResource::Array::const_iterator end = ownerMembers.end();
        for( size_t index = 0; i != end; ++i, ++index )
        {
            // Check to see if this is the resource we're looking for
            if( (*i) == resource )
            {
                // It was!  Save the index
                indexInOwner = index;

                // Exit out of the loop
                break;
            }
        }

        // Make sure we found a valid resource
        if( APP_FATAL( indexInOwner >= ownerMembers.size() )( "[dc] getPath() algorithm is broken" ) )
            break;

        // Add this element to the path
        path->push_front( Path::value_type( resource->getName(), indexInOwner ) );

        // Advance the iterators
        resource = owner;
        owner = resource->getOwner();
    }
}


//-----------------------------------------------------------------------------
// Name: getPathString
// Desc: Gets the path to this resource, starting at the resource with no owner (root)
//-----------------------------------------------------------------------------
std::string dcGenericResource::getPathString() const
{
    // The constructed path
    std::string path = getName();

    // The current resource in the iteration
    const dcGenericResource* resource = getOwner();

    // Search upwards and build the path in reverse
    while( resource != 0 ) {

        // Add to the front of the path
        path.insert( 0, resource->getName() + PATH_SEPARATOR );

        // Advance the iterator
        resource = resource->getOwner();
    }

    // Return the path string we built
    return path;
}






//----[  getPathString  ]------------------------------------------------------
size_t dcGenericResource::getPathString(char* buffer, size_t length) const {
  char* last_written = buildPathString(buffer, buffer + length);
  size_t bytes_consumed = last_written - buffer;
  if (bytes_consumed < length) {
    *last_written = '\0';
    return bytes_consumed;
  } else {
    return 0;
  }
}


//----[  buildPathString  ]----------------------------------------------------
char* dcGenericResource::buildPathString(char* buffer, const char* buffer_end) const {
  const dcGenericResource* owner = myOwner;
  if (owner) {
    buffer = owner->buildPathString(buffer, buffer_end);
    if (buffer < buffer_end) {
      *(buffer++) = PATH_SEPARATOR;
    }
  }

  // Add the name of this resource
  const char* my_name = myName.data();
  char ch;
  while ((buffer < buffer_end) && (ch = *(my_name++)) != 0) {
    *(buffer++) = ch;
  }

  return buffer;
}




//----[  leave  ]--------------------------------------------------------------
dcGenericResource* dcGenericResource::leave() {
  dcGenericResource* old_owner = myOwner;
  myOwner = 0;
  return old_owner;
}


//----[  rejoin  ]-------------------------------------------------------------
void dcGenericResource::rejoin(dcGenericResource* owner) {
  myOwner = owner;
  // TODO: if owner is valid, check to make sure it's *really* the owner
}



//-----------------------------------------------------------------------------
// Name: getOwner
// Desc: Determines the owner of this resource
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericResource::getOwner() const
{
    return myOwner;
}


//-----------------------------------------------------------------------------
// Name: getTypeName
// Desc: Obtains the type name of this resource
//-----------------------------------------------------------------------------
const std::string& dcGenericResource::getTypeName() const
{
    return getManager()->typeName();
}


//-----------------------------------------------------------------------------
// Name: getTypeID
// Desc: Obtains the unique type ID for this resource
//-----------------------------------------------------------------------------
unsigned long dcGenericResource::getTypeID() const
{
    return getManager()->uniqueID();
}


//-----------------------------------------------------------------------------
// Name:  getActions
// Desc:  Gets the list of actions that can be performed on this resource
//-----------------------------------------------------------------------------
dcAction* dcGenericResource::getActions() const
{
    return getManager()->getActions();
}


//-----------------------------------------------------------------------------
// Name:  getIndex
// Desc:  Gets the list of actions that can be performed on this resource
//-----------------------------------------------------------------------------
int dcGenericResource::getIndex() const
{
    dcGenericResource* owner = getOwner();
    if( !owner ) return -1;
    const dcGenericResource::Array& members = owner->getMembers();
    int index = 0;
    for( dcGenericResource::Array::const_iterator j = members.begin(); j != members.end() && this != *j; ++j, ++index );
    return ( index >= members.size() ) ? -1 : index;
}



//----[  acquireMember  ]------------------------------------------------------
dcGenericResource* dcGenericResource::acquireMember(const char* member_name,
                                                    const char* type,
                                                    size_t index_hint) {
  const dcGenericResource::Array& members = getMembers();

  // Check the hint to avoid any iteration
  if (members.size() > index_hint) {
    dcGenericResource* resource = members.at(index_hint);
    if (resource->getName().compare(member_name) == 0) return resource;
  }

  // If the name matches, return it.
  dcGenericResource::Array::const_iterator i, end = members.end();
  for (i = members.begin(); i != end; ++i) {
    dcGenericResource* resource = *i;
    if (resource->getName().compare(member_name) == 0) return resource;
  }

  return NULL;
}




//----[  serialize  ]----------------------------------------------------------
void dcGenericResource::serialize(std::string* string) {
  string->clear();
}



//----[  interpret  ]----------------------------------------------------------
void dcGenericResource::interpret(const std::string& string) {
  interpret(string.c_str());
}


//----[  interpret  ]----------------------------------------------------------
void dcGenericResource::interpret(const char* string) {
}




//----[  getMemberByName  ]----------------------------------------------------
dcGenericResource* dcGenericResource::getMemberByName(const std::string& name) {
  const dcGenericResource::Array& members = getMembers();
  dcGenericResource::Array::const_iterator i, end = members.end();
  for (i = members.begin(); i != end; ++i) {
    dcGenericResource* resource = *i;
    if (resource->getName().compare(name) == 0) return resource;
  }

  return NULL;
}



//-----------------------------------------------------------------------------
// Name:  isCollection
// Desc:  Determines whether or not this resource implements dcGenericCollection
//-----------------------------------------------------------------------------
bool dcGenericResource::isCollection() const
{
    return false;
}


//-----------------------------------------------------------------------------
// Name:  isInterface
// Desc:  Determines whether or not this resource implements dcGenericInterface
//-----------------------------------------------------------------------------
bool dcGenericResource::isInterface() const
{
    return false;
}


//-----------------------------------------------------------------------------
// Name:  isReference
// Desc:  Determines whether or not this resource implements dcGenericReference
//-----------------------------------------------------------------------------
bool dcGenericResource::isReference() const
{
    return false;
}


//-----------------------------------------------------------------------------
// Name: save
// Desc: Default implementation
//-----------------------------------------------------------------------------
bool dcGenericResource::save( dcStreamOut* stream )
{
    dcGenericResource::Array members = getMembers();
    for( Array::iterator i = members.begin(); i != members.end(); ++i )
        if( !(*i)->save( stream ) ) return false;
    return true;
}


//-----------------------------------------------------------------------------
// Name: load
// Desc: Default implementation
//-----------------------------------------------------------------------------
bool dcGenericResource::load( dcStreamIn* stream )
{
    dcGenericResource::Array members = getMembers();
    for( Array::iterator i = members.begin(); i != members.end(); ++i )
        if( !(*i)->load( stream ) ) return false;
    return true;
}




//-----------------------------------------------------------------------------
// Name: compareTo
// Desc: Compares this resource with another resource
//-----------------------------------------------------------------------------
int dcGenericResource::compareTo( const dcGenericResource* other ) const
{
    if( !other ) return +1;
    int retval = getTypeID() - other->getTypeID();

    // If the types are the same, compare properties
    if( retval == 0 )
    {
        // Obtain property lists
        const dcGenericResource::Array thisMembers = this->getMembers();
        const dcGenericResource::Array otherMembers = other->getMembers();

        // Get iterators
        dcGenericResource::Array::const_iterator t = thisMembers.begin();
        dcGenericResource::Array::const_iterator o = otherMembers.begin();

        // Compare properties, in order, to each other until a difference is found or we run out
        // of properties.
        for( ; (retval == 0) && (t != thisMembers.end()) && (o != otherMembers.end()); ++t, ++o )
            retval = (*t)->compareTo( (*o) );

        // Check number of members
        if( retval == 0 )
        {
            // Determine if one ended before the other (as in a collection)
            if( t != thisMembers.end() && o == otherMembers.end() ) return +1;
            if( t == thisMembers.end() && o != otherMembers.end() ) return -1;
        }
    }

    // Return the value
    return retval;
}



//-----------------------------------------------------------------------------
// Name: toString
// Desc: Gets a string representation of this class type
//-----------------------------------------------------------------------------
std::string dcGenericResource::toString() const
{
    // The string being built
    std::string stringValue = "{ ";

    // Go through each member
    dcGenericResource::Array members = getMembers();
    for( dcGenericResource::Array::iterator i = members.begin(); i != members.end(); ++i )
        stringValue += ((i == members.begin()) ? "" : ", ") + (*i)->getName() + "=[" + (*i)->toString() + "]";

    // Append the closing brace
    stringValue += " }";

    // Return the string we just constructed
    return stringValue;
}



//-----------------------------------------------------------------------------
// Name: member
// Desc: Indicates that the provided resource is a member of this resource
//-----------------------------------------------------------------------------
void dcGenericResource::member( const std::string& name, dcGenericResource* resource )
{
    // If the parameter is 'this' just change the name of this resource and set the owner
    if( this == resource )
    {
        myName.assign( name );
        myOwner = 0;
        return;
    }

    // Look through the list of members for this resource
    // TODO: this doesn't follow the rules
    dcGenericResource::Array members = getMembers();
    for( dcGenericResource::Array::iterator i = members.begin(); i != members.end(); ++i )
    {
        if( *i == resource )
        {
            // Give this resource a new name
            (*i)->myName.assign( name );

            // We're done
            return;
        }
    }

    // The resource's name, as modified so that it is unique
    std::string uniqueName = name;

    size_t at;
    const char PATH_SEPARATOR_STRING[] = { PATH_SEPARATOR, '\0' };
    while ((at = uniqueName.find_first_of(PATH_SEPARATOR_STRING)) != uniqueName.npos) {
      uniqueName.replace(at, 1, "_");
    }

    // First, check to see if this resource has a name at all.  If not, assign it one.
    if( uniqueName.empty() ) uniqueName = "resource";

    // Look for a resource with the same name
    bool confirmedUnique = false;
    while( !confirmedUnique )
    {
        // Iterator used to look through the list of members
        dcGenericResource::Array::iterator i;

        // Search for a member with the same name
        for( i = members.begin(); i != members.end(); ++i )
            if( (*i)->myName.compare( uniqueName ) == 0 ) break;

        // If the name is not unique, append a random number
        if( i != members.end() )
        {
            // Create a random number character
            char number[2] = { rand()%10 + '0', '\0' };

            // Append the random number
            uniqueName.append( number );
        }
        else
            confirmedUnique = true;
    }


    // Output a warning if we changed the name
    if( uniqueName.compare( name ) != 0 )
    {
        // Output a warning message
        DEBUG_WARNING( "Resource with duplicate name '%s' added to '%s' as '%s'\n", name.c_str(), myName.c_str(), uniqueName.c_str() );
    }

    // Set the resource's name and assign its owner
    resource->myName.assign( uniqueName );
    resource->myOwner = this;

    // Put this resource into the list of members
    myMembers.push_back( resource );
}