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
#include "shared/mesh/tools/mesh.h"
#include "shared/mesh/tools/d3dxmesh.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"


BEGIN_DEFINING_ACTIONS(Evidyon::Mesh::Tools::D3DXMesh)
END_DEFINING_ACTIONS()




//----[  AddMultipleXMeshesToTable  ]------------------------------------------
int AddMultipleXMeshesToTable(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Mesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Mesh>, table);

  std::string path;
  std::list<std::string> files;

  if (dcx::dcxOpenMultipleFilesDialog(tool->topWindow(),
                                      "Select Meshes",
                                      "D3DX Mesh Files (*.X)\0*.X\0\0",
                                      &path,
                                      &files)) {
    for (std::list<std::string>::iterator i = files.begin(); i != files.end(); ++i)
    {
      // Add this mesh to the table
      D3DXMesh* mesh = (D3DXMesh*)table->create<D3DXMesh>(*i);
      mesh->setSourceFilePath(path + "\\" + *i);
    }

    return 1;
  } else {
    return 0;
  }
}



//----[  LoadMeshes  ]---------------------------------------------------------
int LoadMeshes(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Mesh::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Mesh>, table);
  tool->acquireMeshes();
  return 0;
}




//----[  Mesh  ]---------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::Mesh::Tools::Mesh>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Mesh::Tools::Mesh>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add .X Files...", &AddMultipleXMeshesToTable },
  { "Load All Meshes", &LoadMeshes },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Reference)
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()






//----[  dcx::dcxVector3<dc::dcFloat>  ]---------------------------------------
BEGIN_DEFINING_ACTIONS(dcx::dcxVector3<dc::dcFloat>)
END_DEFINING_ACTIONS()