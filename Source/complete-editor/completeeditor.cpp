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
#include "completeeditor.h"
#include <dc/dcresourcestorage2.h>
#include "common/safemacros.h"
#include "d3dwindow.h"
#include <dcx/d3d>
#include <assert.h>
#include "shared/mesh/meshutils.h"
#include "shared/mesh/mesh.h"
#include <dcx/win32>
#include "shared/sound/sound.h"
#include "shared/animated-mesh/animatedmeshutils.h"
#include "shared/animated-mesh/animatedmeshdata.h"
#include "shared/animated-mesh/renderableanimatedmesh.h"
#include "shared/actor/actorprofile.h"
#include "shared/gamefile/gamefilecompiler.h"
#include "shared/specialfx/tools/specialfxcompiler.h"
#include "shared/specialfx/specialfx.h"
#include "shared/specialfx/specialfxbinding.h"
#include "shared/specialfx/specialfxemitter.h"



namespace Evidyon {
namespace Tools {

//static HWND hD3DWindowOverride = NULL;

  
//----[  CompleteEditor  ]-----------------------------------------------------
CompleteEditor::CompleteEditor() {
  d3d_ = NULL;
  d3d_device_ = NULL;
  window_being_rendered_ = NULL;
  load_animated_meshes_ = true;

  // Properties saved in the editor file
  setName("Volucris"); // holdover :)
  member("Client Data", &client_data_);
  member("Server Data", &server_data_);
  member("Images", &images_);
  member("Textures", &textures_);
  member("Meshes", &meshes_);
  member("VisualFX", &visual_fx_);
  member("Scenery", &scenery_);
  member("Animated Meshes", &animated_meshes_);
  member("Skeletal Animations", &skeletal_animations_);
  member("Skinned Meshes", &skinned_meshes_);
  member("Zones", &zones_);
  member("Maps", &maps_);
  member("Sounds", &sounds_);
  member("Actor Profiles", &actor_profiles_);
  member("Lifeform AI", &lifeform_ai_);
  member("Races", &races_);
  member("Classes", &classes_);
  member("Items", &items_);
  member("Level Definitions", &levels_);
  member("SpecialFX", &special_fx_);
  member("Spells", &spells_);
  member("Treasure", &global_treasure_);
}




//----[  run  ]----------------------------------------------------------------
int CompleteEditor::run(HINSTANCE instance) {
  if (!beginRootDialog(this, IDR_MENU)) return E_FAIL;
  if (!acquireD3D()) return E_FAIL;
  
  while (hasRootDialog()) {

    flushWin32MessageQueue();

    // Update all active rendering windows
    for (ActiveD3DWindows::iterator i = active_d3d_windows_.begin();
         i != active_d3d_windows_.end(); ++i) {
      ActiveD3DWindow* active_window = &(*i);
      LPDIRECT3DSWAPCHAIN9 swap_chain = active_window->swap_chain;
      IDirect3DSurface9* back_buffer_surface;
      if (swap_chain) {
        swap_chain->GetBackBuffer(0,
                                  D3DBACKBUFFER_TYPE_MONO,
                                  &back_buffer_surface);
        d3d_device_->SetRenderTarget(0, back_buffer_surface);
        back_buffer_surface->Release();
        window_being_rendered_ = active_window;
        if (active_window->window->render(this, d3d_device_)) {
          swap_chain->Present(NULL, NULL, NULL, NULL, NULL);
        }
      } else {
        active_window->window->render(this, d3d_device_);
        d3d_device_->Present(NULL, NULL, NULL, NULL);
      }
      window_being_rendered_ = NULL;
    }
  }

  releaseD3D();
  return S_OK;
}



//----[  getD3DDevice  ]-------------------------------------------------------
LPDIRECT3DDEVICE9 CompleteEditor::getD3DDevice() {
  return d3d_device_;
}



//----[  addD3DWindow  ]-------------------------------------------------------
bool CompleteEditor::addD3DWindow(D3DWindow* window) {
  ActiveD3DWindow active_window;
  if (!dcx::dcxD3DDuplicateDeviceForWindow(d3d_device_,
                                           window->getWindowHandle(),
                                           &active_window.swap_chain,
                                           NULL)) return false;

  //active_window.swap_chain = NULL;
  //active_window.window = window;
  //hD3DWindowOverride = window->getWindowHandle();
  //acquireD3D();
  //active_d3d_windows_.push_back(active_window);

  //return true;
  LPDIRECT3DSWAPCHAIN9 swap_chain = active_window.swap_chain;


  // Get the suface description of the back-buffer; this is our rendering window
  IDirect3DSurface9* back_buffer_surface;
  swap_chain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &back_buffer_surface);
  D3DSURFACE_DESC desc;
  back_buffer_surface->GetDesc(&desc);
  SAFE_RELEASE(back_buffer_surface);

  // Check the current stencil surface
  LPDIRECT3DSURFACE9 stencil_surface;
  bool recreateStencil = true;
  HRESULT hr = d3d_device_->GetDepthStencilSurface(&stencil_surface);
  CONFIRM(SUCCEEDED(hr)) else {
    SAFE_RELEASE(active_window.swap_chain);
    return false;
  }

  // Obtain the stencil surface description
  D3DSURFACE_DESC stencil_desc;
  stencil_surface->GetDesc( &stencil_desc );

  // If the stencil is already the right size, we don't have to make a new one
  if (stencil_desc.Width >= desc.Width &&
      stencil_desc.Height >= desc.Height) {
    recreateStencil = false;
  }
  SAFE_RELEASE(stencil_surface);

  // Create a new depth stencil surface, if necesary (to make sure that the entire region is rendered)
  if (recreateStencil) {
    HRESULT hr = d3d_device_->CreateDepthStencilSurface(
      desc.Width,
      desc.Height,
      D3DFMT_D16,
      D3DMULTISAMPLE_NONE,
      0,
      TRUE,
      &stencil_surface,
      NULL);

    CONFIRM(SUCCEEDED(hr)) else {
      SAFE_RELEASE(active_window.swap_chain);
      return false;
    }

    // The surface was created, so set it
    d3d_device_->SetDepthStencilSurface(stencil_surface);
    SAFE_RELEASE(stencil_surface);
  }

  // Success
  active_window.window = window;
  active_d3d_windows_.push_back(active_window);
  return true;
}



//----[  removeD3DWindow  ]----------------------------------------------------
void CompleteEditor::removeD3DWindow(D3DWindow* window) {
  for (ActiveD3DWindows::iterator i = active_d3d_windows_.begin();
       i != active_d3d_windows_.end(); ++i) {
    if (i->window == window) {
      SAFE_RELEASE(i->swap_chain);
      active_d3d_windows_.erase(i);
      return;
    }
  }
  assert(false);
}



//----[  acquireImages  ]------------------------------------------------------
void CompleteEditor::acquireImages(bool force_reload_all) {
  renderstate_manager_.clearTextures(); // implicit
  const dc::dcGenericResource::Array& images = images_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  for (i = images.begin(); i != images.end(); ++i) {
    typedef dc::dcTable<Evidyon::Image::Tools::Image>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Image::Tools::Image* image = element->getImplementation();
    image->loadD3DTexture(d3d_device_, force_reload_all);
  }
}



//----[  acquireTextures  ]----------------------------------------------------
void CompleteEditor::acquireTextures(bool force_reload_images) {
  acquireImages(force_reload_images);
  images_.sort();
  textures_.sort();
  renderstate_manager_.clearTextures();
  const dc::dcGenericResource::Array& textures = textures_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = textures.begin(); i != textures.end(); ++i, ++index) {
    typedef dc::dcTable<Evidyon::Texture::Tools::Texture>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Texture::Tools::Texture* texture = element->getImplementation();
    Texture::TextureDescription description;
    texture->compile(&description);
    texture->compileBase(&description);
    renderstate_manager_.addTexture(index, description);
  }
}



//----[  acquireMeshes  ]------------------------------------------------------
void CompleteEditor::acquireMeshes() {
  mesh_renderer_.clearMeshes();
  const dc::dcGenericResource::Array& meshes = meshes_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = meshes.begin(); i != meshes.end(); ++i, ++index) {
    typedef dc::dcTable<Evidyon::Mesh::Tools::Mesh>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Mesh::Tools::Mesh* mesh = element->getImplementation();
    Evidyon::Mesh::Mesh description;
    mesh->compile(d3d_device_, &description);
    D3DXMATRIX matrix;
    mesh->getTransform(&matrix);
    if (!D3DXMatrixIsIdentity(&matrix)) {
      Evidyon::Mesh::EvidyonMeshUtil_ApplyMeshTransform(&matrix, &description);
    }
    mesh_renderer_.addMesh(index, description);
  }
}


//----[  acquireScenery  ]-----------------------------------------------------
void CompleteEditor::acquireScenery(bool acquire_visual_fx) {
  acquireTextures(false);
  acquireMeshes();
  if (acquire_visual_fx) acquireVisualFX(false);
  scenery_renderer_.clearScenery();
  const dc::dcGenericResource::Array& scenery = scenery_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = scenery.begin(); i != scenery.end(); ++i, ++index) {
    typedef dc::dcList<Evidyon::Scenery::Tools::Scenery>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Scenery::Scenery description;
    element->compile(&mesh_renderer_, &description, NULL);
    scenery_renderer_.addScenery(index, description);
  }
}




//----[  acquireVisualFX  ]----------------------------------------------------
void CompleteEditor::acquireVisualFX(bool acquire_scenery) {
  if (acquire_scenery) acquireScenery(false);
  visual_fx_renderer_.clearVisualFX();
  const dc::dcGenericResource::Array& visual_fx = visual_fx_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = visual_fx.begin(); i != visual_fx.end(); ++i, ++index) {
    typedef dc::dcTable<Evidyon::VisualFX::Tools::VisualFX>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::VisualFX::Tools::VisualFX* visual_fx
      = element->getImplementation();
    Evidyon::VisualFX::VisualFX description;
    visual_fx->compile(&description);
    visual_fx_renderer_.addVisualFX(index, description);
  }
}




//----[  acquireSpecialFX  ]---------------------------------------------------
void CompleteEditor::acquireSpecialFX() {
  acquireVisualFX();
  acquireSounds();

  const dc::dcGenericResource::Array& special_fx = special_fx_.getMembers();
  SpecialFX::Tools::SpecialFXCompiler compiler(special_fx.size());


  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = special_fx.begin(); i != special_fx.end(); ++i, ++index) {
    Tools::SpecialFXElement* element = (Tools::SpecialFXElement*)(*i);
    element->getImplementation()->compile(index, &compiler);
  }

  special_fx_manager_.clearSpecialFX();
  special_fx_manager_.initNumberOfSpecialFX(compiler.numberOfSpecialFX());
  for (index = 0; index < compiler.numberOfSpecialFX(); ++index) {
    SpecialFX::SpecialFX description;
    compiler.compileSpecialFX(index, &description);
    special_fx_manager_.initSpecialFX(index, &description);
  }

  special_fx_manager_.clearBindings();
  special_fx_manager_.initNumberOfBindings(compiler.numberOfBindings());
  for (index = 0; index < compiler.numberOfBindings(); ++index) {
    SpecialFX::SpecialFXBinding description;
    compiler.compileBinding(index, &description);
    special_fx_manager_.initBinding(index, &description);
  }

  special_fx_manager_.clearEmitters();
  special_fx_manager_.initNumberOfEmitters(compiler.numberOfEmitters());
  for (index = 0; index < compiler.numberOfEmitters(); ++index) {
    SpecialFX::SpecialFXEmitter description;
    compiler.compileEmitter(index, &description);
    special_fx_manager_.initEmitter(index, &description);
  }
}



//----[  acquireSounds  ]------------------------------------------------------
void CompleteEditor::acquireSounds() {
  sound_manager_.clearSounds();
  const dc::dcGenericResource::Array& sounds = sounds_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = sounds.begin(); i != sounds.end(); ++i, ++index) {
    typedef dc::dcList<Evidyon::Sound::Tools::WAVSound>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Sound::Sound description;
    element->compile(&description);
    sound_manager_.addSound(index, description);
  }
}




//----[  acquireAnimatedMeshes  ]----------------------------------------------
void CompleteEditor::acquireAnimatedMeshes() {
  animated_mesh_renderer_.clearAnimatedMeshes();
  if (load_animated_meshes_ == false) return;
  using namespace AnimatedMesh;
  const dc::dcGenericResource::Array& animated_meshes
    = animated_meshes_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = animated_meshes.begin();
       i != animated_meshes.end();
       ++i, ++index) {
    typedef dc::dcList<UnseenSkinnedMeshResource>::Element Element;
    Element* element = (Element*)(*i);
    AnimatedMeshData animated_mesh;
    element->loadAnimatedMeshData(true, &animated_mesh);
    RenderableAnimatedMesh renderable_animated_mesh;
    AnimatedMeshUtil_CreateRenderableAnimatedMesh(
      d3d_device_,
      &animated_mesh,
      &renderable_animated_mesh);
    AnimatedMeshUtil_DeallocateAnimatedMeshData(&animated_mesh);
    animated_mesh_renderer_.addRenderableAnimatedMesh(
      index,
      renderable_animated_mesh);
  }
}




//----[  acquireAnimatedMeshAnimations  ]--------------------------------------
void CompleteEditor::acquireAnimatedMeshAnimations() {
  animated_mesh_renderer_.clearAnimatedMeshAnimations();
  using namespace AnimatedMesh;
  const dc::dcGenericResource::Array& animations
    = skeletal_animations_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = animations.begin();
       i != animations.end();
       ++i, ++index) {
    typedef dc::dcList<UnseenKeyframeAnimation>::Element Element;
    Element* element = (Element*)(*i);
    AnimatedMeshAnimation animated_mesh_animation;
    element->loadAnimatedMeshAnimation(false, &animated_mesh_animation);
    animated_mesh_renderer_.addAnimatedMeshAnimation(
      index,
      animated_mesh_animation);
  }
}




//----[  acquireSkinnedMeshes  ]-----------------------------------------------
void CompleteEditor::acquireSkinnedMeshes() {
  skinned_mesh_renderer_.clearSkinnedMeshes();
  using namespace AnimatedMesh;
  const dc::dcGenericResource::Array& skinned_meshes
    = skinned_meshes_.getMembers();
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = skinned_meshes.begin(); i != skinned_meshes.end(); ++i, ++index) {
    typedef dc::dcList<Evidyon::SkinnedMesh::Tools::SkinnedMesh>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::SkinnedMesh::SkinnedMesh description;
    element->compile(&description);
    skinned_mesh_renderer_.addSkinnedMesh(index, description);
  }
}



//----[  acquireActorProfiles  ]-----------------------------------------------
void CompleteEditor::acquireActorProfiles() {
  using namespace Actor;
  const dc::dcGenericResource::Array& actor_profiles
    = actor_profiles_.getMembers();
  actor_renderer_.initNumberOfActorProfiles(actor_profiles.size());
  size_t index = 0;
  dc::dcGenericResource::Array::const_iterator i;
  for (i = actor_profiles.begin(); i != actor_profiles.end(); ++i, ++index) {
    typedef dc::dcList<Evidyon::Actor::Tools::ActorProfile>::Element Element;
    Element* element = (Element*)(*i);
    Evidyon::Actor::ActorProfile description;
    element->compile(&description);
    actor_renderer_.initActorProfile(index, &description);
  }
}



//----[  acquireAllGameObjects  ]----------------------------------------------
void CompleteEditor::acquireAllGameObjects() {
  acquireTextures(true); // calls acquireImages
  acquireVisualFX(); // calls acquireScenery, acquireMeshes, acquireTextures
  acquireSounds();
  acquireAnimatedMeshes();
  acquireAnimatedMeshAnimations();
  acquireSkinnedMeshes();
  acquireActorProfiles();
}


//----[  promptUserForImageFiles  ]--------------------------------------------
bool CompleteEditor::promptUserForImageFiles(const char* prompt_title,
                                              std::string* path,
                                              std::list<std::string>* files) {
  return dcx::dcxOpenMultipleFilesDialog(
      topWindow(),
      "Select Images",
      "Image Files (*.JPG; *.JPEG; *.PNG; *.BMP; *.DDS)\0"\
        "*.JPG;*.JPEG;*.PNG;*.BMP;*.DDS\0\0",
      path,
      files);
}




//----[  getMapFromDescendant  ]-----------------------------------------------
World::Tools::Map*
CompleteEditor::getMapFromDescendant(const dc::dcGenericResource* resource) {
  while (resource) {
    typedef dc::dcClass<dc::dcList<Evidyon::World::Tools::Map>::Element> MapClass;
    if (resource->getTypeID()
      == MapClass::staticResourceManager()->uniqueID()) {
      assert(resource->getOwner() == &maps_);
      return (World::Tools::Map*)resource;
    }
    resource = resource->getOwner();
  }
  return NULL;
}




//----[  renderD3DWindow  ]----------------------------------------------------
void CompleteEditor::renderD3DWindow(D3DWindow* window) {
  // find the window
  ActiveD3DWindow* active_window = NULL;
  for (ActiveD3DWindows::iterator i = active_d3d_windows_.begin();
      i != active_d3d_windows_.end(); ++i) {
    if (i->window == window) {
      active_window = &(*i);
      break;
    }
  }
  if (active_window != NULL && (active_window != window_being_rendered_)) {
    window_being_rendered_ = active_window;
    LPDIRECT3DSWAPCHAIN9 swap_chain = active_window->swap_chain;
    IDirect3DSurface9* back_buffer_surface;
    if (swap_chain) {
      swap_chain->GetBackBuffer(0,
                                D3DBACKBUFFER_TYPE_MONO,
                                &back_buffer_surface);
      d3d_device_->SetRenderTarget(0, back_buffer_surface);
      back_buffer_surface->Release();
      window_being_rendered_ = active_window;
      if (active_window->window->render(this, d3d_device_)) {
        swap_chain->Present(NULL, NULL, NULL, NULL, NULL);
      }
    } else {
      active_window->window->render(this, d3d_device_);
      d3d_device_->Present(NULL, NULL, NULL, NULL);
    }
    window_being_rendered_ = NULL;
  }
}



//----[  renderTranslucentGroundPlane  ]---------------------------------------
void CompleteEditor::renderTranslucentGroundPlane(float x, float y) {
  renderstate_manager_.resetState();
  d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
  d3d_device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
  d3d_device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
  d3d_device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  d3d_device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  d3d_device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  d3d_device_->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
  d3d_device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  d3d_device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  d3d_device_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  d3d_device_->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
  d3d_device_->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
  d3d_device_->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
  d3d_device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
  renderstate_manager_.setWorldMatrix(0, NULL);
  float x_left = x - 20.0f;
  float x_right = x + 20.0f;
  float y_top = y - 20.0f;
  float y_bottom = y + 20.0f;
  RECT visibility_area = {
    int(x_left),
    int(y_top),
    int(x_right),
    int(y_bottom)
  };
  struct Vertex {
    FLOAT x, y, z;
    D3DCOLOR diffuse;
  };
  for (int y = visibility_area.top; y < visibility_area.bottom; ++y) {
    for (int x = visibility_area.left; x < visibility_area.right; ++x) {
      static const D3DCOLOR wire_color = D3DCOLOR_ARGB(255,255,255,255);
      Vertex vertices[4] = {
        { x*1.0f, 0.0f, y*1.0f, wire_color },
        { float(x*1.0f+1.0f), 0.0f, y*1.0f, wire_color },
        { float(x*1.0f+1.0f), 0.0f, (y+1)*1.0f, wire_color },
        { x*1.0f, 0.0f, (y+1)*1.0f, wire_color },
      };
      d3d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
    }
  }
  d3d_device_->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  static const D3DCOLOR color = D3DCOLOR_ARGB(128, 128,64,0);
  Vertex vertices[4] = {
    { x_left, 0.0f, y_top, color },
    { float(x_right), 0.0f, y_top, color },
    { float(x_right), 0.0f, y_bottom, color },
    { x_left, 0.0f, y_bottom, color },
  };
  d3d_device_->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));

}

DWORD WINAPI CompileProjectThread(LPVOID ptr) {
  ((CompleteEditor*)ptr)->compileProjectThreadFunction();
  return -1;
}

//----[  compileProject  ]-----------------------------------------------------
void CompleteEditor::compileProject() {
  CreateThread(NULL, 0, CompileProjectThread, this, 0, NULL);
}


void CompleteEditor::compileProjectThreadFunction() {
  char client_game_file[MAX_PATH], server_game_file[MAX_PATH];
  if (dcx::dcxSaveFileDialog(topWindow(), "Compile Project - Client File",
      "Evidyon Client Game Files (*.evcli)\0*.EVCLI\0\0",
      client_game_file, sizeof(client_game_file)) &&
      dcx::dcxSaveFileDialog(topWindow(), "Compile Project - Server File",
      "Evidyon Server Game Files (*.evsvr)\0*.EVSVR\0\0",
      server_game_file, sizeof(server_game_file))) {
    dcx::dcxWin32SetFileExtension(client_game_file, MAX_PATH, ".evcli");
    dcx::dcxWin32SetFileExtension(server_game_file, MAX_PATH, ".evsvr");

    load_animated_meshes_ = true; // always reset the load flag

    GameFile::GameFileCompiler compiler;
    class Callback : public GameFile::GameFileCompiler::ProgressCallback {
    public:
      std::string messages;
      virtual void echo(Type type, const char* message) {
        messages += message;
        messages += "\n";
      }
    };
    Callback callback;
    if (!compiler.compile(this, false, &callback)) {

      // this code adopted from an article by Steve Rabin in
      // Game Programming Gems (c) 2000 pp 113-114
      if (OpenClipboard(NULL)) {
        HGLOBAL memory;
        size_t bytes = callback.messages.length()+1;
        memory = GlobalAlloc(GHND|GMEM_DDESHARE, bytes);
        if (memory) {
          char* buffer = (char*)GlobalLock(memory);
          memcpy(buffer, callback.messages.c_str(), bytes);
          GlobalUnlock(memory);
          EmptyClipboard();
          SetClipboardData(CF_TEXT, memory);
        }
        CloseClipboard();
      }

      MessageBox(topWindow(),
                 "There was a compilation error; "\
                 "check clipboard for text",
                 "Compile Error",
                 MB_OK);

    } else {
      compiler.write(client_game_file, server_game_file);
      compiler.clear();
    }
  }
}



//----[  dialogMenuNotification  ]---------------------------------------------
void CompleteEditor::dialogMenuNotification(
    Evidyon::Tools::EvidyonToolDialog* dialog,
    WORD item_id,
    WORD item_index) {
  switch (item_id) {
    case ID_NEW: newProject(); break;
    case ID_OPEN: openProject(); break;
    case ID_SAVEAS: saveProjectAs(); break;
    case ID_SAVE: saveProject(); break;
    case ID_COMPILE: compileProject(); break;
    case ID_EXIT: dialog->end(0); break;
    case ID_OPTIONS_DISABLEANIMATEDMESHES: {
      load_animated_meshes_ = false;
      MessageBox(dialog->getWindowHandle(),
                 "Animated meshes will not be loaded until the editor is restarted",
                 "Disabled Animated Meshes", MB_OK);
      } break;
  }
}






//----[  toolReadFrom  ]-------------------------------------------------------
bool CompleteEditor::toolReadFrom(const char* file) {
  dc::dcResourceStorage2 storage;
  bool succeeded = false;
  if (storage.openFile(file, "pv")) {
    succeeded = storage.load("", this, true);
    storage.close();
  }
  return succeeded;
}






//----[  toolWriteTo  ]--------------------------------------------------------
bool CompleteEditor::toolWriteTo(const char* file) {
  dc::dcResourceStorage2 storage;
  bool succeeded = false;
  if (storage.openFile(file, "pv")) {
    succeeded = storage.save("", this);
    storage.close();
  }

  { // save all map masks
    assert(d3d_device_);
    const dc::dcGenericResource::Array& maps = maps_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = maps.begin(); i != maps.end(); ++i) {
      Evidyon::World::Tools::Map* map
        = (Evidyon::World::Tools::Map*)(*i);
      map->saveChangesToMapMasks(d3d_device_);
    }
  }

  return succeeded;
}





//----[  toolReset  ]----------------------------------------------------------
void CompleteEditor::toolReset() {
  {
    const dc::dcGenericResource::Array& images = images_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = images.begin(); i != images.end(); ++i) {
      typedef dc::dcTable<Evidyon::Image::Tools::Image>::Element Element;
      Element* element = (Element*)(*i);
      Evidyon::Image::Tools::Image* image = element->getImplementation();
      image->unloadD3DTexture();
    }
    images_.clear();
  }

  textures_.clear();
  meshes_.clear();
  visual_fx_.clear();
  scenery_.clear();
  maps_.clear();
  {
    const dc::dcGenericResource::Array& animated_meshes
      = animated_meshes_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = animated_meshes.begin(); i != animated_meshes.end(); ++i) {
      Tools::AnimatedMeshElement* element
        = (Tools::AnimatedMeshElement*)(*i);
      element->unloadSkinnedMesh();
    }
    animated_meshes_.clear();
  }
  {
    const dc::dcGenericResource::Array& skeletal_animations
      = skeletal_animations_.getMembers();
    dc::dcGenericResource::Array::const_iterator i;
    for (i = skeletal_animations.begin(); i != skeletal_animations.end(); ++i) {
      Tools::SkeletalAnimationElement* element
        = (Tools::SkeletalAnimationElement*)(*i);
      element->unloadAnimation();
    }
    skeletal_animations_.clear();
  }
  skinned_meshes_.clear();
  sounds_.clear();
  actor_profiles_.clear();
  lifeform_ai_.clear();
  items_.clear();
  special_fx_.clear();
  zones_.clear();

  sound_manager_.clearSounds();
  visual_fx_renderer_.clearVisualFX();
  scenery_renderer_.clearScenery();
  renderstate_manager_.clearTextures();
  mesh_renderer_.clearMeshes();
}





//----[  acquireD3D  ]---------------------------------------------------------
bool CompleteEditor::acquireD3D() {
  if (NULL == (d3d_ = Direct3DCreate9(D3D_SDK_VERSION))) {
    MessageBox(GetDesktopWindow(), "Unable to initialize the Direct3D object.  Make sure the latest version "
                                   "of DirectX is installed (microsoft.com/DirectX)", "Error", MB_OK);
	  return false;
  }

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory(&d3dpp, sizeof(d3dpp));
  d3dpp.Windowed = TRUE;
  d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
  d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

  // Create the Direct3D device on the root dialog.  We never actually use this
  // device to render, just to fork off other rendering dialogs.
  if (FAILED(d3d_->CreateDevice(D3DADAPTER_DEFAULT,
                                D3DDEVTYPE_HAL,
                                topWindow(),
                                /* use software vertex processing to support a bunch of
                                   indexed blended matrices, which newer hardware doesn't */
                                D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,
                                &d3dpp,
                                &d3d_device_))) {
    d3d_->Release();
    return false;
  }

  d3d_device_->SetRenderState(D3DRS_ZENABLE, TRUE);

  renderstate_manager_.create(d3d_device_);
  mesh_renderer_.create(d3d_device_);
  light_manager_.create(d3d_device_);
  visual_fx_renderer_.create(d3d_device_,
                             &mesh_renderer_,
                             &scenery_renderer_,
                             &light_manager_,
                             &renderstate_manager_);
  scenery_renderer_.create(&mesh_renderer_, &visual_fx_renderer_);
  animated_mesh_renderer_.create(d3d_device_);
  skinned_mesh_renderer_.create(&animated_mesh_renderer_);
  sound_manager_.create(topWindow());
  actor_renderer_.create(&skinned_mesh_renderer_,
                         &scenery_renderer_,
                         &sound_manager_,
                         &special_fx_manager_);
  special_fx_manager_.create(&visual_fx_renderer_,
                             &sound_manager_);

  return true;
}



//----[  releaseD3D  ]---------------------------------------------------------
void CompleteEditor::releaseD3D() {
  toolReset();
  actor_renderer_.destroy();
  sound_manager_.destroy();
  animated_mesh_renderer_.destroy();
  skinned_mesh_renderer_.destroy();
  visual_fx_renderer_.destroy();
  scenery_renderer_.destroy();
  light_manager_.destroy();
  mesh_renderer_.destroy();
  renderstate_manager_.destroy();
  for (ActiveD3DWindows::iterator i = active_d3d_windows_.begin();
       i != active_d3d_windows_.end(); ++i) {
    SAFE_RELEASE(i->swap_chain);
  }
  active_d3d_windows_.clear();
  SAFE_RELEASE(d3d_device_);
  SAFE_RELEASE(d3d_);
}





///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


Evidyon::Client::Tools::ClientData* CompleteEditor::getClientData() { return &client_data_; }
Evidyon::Server::Tools::ServerData* CompleteEditor::getServerData() { return &server_data_; }
dc::dcTable<Image::Tools::Image>* CompleteEditor::getImageTable() { return &images_; }
dc::dcTable<Texture::Tools::Texture>* CompleteEditor::getTextureTable() { return &textures_; }
dc::dcTable<Mesh::Tools::Mesh>* CompleteEditor::getMeshTable() { return &meshes_; }
dc::dcList<Scenery::Tools::Scenery>* CompleteEditor::getSceneryList() { return &scenery_; }
dc::dcTable<VisualFX::Tools::VisualFX>* CompleteEditor::getVisualFXTable() { return &visual_fx_; }
dc::dcTable<LifeformAI::Tools::LifeformAI>* CompleteEditor::getLifeformAITable() { return &lifeform_ai_; }

Avatar::Tools::AvatarLevelList*  CompleteEditor::getLevels() { return &levels_; }
Avatar::Tools::AvatarRaceList*  CompleteEditor::getRaces() { return &races_; }
Avatar::Tools::AvatarClassList* CompleteEditor::getClasses() { return &classes_; }
Evidyon::Treasure::Tools::GlobalTreasure* CompleteEditor::getGlobalTreasure() { return &global_treasure_; }

const dc::dcGenericResource::Array& CompleteEditor::getImages() { return images_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getTextures() { return textures_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getMeshes() { return meshes_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getScenery() { return scenery_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getVisualFX() { return visual_fx_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getZones() { return zones_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getMaps() { return maps_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getAnimatedMeshes() { return animated_meshes_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getSkeletalAnimations() { return skeletal_animations_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getSkinnedMeshes() { return skinned_meshes_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getActorProfiles() { return actor_profiles_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getSounds() { return sounds_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getLifeformAIs() { return lifeform_ai_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getItems() { return items_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getSpecialFX() { return special_fx_.getMembers(); }
const dc::dcGenericResource::Array& CompleteEditor::getSpells() { return spells_.getMembers(); }




void CompleteEditor::flushWin32MessageQueue() {
  // Handle all of the messages in the queue
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE )) {
    TranslateMessage( &msg );
    DispatchMessage( &msg );

    // Clear out the message structure
    ZeroMemory(&msg, sizeof(msg));
  }
}






}
}