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
#ifndef __DCREFERENCE_H__
#define __DCREFERENCE_H__

namespace dc {


/**
 * A reference is used like a C++ pointer, to refer to an object that is somewhere else in the
 * program.  This structure is more effective than using an actual pointer when programming
 * because it can be saved to a stream and recovered later.
 */
class dcGenericReference : public dcGenericResource
{
public:

  /**
   * Intiailizes this class
   */
  dcGenericReference();

  /**
   * Returns 'true' because all subclasses will implement dcGenericReference
   */
  bool isReference() const;

  /**
   * Obtains a generic interface to the referenced resource.  If the resource couldn't be
   * found, then this method returns null.
   */
  dcGenericResource* getGenericReferencedResource();

  /**
   * Obtains a generic interface to the referenced resource.  If the resource couldn't be
   * found, then this method returns null.  This method does not update path hints, so the
   * non-const version should be used if at all possible.
   */
  const dcGenericResource* getGenericReferencedResource() const;

  /**
   * Returns getIndex() of the referenced resource, or -1 if the reference is invalid.
   */
  int getReferencedResourceIndex() const;

  /**
   * Uses an algorithm to find the path that connects this reference to another resource.
   * These resources must share a common root member--one with a null owner--at some
   * point in the member hierarchy.  The root member will not necessairily be involved
   * in the reference, however--the algorithm optimizes for the shortest path.  If passed
   * a null pointer, the reference is erased.
   */
  bool setGenericReferencedResource( dcGenericResource* resource );

  /**
   * If a resource is renamed, this method can be called to make sure that the references
   * involving that resources remain valid.  If a part of the old path is involved in
   * this reference, it will replace that path with the new path.
   *
   * For example:
   *      current = "Atlas\Map Layers\grass\Terrain Options\grass1"
   *      oldPath = "Atlas\Map Layers\grass"
   *      newPath = "Atlas\Map Layers\ground"
   *
   * This will change the current path to:
   *      current = "Atlas\Map Layers\ground\Terrain Options\grass1"
   *
   * @oldPath The old path
   * @newPath The new path
   */
  void updateReferencedPath( const Path& oldPath, const Path& newPath );

  // Makes this resource point to a new resource based on the strings provided.
  // This functionality is provided to facilitate the following situation:
  //  ROOT contains resources A and 1.  A contains B.  1 contains 2.  Using a
  // dcResourceStorage file type, an editor was designed only to access resource B,
  // but B has members that reference resources that are members of 2.
  // In order for these members to be editable, the editor can load members of 2
  // as name-only generic resources and create path-strings that allow the
  // resource to be saved correctly even if they don't technically exist in memory.
  void setGenericReferencedResourceByPathString(const std::string& my_path_string,
                                                const std::string& referenced_path_string);

  // Obtains the name of the resource that this reference points to.  Useful as
  // a shortcut if you know the full path to the target's owner (everything
  // except for the exact member.)
  std::string getReferencedResourceName() const;

  // Returns true if the reference has a path, but it can't be resolved
  bool isMissingReference() const;

  // Replaces a part of the path.  Returns 'true' if a replacement was made.
  bool replaceInPath(const std::string& find, const std::string& replace);

public:

  /**
   * Obtains the type of resource that this reference can point to
   */
  virtual unsigned long getReferenceableTypeID() const = 0;


public:

  /**
   * Saves this resource into the destination stream
   */
  bool save( dcStreamOut* stream );

  /**
   * Loads this resource from the source stream
   */
  bool load( dcStreamIn* stream );

  /**
   * Gets the string representation of this resource
   */
  std::string toString() const;

  /**
   * Compares this resource with another resource
   */
  int compareTo( const dcGenericResource* other ) const;

  virtual void serialize(std::string* string);
  virtual void interpret(const char* string);

protected:

  /// How many levels to ascend in the resource tree to reach the common root
  size_t myCommonRootHeight;

  /// The composed reference path from the common root
  Path myPath;
};


/**
* References a resource of a specific type
* TODO: cleanup, update naming
*/
template <typename T> class dcReference : public dcResource<dcReference<T>,dcGenericReference>
{
public:

  /// The type of resource that this class can reference
  typedef T ResourceType;


public:

  /**
   * Gets the resource that this class references
   */
  inline ResourceType* getReferencedResource()
  {
      return (ResourceType*)getGenericReferencedResource();
  }

  /**
   * Gets the resource that this class references
   */
  inline const ResourceType* getReferencedResource() const
  {
      return (const ResourceType*)getGenericReferencedResource();
  }

  /**
   * 
   */
  void setReferencedResource( ResourceType* resource )
  {
      setGenericReferencedResource( resource );
  }

  /**
   * Gets the type ID of the resource type that this reference can point to
   */
  unsigned long getReferenceableTypeID() const
  {
      return dcClass<T>::staticResourceManager()->uniqueID();
  }

  /**
   * Gets the type ID of the resource type that this reference can point to
   */
  static unsigned long staticGetReferenceableTypeID()
  {
      return dcClass<T>::staticResourceManager()->uniqueID();
  }


public:

  /**
   * Obtain the name of this resource type
   */
  static std::string staticTypeName()
  {
      return "Reference to " + T::staticTypeName();
  }
};

}


#endif