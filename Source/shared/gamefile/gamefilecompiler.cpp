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
#include "gamefilecompiler.h"
#include "clientgamefiledata.h"
#include "servergamefiledata.h"
#include "complete-editor/completeeditor.h"
#include "common/safemacros.h"
#include "shared/image/tools/image.h"
#include "shared/sound/tools/wavsound.h"
#include "shared/mesh/meshutils.h"
#include "shared/animated-mesh/animatedmeshutils.h"
#include "shared/actor/tools/actortemplatecompiler.h"
#include "shared/world/tools/mapcompilationcontext.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/world/tools/map.h"
#include "shared/visualfx/tools/visualfx.h"
#include "shared/world/tools/maplocationreference.h"
#include "shared/zone/tools/zone.h"
#include "shared/specialfx/tools/specialfxcompiler.h"
#include "shared/event/tools/eventcompiler.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/treasure/tools/treasurecompiler.h"
#include "shared/geosid/tools/geosidcompiler.h"
#include "shared/lifeformai/tools/lifeformaicompiler.h"
#include <assert.h>

#include "shared/packets/eventpackets.h"


namespace Evidyon {
namespace GameFile {




  
  
//----[  EvidyonGameFileUtil_AssembleTextureData  ]----------------------------
bool EvidyonGameFileUtil_AssembleTextureData(LPDIRECT3DTEXTURE9 texture,
                                             bool optimize,
                                             void** png_data,
                                             size_t* png_size) {
  if (optimize && false) {
    // algorithm:
    //  - save texture to temporary PNG file
    //  - run pngout on it to compress
    //  - load data from file into png data buffer
  } else {

    // Create a PNG for the image
    LPD3DXBUFFER png_buffer;
    HRESULT hr;
    hr = D3DXSaveTextureToFileInMemory(
      &png_buffer,
      D3DXIFF_PNG,
      texture,
      NULL);

    if (FAILED(hr)) {
      return false;
    }

    // Copy the D3DX buffer into a local memory buffer
    size_t data_size = png_buffer->GetBufferSize();
    void* internal_png_data = new BYTE[data_size];
    assert(internal_png_data);
    if (!internal_png_data) {
      SAFE_RELEASE(png_buffer);
      return false;
    }

    memcpy(internal_png_data,
           png_buffer->GetBufferPointer(),
           data_size);

    *png_size = data_size;
    *png_data = internal_png_data;

    SAFE_RELEASE(png_buffer);
  }

  return true;
}






  
//----[  ProgressCallback  ]---------------------------------------------------
GameFileCompiler::ProgressCallback::~ProgressCallback() {
}

void GameFileCompiler::ProgressCallback::echo(Type /*type*/,
                                              const char* message) {
  OutputDebugString(message);
  OutputDebugString("\n");
}



  
//----[  GameFileCompiler  ]---------------------------------------------------
GameFileCompiler::GameFileCompiler() {
  zero();
}



//----[  ~GameFileCompiler  ]--------------------------------------------------
GameFileCompiler::~GameFileCompiler() {
  clear();
}




//----[  clear  ]--------------------------------------------------------------
void GameFileCompiler::clear() {
  client_writer_.destroy();
  server_writer_.destroy();
  if (client_data_) {
    if (client_data_->images_array) {
      for (size_t i = 0; i < client_data_->images.array_entries; ++i) {
        SAFE_DELETE_ARRAY(client_data_->images_array[i].data.png_data);
      }
      SAFE_DELETE_ARRAY(client_data_->images_array);
    }
    SAFE_DELETE_ARRAY(client_data_->textures_array);
    SAFE_DELETE_ARRAY(client_data_->visual_fx_array);
    if (client_data_->meshes_array) {
      for (size_t i = 0; i < client_data_->meshes.array_entries; ++i) {
        Evidyon::Mesh::Mesh& mesh = client_data_->meshes_array[i].data;
        for (unsigned int i = 0; i < mesh.number_of_subsets; ++i) {
          SAFE_DELETE_ARRAY(mesh.subsets[i].vertices);
          SAFE_DELETE_ARRAY(mesh.subsets[i].indices);
        }
        SAFE_DELETE_ARRAY(mesh.subsets);
      }
      SAFE_DELETE_ARRAY(client_data_->meshes_array);
    }
    if (client_data_->scenery_array) {
      for (size_t i = 0; i < client_data_->scenery.array_entries; ++i) {
        SAFE_DELETE_ARRAY(client_data_->scenery_array[i].data.subset_textures);
      }
      SAFE_DELETE_ARRAY(client_data_->scenery_array);
    }
    if (client_data_->animated_meshes_array) {
      for (size_t i = 0; i < client_data_->animated_meshes.array_entries; ++i) {
        AnimatedMeshUtil_DeallocateAnimatedMeshData(
          &client_data_->animated_meshes_array[i].data);
      }
      SAFE_DELETE_ARRAY(client_data_->animated_meshes_array);
    }
    if (client_data_->animated_mesh_animations_array) {
      for (size_t i = 0; i < client_data_->animated_mesh_animations.array_entries; ++i) {
        AnimatedMeshUtil_DeallocateAnimatedMeshAnimation(
          &client_data_->animated_mesh_animations_array[i].data);
      }
      SAFE_DELETE_ARRAY(client_data_->animated_mesh_animations_array);
    }
    if (client_data_->sounds_array) {
      for (size_t i = 0; i < client_data_->sounds.array_entries; ++i) {
        SAFE_DELETE_ARRAY(client_data_->sounds_array[i].data.waveform);
      }
      SAFE_DELETE_ARRAY(client_data_->sounds_array);
    }
    if (client_data_->zones_array) {
      SAFE_DELETE_ARRAY(client_data_->zones_array);
    }
    if (client_data_->actor_profiles_array) {
      SAFE_DELETE_ARRAY(client_data_->actor_profiles_array);
    }
    if (client_data_->maps_array) {
      for (size_t i = 0; i < client_data_->maps.array_entries; ++i) {
        SAFE_DELETE_ARRAY(client_data_->maps_array[i].data.location_table_data);
        SAFE_DELETE_ARRAY(client_data_->maps_array[i].data.unique_map_locations);
      }
      SAFE_DELETE_ARRAY(client_data_->maps_array);
    }
    SAFE_DELETE_ARRAY(client_data_->items_array);
    SAFE_DELETE_ARRAY(client_data_->events_array);
    {
      for (int i = 0; i < Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
        SAFE_DELETE_ARRAY(client_data_->enchantments[i].event_indices);
      }
    }
    SAFE_DELETE_ARRAY(client_data_->spells_array);
    SAFE_DELETE(client_data_);
  }
  if (server_data_) {
    if (server_data_->maps_array) {
      for (size_t i = 0; i < server_data_->maps.array_entries; ++i) {
        SAFE_DELETE_ARRAY(server_data_->maps_array[i].data.locations);
        SAFE_DELETE_ARRAY(server_data_->maps_array[i].data.regions);
        SAFE_DELETE_ARRAY(server_data_->maps_array[i].data.triggers);
      }
    }
    SAFE_DELETE_ARRAY(server_data_->items_array);
    SAFE_DELETE_ARRAY(server_data_->lifeform_ai_array);
    SAFE_DELETE_ARRAY(server_data_->magic_array);
    SAFE_DELETE_ARRAY(server_data_->spells_array);
    SAFE_DELETE(server_data_);
  }
}




//----[  compile  ]------------------------------------------------------------
bool GameFileCompiler::compile(Tools::CompleteEditor* source,
                               bool optimize,
                               ProgressCallback* progress) {

  client_data_ = new ClientGameFileData;
  server_data_ = new ServerGameFileData;
  assert(client_data_);
  assert(server_data_);
  if (!client_data_ || !server_data_) {
    return false;
  }
  memset(client_data_, 0, sizeof(ClientGameFileData));
  memset(server_data_, 0, sizeof(ServerGameFileData));

  if (!client_writer_.create(1000000) ||
      !server_writer_.create(1000000)) {
      return false;
  }

  source_ = source;
  ProgressCallback default_progress;
  progress_ = progress ? progress : &default_progress;

  bool return_value
    = prebuild(optimize) &&
      assemble(optimize) &&
      serialize();

  if (!return_value) error("build failed");
  progress_ = NULL;
  return return_value;
}






//----[  write  ]--------------------------------------------------------------
bool GameFileCompiler::write(const char* client_file,
                             const char* server_file) {
  return writeGameFile(client_file, &client_writer_) &&
         writeGameFile(server_file, &server_writer_);
}





//----[  getAllResourcesOfType  ]----------------------------------------------
void GameFileCompiler::getAllResourcesOfType(
    unsigned long type,
    dc::dcGenericResource::Array& resources) {
  assert(false);
}





//----[  prebuild  ]-----------------------------------------------------------
bool GameFileCompiler::prebuild(bool optimize) {
  if (optimize) {
    // for each resource type:
    //  get a list of all references
    //  mark all referenced entries
    //  remove entries without references
    // for all images:
    //  check to see if any images are identical (same file & settings)
    //  migrate references, delete duplicate objects
    // source_->updateImageFlags();
  }

  source_->getImageTable()->sort();
  source_->getTextureTable()->sort();
  source_->getSceneryList()->sort();

  if (Evidyon::Texture::TextureDescription::NUMBER_OF_BLEND_TYPES > 
      ClientGameFileTextureData::MAXIMUM_BLEND_TYPES) {
    error("Too many blend types to save into the "\
          "client's texture description");
    return false;
  }
  if (Evidyon::Texture::TextureDescription::NUMBER_OF_ANIMATION_TYPES >
      ClientGameFileTextureData::MAXIMUM_ANIMATION_TYPES) {
    error("Too many texture animation types to save "\
          "into the client's texture description");
    return false;
  }

  return true;
}





#define BEGIN_GAME_FILE_ARRAY_ALLOC(target, GameFileArrayType, array_name)\
  target##_data_->array_name.array_entries = number_of_members;\
  GameFileArrayType* target##_data_array = NULL;\
  if (number_of_members > 0) {\
    target##_data_array = new GameFileArrayType[number_of_members];\
    assert(target##_data_array);\
    if (!target##_data_array)\
      {error("Out of memory allocating %s", #array_name); return false;}\
    memset(target##_data_array, 0, sizeof(GameFileArrayType)*number_of_members);\
  }else{\
    warning("WARNING:  array of %s is empty", #array_name);\
  }\
  target##_data_->array_name##_array = target##_data_array;


// Automatically allocates the game file data array for the given array of
// resources.  Provides the values 'element' and 'data' inside of an
// array loop; the user code should fill out all members of 'data' using values
// from the source resource 'element'.
#define BEGIN_GAME_FILE_ARRAY(target, getMethod, ElementType,\
                              GameFileArrayType, array_name)\
{ const dc::dcGenericResource::Array& members = source_->getMethod();\
  size_t number_of_members = members.size();\
  BEGIN_GAME_FILE_ARRAY_ALLOC(target, GameFileArrayType, array_name)\
  size_t __index = 0;\
  for (dc::dcGenericResource::Array::const_iterator iter = members.begin();\
       iter != members.end(); ++iter, ++__index) {\
    ElementType* element = reinterpret_cast<ElementType*>(*iter);\
    info("Loading %s", element->getPathString().c_str());\
    GameFileArrayType* data\
      = target##_data_array + __index;

#define BEGIN_DUAL_GAME_FILE_ARRAY(getMethod, ElementType,\
                                   GameFileArraySubType, array_name)\
{ const dc::dcGenericResource::Array& members = source_->getMethod();\
  size_t number_of_members = members.size();\
  BEGIN_GAME_FILE_ARRAY_ALLOC(client, ClientGameFileData::GameFileArraySubType, array_name)\
  BEGIN_GAME_FILE_ARRAY_ALLOC(server, ServerGameFileData::GameFileArraySubType, array_name)\
  size_t __index = 0;\
  for (dc::dcGenericResource::Array::const_iterator iter = members.begin();\
       iter != members.end(); ++iter, ++__index) {assert(__index < number_of_members);\
    ElementType* element = reinterpret_cast<ElementType*>(*iter);\
    info("Loading %s", element->getPathString().c_str());\
    ClientGameFileData::GameFileArraySubType* client_data\
      = &client_data_array[__index];\
    ServerGameFileData::GameFileArraySubType* server_data\
      = &server_data_array[__index];

#define END_GAME_FILE_ARRAY() }}


#define return_error_loading(type) \
  {error("Failed to load "##type##" %s", element->getName().c_str());\
   return false;}

// This is a modified implementation of the CRT's bsearch() method.  It
// is specialized for this map location data, and instead of returning
// NULL it returns the location at which the key *should* exist.
// This is an optimized method, and we know that *found_match = false
// by default so this value isn't assigned.
ClientGameFileMapLocationData* bsearch_client_map_location_data(
    const ClientGameFileMapLocationData* key,
    const ClientGameFileMapLocationData* base,
    size_t num,
    bool* found_match) {
  assert(key);
  assert(base);
  assert(found_match);
  register ClientGameFileMapLocationData* lo =
    (ClientGameFileMapLocationData*)base;
  register ClientGameFileMapLocationData* hi =
    (ClientGameFileMapLocationData*)base + (num - 1);
  register ClientGameFileMapLocationData* mid;
  size_t half;
  int result;

  while (lo <= hi) {
    if ((half = num / 2) != 0) {
      mid = lo + (num & 1 ? half : (half - 1));
      if (!(result = memcmp(key, mid, sizeof(ClientGameFileMapLocationData)))) {
        *found_match = true;
        return mid;
      } else if (result < 0) {
        hi = mid - 1;
        num = num & 1 ? half : half-1;
      } else {
        lo = mid + 1;
        num = half;
      }
    } else /*if (num) */{ // num == 1, lo == hi
      assert(lo == hi);
      assert(num == 1);
      if (!(result = memcmp(key, lo, sizeof(ClientGameFileMapLocationData)))) {
        *found_match = true;
        return lo;
      } else if (result < 0) {
        return lo;     // insert before this element
      } else {
        return hi + 1; // append to the end of the array
      }
    } /*else {
      break; // couldn't find the element
    }*/
  }
  return lo;
}




//----[  UniqueMapLocationData  ]----------------------------------------------
struct UniqueMapLocationData {
  ClientGameFileMapLocationData* unique_location;
  int number_of_references;
};


//----[  CompareUniqueMapLocationData  ]---------------------------------------
int CompareUniqueMapLocationData(const void* p1, const void* p2) {
  return ((UniqueMapLocationData*)(p2))->number_of_references
       - ((UniqueMapLocationData*)(p1))->number_of_references;
}




//----[  assemble  ]-----------------------------------------------------------
bool GameFileCompiler::assemble(bool optimize) {

  Evidyon::Event::Tools::EventCompiler event_compiler;
  Evidyon::Magic::Tools::MagicCompiler magic_compiler(&event_compiler);
  Evidyon::Actor::Tools::ActorTemplateCompiler actor_template_compiler;
  Evidyon::Treasure::Tools::TreasureCompiler treasure_compiler;
  treasure_compiler.initialize(source_->getGlobalTreasure());
  Evidyon::LifeformAI::Tools::LifeformAICompiler lifeform_ai_compiler(
    &treasure_compiler,
    &event_compiler,
    &actor_template_compiler);
  Evidyon::Geosid::Tools::GeosidCompiler geosid_compiler(
    &magic_compiler,
    &lifeform_ai_compiler);

  //----[  client data  ]------------------------------------------------------
  {
    Evidyon::Client::Tools::ClientData* client_data = source_->getClientData();
    for (int i = 0; i < Evidyon::Client::NUMBER_OF_CLIENT_IMAGES; ++i) {
      client_data->compileImage(i, &client_data_->client_data.images[i]);
    }
    for (int i = 0; i < Evidyon::Client::NUMBER_OF_CLIENT_SOUNDS; ++i) {
      client_data->compileSound(i, &client_data_->client_data.sounds[i]);
    }
    for (int i = 0; i < Evidyon::Client::NUMBER_OF_CLIENT_SPECIALFX; ++i) {
      client_data->compileSpecialFX(i, &client_data_->client_data.special_fx[i]);
    }
    client_data->compileFootstepSounds(
      &client_data_->client_data.land_footstep_sound,
      &client_data_->client_data.water_footstep_sound);
  }


  //----[  server data  ]------------------------------------------------------
  {
    Evidyon::Server::Tools::ServerData* server_data = source_->getServerData();
    server_data->getGoodSpawnPoint()->compile(
      &server_data_->server_data.good_spawn_point.map,
      &server_data_->server_data.good_spawn_point.x,
      &server_data_->server_data.good_spawn_point.y);
    server_data->getEvilSpawnPoint()->compile(
      &server_data_->server_data.evil_spawn_point.map,
      &server_data_->server_data.evil_spawn_point.x,
      &server_data_->server_data.evil_spawn_point.y);
    server_data_->server_data.gold_scenery = server_data->getGoldScenery();
  }

  //----[  images  ]-----------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getImages,
                        Evidyon::Tools::ImageElement,
                        ClientGameFileData::Image,
                        images)
    Evidyon::Image::Tools::Image* image = element->getImplementation();
    if (!image->loadD3DTexture(source_->getD3DDevice(), true)) {
      return_error_loading("image");
    }

    if (!EvidyonGameFileUtil_AssembleTextureData(image->getD3DTexture(),
                                                 optimize,
                                                 &data->data.png_data,
                                                 &data->data.png_size)) {
      error("unable to assemble PNG texture data for %s",
            element->getName().c_str());
      return_error_loading("image");
    }


  END_GAME_FILE_ARRAY()




    
  //----[  textures  ]---------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getTextures,
                        Evidyon::Tools::TextureElement,
                        ClientGameFileData::Texture,
                        textures)

    Evidyon::Texture::Tools::Texture* texture = element->getImplementation();
    assert(texture);

    Evidyon::Texture::TextureDescription description;
    texture->compile(&description);
    texture->compileBase(&description);
    if (!description.d3d_texture) {
      warning("%s:  no texture selected", element->getName().c_str());
    } else {
      SAFE_RELEASE(description.d3d_texture);
    }

    data->data.image_index = texture->getSourceImageIndex();
    data->data.blend_type = description.blend_type;
    data->data.flip_horizontal = description.flip_horizontal;
    data->data.flip_vertical = description.flip_vertical;
    data->data.animation_type = description.animation_type;
    memcpy(&data->data.animation,
           &description.animation,
           sizeof(Evidyon::Texture::TextureDescription::Animation));

  END_GAME_FILE_ARRAY()


    
  //----[  visual fx  ]----------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getVisualFX,
                        Evidyon::Tools::VisualFXElement,
                        ClientGameFileData::VisualFX,
                        visual_fx)
    Evidyon::VisualFX::Tools::VisualFX* visual_fx = element->getImplementation();
    visual_fx->compile(&data->data);
  END_GAME_FILE_ARRAY()



  //----[  meshes  ]-----------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getMeshes,
                        Evidyon::Tools::MeshElement,
                        ClientGameFileData::Mesh,
                        meshes)

    Evidyon::Mesh::Tools::Mesh* mesh = element->getImplementation();
    assert(mesh);
    if (!mesh->compile(source_->getD3DDevice(), &data->data)) {
      error("%s:  couldn't load mesh", element->getName().c_str());
      return false;
    }

    D3DXMATRIX matrix;
    mesh->getTransform(&matrix);
    if (!D3DXMatrixIsIdentity(&matrix)) {
      Evidyon::Mesh::EvidyonMeshUtil_ApplyMeshTransform(&matrix, &data->data);
    }

  END_GAME_FILE_ARRAY()


    
  //----[  scenery  ]----------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getScenery,
                        Evidyon::Tools::SceneryElement,
                        ClientGameFileData::Scenery,
                        scenery)
    size_t number_of_subsets = 0;
    element->compile(NULL,
                     &data->data,
                     &number_of_subsets);
    if (data->data.mesh == Evidyon::Mesh::INVALID_MESH_INDEX) {
      error("%s:  no referenced mesh", element->getName().c_str());
      return false;
    } else {
      Evidyon::Mesh::MeshIndex mesh_index = data->data.mesh;
      bool mesh_index_is_valid
        = mesh_index < client_data_->meshes.array_entries;
      assert(mesh_index_is_valid);
      bool has_correct_number_of_subsets
        = mesh_index_is_valid &&
          (client_data_
            ->meshes_array[mesh_index].data.number_of_subsets
           == number_of_subsets);
      if (!has_correct_number_of_subsets) {
        error("%s: incorrect number of mesh subsets",
              element->getName().c_str());
        return false;
      }
    }
    if (number_of_subsets == 0) {
      warning("%s:  no subset textures", element->getName().c_str());
    }
    for (size_t i = 0; i < number_of_subsets; ++i) {
      if (data->data.subset_textures[i] ==
          Evidyon::Texture::INVALID_TEXTURE_INDEX) {
            warning("%s:  subset %i has no texture",
                    element->getName().c_str(),
                    i);
      }
    }
  END_GAME_FILE_ARRAY()


    
  //----[  animated meshes  ]--------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getAnimatedMeshes,
                        Evidyon::Tools::AnimatedMeshElement,
                        ClientGameFileData::AnimatedMesh,
                        animated_meshes)
    if (!element->loadAnimatedMeshData(true, // force reload
                                       &data->data)) {
      return_error_loading("animated mesh");
    }
  END_GAME_FILE_ARRAY()



  //----[  animated mesh animations  ]-----------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getSkeletalAnimations,
                        Evidyon::Tools::SkeletalAnimationElement,
                        ClientGameFileData::AnimatedMeshAnimation,
                        animated_mesh_animations)
    if (!element->loadAnimatedMeshAnimation(true, // force reload
                                            &data->data)) {
      return_error_loading("animated mesh animation");
    }
  END_GAME_FILE_ARRAY()



  //----[  skinned meshes  ]---------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getSkinnedMeshes,
                        Evidyon::Tools::SkinnedMeshElement,
                        ClientGameFileData::SkinnedMesh,
                        skinned_meshes)
    if (!element->compile(&data->data)) {
      return_error_loading("skinned mesh");
    }

    // validate the skinned mesh's information
    for (size_t i = 0; i < data->data.number_of_forms; ++i) {
      Evidyon::AnimatedMesh::AnimatedMeshIndex animated_mesh 
        = data->data.forms[i].animated_mesh;
      if (animated_mesh ==
            Evidyon::AnimatedMesh::INVALID_ANIMATED_MESH_INDEX) {
        error("Skinned mesh %s form %s references an invalid animated mesh",
              element->getName().c_str(),
              element->forms().at(i)->getName().c_str());
        return false;
      }
      assert(animated_mesh < client_data_->animated_meshes.array_entries);
      size_t number_of_texture_group_textures
        = client_data_
            ->animated_meshes_array[animated_mesh]
                .data
                  .number_of_texture_groups;
      for (size_t t = 0; t < number_of_texture_group_textures; ++t) {
        if (data->data.forms[i].texture_group_textures[t] !=
            Evidyon::Texture::INVALID_TEXTURE_INDEX) {
          continue;
        }
        warning("Skinned mesh %s form %s: texture group %lu has no texture",
                element->getName().c_str(),
                element->forms().at(i)->getName().c_str(),
                t);
      }
    }
    for (size_t i = 0; i < data->data.number_of_animations; ++i) {
      if (data->data.animations[i].animation ==
        Evidyon::AnimatedMesh::INVALID_ANIMATED_MESH_ANIMATION_INDEX) {
        error("Skinned mesh %s animation %s references an invalid animation",
              element->getName().c_str(),
              element->animations().at(i)->getName().c_str());
        return false;
      }
    }
  END_GAME_FILE_ARRAY()



  //----[  sounds  ]-----------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getSounds,
                        Evidyon::Tools::SoundElement,
                        ClientGameFileData::Sound,
                        sounds)
    if (!element->compile(&data->data)) {
      return_error_loading("sound");
    }
  END_GAME_FILE_ARRAY()



    
  //----[  special fx  ]-------------------------------------------------------
  {
    const dc::dcGenericResource::Array& special_fx = source_->getSpecialFX();
    SpecialFX::Tools::SpecialFXCompiler compiler(special_fx.size());
    size_t index = 0;
    dc::dcGenericResource::Array::const_iterator i;
    for (i = special_fx.begin(); i != special_fx.end(); ++i, ++index) {
      Tools::SpecialFXElement* element = (Tools::SpecialFXElement*)(*i);
      element->getImplementation()->compile(index, &compiler);
    }

    { // special fx
      size_t count = compiler.numberOfSpecialFX();
      client_data_->special_fx.array_entries = count;
      client_data_->special_fx_array
        = new ClientGameFileData::SpecialFX[count];
      for (size_t i = 0; i < count; ++i) {
        compiler.compileSpecialFX(
          i,
          &client_data_->special_fx_array[i].data);
      }
      info("compiled %lu special fx", count);
    }

    { // special fx bindings
      size_t count = compiler.numberOfBindings();
      client_data_->special_fx_bindings.array_entries = count;
      client_data_->special_fx_bindings_array
        = new ClientGameFileData::SpecialFXBinding[count];
      for (size_t i = 0; i < count; ++i) {
        compiler.compileBinding(
          i,
          &client_data_->special_fx_bindings_array[i].data);
      }
      info("compiled %lu special fx bindings", count);
    }

    { // special fx emitters
      size_t count = compiler.numberOfEmitters();
      client_data_->special_fx_emitters.array_entries = count;
      client_data_->special_fx_emitters_array
        = new ClientGameFileData::SpecialFXEmitter[count];
      for (size_t i = 0; i < count; ++i) {
        compiler.compileEmitter(
          i,
          &client_data_->special_fx_emitters_array[i].data);
      }
      info("compiled %lu special fx emitters", count);
    }
  }



  //----[  zones  ]------------------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getZones,
                        Evidyon::Tools::ZoneElement,
                        ClientGameFileData::Zone,
                        zones)
    element->compile(&data->data);
  END_GAME_FILE_ARRAY()


  //----[  actor profiles  ]---------------------------------------------------
  BEGIN_GAME_FILE_ARRAY(client,
                        getActorProfiles,
                        Evidyon::Tools::ActorProfileElement,
                        ClientGameFileData::ActorProfile,
                        actor_profiles)
    element->compile(&data->data);

    // warn about invalid references
    for (size_t i = 0; i < Evidyon::Actor::NUMBER_OF_CONSTANT_ACTOR_ANIMATIONS; ++i) {
      using namespace Evidyon::SkinnedMesh;
      using namespace Evidyon::Actor;
      if (data->data.constant_animations[i] == INVALID_SKINNED_MESH_ANIMATION_INDEX) {
        warning("constant animation %s of actor profile %s has not been set",
                ConstantActorAnimationString(ConstantActorAnimation(i)),
                element->getName().c_str());
      }
    }
    for (size_t i = 0; i < Evidyon::Actor::NUMBER_OF_ACTOR_SOUNDS; ++i) {
      using namespace Evidyon::Sound;
      using namespace Evidyon::Actor;
      if (data->data.sounds[i] == INVALID_SOUND_INDEX) {
        warning("sound %s of actor profile %s has not been set",
                ActorSoundString(ActorSound(i)),
                element->getName().c_str());
      }
    }
    for (size_t i = 0;
         i < Evidyon::Actor::NUMBER_OF_ACTOR_PROFILE_SPECIALIZATION_TYPES; ++i) {
      using namespace Evidyon::SkinnedMesh;
      using namespace Evidyon::Actor;
      for (size_t j = 0;
           j < Evidyon::Actor::NUMBER_OF_SPECIALIZED_ACTOR_ANIMATIONS; ++j) {
        if (data->data.specializations[i].animations[j]
            == INVALID_SKINNED_MESH_ANIMATION_INDEX) {
          warning("specialized animation %s of specialization %s "\
                  "of actor profile %s has not been set",
                  SpecializedActorAnimationString(
                    SpecializedActorAnimation(j)),
                  ActorProfileSpecializationTypeString(
                    ActorProfileSpecializationType(i)),
                  element->getName().c_str());
        }
      }
    }
  END_GAME_FILE_ARRAY()


 
  //----[  lifeform ai  ]------------------------------------------------------
  {
    const dc::dcGenericResource::Array& lifeforms = source_->getLifeformAIs();
    for (dc::dcGenericResource::Array::const_iterator i = lifeforms.begin(); i != lifeforms.end(); ++i) {
      Tools::LifeformAIElement* element
        = (Tools::LifeformAIElement*)(*i);
      Evidyon::LifeformAI::Tools::LifeformAI* lifeform_ai
        = element->getImplementation();
      LifeformAI::LifeformAI description;
      if (!lifeform_ai->compile(&treasure_compiler,
                                &event_compiler,
                                &actor_template_compiler,
                                &description)) {
        return_error_loading("lifeform ai");
      }
      // ensure that the lifeforms are loaded into their correct indices
      LifeformAI::LifeformAIIndex index
        = lifeform_ai_compiler.add(description);
      if (index != element->getIndex()) {
        return_error_loading("lifeform ai");
      }
    }
  }


  //----[  races  ]------------------------------------------------------------
  if (!source_->getRaces()->compile(&actor_template_compiler,
                                    client_data_->races,
                                    server_data_->races)) {
    return false;
  }
    
  //----[  classes  ]----------------------------------------------------------
  source_->getClasses()->compile(client_data_->classes,
                                 server_data_->classes,
                                 source_->getSpells());



  //----[  levels  ]-----------------------------------------------------------
  source_->getLevels()->compile(server_data_->levels);


  //----[  maps  ]-------------------------------------------------------------
  // The spawn point indices are unique across all maps.  This allows every
  // single spawn point to be able to identify the lifeform that it has
  // generated by looking up the index in the server's lifeform AI manager's
  // map.
  LifeformAI::SpawnPointIndex next_spawn_point_index = 0;
  // Iterate through all maps
  BEGIN_DUAL_GAME_FILE_ARRAY(getMaps,
                             Evidyon::Tools::MapElement,
                             Map,
                             maps)
    Evidyon::World::Tools::MapCompilationContext compilation_context;
    if (!element->createCompilationContext(source_->getD3DDevice(),
                                           &compilation_context)) {
      return_error_loading("map");
    }
    element->buildContextContents(NULL, &compilation_context);
    element->compileGeosids(&geosid_compiler, &compilation_context);


    unsigned int number_of_locations
      = compilation_context.width * compilation_context.height;
    const unsigned int MAXIMUM_MAP_DIMENSION = 1<<(16-5+1);
    info("%s map is %lu x %lu", 
          element->getName().c_str(),
          compilation_context.width,
          compilation_context.height);
    if (compilation_context.width > (1<<(16-5+1)) ||
        compilation_context.height > (1<<(16-5+1))) {
      error("%s map is too large! maximum size is %lu x %lu",
            element->getName().c_str(),
            MAXIMUM_MAP_DIMENSION, MAXIMUM_MAP_DIMENSION);
      return false;
    }

    //----[  client map  ]-----------------------------------------------------
    ClientGameFileMapLocationData* client_map_locations
      = new ClientGameFileMapLocationData[number_of_locations];
    for (size_t i = 0; i < number_of_locations; ++i) {
      ClientGameFileMapLocationData* client_map_location
        = client_map_locations + i;
      Evidyon::World::Tools::MapLocationData* map_location
        = compilation_context.locations + i;

      memcpy(&client_map_location->visual_data,
             &map_location->this_layer.visual_data,
             sizeof(Evidyon::World::LocationVisualData));
      client_map_location->navigability
        = map_location->this_layer.navigability;
      client_map_location->zone = map_location->this_layer.zone;
    }
    { // find the chaos level for the locations
      const dc::dcGenericResource::Array& zones = source_->getZones();
      dc::dcGenericResource::Array::const_iterator i = zones.begin();
      static const int BOX_EXTENT = 6;
      static const int BOX_SIZE = 1 + BOX_EXTENT*2;
      static const int CHAOS_ELEMENT = 0xFFFFFFFF / (BOX_SIZE * BOX_SIZE);
      size_t* chaos_zone = new size_t[zones.size()];
      size_t index = 0;
      while (i != zones.end()) {
        Tools::ZoneElement* zone_element = (Tools::ZoneElement*)(*i);
        chaos_zone[index] = zone_element->isChaosZone()
          ? CHAOS_ELEMENT
          : 0;
        ++i;
        ++index;
      }
      ClientGameFileMapLocationData* client_map_location
        = client_map_locations;
      for (int z = 0; z < compilation_context.height; ++z) {
        for (int x = 0; x < compilation_context.width; ++x) {
          size_t chaos_level = 0;
          for (int dz = z - BOX_EXTENT; dz <= z + BOX_EXTENT; ++dz) {
            for (int dx = x - BOX_EXTENT; dx <= x + BOX_EXTENT; ++dx) {
              Zone::ZoneIndex zone_index
                = compilation_context.atOrDefault(dx, dz)->this_layer.zone;
              chaos_level += zone_index == Zone::INVALID_ZONE_INDEX
                ? CHAOS_ELEMENT
                : chaos_zone[zone_index];
            }
          }
          (client_map_location++)->chaos_level
            = float(double(chaos_level) / double(0xFFFFFFFF));
        }
      }
      SAFE_DELETE_ARRAY(chaos_zone);
    }


    size_t number_of_unique_map_locations = 0;
    size_t translation_table_capacity = 8;
    unsigned int* index_to_unique_location
      = (unsigned int*)malloc(translation_table_capacity*sizeof(unsigned int));
    int* index_to_number_of_references
      = (int*)malloc(translation_table_capacity*sizeof(int));
    unsigned int* unique_location_to_index
      = (unsigned int*)malloc(translation_table_capacity*sizeof(unsigned int));
    unsigned int* map_location_table_index
      = new unsigned int[number_of_locations];
    for (size_t i = 0; i < number_of_locations; ++i) {
      ClientGameFileMapLocationData* client_map_location
        = client_map_locations + i;
      bool found_match = false;
      ClientGameFileMapLocationData* unique_location
        = bsearch_client_map_location_data(client_map_location,
                                           client_map_locations,
                                           number_of_unique_map_locations,
                                           &found_match);
      size_t unique_location_index = unique_location - client_map_locations;
      if (!found_match) { // insert this new location
        size_t number_of_unique_locations_after
          = number_of_unique_map_locations - unique_location_index;
        ClientGameFileMapLocationData temp;
        ClientGameFileMapLocationData* source_location = client_map_location;
        if (number_of_unique_locations_after > 0) {
          if (i == number_of_unique_map_locations) {
            // if this element is directly after the list, it will be
            // overwritten by the memmove.  this prevents that from
            // happening.
            memcpy(&temp, client_map_location, sizeof(temp));
            source_location = &temp;
          }
          memmove(unique_location + 1,
                  unique_location,
                  number_of_unique_locations_after
                    * sizeof(ClientGameFileMapLocationData));
          memmove(unique_location_to_index + unique_location_index + 1,
                  unique_location_to_index + unique_location_index,
                  number_of_unique_locations_after
                    * sizeof(unsigned int));
        }
        memcpy(unique_location,
               source_location,
               sizeof(ClientGameFileMapLocationData));
        unique_location_to_index[unique_location_index]
          = number_of_unique_map_locations;
        index_to_number_of_references[number_of_unique_map_locations] = 0;
        for (size_t j = 0; j < number_of_unique_map_locations; ++j) {
          if (index_to_unique_location[j] >= unique_location_index) {
            ++index_to_unique_location[j];
          }
        }
        index_to_unique_location[number_of_unique_map_locations]
           = unique_location_index;
        if (++number_of_unique_map_locations == translation_table_capacity) {
          translation_table_capacity <<= 1;
          index_to_unique_location 
            = (unsigned int*)realloc(
                index_to_unique_location,
                translation_table_capacity*sizeof(unsigned int));
          index_to_number_of_references 
            = (int*)realloc(
                index_to_number_of_references,
                translation_table_capacity*sizeof(int));
          unique_location_to_index
            = (unsigned int*)realloc(
                unique_location_to_index,
                translation_table_capacity*sizeof(unsigned int));
        }
      }

      unsigned int location_index
        = unique_location_to_index[unique_location_index];
      map_location_table_index[i] = location_index;
      ++index_to_number_of_references[location_index];
    }

    // For each location, index_to_unique_location[map_location_table_index[i]]
    // gives the index of the unique description of that location.  The
    // client_map_locations array contains 'number_of_unique_map_locations' of
    // them.  The last thing to do now is to re-sort the unique locations
    // by how frequently they are referenced.  This makes the data more
    // localized in memory and should imrove performance by increasing the
    // number of cache hits.
    {
      UniqueMapLocationData* sorted_unique_locations
        = new UniqueMapLocationData[number_of_unique_map_locations];
      for (size_t i = 0; i < number_of_unique_map_locations; ++i) {
        sorted_unique_locations[i].unique_location
          = &client_map_locations[i];
        sorted_unique_locations[i].number_of_references
          = index_to_number_of_references[i];
      }
      qsort(sorted_unique_locations,
            number_of_unique_map_locations,
            sizeof(UniqueMapLocationData),
            CompareUniqueMapLocationData);
      ClientGameFileMapLocationData* unique_map_locations
        = new ClientGameFileMapLocationData[number_of_unique_map_locations];
      for (size_t i = 0; i < number_of_unique_map_locations; ++i) {
        // remap the unique location
        index_to_unique_location[
          unique_location_to_index[
            sorted_unique_locations[i].unique_location - client_map_locations]]
          = i;
        // save the unique location into the final sorted buffer
        memcpy(unique_map_locations + i,
               sorted_unique_locations[i].unique_location,
               sizeof(ClientGameFileMapLocationData));
      }
      SAFE_DELETE_ARRAY(sorted_unique_locations);
      client_data->data.unique_map_locations = unique_map_locations;
      client_data->data.number_of_unique_map_locations
        = number_of_unique_map_locations;
    }
    for (size_t i = 0; i < number_of_locations; ++i) {
      // translate the intermediary index currently stored as the table index
      // to the final index into the sorted array of unique locations
      map_location_table_index[i]
        = index_to_unique_location[map_location_table_index[i]];
    }
    free(index_to_unique_location);
    index_to_unique_location = NULL;
    free(index_to_number_of_references);
    index_to_number_of_references = NULL;
    free(unique_location_to_index);
    unique_location_to_index = NULL;

    // make sure everything worked.  we have to skip this check for the first
    // chunk of size equal to the number of unique locations, because they have
    // been overwritten during the build process
    for (size_t i = number_of_unique_map_locations;
                i < number_of_locations; ++i) {
      assert(memcmp(&client_data->data.unique_map_locations[map_location_table_index[i]],
                    &client_map_locations[i],
                    sizeof(ClientGameFileMapLocationData)) == 0);
    }
    SAFE_DELETE_ARRAY(client_map_locations);


    // which index type will we choose?
    size_t index_bytes
      = (number_of_unique_map_locations <= 0xFF)
          ? 1
          : (number_of_unique_map_locations <= 0xFFFF)
            ? 2
            : 4;

    size_t uncompressed_size
      = number_of_locations * sizeof(ClientGameFileMapLocationData);
    size_t tabulated_size
      = number_of_unique_map_locations * sizeof(ClientGameFileMapLocationData)
        + number_of_locations * index_bytes;
    info("map %s has %lu unique locations (%.02f%%) and fits in %lu bytes "\
         "(%lu/coordinate);  "\
         "compression ratio = %.02f%%",
         element->getName().c_str(),
         number_of_unique_map_locations,
         float(double(number_of_unique_map_locations) * 100.0
              /double(number_of_locations)),
         tabulated_size,
         float(double(tabulated_size) * 100.0 / double(uncompressed_size)),
         index_bytes);

    client_data->data.width = compilation_context.width;
    client_data->data.height = compilation_context.height;
    size_t number_of_map_locations
      = compilation_context.width * compilation_context.height;

    if (index_bytes == 4) {
      unsigned __int32* table = new unsigned __int32[number_of_map_locations];
      size_t table_byte_length = sizeof(unsigned __int32) * number_of_map_locations;
      client_data->data.location_table_data = (void*)table;
      client_data->data.location_table_length = table_byte_length;
      memcpy(table, map_location_table_index, table_byte_length);
    } else if (index_bytes == 2) {
      unsigned __int16* table = new unsigned __int16[number_of_map_locations];
      size_t table_byte_length = sizeof(unsigned __int16) * number_of_map_locations;
      client_data->data.location_table_data = (void*)table;
      client_data->data.location_table_length = table_byte_length;
      for (size_t index = 0; index < number_of_map_locations; ++index) {
        table[index] = map_location_table_index[index];
      }
    } else {
      assert(index_bytes == 1);
      unsigned __int8* table = new unsigned __int8[number_of_map_locations];
      size_t table_byte_length = sizeof(unsigned __int8) * number_of_map_locations;
      client_data->data.location_table_data = (void*)table;
      client_data->data.location_table_length = table_byte_length;
      for (size_t index = 0; index < number_of_map_locations; ++index) {
        table[index] = map_location_table_index[index];
      }
    }
    
    SAFE_DELETE_ARRAY(map_location_table_index);



    //----[  server map  ]-----------------------------------------------------
    server_data->data.map_hash =
      dc::dcResourceManager::uniqueIDOfTypeName(element->getName());

    ServerGameFileMapLocationData* server_map_locations
      = new ServerGameFileMapLocationData[number_of_locations];
    for (size_t i = 0; i < number_of_locations; ++i) {
      ServerGameFileMapLocationData* server_map_location
        = server_map_locations + i;
      Evidyon::World::Tools::MapLocationData* map_location
        = compilation_context.locations + i;

      Zone::ZoneIndex zone = map_location->this_layer.zone;
      Zone::ZoneRules rules = Zone::ZONERULES_NORMAL;
      if (zone != Zone::INVALID_ZONE_INDEX) {
        server_map_location->zone_rules
          = ((Tools::ZoneElement*)source_->getZones().at(zone))->getZoneRules();
      }

      server_map_location->navigability
        = map_location->this_layer.navigability;
      server_map_location->trigger
        = map_location->this_layer.trigger;
      server_map_location->occluding
        = map_location->this_layer.visual_data.occluding;
      server_map_location->nearby_geosid
        = Geosid::INVALID_GEOSID_INDEX;

      server_map_location->geonite_burst_magic
        = Evidyon::Magic::INVALID_MAGIC_INDEX;
      server_map_location->geonite_burst_cost = rand()%50;

      // reset the path directions--they are calculated next
      server_map_location->walking_path[0] = World::PATHDIRECTION_NONE;
      server_map_location->walking_path[1] = World::PATHDIRECTION_NONE;
    }
    {
      const dc::dcGenericResource::Array& geosids = element->getGeosids();
      for (dc::dcGenericResource::Array::const_iterator
            i = geosids.begin(); i != geosids.end(); ++i) {
        Geosid::Tools::Geosid* geosid = (Geosid::Tools::Geosid*)(*i);
        float gx, gz;
        geosid->getLocation(&gx, &gz);
        int x = int(gx), z = int(gz);
        static const int BOX_EXTENT = 8;
        for (int dz = z - BOX_EXTENT; dz <= z + BOX_EXTENT; ++dz) {
          for (int dx = x - BOX_EXTENT; dx <= x + BOX_EXTENT; ++dx) {
            server_map_locations[dz * compilation_context.width + dx].nearby_geosid
              = geosid->getIndex();
          }
        }
      }
    }
    { // add in all of the geonite burst locations
      size_t number = element->numberOfGeoniteBurstLocations();
      Magic::MagicIndex magic_index;
      Geosid::Geonite geonite;
      for (size_t i = 0; i < number; ++i) {
        int x, y;
        element->compileGeoniteBurstLocation(
          i,
          &magic_compiler,
          &x,
          &y,
          &magic_index,
          &geonite);
        if (x >= 0 && x < compilation_context.width &&
            y >= 0 && y < compilation_context.height) {
          ServerGameFileMapLocationData* location
            = &server_map_locations[y * compilation_context.width + x];
          location->geonite_burst_magic = magic_index;
          location->geonite_burst_cost = geonite;
        }
      }
    }
    { // calculate the path directions for the map
      // first, set the following patterns for solid (x) tiles:
      // clockwise:     counter-clockwise:
      //   E E S          S W W
      //   N x S          S x N
      //   N W W          E E N
      // We only iterate through the centeral area, so all points touched
      // are guaranteed to have surrounding tiles.
      for (unsigned int z = compilation_context.height - 2; z >= 1; --z) {
        unsigned int z_b = z * compilation_context.width;   // this row
        unsigned int z_n = z_b - compilation_context.width; // north
        unsigned int z_s = z_b + compilation_context.width; // south
        for (unsigned int x = compilation_context.width - 2; x >= 1; --x) {
          unsigned int x_w = x - 1; // west
          unsigned int x_e = x + 1; // east

          // Fill surrounding tiles for the solid-to-walking path
          if (World::NavigabilitySolidToWalking(
               server_map_locations[z_b + x].navigability)) {
            World::FillPathDirectionsCW(
              &server_map_locations[z_n + x_w].walking_path[0],
              &server_map_locations[z_n + x  ].walking_path[0],
              &server_map_locations[z_n + x_e].walking_path[0],
              &server_map_locations[z_b + x_w].walking_path[0],
              &server_map_locations[z_b + x_e].walking_path[0],
              &server_map_locations[z_s + x_w].walking_path[0],
              &server_map_locations[z_s + x  ].walking_path[0],
              &server_map_locations[z_s + x_e].walking_path[0]);
            World::FillPathDirectionsCCW(
              &server_map_locations[z_n + x_w].walking_path[1],
              &server_map_locations[z_n + x  ].walking_path[1],
              &server_map_locations[z_n + x_e].walking_path[1],
              &server_map_locations[z_b + x_w].walking_path[1],
              &server_map_locations[z_b + x_e].walking_path[1],
              &server_map_locations[z_s + x_w].walking_path[1],
              &server_map_locations[z_s + x  ].walking_path[1],
              &server_map_locations[z_s + x_e].walking_path[1]);
          }
        }
      }

      // "Groom" the paths so that no element faces into a solid tile.  We
      // have to go through every element here, so it's necessary to check
      // to make sure we don't go off of the boundaries of the map.
      for (unsigned int z = 0; z < compilation_context.height; ++z) {
        unsigned int z_b = z * compilation_context.width;   // this row
        for (unsigned int x = 0; x < compilation_context.width; ++x) {
          { // Clockwise, Walking
            bool invalid = true;
            World::PathDirection original_direction
              = server_map_locations[z_b + x].walking_path[0];
            World::PathDirection adjusted_direction
              = World::NavigabilitySolidToWalking(
                  server_map_locations[z_b + x].navigability)
                  ? World::PATHDIRECTION_NONE
                  : original_direction;
            while (adjusted_direction != World::PATHDIRECTION_NONE) {
              int tile_x = int(x), tile_z = int(z);
              World::OffsetByPathDirection(
                adjusted_direction,
                &tile_x,
                &tile_z);
              invalid = tile_x < 0 ||
                        tile_z < 0 ||
                        tile_x >= compilation_context.width ||
                        tile_z >= compilation_context.height ||
                        World::NavigabilitySolidToWalking(
                          server_map_locations[
                              tile_z * compilation_context.width
                                + tile_x].navigability);
              if (invalid) {
                // Rotate in the opposite direction so that we avoid this convex
                // barrier and maintain our rotation about the object.
                adjusted_direction
                  = World::RotatePathDirectionCCW(adjusted_direction);
                if (adjusted_direction == original_direction) {
                  // there is no direction to go from here!
                  adjusted_direction = World::PATHDIRECTION_NONE;
                }
              } else {
                break; // we can use this direction!
              }
            }
            // Store the corrected direction
            server_map_locations[z_b + x].walking_path[0]
              = adjusted_direction;
          }
          { // Counter-Clockwise, Walking
            bool invalid = true;
            World::PathDirection original_direction
              = server_map_locations[z_b + x].walking_path[1];
            World::PathDirection adjusted_direction
              = World::NavigabilitySolidToWalking(
                  server_map_locations[z_b + x].navigability)
                  ? World::PATHDIRECTION_NONE
                  : original_direction;
            while (adjusted_direction != World::PATHDIRECTION_NONE) {
              int tile_x = int(x), tile_z = int(z);
              World::OffsetByPathDirection(
                adjusted_direction,
                &tile_x,
                &tile_z);
              invalid = tile_x < 0 ||
                        tile_z < 0 ||
                        tile_x >= compilation_context.width ||
                        tile_z >= compilation_context.height ||
                        World::NavigabilitySolidToWalking(
                          server_map_locations[
                              tile_z * compilation_context.width
                                + tile_x].navigability);
              if (invalid) {
                // Rotate in the opposite direction so that we avoid this convex
                // barrier and maintain our rotation about the object.
                adjusted_direction
                  = World::RotatePathDirectionCW(adjusted_direction);
                if (adjusted_direction == original_direction) {
                  // there is no direction to go from here!
                  adjusted_direction = World::PATHDIRECTION_NONE;
                }
              } else {
                break; // we can use this direction!
              }
            }
            // Store the corrected direction
            server_map_locations[z_b + x].walking_path[1]
              = adjusted_direction;
          }
        }
      }
    }
//////////////////////////////////////////////////////////////////////////////////
    { // find the chaos level for the locations
      const dc::dcGenericResource::Array& zones = source_->getZones();
      dc::dcGenericResource::Array::const_iterator i = zones.begin();
      static const int BOX_EXTENT = 10;
      static const int BOX_SIZE = 1 + BOX_EXTENT*2;
      static const int CHAOS_ELEMENT = 0xFFFFFFFF / (BOX_SIZE * BOX_SIZE);
      size_t* chaos_zone = new size_t[zones.size()];
      size_t index = 0;
      while (i != zones.end()) {
        Tools::ZoneElement* zone_element = (Tools::ZoneElement*)(*i);
        chaos_zone[index] = zone_element->isChaosZone()
          ? CHAOS_ELEMENT
          : 0;
        ++i;
        ++index;
      }
      ServerGameFileMapLocationData* server_map_location
        = server_map_locations;
      for (int z = 0; z < compilation_context.height; ++z) {
        for (int x = 0; x < compilation_context.width; ++x) {
          size_t chaos_level = 0;
          for (int dz = z - BOX_EXTENT; dz <= z + BOX_EXTENT; ++dz) {
            for (int dx = x - BOX_EXTENT; dx <= x + BOX_EXTENT; ++dx) {
              Zone::ZoneIndex zone_index
                = compilation_context.atOrDefault(dx, dz)->this_layer.zone;
              chaos_level += zone_index == Zone::INVALID_ZONE_INDEX
                ? CHAOS_ELEMENT
                : chaos_zone[zone_index];
            }
          }
          (server_map_location++)->chaos_level = chaos_level;
        }
      }
      SAFE_DELETE_ARRAY(chaos_zone);
    }
//////////////////////////////////////////////////////////////////////////////////
    server_data->data.locations = server_map_locations;
    server_data->data.locations_wide = compilation_context.width;
    server_data->data.locations_high = compilation_context.height;
   

    static const unsigned int REGION_SIZE = 16;
    unsigned int regions_wide
      = (compilation_context.width + REGION_SIZE - 1) / REGION_SIZE;
    unsigned int regions_high
      = (compilation_context.height + REGION_SIZE - 1) / REGION_SIZE;
    unsigned int number_of_regions = regions_wide * regions_high;
    ServerGameFileMapRegionData* server_map_regions
      = new ServerGameFileMapRegionData[number_of_regions];
    server_data->data.regions = server_map_regions;
    server_data->data.regions_high = regions_high;
    server_data->data.regions_wide = regions_wide;
    for (unsigned int region_y = 0; region_y < regions_high; ++region_y) {
      int top = region_y * REGION_SIZE;
      int bottom = top + REGION_SIZE;
      bottom = min(bottom, compilation_context.height);
      for (unsigned int region_x = 0; region_x < regions_wide; ++region_x) {
        int left = region_x * REGION_SIZE;
        int right = left + REGION_SIZE;
        right = min(right, compilation_context.width);
        ServerGameFileMapRegionData* region
          = &server_map_regions[region_y * regions_wide + region_x];
        int number_of_region_inhabitants = 0;

        // Reset all of the inhabitants
        for (int i = 0;
             i < ServerGameFileMapRegionData::NUMBER_OF_INHABITANTS;
           ++i) {
          region->inhabitants[i].spawn_point = next_spawn_point_index++;
          region->inhabitants[i].lifeform_ai
            = LifeformAI::INVALID_LIFEFORM_AI_INDEX;
          region->inhabitants[i].x_subcoordinate = -1;
          region->inhabitants[i].y_subcoordinate = -1;
        }

        // Load up all of the spawn data
        typedef std::set<std::pair<int,int>> Locations;
        Locations locations;
        typedef std::multimap<LifeformAI::LifeformAIIndex,std::pair<int,int>> Spawns;
        Spawns spawns;
        for (int y = top; y < bottom; ++y) {
          for (int x = left; x < right; ++x) {
            World::Tools::MapLocationData*
              map_location = compilation_context.atOrDefault(x,y);
            switch (map_location->this_layer.spawn_type) {
              using namespace Evidyon::World::Tools;
              default:
                assert(false);
              case MapLocationData::INVALID_SPAWN_TYPE:
                break;
              case MapLocationData::SPAWN_INHABITANT:
                if (World::NavigabilitySolidToWalking(
                      map_location->this_layer.navigability)) {
                  warning("inhabitant spawn location %i,%i ignored because "\
                          "of navigability", x, y);
                } else if (number_of_region_inhabitants >=
                           ServerGameFileMapRegionData::NUMBER_OF_INHABITANTS) {
                  warning("too many inhabitants in map region; ignoring %i,%i",
                          x,
                          y);
                } else {
                  ServerGameFileMapRegionData::Inhabitant* inhabitant
                    = &region->inhabitants[number_of_region_inhabitants++];
                  assert(map_location->this_layer.number_of_spawns == 1);
                  inhabitant->lifeform_ai = map_location->this_layer.spawn[0];
                  inhabitant->x_subcoordinate = x - left;
                  inhabitant->y_subcoordinate = y - top;
                  DEBUG_INFO("Inhabitant %lu at %i,%i",inhabitant->lifeform_ai,x,y);
                }
                break;
              case MapLocationData::SPAWN_SWARM:
                if (World::NavigabilitySolidToWalking(
                      map_location->this_layer.navigability)) {
                  warning("swarm spawn location %i,%i ignored because of navigability",
                          x, y);
                } else {
                  int number_of_spawns
                    = map_location->this_layer.number_of_spawns;
                  while (number_of_spawns-- > 0) {
                    std::pair<int,int> location(x,y);
                    spawns.insert(
                      Spawns::value_type(
                        map_location->this_layer.spawn[number_of_spawns],
                        location));
                    locations.insert(location);
                  }
                } break;
            }
          }
        }

        if (spawns.empty() || locations.empty()) {
          for (size_t i = 0;
                      i < ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS;
                    ++i) {
            region->swarm_spawn[i].lifeform_ai = LifeformAI::INVALID_LIFEFORM_AI_INDEX;
          }
          memset(region->spawn_locations, 0, sizeof(region->spawn_locations));
        } else {
          size_t index = 0;

          // remove random locations and the lifeforms that spawn there until
          // there are few enough to fit in the buffer
          if (locations.size() >
              ServerGameFileMapRegionData::NUMBER_OF_SPAWN_LOCATIONS) {
            warning("%s map's region %i,%i - %i,%i contains %lu spawn "\
                    "locations, but the max is %lu.  Some will not be used.",
                    element->getName().c_str(),
                    left, top, right, bottom, locations.size(),
                    ServerGameFileMapRegionData::NUMBER_OF_SPAWN_LOCATIONS);
          }
          while (locations.size() >
                 ServerGameFileMapRegionData::NUMBER_OF_SPAWN_LOCATIONS) {
            int index = rand()%locations.size();
            Locations::iterator i = locations.begin();
            while (index--) ++i;
            for (Spawns::iterator j = spawns.begin(); j != spawns.end();) {
              if (j->second == *i) {
                Spawns::iterator next = j; ++next;
                spawns.erase(j);
                j = next;
              } else {
                ++j;
              }
            }
            locations.erase(i);
          }

          // Randomly remove spawns until there are few enough to fit in the
          // buffer.  If removing a spawn removes the last reference to a
          // location, erase that location.
          if (spawns.size() >
              ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS) {
            warning("%s map's region %i,%i - %i,%i contains %lu swarm "\
                    "types, but the max is %lu.  Some will not be used.",
                    element->getName().c_str(),
                    left, top, right, bottom, spawns.size(),
                    ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS);
          }
          while (spawns.size() >
              ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS) {
            int index = rand()%spawns.size();
            Spawns::iterator i = spawns.begin();
            while (index--) ++i;
            // does any other spawn use this location?
            bool location_in_use = false;
            for (Spawns::iterator j = spawns.begin(); j != spawns.end(); ++j) {
              if (j->second != i->second) continue;
              location_in_use = true;
              break;
            }
            if (!location_in_use) locations.erase(i->second);
            spawns.erase(i);
          }

          // copy all of the spawn locations, adjusting the absolute coordinates
          // stored in the iterator down to the coordinate within the region and
          // reset all swarm spawning masks
          for (Locations::iterator i = locations.begin();
                                   i != locations.end(); ++i, ++index) {
            region->spawn_locations[index].intended_level =
              compilation_context.atOrDefault(i->first, i->second)
                ->this_layer.spawn_intended_level;
            region->spawn_locations[index].swarms_mask = 0;
            region->spawn_locations[index].x_subcoordinate = i->first - left;
            region->spawn_locations[index].y_subcoordinate = i->second - top;
            region->spawn_locations[index].spawn_point = next_spawn_point_index++;
          }
          while (index < ServerGameFileMapRegionData::NUMBER_OF_SPAWN_LOCATIONS) {
            region->spawn_locations[index++].swarms_mask = 0;
          }

          // copy all of the spawn monster types
          index = 0;
          assert(spawns.size() <= ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS);
          assert(ServerGameFileMapRegionData::NUMBER_OF_SWARM_SPAWNS < 32);
          Spawns::iterator last_spawn = spawns.begin();
          for (Spawns::iterator i = spawns.begin();
                                i != spawns.end(); ++i) {
            if (i->first != last_spawn->first) {
              ++index;
            }
            last_spawn = i;
            region->swarm_spawn[index].lifeform_ai = i->first;

            // find the location at which this lifeform spawns
            size_t location = 0;
            int x_subcoordinate = i->second.first - left;
            int y_subcoordinate = i->second.second - top;
            for (; location < locations.size(); ++location) {
              if (region->spawn_locations[location].x_subcoordinate
                    == x_subcoordinate &&
                  region->spawn_locations[location].y_subcoordinate
                    == y_subcoordinate) {
                break;
              }
            }

            // set this spawn into the location's mask
            assert(location < locations.size()); // always find a location
            region->spawn_locations[location].swarms_mask |= 1<<index;
          }
        }
      }
    }

    // Finally! We're done with the compilation context
    element->destroyCompilationContext(&compilation_context);

    // Compile the set of triggers for this map
    size_t number_of_triggers = compilation_context.triggers.size();
    server_data->data.number_of_triggers = number_of_triggers;
    server_data->data.triggers
      = new Evidyon::Trigger::Trigger[number_of_triggers];
    compilation_context.compileTriggers(
      server_data->data.triggers, number_of_triggers);

  END_GAME_FILE_ARRAY()


  //----[  items  ]------------------------------------------------------------
  BEGIN_DUAL_GAME_FILE_ARRAY(getItems,
                             Evidyon::Item::Tools::Item,
                             Item,
                             items)
    element->compile(&treasure_compiler,
                     &magic_compiler,
                     &client_data->data,
                     &server_data->data);
  END_GAME_FILE_ARRAY()


  //----[  spells  ]-------------------------------------------------------------
  BEGIN_DUAL_GAME_FILE_ARRAY(getSpells,
                             Evidyon::Tools::SpellElement,
                             Spell,
                             spells)
    element->compile(&magic_compiler, &client_data->data, &server_data->data);
  END_GAME_FILE_ARRAY()


  //----[  events  ]-----------------------------------------------------------
  {
    size_t count = event_compiler.numberOfEvents();
    if (count > Evidyon::Packets::EVENT_INDEX_LIMIT) {
      // there are too many events to fit in a packet--the datatype must be
      // changed to accomodate
      assert(false && "there are too many events to fit in a packet!");
      error("there are too many events to fit in a packet!");
      return false;
    }
    client_data_->events.array_entries = count;
    client_data_->events_array = new ClientGameFileData::Event[count];
    for (size_t i = 0; i < count; ++i) {
      event_compiler.compileEvent(
        i,
        &client_data_->events_array[i].data);
    }
    info("compiled %lu events", count);
  }



  //----[  magic  ]------------------------------------------------------------
  {
    size_t count = magic_compiler.numberOfMagics();
    server_data_->magic.array_entries = count;
    server_data_->magic_array = new ServerGameFileData::Magic[count];
    for (size_t i = 0; i < count; ++i) {
      magic_compiler.compileMagic(
        i,
        &server_data_->magic_array[i].data);
    }
    info("compiled %lu magic", count);
  }
  {
    for (int i = 0; i < Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
      size_t enchantments = magic_compiler.numberOfEnchantmentsOfType(
        Evidyon::Magic::EnchantmentType(i));
      server_data_->enchantments_in_type[i] = enchantments;
      client_data_->enchantments[i].number_of_type = enchantments;
      client_data_->enchantments[i].event_indices
        = new Event::EventIndex[enchantments];
      magic_compiler.compileEnchantmentTypeEvents(
        Evidyon::Magic::EnchantmentType(i),
        client_data_->enchantments[i].event_indices);
    }
  }



  //----[  treasure  ]---------------------------------------------------------
  {
    { // Epic Treasures
      size_t count = treasure_compiler.numberOfEpicTreasures();
      server_data_->epic_treasure.array_entries = count;
      server_data_->epic_treasure_array
        = new ServerGameFileData::EpicTreasure[count];
      for (size_t i = 0; i < count; ++i) {
        treasure_compiler.compileEpicTreasure(
          i,
          &server_data_->epic_treasure_array[i].data);
      }
      info("compiled %lu epic treasures", count);
    }
    { // Treasure Classes
      size_t count = treasure_compiler.numberOfTreasureClasses();
      server_data_->treasure_classes.array_entries = count;
      server_data_->treasure_classes_array
        = new ServerGameFileData::TreasureClass[count];
      for (size_t i = 0; i < count; ++i) {
        treasure_compiler.compileTreasureClass(
          i,
          &server_data_->treasure_classes_array[i].data);
      }
      info("compiled %lu treasure classes", count);
    }
    { // Treasure Drop Sequence Elements
      size_t count = treasure_compiler.numberOfDropSequenceElements();
      server_data_->treasure_drop_sequence.array_entries = count;
      server_data_->treasure_drop_sequence_array
        = new ServerGameFileData::TreasureDropSequenceElement[count];
      for (size_t i = 0; i < count; ++i) {
        treasure_compiler.compileDropSequenceElement(
          i,
          &server_data_->treasure_drop_sequence_array[i].data);
      }
      info("compiled %lu treasure drop sequence elements", count);
    }
  }

  //----[  geosids  ]----------------------------------------------------------
  {
    size_t count = geosid_compiler.numberOfGeosids();
    client_data_->geosids.array_entries = count;
    client_data_->geosids_array
      = new ClientGameFileData::Geosid[count];
    server_data_->geosids.array_entries = count;
    server_data_->geosids_array
      = new ServerGameFileData::Geosid[count];
    for (size_t i = 0; i < count; ++i) {
      geosid_compiler.compileClient(
        i,
        &client_data_->geosids_array[i].data);
      geosid_compiler.compileServer(
        i,
        &server_data_->geosids_array[i].data);
    }
    info("compiled %lu geosids", count);
  }


    
  //----[  actor templates  ]--------------------------------------------------
  {
    size_t number_of_actor_templates
      = actor_template_compiler.numberOfTemplates();
    client_data_->actor_templates.array_entries = number_of_actor_templates;
    client_data_->actor_templates_array
      = new ClientGameFileData::ActorTemplate[number_of_actor_templates];
    for (size_t i = 0; i < number_of_actor_templates; ++i) {
      actor_template_compiler.compile(
        i,
        &client_data_->actor_templates_array[i].data);
    }
    info("compiled %lu actor templates", number_of_actor_templates);
  }


  //----[  lifeforms  ]--------------------------------------------------------
  {
    size_t count = lifeform_ai_compiler.numberOfLifeforms();
    server_data_->lifeform_ai.array_entries = count;
    server_data_->lifeform_ai_array
      = new ServerGameFileData::LifeformAI[count];
    for (size_t i = 0; i < count; ++i) {
      lifeform_ai_compiler.compile(
        i,
        &server_data_->lifeform_ai_array[i].data);
    }
    info("compiled %lu lifeforms", count);
  }

  return true;
}













































































//----[  serialize  ]----------------------------------------------------------
bool GameFileCompiler::serialize() {

  // reserve space for the file header
  ClientGameFileHeader client_header;
  memset(&client_header, 0, sizeof(client_header));
  client_writer_.setWritePointOffset(sizeof(client_header));
  ServerGameFileHeader server_header;
  memset(&server_header, 0, sizeof(server_header));
  server_writer_.setWritePointOffset(sizeof(server_header));


#define FOR_EACH_ARRAY_ELEMENT(target, Type, member)\
  target##_header.member.array_entries = target##_data_->member.array_entries;\
  target##_header.member.first_byte_offset =\
    target##_writer_.getWritePointOffset();\
  for (size_t __i = 0; __i < target##_data_->member.array_entries; ++__i) {\
  Type* element = &target##_data_->member##_array[__i];\
  Type::DataType* data = &element->data;\
  bool succeeded = false;
#define END_FOR_EACH_ARRAY_ELEMENT()\
  if (!succeeded) {error("serialization failed"); return false;} }



  //----[  client data  ]------------------------------------------------------
  if (!ClientDataSerializer::write(&client_writer_,
                                   client_data_->client_data)) {
    error("serialization failed");
    return false;
  }

  //----[  server data  ]------------------------------------------------------
  if (!ServerDataSerializer::write(&server_writer_,
                                   server_data_->server_data)) {
    error("serialization failed");
    return false;
  }

  //----[  races  ]------------------------------------------------------------
  if (!AvatarRaceClientSerializer::write(
    &client_writer_,
    &CopiedList<Evidyon::Avatar::AvatarRaceClientDescription>(
      client_data_->races))) {
    return false;
  }
  if (!AvatarRaceServerSerializer::write(
    &server_writer_,
    &CopiedList<Evidyon::Avatar::AvatarRaceServerDescription>(
      server_data_->races))) {
    return false;
  }


  //----[  classes  ]------------------------------------------------------------
  if (!AvatarClassClientSerializer::write(
    &client_writer_,
    &CopiedList<Evidyon::Avatar::AvatarClassClientDescription>(
      client_data_->classes))) {
    error("serialization error");
    return false;
  }
  if (!AvatarClassServerSerializer::write(
    &server_writer_,
    &CopiedList<Evidyon::Avatar::AvatarClassServerDescription>(
      server_data_->classes))) {
    error("serialization error");
    return false;
  }


  //----[  levels  ]-----------------------------------------------------------
  if (!AvatarLevelSerializer::write(
      &server_writer_,
      &CopiedList<Evidyon::Avatar::AvatarLevelDescription>(
        server_data_->levels))) {
    error("serialization error");
    return false;
  }



  //----[  images  ]-----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client, ClientGameFileData::Image, images)
    succeeded = ImageSerializer::write(
      &client_writer_,
      &CopiedArray<unsigned char>((unsigned char*)data->png_data,
                                  data->png_size));
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  textures  ]---------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client, ClientGameFileData::Texture, textures)
    succeeded = TextureSerializer::write(
      &client_writer_,
       data);
  END_FOR_EACH_ARRAY_ELEMENT()

    
  //----[  meshes  ]-----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client, ClientGameFileData::Mesh, meshes)
    unsigned int number_of_subsets = data->number_of_subsets;
    succeeded = MeshSerializer::write(&client_writer_,
                                      number_of_subsets);
    for (unsigned int i = 0; succeeded && i < number_of_subsets; ++i) {
      using namespace Evidyon::Mesh;
      succeeded = MeshSubsetSerializer::write(&client_writer_,
       &CopiedArray<GeometryVertex>(data->subsets[i].vertices,
                                    data->subsets[i].number_of_vertices),
       &CopiedArray<GeometryIndex>(data->subsets[i].indices,
                                   data->subsets[i].number_of_indices));
    }
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  visual fx  ]--------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client, ClientGameFileData::VisualFX, visual_fx)
    succeeded = VisualFXSerializer::write(&client_writer_, data);
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  scenery  ]----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client, ClientGameFileData::Scenery, scenery)
    size_t number_of_subsets
      = client_data_->meshes_array[data->mesh].data.number_of_subsets;
    succeeded = ScenerySerializer::write(
      &client_writer_,
      data->mesh,
      &CopiedArray<Evidyon::Texture::TextureIndex>(data->subset_textures,
                                                   number_of_subsets),
      data->local_transform,
      data->visual_fx,
      data->visual_fx_offset);
  END_FOR_EACH_ARRAY_ELEMENT()

    
  //----[  animated meshes  ]--------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::AnimatedMesh,
                         animated_meshes)
    size_t number_of_texture_groups = data->number_of_texture_groups;
    succeeded = AnimatedMeshSerializer::write(
      &client_writer_,
      &CopiedArray<Evidyon::AnimatedMesh::AnimatedMeshData::Frame>(
        data->frames, data->number_of_frames),
      &CopiedArray<Evidyon::AnimatedMesh::AnimatedMeshData::Bone>(
        data->bones, data->number_of_bones),
      number_of_texture_groups);
    for (size_t i = 0; succeeded && i < number_of_texture_groups; ++i) {
      succeeded = AnimatedMeshTextureGroupSerializer::write(
        &client_writer_,
        &CopiedArray<Evidyon::AnimatedMesh::AnimatedGeometryVertex>(
          data->texture_groups[i].vertices,
          data->texture_groups[i].number_of_vertices),
        &CopiedArray<Evidyon::AnimatedMesh::AnimatedGeometryIndex>(
          data->texture_groups[i].indices,
          data->texture_groups[i].number_of_indices));
    }
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  animated mesh animations  ]-----------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::AnimatedMeshAnimation,
                         animated_mesh_animations)
    succeeded = AnimatedMeshAnimationSerializer::write(
      &client_writer_,
      data->number_of_total_frames,
      &CopiedArray<unsigned int>(data->animated_frame_to_frame_table,
                                 data->number_of_animated_frames),
      &CopiedArray<Evidyon::AnimatedMesh::AnimatedMeshAnimation::AnimatedFrameTransform>
        (data->animated_frame_transforms,
         data->number_of_keys * data->number_of_animated_frames));
  END_FOR_EACH_ARRAY_ELEMENT()



    
  //----[  skinned meshes  ]---------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::SkinnedMesh,
                         skinned_meshes)
    using namespace Evidyon::SkinnedMesh;
    size_t number_of_forms = data->number_of_forms;
    succeeded = SkinnedMeshSerializer::write(
      &client_writer_,
      &CopiedArray<SkinnedMeshAnimation>(
        data->animations,
        data->number_of_animations),
      &CopiedList<SkinnedMeshAttachmentPointOffset>(
        data->attachment_point_offsets),
      number_of_forms);
    for (size_t i = 0; succeeded && i < number_of_forms; ++i) {
      Evidyon::AnimatedMesh::AnimatedMeshIndex animated_mesh
        = data->forms[i].animated_mesh;
      size_t number_of_texture_groups
        = client_data_
            ->animated_meshes_array[animated_mesh]
              .data.number_of_texture_groups;
      succeeded = SkinnedMeshFormSerializer::write(
        &client_writer_,
        animated_mesh,
        &CopiedArray<Evidyon::Texture::TextureIndex>(
          data->forms[i].texture_group_textures,
          number_of_texture_groups),
        data->forms[i].transform);
    }
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  sounds  ]-----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Sound,
                         sounds)
    succeeded = SoundSerializer::write(
      &client_writer_,
      &CopiedArray<unsigned char>((const unsigned char*)data->waveform,
                                  data->waveform_size),
      data->wave_format);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  special fx  ]-------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::SpecialFX,
                         special_fx)
    succeeded = SpecialFXSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  special fx binding  ]-----------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::SpecialFXBinding,
                         special_fx_bindings)
    succeeded = SpecialFXBindingSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  special fx emitter  ]-----------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::SpecialFXEmitter,
                         special_fx_emitters)
    succeeded = SpecialFXEmitterSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  zones  ]------------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Zone,
                         zones)
    succeeded = ZoneSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  actor profiles  ]---------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::ActorProfile,
                         actor_profiles)
    succeeded = ActorProfileSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()




  //----[  lifeform ai  ]------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::LifeformAI,
                         lifeform_ai)
    succeeded = LifeformAISerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  actor templates  ]--------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::ActorTemplate,
                         actor_templates)
    succeeded = ActorTemplateSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  maps  ]-------------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Map,
                         maps)
    succeeded = ClientMapSerializer::write(
      &client_writer_,
      data->width,
      data->height,
      &CopiedArray<unsigned char>((unsigned char*)data->location_table_data,
                                  data->location_table_length),
      &CopiedArray<ClientGameFileMapLocationData>(
        data->unique_map_locations,
        data->number_of_unique_map_locations));
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::Map,
                         maps)
    succeeded = ServerMapSerializer::write(
      &server_writer_,
      data->map_hash,
      data->locations_wide,
      data->locations_high,
      &CopiedArray<ServerGameFileMapLocationData>(
        data->locations,
        data->locations_wide * data->locations_high),
      data->regions_wide,
      data->regions_high,
      &CopiedArray<ServerGameFileMapRegionData>(
        data->regions,
        data->regions_wide * data->regions_high),
      &CopiedArray<Evidyon::Trigger::Trigger>(
        data->triggers,
        data->number_of_triggers));
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  items  ]--------------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Item,
                         items)
    succeeded = ItemClientSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::Item,
                         items)
    succeeded = ItemServerSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()




    
  //----[  events  ]-----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Event,
                         events)
    succeeded = EventSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  //----[  magic  ]------------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::Magic,
                         magic)
    //assert(false);
    succeeded = MagicSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


    
  //----[  enchantments  ]-----------------------------------------------------
  for (int i = 0; i < Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    ClientEnchantmentSerializer::write(
      &client_writer_,
      &CopiedArray<Event::EventIndex>(
        client_data_->enchantments[i].event_indices,
        client_data_->enchantments[i].number_of_type));
  }
  ServerEnchantmentSerializer::write(
    &server_writer_,
    &CopiedList<size_t>(server_data_->enchantments_in_type));


  //----[  spells  ]-----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Spell,
                         spells)
    succeeded = ClientSpellSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::Spell,
                         spells)
    succeeded = ServerSpellSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()



  //----[  treasure  ]---------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::EpicTreasure,
                         epic_treasure)
    succeeded = EpicTreasureSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::TreasureClass,
                         treasure_classes)
    succeeded = TreasureClassSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::TreasureDropSequenceElement,
                         treasure_drop_sequence)
    succeeded = TreasureDropSequenceSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()

  
  //----[  geosids  ]----------------------------------------------------------
  FOR_EACH_ARRAY_ELEMENT(client,
                         ClientGameFileData::Geosid,
                         geosids)
    succeeded = ClientGeosidSerializer::write(
      &client_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()
  FOR_EACH_ARRAY_ELEMENT(server,
                         ServerGameFileData::Geosid,
                         geosids)
    succeeded = ServerGeosidSerializer::write(
      &server_writer_,
      data);
  END_FOR_EACH_ARRAY_ELEMENT()


  // Finished building!
  info("==== Compilation succeeded.  %lu bytes client / %lu bytes server ====",
       client_writer_.getWritePointOffset(),
       server_writer_.getWritePointOffset());

  // write the header information
  client_writer_.setWritePointOffset(0);
  memcpy(client_writer_.getWritePoint(), &client_header, sizeof(client_header));
  server_writer_.setWritePointOffset(0);
  memcpy(server_writer_.getWritePoint(), &server_header, sizeof(server_header));


  return true;
}





//----[  zero  ]---------------------------------------------------------------
void GameFileCompiler::zero() {
  source_ = NULL;
  progress_ = NULL;
  client_data_ = NULL;
  server_data_ = NULL;
}





//----[  info  ]---------------------------------------------------------------
void GameFileCompiler::info(const char* message, ...) {
  char buffer[2048];
  va_list args;
  va_start(args, message);
  _vsnprintf_s(buffer,
               sizeof(buffer),
               sizeof(buffer),
               message,
               args);  // Output to the buffer
  buffer[sizeof(buffer)-1] = '\0';
  va_end(args);
  progress_->echo(ProgressCallback::ECHO_INFO, buffer);
}




//----[  warning  ]------------------------------------------------------------
void GameFileCompiler::warning(const char* message, ...) {
  char buffer[2048];
  va_list args;
  va_start(args, message);
  _vsnprintf_s(buffer,
               sizeof(buffer),
               sizeof(buffer),
               message,
               args);  // Output to the buffer
  buffer[sizeof(buffer)-1] = '\0';
  va_end(args);
  progress_->echo(ProgressCallback::ECHO_WARNING, buffer);
}




//----[  error  ]--------------------------------------------------------------
void GameFileCompiler::error(const char* message, ...) {
  char buffer[2048];
  va_list args;
  va_start(args, message);
  _vsnprintf_s(buffer,
               sizeof(buffer),
               sizeof(buffer),
               message,
               args);  // Output to the buffer
  buffer[sizeof(buffer)-1] = '\0';
  va_end(args);
  progress_->echo(ProgressCallback::ECHO_ERROR, buffer);
}



//----[  writeGameFile  ]------------------------------------------------------
bool GameFileCompiler::writeGameFile(const char* file_name,
                                     GameFileWriter* writer) {

  HANDLE game_file_handle = CreateFile(file_name,
                                       GENERIC_WRITE,
                                       FILE_SHARE_WRITE,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
                                        | FILE_FLAG_WRITE_THROUGH
                                        | FILE_FLAG_SEQUENTIAL_SCAN,
                                       NULL);
  if (game_file_handle == NULL) return false;

  const char* data = NULL;
  size_t length = 0;
  bool succeeded = writer->finalize(&data, &length);
  DWORD bytes_written = 0;
  succeeded = succeeded && WriteFile(game_file_handle,
                                     data,
                                     length,
                                     &bytes_written,
                                     NULL);
  if (!succeeded) {
    return false;
  } else {
    assert(bytes_written == length);
  }
  CloseHandle(game_file_handle);

  return true;
}




}
}

