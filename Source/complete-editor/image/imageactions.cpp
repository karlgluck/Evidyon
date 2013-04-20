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
#include "shared/image/tools/image.h"
#include "shared/image/tools/d3dximage.h"
#include "shared/image/tools/colorkey.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"
#include "complete-editor/image/previewimagedialog.h"



//----[  SetColor  ]-----------------------------------------------------------
int SetColor(dc::dcGenericResource* resource, void* param) {
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dcx::dcxD3DColor, color);
  COLORREF chosen_color;
  if (!dcx::dcxChooseColorDialog(window,
                                 color->getRGBColor(),
                                 &chosen_color)) {
    return 0;
  }
  color->setRGBColor(chosen_color);
  return 1;
}


//----[  ColorKey  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dcx::dcxD3DColor)
  { "Set Color", &SetColor },
END_DEFINING_ACTIONS()


//----[  SetColorKey  ]--------------------------------------------------------
int SetColorKey(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(ColorKey, color_key);
  if (0 == SetColor(color_key->getColor(), param)) return 0;
  color_key->setEnabled(true);
  return 1;
}



//----[  DisableColorKey  ]----------------------------------------------------
int DisableColorKey(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(ColorKey, color_key);
  color_key->setEnabled(false);
  return 1;
}


//----[  ColorKey  ]-----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Image::Tools::ColorKey)
  { "Set Color", &SetColorKey },
  { "Disable", &DisableColorKey },
END_DEFINING_ACTIONS()




//----[  AddD3DXImagesToTable  ]-----------------------------------------------
int AddD3DXImagesToTable(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Image>, table);

  std::string path;
  std::list<std::string> files;

  if (!tool->promptUserForImageFiles("Select Images", &path, &files)) {
    return 0;
  }

  for (std::list<std::string>::iterator i = files.begin();
       i != files.end();
       ++i) {
    D3DXImage* image = (D3DXImage*)table->create<D3DXImage>(*i);
    image->getFileName()->setValue(path + "\\" + *i);
  }

  return 1;
}


//----[  ImportSubdividedImages  ]---------------------------------------------
// Lets the user pick images that contain multiple sub-images and import them
int ImportSubdividedImages(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Image>, imageTable);

  std::string path;
  std::list<std::string> files;
  size_t horizontal, vertical;

  if (!tool->promptUserForImageFiles("Select Image(s)", &path, &files)) {
      return 0;
  }
  if (!dcx::dcxWin32PromptNumericInput(
          window,
          "Import Subdivided Images",
          files.size() == 1 ? "How many columns of sub-images does this image contain?" :
                              "How many columns of sub-images do these images contain?",
          &horizontal,
          false)) {
    return 0;
  }
  if (!dcx::dcxWin32PromptNumericInput(
          window,
          "Import Subdivided Images",
          files.size() == 1 ? "How many rows of sub-images does this image contain?" :
                              "How many rows of sub-images do these images contain?",
          &vertical,
          false))
      return 0;

  // The number of subdivisions
  size_t totalSubImages = horizontal * vertical;

  // For each of the files in the list, import divisions
  for (std::list<std::string>::iterator fileIter = files.begin();
       fileIter != files.end(); ++fileIter) {

    // Build the path for this image
    std::string sourceFile = path  + "\\" + *fileIter;

    // Repeat for each sub-image
    for (size_t x = 0; x < horizontal; ++x) {
      for (size_t y = 0; y < vertical; ++y) {
        // Get the subdivision index and its string equivalent
        int subImage = y * horizontal + x;
        char subImageText[16];
        sprintf_s(subImageText, sizeof(subImageText), " (%i of %i)", subImage + 1, totalSubImages);


        // Create the image
        D3DXImage* image = (D3DXImage*)imageTable->create<D3DXImage>(*fileIter + subImageText);
        image->getFileName()->setValue(sourceFile.c_str());
        image->getSubImage()->setValue(subImage);
        image->getSubImageColumns()->setValue(horizontal);
        image->getSubImageRows()->setValue(vertical);
      }
    }
  }

  // Changed something
  return 1;
}



//----[  LoadImage  ]----------------------------------------------------------
int LoadImage(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Image>::Element, image_interface);
  Image* image = image_interface->getImplementation();
  image->loadD3DTexture(tool->getD3DDevice(), true);
  return 1;
}






//----[  PreviewImage  ]-------------------------------------------------------
int PreviewImage(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Image>::Element, image_interface);
  Evidyon::PreviewImageDialog* pid = new Evidyon::PreviewImageDialog();
  if (!pid->begin(tool, image_interface->getImplementation())) {
    delete pid;
  }
  return 0;
}


//----[  SelectImage  ]--------------------------------------------------------
int SelectImage(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Image::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Image>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Image",
        "Pick the image to reference",
        tool->getImages(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}



//----[  Image  ]--------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::Image::Tools::Image>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Image::Tools::Image>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Image Files...", &AddD3DXImagesToTable },
  { "Import Subdivided Images...", &ImportSubdividedImages },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Image::Tools::Image>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Load Image", &LoadImage },
  { "Preview", &PreviewImage },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Image::Tools::Image>::Reference)
  { "Select...", &SelectImage },
END_DEFINING_ACTIONS()




//----[  D3DXImage  ]----------------------------------------------------------
BEGIN_DEFINING_ACTIONS(Evidyon::Image::Tools::D3DXImage)
END_DEFINING_ACTIONS()