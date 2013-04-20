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
#include "../dc/dcreference.h"

// TODO: replace 0xFFFFFFFF with a named constant


using namespace dc;


//-----------------------------------------------------------------------------
// Name:  dcGenericReference
// Desc:  Intiailizes this class
//-----------------------------------------------------------------------------
dcGenericReference::dcGenericReference()
{
    myCommonRootHeight = 0xFFFFFFFF;
}


//-----------------------------------------------------------------------------
// Name:  isReference
// Desc:  Determines whether or not this resource implements dcGenericReference
//-----------------------------------------------------------------------------
bool dcGenericReference::isReference() const
{
    return true;
}



//-----------------------------------------------------------------------------
// Name:  getGenericReferencedResource
// Desc:  Obtains a generic interface to the referenced resource.  If the resource couldn't be
//-----------------------------------------------------------------------------
dcGenericResource* dcGenericReference::getGenericReferencedResource()
{
    // Get the root resource
    dcGenericResource* commonRoot = this;
    for( size_t i = 0; (i < myCommonRootHeight) && (commonRoot != 0); ++i )
        commonRoot = commonRoot->getOwner();

    // Make sure that the common root is valid; if it isn't, this is just a null reference
    if( !commonRoot ) return 0;

    // Used to iterate through the path
    dcGenericResource* resource = commonRoot;

    // Iterate through the path listing
    for( dcGenericResource::Path::iterator path = myPath.begin(); path != myPath.end(); ++path )
    {
        // Get the members of the current resource
        const dcGenericResource::Array& members = resource->getMembers();
        dcGenericResource::Array::const_iterator i = members.begin(), end = members.end();
        size_t index = 0;

        // Get the referenced resource using the index hint (if possible)
        dcGenericResource* component = 0;
        size_t indexHint = path->second;
        if( indexHint < members.size() )
            component = members.at( indexHint );
        else if( i != end )
            component = *i;
        else
        {
            // Warn about this behaviour
            DEBUG_ERROR( "Path %s can't be resolved", toString().c_str() );

            // The reference can't be resolved
            return 0;
        }

        // Make sure this is the component we want
        while( 0 != component->getName().compare( path->first ) )
        {
            // If we reach the end of the iteration, this path is invalid
            if( APP_WARNING( i == end )( "Invalid path:  %s", toString().c_str() ) ) return 0;

            // Get the resource from the current member iterator
            component = *i;

            // Advance the iterator and the index value
            ++i;
            ++index;
        }

        // If the index hint was wrong, save the index
        if( i != members.begin() )
            path->second = indexHint;

        // This is the new based resource
        resource = component;
    }

    // Return the resource we found
    return resource;
}



//-----------------------------------------------------------------------------
// Name:  getGenericReferencedResource
// Desc:  Obtains a generic interface to the referenced resource
//-----------------------------------------------------------------------------
const dcGenericResource* dcGenericReference::getGenericReferencedResource() const
{
    // Get the root resource
    const dcGenericResource* commonRoot = this;
    for( size_t i = 0; (i < myCommonRootHeight) && (commonRoot != 0); ++i )
        commonRoot = commonRoot->getOwner();

    // Make sure that the common root is valid; if it isn't, this is just a null reference
    if( !commonRoot ) return 0;

    // Used to iterate through the path
    const dcGenericResource* resource = commonRoot;

    // Iterate through the path listing
    for( dcGenericResource::Path::const_iterator path = myPath.begin(); path != myPath.end(); ++path )
    {
        // Get the members of the current resource
        const dcGenericResource::Array& members = resource->getMembers();
        dcGenericResource::Array::const_iterator i = members.begin(), end = members.end();
        size_t index = 0;

        // Get the referenced resource using the index hint (if possible)
        const dcGenericResource* component = 0;
        size_t indexHint = path->second;
        if( indexHint < members.size() )
            component = members.at( indexHint );
        else if( i != end )
            component = *i;
        else
        {
            // Warn about this behaviour
            DEBUG_ERROR( "Path %s can't be resolved", toString().c_str() );

            // The reference can't be resolved
            return 0;
        }

        // Make sure this is the component we want
        while( 0 != component->getName().compare( path->first ) )
        {
            // If we reach the end of the iteration, this path is invalid
            if( APP_WARNING( i == end )( "Invalid path:  %s", toString().c_str() ) ) return 0;

            // Get the resource from the current member iterator
            component = *i;

            // Advance the iterator and the index value
            ++i;
            ++index;
        }

        // This is the new based resource
        resource = component;
    }

    // Return the resource we found
    return resource;
}



//-----------------------------------------------------------------------------
// Name:  getReferencedResourceIndex
// Desc:  Returns getIndex() of the referenced resource, or -1 if the reference is invalid.
//-----------------------------------------------------------------------------
int dcGenericReference::getReferencedResourceIndex() const {
   const dcGenericResource* resource = getGenericReferencedResource();
   return resource == NULL ? -1 : resource->getIndex();
}



//-----------------------------------------------------------------------------
// Name:  setGenericReferencedResource
// Desc:  Uses an algorithm to find the path that connects this reference to another resource.
//-----------------------------------------------------------------------------
bool dcGenericReference::setGenericReferencedResource( dcGenericResource* resource )
{
    // Clear the current reference path
    myPath.clear();
    myCommonRootHeight = 0xFFFFFFFF;

    // If the parameter doesn't exist, all we had to do was erase the reference path and set
    // an invalid root index (so that when the get method is called, obtaining the root index
    // results in null)
    if( !resource ) return true;

    // Get the path of the two resources
    {
        // The paths of this resource and the one we're linking to
        dcGenericResource::Path resourcePath, referencePath;

        // Get the two paths
        resource->getPath( &resourcePath );
        this->getPath( &referencePath );

        // Use these iterators to look through the paths
        dcGenericResource::Path::iterator resourcePathIter = resourcePath.begin();
        dcGenericResource::Path::iterator referencePathIter = referencePath.begin();

        // Iterate past elements that reference the same objects (we can do
        // equality comparisons because we JUST generated these paths, and
        // the indices should be the same if the resources are, in fact, the same)
        while( resourcePathIter != resourcePath.end() &&
               referencePathIter != referencePath.end() &&
               (*resourcePathIter) == (*referencePathIter) )
        {
            resourcePathIter++;
            referencePathIter++;
        }

        // Erase the common parts of the paths
        resourcePath.erase( resourcePath.begin(), resourcePathIter );
        referencePath.erase( referencePath.begin(), referencePathIter );

        // Save the common node height
        myCommonRootHeight = referencePath.size();

        // Save the reference path
        myPath.assign( resourcePath.begin(), resourcePath.end() );
    }

    // Success
    return getGenericReferencedResource() == resource;
}



//-----------------------------------------------------------------------------
// Name:  updateReferencedPath
// Desc:  Changes whatever part of the internal path matches the old path to the new one
//-----------------------------------------------------------------------------
void dcGenericReference::updateReferencedPath( const Path& oldPath, const Path& newPath )
{
    Path::const_iterator oldPathIter = oldPath.begin(),
                         oldPathEnd  = oldPath.end();
    Path::iterator myPathIter  = myPath.begin(),
                   myPathEnd   = myPath.end();

    // Iterate through the paths, ensuring they match
    for( ; oldPathIter != oldPath.end() && myPathIter != myPath.end(); ++oldPathIter, ++myPathIter )
    {
        // If the strings don't match, don't update this reference
        if( 0 != oldPathIter->first.compare( myPathIter->first ) )
            return;
    }

    if( oldPathIter == oldPath.end() && myPathIter == myPath.end() )
    {
        // The paths match completely, so remove the old stuff from this path
        myPath.erase( myPath.begin(), myPathIter );

        // Insert elements from the new path at the start of this reference
        myPath.insert( myPath.begin(), newPath.begin(), newPath.end() );
    }
}




//----[  isMissingReference  ]-------------------------------------------------
bool dcGenericReference::isMissingReference() const {
  return !getGenericReferencedResource() && !myPath.empty();
}



//----[  replaceInPath  ]------------------------------------------------------
bool dcGenericReference::replaceInPath(const std::string& find, const std::string& replace) {
  Path::iterator myPathIter  = myPath.begin(),
                 myPathEnd   = myPath.end();
  bool changed = false;
  for (; myPathIter != myPathEnd; ++myPathIter) {
    std::string::size_type at = myPathIter->first.find(find);
    if (at != std::string::npos) {
      myPathIter->first.replace(at, find.length(), replace);
      myPathIter->second = 0;
      changed = true;
    }
  }
  return changed;
}


//----[  setGenericReferencedResourceByPathString  ]---------------------------
void dcGenericReference::setGenericReferencedResourceByPathString(
    const std::string& my_path_string,
    const std::string& referenced_path_string) {
  const char *p1 = my_path_string.c_str(), *p2 = referenced_path_string.c_str();

  size_t chars_to_remove = 0;
  while ((*p1 == *p2) && (*p1 != '\0')) {
    ++p1;
    ++p2;
    ++chars_to_remove;
  }

  // Get each's unique path
  std::string my_unique_path = my_path_string.substr(chars_to_remove);
  std::string ref_unique_path = referenced_path_string.substr(chars_to_remove);

  // Find the common node height from this resource's remaining path
  char PATH_SEPARATOR_STRING[] = { dc::dcGenericResource::PATH_SEPARATOR, '\0' };
  std::string separator_string = PATH_SEPARATOR_STRING;
  size_t offset = 0;
  size_t common_root_height = 0;
  do {
    ++common_root_height;
    offset = my_unique_path.find_first_of(separator_string, offset);
  } while ((offset++) != my_unique_path.npos);
  myCommonRootHeight = common_root_height;

  // Break up the path and build the reference
  myPath.clear();
  size_t last_offset = 0;
  do {
    offset = ref_unique_path.find_first_of(separator_string, last_offset);
    myPath.push_back(Path::value_type(ref_unique_path.substr(last_offset, offset - last_offset), 0));
    last_offset = offset + 1;
  } while (offset != ref_unique_path.npos);
}


//----[  getReferencedResourceName  ]------------------------------------------
std::string dcGenericReference::getReferencedResourceName() const {
  return (myPath.empty()) ? std::string("") : myPath.back().first;
}


//-----------------------------------------------------------------------------
// Name:  save
// Desc:  Saves this resource
//-----------------------------------------------------------------------------
bool dcGenericReference::save( dcStreamOut* stream )
{
    // Save the height of the common resource
    if( !stream->write( &myCommonRootHeight, sizeof(myCommonRootHeight) ) ) return false;

    // Save the length of the path
    size_t length = myPath.size();

    // Write the reference path's length
    if( !stream->write( &length, sizeof(length) ) ) return false;

    // Save each element of the path
    for( dcGenericResource::Path::iterator i = myPath.begin(); i != myPath.end(); ++i )
    {
        // Write the name component
        if( !stream->writeString( i->first ) ) return false;

        // Write the index hint
        if( !stream->write( &i->second, sizeof(size_t) ) ) return false;
    }

    // Success
    return true;
}


//-----------------------------------------------------------------------------
// Name:  load
// Desc:  Loads this resource
//-----------------------------------------------------------------------------
bool dcGenericReference::load( dcStreamIn* stream )
{
    // Read the height of the common resource
    if( !stream->read( &myCommonRootHeight, sizeof(myCommonRootHeight) ) ) return false;

    // Clear the current path
    myPath.clear();

    // Read the length of the path
    size_t length;
    if( !stream->read( &length, sizeof(length) ) ) return false;

    // Read each element of the path
    for( size_t i = 0; i < length; ++i )
    {
        // The path element we're building
        dcGenericResource::Path::value_type element;

        // Read the element string
        if( !stream->readString( &element.first ) ) return false;

        // Read the index hint
        if( !stream->read( &element.second, sizeof(size_t) ) ) return false;

        // Add to the path
        myPath.push_back( element );
    }

    // Success
    return true;
}


//-----------------------------------------------------------------------------
// Name:  toString
// Desc:  Gets the string representation of this resource
//-----------------------------------------------------------------------------
std::string dcGenericReference::toString() const
{
    // Check the root height
    if( myCommonRootHeight == 0xFFFFFFFF ) return "null";

    // Get the root height
    char root[32];
    sprintf_s(root,
              sizeof(root),
              "root(%lu):%c",
              myCommonRootHeight,
              dcGenericResource::PATH_SEPARATOR);

    // Build the string
    std::string referenceString = root;
    for( dcGenericResource::Path::const_iterator i = myPath.begin(); i != myPath.end(); ++i )
    {
        // Construct the prefix
      if (i != myPath.begin()) {
        char seperator[] = { dcGenericResource::PATH_SEPARATOR, '\0' };
        referenceString.append(seperator);
      }
      referenceString.append(i->first);
    }

    // Save the string
    return referenceString;
}



//-----------------------------------------------------------------------------
// Name:  compareTo
// Desc:  Compares this path to another path
//-----------------------------------------------------------------------------
int dcGenericReference::compareTo( const dcGenericResource* other ) const
{
    if( other == 0 || getTypeID() != other->getTypeID() )
        return dcGenericResource::compareTo( other );

    // since the path strings contain all of the information, we can just compare them!
    return toString().compare(((dcGenericReference*)other)->toString());
}




//----[  serialize  ]----------------------------------------------------------
void dcGenericReference::serialize(std::string* string) {
  string->assign(toString());
}




//----[  interpret  ]----------------------------------------------------------
void dcGenericReference::interpret(const char* string) {
  const char* path = string;
  if (stricmp(string, "null") == 0) {
    myPath.clear();
    myCommonRootHeight = 0xFFFFFFFF;
    return;
  }
  myPath.clear();
  while (*path != dc::dcGenericResource::PATH_SEPARATOR) {
    if (*path == '\0') {
      myCommonRootHeight = 0xFFFFFFFF;
      return;
    } else {
      ++path;
    }
  }
  sscanf_s(string, "root(%lu)", &myCommonRootHeight);
  while (*(++path) != '\0') {
    string = path;
    while (*path != '\0' &&
           *path != dc::dcGenericResource::PATH_SEPARATOR) {
      ++path;
    }
    Path::value_type value;
    value.first.assign(string, path - string);
    value.second = 0;
    myPath.push_back(value);
  }
}

