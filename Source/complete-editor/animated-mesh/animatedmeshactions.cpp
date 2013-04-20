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
#include "shared/animated-mesh/tools/unseenskinnedmeshresource.h"
#include "shared/animated-mesh/tools/unseenkeyframeanimation.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "shared/animated-mesh/animatedmeshutils.h"
#include "shared/animated-mesh/animatedmeshdata.h"
#include "shared/animated-mesh/renderableanimatedmesh.h"



//----[  UnseenSkinnedMeshResource  ]------------------------------------------
BEGIN_DEFINING_ACTIONS(UnseenSkinnedMeshResource)
END_DEFINING_ACTIONS()




//----[  UnseenKeyframeAnimation  ]--------------------------------------------
BEGIN_DEFINING_ACTIONS(UnseenKeyframeAnimation)
END_DEFINING_ACTIONS()



//----[  SelectAnimatedMesh  ]------------------------------------------------------
int SelectAnimatedMesh(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<UnseenSkinnedMeshResource>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow( ),
        "Select AnimatedMesh",
        "Pick the Animated Mesh to reference",
        tool->getAnimatedMeshes(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}





//----[  AddAnimatedMeshes  ]--------------------------------------------------
int AddAnimatedMeshes(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<UnseenSkinnedMeshResource>, list);
  std::string path;
  std::list<std::string> files;
  if (!dcx::dcxOpenMultipleFilesDialog(
        tool->topWindow(),
        "Select Sounds",
        "Unseen Animated Mesh Files (*.usm)\0"\
          "*.USM\0\0",
        &path,
        &files)) return 0;
  for (std::list<std::string>::iterator i = files.begin();
       i != files.end();
       ++i) {
    UnseenSkinnedMeshResource* animated_mesh
      = (UnseenSkinnedMeshResource*)list->generateElement(*i);
    animated_mesh->getFileName()->setValue(path + "\\" + *i);
  }

  return 1;
}



//----[  ValidateAnimatedMesh  ]-----------------------------------------------
int ValidateAnimatedMesh(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::AnimatedMesh;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<UnseenSkinnedMeshResource>::Element, mesh);

  UnseenSkinnedMesh* unseen_mesh;
  if (!mesh->loadSkinnedMesh(true, &unseen_mesh)) {
    return 0;
  }

  AnimatedMeshData animated_mesh;
  AnimatedMeshUtil_InterpretUnseenSkinnedMesh(unseen_mesh, &animated_mesh);

  RenderableAnimatedMesh renderable_animated_mesh;
  AnimatedMeshUtil_CreateRenderableAnimatedMesh(
    tool->getD3DDevice(),
    &animated_mesh,
    &renderable_animated_mesh);
  AnimatedMeshUtil_DeallocateRenderableAnimatedMesh(&renderable_animated_mesh);

  AnimatedMeshUtil_DeallocateAnimatedMeshData(&animated_mesh);

  return 1;
}

//----[  dcList<UnseenSkinnedMeshResource>  ]----------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenSkinnedMeshResource>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Animated Meshes...", &AddAnimatedMeshes },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenSkinnedMeshResource>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Validate", &ValidateAnimatedMesh },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenSkinnedMeshResource>::Reference)
  { "Select...", &SelectAnimatedMesh },
  LIST_REFERENCE_ACTIONS(UnseenSkinnedMeshResource),
END_DEFINING_ACTIONS()



//----[  AddSkeletalAnimations  ]----------------------------------------------
int AddSkeletalAnimations(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Sound::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<UnseenKeyframeAnimation>, list);
  std::string prefix;
  if (!dcx::dcxWin32PromptTextInput(
      tool->topWindow(),
      "Add Skeletal Animations",
      "Type a prefix for these animations:",
      &prefix,
      false, // password
      false, // numeric
      false)) { // uppercase
    return 0;
  }
  std::string path;
  std::list<std::string> files;
  if (!dcx::dcxOpenMultipleFilesDialog(
        tool->topWindow(),
        "Add Skeletal Animations",
        "Unseen Keyframe Animation Files (*.uka)\0"\
          "*.UKA\0\0",
        &path,
        &files)) {
    return 0;
  }
  for (std::list<std::string>::iterator i = files.begin();
       i != files.end();
       ++i) {
    UnseenKeyframeAnimation* animation
      = (UnseenKeyframeAnimation*)list->generateElement(prefix + (*i));
    animation->getFileName()->setValue(path + "\\" + *i);
  }

  return 1;
}



//----[  dcList<UnseenKeyframeAnimation>  ]------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenKeyframeAnimation>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Skeletal Animations...", &AddSkeletalAnimations },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenKeyframeAnimation>::Element)
  COLLECTION_ELEMENT_ACTIONS,
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<UnseenKeyframeAnimation>::Reference)
  LIST_REFERENCE_ACTIONS(UnseenKeyframeAnimation),
END_DEFINING_ACTIONS()

