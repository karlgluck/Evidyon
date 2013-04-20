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
#ifndef __COMPLETEEDITOR_H__
#define __COMPLETEEDITOR_H__
#pragma once



#include "shared/evidyontool/evidyontool.h"
#include "shared/evidyontool/directcraftactions.h"
#include "shared/evidyontool/evidyontooldialog.h"
#include "resource.h"

#include <dc/list>
#include <dc/table>
#include "shared/image/tools/image.h"
#include "shared/texture/tools/texture.h"
#include "shared/mesh/tools/mesh.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/world/tools/map.h"
#include "shared/world/tools/maplocationreference.h"
#include "shared/visualfx/tools/visualfx.h"
#include "shared/sound/tools/wavsound.h"
#include "shared/animated-mesh/tools/unseenskinnedmeshresource.h"
#include "shared/animated-mesh/tools/unseenkeyframeanimation.h"
#include "shared/skinned-mesh/tools/skinnedmesh.h"
#include "shared/actor/tools/actorprofile.h"
#include "shared/lifeformai/tools/lifeformai.h"
#include "shared/client/tools/clientdata.h"
#include "shared/zone/tools/zone.h"
#include "shared/avatar/tools/avatarracelist.h"
#include "shared/avatar/tools/avatarclasslist.h"
#include "shared/item/tools/item.h"
#include "shared/avatar/tools/avatarlevellist.h"
#include "shared/server/tools/serverdata.h"
#include "shared/specialfx/tools/specialfx.h"
#include "shared/spell/tools/spell.h"
#include "shared/treasure/tools/globaltreasure.h"

#include "shared/evidyond3drenderstatemanager.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/mesh/meshrenderer.h"
#include "shared/scenery/sceneryrenderer.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/sound/soundmanager.h"
#include "shared/animated-mesh/animatedmeshrenderer.h"
#include "shared/skinned-mesh/skinnedmeshrenderer.h"
#include "shared/actor/actorrenderer.h"
#include "shared/specialfx/specialfxmanager.h"


// Used when defining resources
#define BEGIN_ACTION_BODY() \
  Evidyon::Tools::EvidyonToolDialog* dialog =\
    static_cast<Evidyon::Tools::EvidyonToolDialog*>(param);\
  Evidyon::Tools::CompleteEditor* tool =\
    reinterpret_cast<Evidyon::Tools::CompleteEditor*>(dialog->getTool());\
  HWND window = dialog->getWindowHandle();
#define CAST_ACTION_RESOURCE(resource_type, resource_name)\
  resource_type* resource_name = static_cast<resource_type*>(resource);

namespace Evidyon {
namespace Tools {

class D3DWindow;



  
//----[  CompleteEditor  ]-----------------------------------------------------
class CompleteEditor : public Evidyon::Tools::EvidyonTool,
                       public dc::dcResource<CompleteEditor> {
public:
  CompleteEditor();
  int run(HINSTANCE instance);

  // Obtains a pointer to the rendering device.  This does not increment the
  // reference-count of the device.
  LPDIRECT3DDEVICE9 getD3DDevice();

  // Creates a swap chain for the given window and adds it to the list
  // of window to render.
  bool addD3DWindow(D3DWindow* window);

  // Frees D3D objects used internally to render the given window.  Be sure to
  // call this before the window is destroyed.
  void removeD3DWindow(D3DWindow* window);

  // Loads images into memory.  Will clear the texture bank.
  void acquireImages(bool force_reload_all);

  // Sets up textures in the renderstate-manager
  void acquireTextures(bool force_reload_images);

  // Loads meshes into the mesh renderer
  void acquireMeshes();

  // Loads scenery into the scenery renderer
  void acquireScenery(bool acquire_visual_fx = true);

  // Loads all visual FX objects
  void acquireVisualFX(bool acquire_scenery = true);

  // Loads all special FX and visual FX objects
  void acquireSpecialFX();

  // Load all sounds
  void acquireSounds();

  void acquireAnimatedMeshes();
  void acquireAnimatedMeshAnimations();
  void acquireSkinnedMeshes();

  void acquireActorProfiles();

  // Loads images, textures, meshes, scenery, etc. into memory.  Invoked by the
  // map editor and other tools that rely on many components.
  void acquireAllGameObjects();

  // Asks the user to pick image files.  If the dialog was cancelled, this method
  // returns 'false'.
  bool promptUserForImageFiles(const char* prompt_title,
                               std::string* path,
                               std::list<std::string>* files);

  // Returns the map that is in the owner path of 'resource'.  If this resource
  // is not the descendant of a map, the value is NULL.
  World::Tools::Map* getMapFromDescendant(const dc::dcGenericResource* resource);

  // Called by D3D windows when they want to render asynchronously to handle
  // unusual events (such as WM_PAINT messages).  Don't call this method to
  // perform normal rendering, as it involves a search operation to get the
  // window's swap chain.
  void renderD3DWindow(D3DWindow* window);

  // Draws a ground plane in (x-20,y-20) to (x+20,y+20) in the current device.
  // This is commonly used by preview dialogs that only edit portions of the
  // game in order to give some frame of reference.
  void renderTranslucentGroundPlane(float x, float y);

  // Runs the compilation process for the game in its current state
  void compileProject();

  // Actually runs the compilation (separate thread to keep window active)
  void compileProjectThreadFunction();

public:

  // Accessors for self-contained objects
  EvidyonD3DRenderStateManager* getRenderStateManager() { return &renderstate_manager_; }
  Mesh::MeshRenderer* getMeshRenderer() { return &mesh_renderer_; }
  EvidyonD3DLightManager* getLightManager() { return &light_manager_; }
  VisualFX::VisualFXRenderer* getVisualFXRenderer() { return &visual_fx_renderer_; }
  Scenery::SceneryRenderer* getSceneryRenderer() { return &scenery_renderer_; }
  AnimatedMesh::AnimatedMeshRenderer* getAnimatedMeshRenderer() { return &animated_mesh_renderer_; }
  SkinnedMesh::SkinnedMeshRenderer* getSkinnedMeshRenderer() { return &skinned_mesh_renderer_; }
  Actor::ActorRenderer* getActorRenderer() { return &actor_renderer_; }
  SpecialFX::SpecialFXManager* getSpecialFXManager() { return &special_fx_manager_; }

private:
  // Invoked by the topmost tool dialog's menu when the user picks something
  virtual void dialogMenuNotification(Evidyon::Tools::EvidyonToolDialog* dialog,
                                      WORD item_id,
                                      WORD item_index);

  virtual bool toolReadFrom(const char* file);
  virtual bool toolWriteTo(const char* file);
  virtual void toolReset();


private:
  bool acquireD3D();
  void releaseD3D();
  void flushWin32MessageQueue();


public:

  Evidyon::Client::Tools::ClientData* getClientData();
  Evidyon::Server::Tools::ServerData* getServerData();
  dc::dcTable<Image::Tools::Image>* getImageTable();
  dc::dcTable<Texture::Tools::Texture>* getTextureTable();
  dc::dcTable<Mesh::Tools::Mesh>* getMeshTable();
  dc::dcList<Scenery::Tools::Scenery>* getSceneryList();
  dc::dcTable<VisualFX::Tools::VisualFX>* getVisualFXTable();
  dc::dcTable<LifeformAI::Tools::LifeformAI>* getLifeformAITable();

  Avatar::Tools::AvatarLevelList*  getLevels();
  Avatar::Tools::AvatarRaceList*  getRaces();
  Avatar::Tools::AvatarClassList* getClasses();
  Evidyon::Treasure::Tools::GlobalTreasure* getGlobalTreasure();

  // list accessors (for methods that set references)
  const dc::dcGenericResource::Array& getImages();
  const dc::dcGenericResource::Array& getTextures();
  const dc::dcGenericResource::Array& getMeshes();
  const dc::dcGenericResource::Array& getScenery();
  const dc::dcGenericResource::Array& getVisualFX();
  const dc::dcGenericResource::Array& getZones();
  const dc::dcGenericResource::Array& getMaps();
  const dc::dcGenericResource::Array& getAnimatedMeshes();
  const dc::dcGenericResource::Array& getSkeletalAnimations();
  const dc::dcGenericResource::Array& getSkinnedMeshes();
  const dc::dcGenericResource::Array& getActorProfiles();
  const dc::dcGenericResource::Array& getSounds();
  const dc::dcGenericResource::Array& getLifeformAIs();
  const dc::dcGenericResource::Array& getItems();
  const dc::dcGenericResource::Array& getSpecialFX();
  const dc::dcGenericResource::Array& getSpells();

private:
  Evidyon::Client::Tools::ClientData client_data_;
  Evidyon::Server::Tools::ServerData server_data_;
  dc::dcTable<Image::Tools::Image> images_;
  dc::dcTable<Texture::Tools::Texture> textures_;
  dc::dcTable<Mesh::Tools::Mesh> meshes_;
  dc::dcTable<VisualFX::Tools::VisualFX> visual_fx_;
  dc::dcList<Scenery::Tools::Scenery> scenery_;
  dc::dcList<UnseenSkinnedMeshResource> animated_meshes_;
  dc::dcList<UnseenKeyframeAnimation> skeletal_animations_;
  dc::dcList<SkinnedMesh::Tools::SkinnedMesh> skinned_meshes_;
  Evidyon::Tools::ZoneList zones_;
  dc::dcList<World::Tools::Map> maps_;
  dc::dcList<Sound::Tools::WAVSound> sounds_;
  dc::dcList<Actor::Tools::ActorProfile> actor_profiles_;
  dc::dcTable<LifeformAI::Tools::LifeformAI> lifeform_ai_;
  Avatar::Tools::AvatarRaceList races_;
  Avatar::Tools::AvatarClassList classes_;
  Evidyon::Tools::ItemList items_;
  Evidyon::Avatar::Tools::AvatarLevelList levels_;
  Evidyon::Tools::SpecialFXTable special_fx_;
  Evidyon::Tools::SpellList spells_;
  Evidyon::Treasure::Tools::GlobalTreasure global_treasure_;

///////////////////////////////////////////////////////////////////////////////
public:
  size_t getPhysicalHitMagicFXResourceIndex();
  size_t getSpawnMagicFXResourceIndex();
///////////////////////////////////////////////////////////////////////////////


private:

  // Managers that visualize various parts of the data
  EvidyonD3DRenderStateManager renderstate_manager_;
  Mesh::MeshRenderer mesh_renderer_;
  Evidyon::EvidyonD3DLightManager light_manager_;
  VisualFX::VisualFXRenderer visual_fx_renderer_;
  Scenery::SceneryRenderer scenery_renderer_;
  Sound::SoundManager sound_manager_;
  AnimatedMesh::AnimatedMeshRenderer animated_mesh_renderer_;
  SkinnedMesh::SkinnedMeshRenderer skinned_mesh_renderer_;
  Actor::ActorRenderer actor_renderer_;
  Evidyon::SpecialFX::SpecialFXManager special_fx_manager_;

private:

  // This is all the data used to run Direct3D for the editor.  To use the
  // device, individual editor windows can be spawned for specific
  // purposes and they are put in a rendering chain.
  LPDIRECT3D9 d3d_;
  LPDIRECT3DDEVICE9 d3d_device_;


  struct ActiveD3DWindow {
    LPDIRECT3DSWAPCHAIN9 swap_chain;
    D3DWindow* window;
  };

  typedef std::vector<ActiveD3DWindow> ActiveD3DWindows;
  ActiveD3DWindows active_d3d_windows_;

  // Prevents renderD3DWindow from being called multiple times on a single
  // window.  This allows WM_PAINT to send render calls without having to
  // worry about whether or not the window is already rendering.
  ActiveD3DWindow* window_being_rendered_;

  // If flagged, disables any animated mesh from loading.  This is necessary
  // so that Erich can use this version of the editor.
  bool load_animated_meshes_;

private:
  CompleteEditor(CompleteEditor&);
  void operator =(CompleteEditor&);
};

}
}

#endif