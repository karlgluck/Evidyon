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
#ifndef __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILEDATA_H__
#define __COMPLETE_EDITOR_GAMEFILE_CLIENTGAMEFILEDATA_H__
#pragma once




#include "clientgamefileclientdata.h"
#include "clientgamefileimagedata.h"
#include "clientgamefiletexturedata.h"
#include "shared/scenery/scenery.h"
#include "clientgamefilemapdata.h"
#include "clientgamefilemaplocationdata.h"
#include "shared/mesh/mesh.h"
#include "shared/scenery/sceneryrenderer.h"
#include "shared/visualfx/visualfxrenderer.h"
#include "shared/evidyond3dlightmanager.h"
#include "shared/sound/sound.h"
#include "shared/specialfx/specialfx.h"
#include "shared/specialfx/specialfxbinding.h"
#include "shared/specialfx/specialfxemitter.h"
#include "shared/animated-mesh/animatedmeshdata.h"
#include "shared/animated-mesh/animatedmeshanimation.h"
#include "shared/skinned-mesh/skinnedmesh.h"
#include "shared/skinned-mesh/skinnedmeshform.h"
#include "shared/skinned-mesh/skinnedmeshanimation.h"
#include "shared/avatar/avatarraceclientdescription.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclassclientdescription.h"
#include "shared/avatar/avatarclass.h"
#include "shared/actor/actorprofile.h"
#include "shared/actor/actortemplate.h"
#include "shared/zone/zone.h"
#include "shared/item/itemclientdescription.h"
#include "shared/event/event.h"
#include "shared/spell/spellclientdescription.h"
#include "shared/geosid/geosidclientdescription.h"
#include "clientgamefileenchantmentdata.h"
#include "shared/magic/enchantmenttype.h"
#include "gamefiledata.h"




namespace Evidyon {
namespace GameFile {

typedef EvidyonGameFileSerializer<
    ClientGameFileClientData
  >ClientDataSerializer;

typedef EvidyonGameFileSerializer<Array<unsigned char, unsigned int>> ImageSerializer;
typedef EvidyonGameFileSerializer<ClientGameFileTextureData> TextureSerializer;

typedef EvidyonGameFileSerializer<
  unsigned int // # of subsets
  >MeshSerializer;

typedef EvidyonGameFileSerializer<
    Array<Evidyon::Mesh::GeometryVertex,unsigned int>,  // vertices
    Array<Evidyon::Mesh::GeometryIndex, unsigned int>   // indices
  >MeshSubsetSerializer;

typedef EvidyonGameFileSerializer<
    Mesh::MeshIndex,                                      // source mesh
    Array<Evidyon::Texture::TextureIndex,unsigned int>,  // subsets
    D3DXMATRIX,                                           // local transform
    VisualFX::VisualFXIndex,                              // attached visual fx
    D3DXVECTOR3                                           // visual fx offset
  >ScenerySerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::VisualFX::VisualFX
  >VisualFXSerializer;

typedef EvidyonGameFileSerializer<
    Array<Evidyon::AnimatedMesh::AnimatedMeshData::Frame, size_t>,  // frames
    Array<Evidyon::AnimatedMesh::AnimatedMeshData::Bone, size_t>, // bones
    size_t // number of texture groups
  >AnimatedMeshSerializer;
typedef EvidyonGameFileSerializer<
    Array<Evidyon::AnimatedMesh::AnimatedGeometryVertex, size_t>, // vertices
    Array<Evidyon::AnimatedMesh::AnimatedGeometryIndex,  size_t>  // indices
  >AnimatedMeshTextureGroupSerializer;


typedef EvidyonGameFileSerializer<
  size_t,                       // total number of frames
  Array<unsigned int, size_t>,  // animated frame to frame table (& # of frames)
  Array<Evidyon::AnimatedMesh::AnimatedMeshAnimation::AnimatedFrameTransform,
        size_t> // animated frame transforms
  // NOTE: # of keys = # of animated frame transforms / # of animated frames
  >AnimatedMeshAnimationSerializer;

typedef EvidyonGameFileSerializer<
  Array<Evidyon::SkinnedMesh::SkinnedMeshAnimation, size_t>, // animations
  List<Evidyon::SkinnedMesh::SkinnedMeshAttachmentPointOffset,
      Evidyon::SkinnedMesh::NUMBER_OF_ATTACHMENT_POINTS>,
  size_t  // number of forms
  >SkinnedMeshSerializer;
typedef EvidyonGameFileSerializer<
  Evidyon::AnimatedMesh::AnimatedMeshIndex, // source mesh
  Array<Evidyon::Texture::TextureIndex,unsigned char>,// texture group textures
  D3DXMATRIX                       // transform
  >SkinnedMeshFormSerializer;

typedef EvidyonGameFileSerializer<
  Array<unsigned char, size_t>, // waveform
  WAVEFORMATEX                  // sound format descriptor
  >SoundSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::SpecialFX::SpecialFX
  >SpecialFXSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::SpecialFX::SpecialFXBinding
  >SpecialFXBindingSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::SpecialFX::SpecialFXEmitter
  >SpecialFXEmitterSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::Zone::Zone
  >ZoneSerializer;

typedef EvidyonGameFileSerializer<
    List<Evidyon::Avatar::AvatarRaceClientDescription,
         Evidyon::Avatar::NUMBER_OF_AVATAR_RACES>
  >AvatarRaceClientSerializer;

typedef EvidyonGameFileSerializer<
    List<Evidyon::Avatar::AvatarClassClientDescription,
         Evidyon::Avatar::NUMBER_OF_AVATAR_CLASSES>
  >AvatarClassClientSerializer;


typedef EvidyonGameFileSerializer<
    Evidyon::Actor::ActorProfile
  >ActorProfileSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::Actor::ActorTemplate
  >ActorTemplateSerializer;

typedef EvidyonGameFileSerializer<
    size_t,                                      // width
    size_t,                                      // height
    Array<unsigned char, size_t>,                // location table data
    Array<ClientGameFileMapLocationData, size_t> // unique map locations
  >ClientMapSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::Item::ItemClientDescription
  >ItemClientSerializer;

typedef EvidyonGameFileSerializer<
    Evidyon::Event::Event
  >EventSerializer;

typedef EvidyonGameFileSerializer<
  Evidyon::Spell::SpellClientDescription
  > ClientSpellSerializer;
  
typedef EvidyonGameFileSerializer<
  Evidyon::Geosid::GeosidClientDescription
  > ClientGeosidSerializer;

typedef EvidyonGameFileSerializer<
  Array<Evidyon::Event::EventIndex, size_t> // events in this type
  > ClientEnchantmentSerializer;


//----[  ClientGameFileData  ]-------------------------------------------------
// Members are listed in the order in which they are serialized in the file
struct ClientGameFileData {
  struct GameFileArrayData {
    size_t array_entries;
    size_t first_byte_offset;
    //size_t hash;

    // These byte sizes are the total number of bytes needed to allocate all
    // of the data for all of the entries in this array.
    size_t total_size_in_memory;
    size_t total_size_in_file;
  };

  ClientGameFileClientData client_data;
  Evidyon::Avatar::AvatarRaceClientDescription races[Avatar::NUMBER_OF_AVATAR_RACES];
  Evidyon::Avatar::AvatarClassClientDescription classes[Avatar::NUMBER_OF_AVATAR_CLASSES];
  ClientGameFileEnchantmentData enchantments[Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES];

  GAME_FILE_ARRAY(Image, ClientGameFileImageData, images);
  GAME_FILE_ARRAY(Texture, ClientGameFileTextureData, textures);
  GAME_FILE_ARRAY(Mesh, Evidyon::Mesh::Mesh, meshes);
  GAME_FILE_ARRAY(VisualFX, Evidyon::VisualFX::VisualFX, visual_fx);
  GAME_FILE_ARRAY(Scenery, Evidyon::Scenery::Scenery, scenery);
  GAME_FILE_ARRAY(AnimatedMesh, Evidyon::AnimatedMesh::AnimatedMeshData, animated_meshes);
  GAME_FILE_ARRAY(AnimatedMeshAnimation, Evidyon::AnimatedMesh::AnimatedMeshAnimation, animated_mesh_animations);
  GAME_FILE_ARRAY(SkinnedMesh, Evidyon::SkinnedMesh::SkinnedMesh, skinned_meshes);
  GAME_FILE_ARRAY(Sound, Evidyon::Sound::Sound, sounds);
  GAME_FILE_ARRAY(SpecialFX, Evidyon::SpecialFX::SpecialFX, special_fx);
  GAME_FILE_ARRAY(SpecialFXBinding, Evidyon::SpecialFX::SpecialFXBinding, special_fx_bindings);
  GAME_FILE_ARRAY(SpecialFXEmitter, Evidyon::SpecialFX::SpecialFXEmitter, special_fx_emitters);
  GAME_FILE_ARRAY(Zone, Evidyon::Zone::Zone, zones);
  GAME_FILE_ARRAY(ActorProfile, Evidyon::Actor::ActorProfile, actor_profiles);
  GAME_FILE_ARRAY(ActorTemplate, Evidyon::Actor::ActorTemplate, actor_templates);
  GAME_FILE_ARRAY(Map, Evidyon::GameFile::ClientGameFileMapData, maps);
  GAME_FILE_ARRAY(Item, Evidyon::Item::ItemClientDescription, items);
  GAME_FILE_ARRAY(Event, Evidyon::Event::Event, events);
  GAME_FILE_ARRAY(Spell, Evidyon::Spell::SpellClientDescription, spells);
  GAME_FILE_ARRAY(Geosid, Evidyon::Geosid::GeosidClientDescription, geosids);
};



//----[  ClientGameFileHeader  ]-----------------------------------------------
struct ClientGameFileHeader {
  struct Array {
    size_t first_byte_offset;
    size_t array_entries;
  };
  Array images;
  Array textures;
  Array meshes;
  Array visual_fx;
  Array scenery;
  Array animated_meshes;
  Array animated_mesh_animations;
  Array skinned_meshes;
  Array sounds;
  Array special_fx;
  Array special_fx_bindings;
  Array special_fx_emitters;
  Array zones;
  Array actor_profiles;
  Array actor_templates;
  Array maps;
  Array items;
  Array events;
  Array spells;
  Array geosids;
};



}
}

#endif