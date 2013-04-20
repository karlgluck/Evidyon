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
#ifndef __UNSEENSKINNEDMESHRESOURCE_H__
#define __UNSEENSKINNEDMESHRESOURCE_H__
#pragma once



#include <dc/filename>
#include <dc/boolean>
#include "shared/animated-mesh/unseenskinnedmesh.h"
#include <dcx/d3dxmatrix>

namespace Evidyon {
namespace AnimatedMesh {
struct AnimatedMeshData;
}
}


/**
 * This class references a .usm file containing a skinned mesh ready for animation
 */
class UnseenSkinnedMeshResource :
    public dc::dcResource<UnseenSkinnedMeshResource> {
public:

  /**
   * Initializes this resource
   */
  UnseenSkinnedMeshResource();


public:

  // Loads the referenced mesh from the source file.  If it already exists
  // in memory, the internal reference is returned without reloading.
  // The method can be forced to read from disk by setting the appropriate
  // flag.
  bool loadSkinnedMesh(bool force_reload,
                       UnseenSkinnedMesh** skinnedMesh);
  void unloadSkinnedMesh();
  bool loadAnimatedMeshData(bool force_reload,
    Evidyon::AnimatedMesh::AnimatedMeshData* animated_mesh_data);

  // Accessors
  dc::dcFileName* getFileName();
  int numberOfTextureGroups();

public:

  /**
   * Gets the string name of this resource type
   * @return Class name
   */
  static std::string staticTypeName();


protected:

  // The file from which to load the mesh
  dc::dcFileName mySourceFile;

  // Whether or not to reverse the winding order of this mesh.  Setting
  // this flag corrects meshes that are inside-out.
  dc::dcBoolean myReverseWindingOrderFlag;

private:
  UnseenSkinnedMesh* loaded_mesh_;
};

#include <dc/list>

namespace Evidyon {
namespace Tools {
typedef dc::dcList<UnseenSkinnedMeshResource> AnimatedMeshList;
typedef dc::dcList<UnseenSkinnedMeshResource>::Element AnimatedMeshElement;
typedef dc::dcList<UnseenSkinnedMeshResource>::Reference AnimatedMeshReference;
}
}



#endif