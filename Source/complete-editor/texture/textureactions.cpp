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
#include "shared/texture/tools/texture.h"
#include "shared/texture/tools/animatedtexture.h"
#include "shared/texture/tools/statictexture.h"
#include "shared/texture/tools/slidingtexture.h"
#include "shared/texture/tools/circlingtexture.h"
#include "shared/evidyontool/directcraftactions.h"
#include "complete-editor/completeeditor.h"
#include <dcx/win32>
#include "complete-editor/d3dwindow.h"




//----[  TextureDescription::BlendType  ]--------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcEnum<Evidyon::Texture::TextureDescription::BlendType>)
  ENUM_ACTIONS(Evidyon::Texture::TextureDescription::BlendType),
END_DEFINING_ACTIONS()



BEGIN_DEFINING_ACTIONS(Evidyon::Texture::Tools::AnimatedTexture)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Texture::Tools::SlidingTexture)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Texture::Tools::StaticTexture)
END_DEFINING_ACTIONS()

BEGIN_DEFINING_ACTIONS(Evidyon::Texture::Tools::CirclingTexture)
END_DEFINING_ACTIONS()





//----[  AddMultipleTexturesToTable  ]-----------------------------------------
template <typename T> int AddMultipleTexturesToTable(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Texture>, table);

  unsigned long thisID = dc::dcClass<T>::staticResourceManager()->uniqueID();
  const unsigned long STATIC_TEXTURE = dc::dcClass<StaticTexture>::staticResourceManager()->uniqueID();
  const unsigned long SLIDING_TEXTURE = dc::dcClass<SlidingTexture>::staticResourceManager()->uniqueID();
  const unsigned long ANIMATED_TEXTURE = dc::dcClass<AnimatedTexture>::staticResourceManager()->uniqueID();
  const unsigned long CIRCLING_TEXTURE = dc::dcClass<CirclingTexture>::staticResourceManager()->uniqueID();

  // Get the name of this type
  std::string suffix;
  if (thisID == STATIC_TEXTURE) {
      suffix = "static";
  } else if (thisID == SLIDING_TEXTURE) {
      suffix = "sliding";
  } else if (thisID == ANIMATED_TEXTURE) {
      suffix = "animated";
  } else if (thisID == CIRCLING_TEXTURE) {
      suffix = "circling";
  }
  else {
      suffix = "???";
  }

  // Build a prompt
  std::string prompt =
    "Pick images for which to add " + suffix + " textures";

  // Let the user pick a set of textures
  dc::dcGenericResource::Array selection;
  if (DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Multiple Textures",
        prompt.c_str(),
        tool->getImages(),
        &selection,
        false))
  {
    if (thisID == STATIC_TEXTURE) {
      // Remove the suffix for static textures
      suffix = "";
    } else {
      suffix = " (" + suffix + ")";
    }

    // Create a texture for each of these selections
    for (dc::dcGenericResource::Array::iterator i = selection.begin(); i != selection.end(); ++i) {
      T* texture = (T*)table->create<T>((*i)->getName() + suffix);
      texture->setImageReference(*i);
    }
  }

  return 1;
}





//----[  PreviewTextureDialog  ]-----------------------------------------------
class PreviewTextureDialog : public dcx::dcxWin32DialogController, Evidyon::Tools::D3DWindow {
public:
  bool begin(Evidyon::Tools::CompleteEditor* editor,
             size_t texture_index,
             Evidyon::Texture::Tools::Texture* texture) {
    editor->acquireTextures(false);
    texture_index_ = texture_index;
    Evidyon::Image::Tools::Image* image;
    texture->getSourceImage(&image);
    if (!image) return false;
    LPDIRECT3DTEXTURE9 d3dtexture = image->getD3DTexture();
    if (!texture) return false;
    d3dtexture->GetLevelDesc(0, &desc);
    editor_ = editor;
    dcx::dcxWin32DialogTemplate dialog;
    dialog.setStyleAsResizeableAppWindow();
    dialog.setTitle("Image Preview");
    BYTE buffer[256];
    LPCDLGTEMPLATE dlg_template = dialog.compile(buffer, sizeof(buffer));
    dialog_handle_ = createDialog(GetModuleHandle(NULL),
                                  dlg_template,
                                  editor->topWindow());
    dcx::dcxWin32SetWindowClientSize(dialog_handle_, desc.Width, desc.Height);
    return editor->addD3DWindow(this);
  }

  HWND getWindowHandle() { return dialog_handle_; }


  //----[  render  ]-------------------------------------------------------------
  bool render(Evidyon::Tools::CompleteEditor* editor,
              LPDIRECT3DDEVICE9 d3d_device) {
    if (SUCCEEDED(d3d_device->BeginScene())) {
      d3d_device->Clear(0,
                        NULL,
                        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                        D3DCOLOR_XRGB(255,0,0),
                        1.0f,
                        0);
      editor->getRenderStateManager()->resetState();
      editor->getRenderStateManager()->setGeometryTexture(texture_index_, false);
      struct Vertex {
        FLOAT x, y, z, rhw; // The transformed position for the vertex
        FLOAT u, v;         // The coordinates
      } vertices[4] = {
        { 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f },
        { float(desc.Width), 0.0f, 0.0f, 0.5f, 1.0f, 0.0f },
        { float(desc.Width), float(desc.Height), 0.0f, 0.5f, 1.0f, 1.0f },
        { 0.0f, float(desc.Height), 0.0f, 0.5f, 0.0f, 1.0f },
      };
      d3d_device->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
      d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
      d3d_device->EndScene();
    }
    return true;
  }

private:


  //----[  dialogEvent  ]------------------------------------------------------
  virtual INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLOSE) {
      editor_->removeD3DWindow(this);
      EndDialog(hDlg, 0);
      DestroyWindow(hDlg); 
      SetWindowLong(hDlg, GWL_USERDATA, NULL);
      delete this;
    } else if (uMsg == WM_PAINT) {
      editor_->renderD3DWindow(this);
    }
    return 0;
  }

private:
  size_t texture_index_;
  HWND dialog_handle_;
  Evidyon::Tools::CompleteEditor* editor_;
  D3DSURFACE_DESC desc;
};



//----[  PreviewTexture  ]-----------------------------------------------------
int PreviewTexture(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Texture>::Element, texture_interface);
  PreviewTextureDialog* ptd = new PreviewTextureDialog();
  if (!ptd->begin(tool,
                  texture_interface->getIndex(),
                  texture_interface->getImplementation())) {
    delete ptd;
  }
  return 0;
}




//----[  SelectTexture  ]------------------------------------------------------
int SelectTexture(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Texture>::Reference, reference);

  // Let the user pick a source mesh
  dc::dcGenericResource* selection = reference->getGenericReferencedResource();
  if (DirectCraftActions::PromptSelectSingleResource(
        tool->topWindow(),
        "Select Texture",
        "Pick the texture to reference",
        tool->getTextures(),
        &selection)) {
    reference->setGenericReferencedResource(selection);
    return 1;
  }

  return 0;
}


#define IMAGE_FILE_FILTER "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0"


//----[  AddStaticImageFiles  ]------------------------------------------------
int AddStaticImageFiles(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Texture>, texture_table);

  using namespace Evidyon::Image::Tools;
  dc::dcTable<Image>* image_table = tool->getImageTable();

  std::string path;
  std::list<std::string> files;

  if (!dcx::dcxOpenMultipleFilesDialog(
      window,
      "Select Images",
      IMAGE_FILE_FILTER,
      &path,
      &files)) {
    return 0;
  }

  for (std::list<std::string>::iterator i = files.begin();
       i != files.end();
       ++i) {
    D3DXImage* image = (D3DXImage*)image_table->create<D3DXImage>(*i);
    image->getFileName()->setValue(path + "\\" + *i);

    Texture* texture = (Texture*)texture_table->create<StaticTexture>(*i);
    texture->setImageReference(image->getOwner());
  }

  return 1;
}






//----[  AddStaticSubdividedImageFiles  ]--------------------------------------
int AddStaticSubdividedImageFiles(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcTable<Texture>, texture_table);

  using namespace Evidyon::Image::Tools;
  dc::dcTable<Image>* image_table = tool->getImageTable();

  std::string path;
  std::list<std::string> files;
  size_t horizontal, vertical;

  if (!dcx::dcxOpenMultipleFilesDialog(
        window,
        "Select Image(s)",
        IMAGE_FILE_FILTER,
        &path, &files))
      return 0;
  if (!dcx::dcxWin32PromptNumericInput(window, "Import Subdivided Images",
                                        files.size() == 1 ? "How many columns of sub-images does this image contain?" :
                                                            "How many columns of sub-images do these images contain?", &horizontal, false))
      return 0;
  if (!dcx::dcxWin32PromptNumericInput(window, "Import Subdivided Images",
                                        files.size() == 1 ? "How many rows of sub-images does this image contain?" :
                                                            "How many rows of sub-images do these images contain?", &vertical, false))
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
        D3DXImage* image = (D3DXImage*)image_table->create<D3DXImage>(*fileIter + subImageText);
        image->getFileName()->setValue(sourceFile.c_str());
        image->getSubImage()->setValue(subImage);
        image->getSubImageColumns()->setValue(horizontal);
        image->getSubImageRows()->setValue(vertical);

        Texture* texture = (Texture*)texture_table->create<StaticTexture>(*fileIter + subImageText);
        texture->setImageReference(image->getOwner());
      }
    }
  }

  // Changed something
  return 1;
}




//----[  Texture  ]------------------------------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcInterface<Evidyon::Texture::Tools::Texture>)
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Texture::Tools::Texture>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Static Textures...", &AddMultipleTexturesToTable<Evidyon::Texture::Tools::StaticTexture> },
  { "Add Circling Textures...", &AddMultipleTexturesToTable<Evidyon::Texture::Tools::CirclingTexture> },
  { "Add Sliding Textures...", &AddMultipleTexturesToTable<Evidyon::Texture::Tools::SlidingTexture> },
  { "Add Animated Textures...", &AddMultipleTexturesToTable<Evidyon::Texture::Tools::AnimatedTexture> },
  ACTION_LIST_SEPARATOR,
  { "Add Static Image Files...", &AddStaticImageFiles },
  { "Add Static Subdivided Image Files...", &AddStaticSubdividedImageFiles },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Texture::Tools::Texture>::Element)
  COLLECTION_ELEMENT_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Preview", &PreviewTexture },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcTable<Evidyon::Texture::Tools::Texture>::Reference)
  { "Select", &SelectTexture },
  { "Clear", &DirectCraftActions::ClearReference },
END_DEFINING_ACTIONS()






//----[  AddTextureReferencesToList  ]-----------------------------------------
int AddTextureReferencesToList(dc::dcGenericResource* resource, void* param) {
  using namespace Evidyon::Texture::Tools;
  BEGIN_ACTION_BODY();
  CAST_ACTION_RESOURCE(dc::dcList<Evidyon::Tools::TextureReference>, list);

  // Let the user pick a set of textures
  dc::dcGenericResource::Array selection;
  if (!DirectCraftActions::PromptSelectMultipleResources(
        tool->topWindow(),
        "Add Textures",
        "Pick texture references to add to the list.",
        tool->getTextures(),
        &selection,
        false)) {
    return 0;
  }

  // Create references
  for (dc::dcGenericResource::Array::iterator i = selection.begin();
       i != selection.end(); ++i) {
    typedef dc::dcList<Evidyon::Tools::TextureReference>::Element Element;
    Element* element = (Element*)list->generateElement((*i)->getName());
    element->setGenericReferencedResource(*i);
  }

  return 1;
}


//----[  dc::dcList<TextureReference>  ]---------------------------------------
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::TextureReference>)
  COLLECTION_ACTIONS,
  ACTION_LIST_SEPARATOR,
  { "Add Textures...", &AddTextureReferencesToList },
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::TextureReference>::Element)
  { "Select", &SelectTexture },
  LIST_REFERENCE_ACTIONS(Evidyon::Tools::Texture),
END_DEFINING_ACTIONS()
BEGIN_DEFINING_ACTIONS(dc::dcList<Evidyon::Tools::TextureReference>::Reference)
  LIST_REFERENCE_ACTIONS(Evidyon::Tools::TextureReference),
END_DEFINING_ACTIONS()