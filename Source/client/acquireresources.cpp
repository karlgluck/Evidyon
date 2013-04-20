//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include "stdafx.h"
#include "evidyonguidefs.h"





#include "shared/gamefile/clientgamefiledata.h"
#include "shared/gamefile/gamefilereader.h"
#include "shared/animated-mesh/animatedmeshutils.h"


//----[  acquireResources  ]---------------------------------------------------
bool EvidyonClient::acquireResources() {
  using namespace Evidyon::GameFile;
  using namespace Evidyon::Avatar;


  GameFileReader reader;
  if (!reader.open(config_.getSetting("Media",
                                      "MediaFile",
                                      STRING_DEFAULT_GAME_FILE))) {
    return false;
  }

  ClientGameFileHeader header;
  memcpy(&header, reader.getReadPoint(), sizeof(header));
  reader.setReadPoint(reader.getReadPoint() + sizeof(header));



  {
    bool ok = true; // a short variable name helps :)

    renderstate_manager_.create(d3d_device_);
    ok = ok && mesh_renderer_.create(d3d_device_);
    ok = ok && light_manager_.create(d3d_device_);
    ok = ok && visual_fx_renderer_.create(d3d_device_,
                                          &mesh_renderer_,
                                          &scenery_renderer_,
                                          &light_manager_,
                                          &renderstate_manager_);
    ok = ok && scenery_renderer_.create(&mesh_renderer_,
                                        &visual_fx_renderer_);
    ok = ok && animated_mesh_renderer_.create(d3d_device_);
    ok = ok && skinned_mesh_renderer_.create(&animated_mesh_renderer_);
    if (config_.getSetting("Audio", "Enable", 1) != 0) {
      ok = ok && sound_manager_.create(myMainWindow);
    }
    ok = ok && actor_renderer_.create(&skinned_mesh_renderer_,
                                      &scenery_renderer_,
                                      &sound_manager_,
                                      &special_fx_manager_);
    ok = ok && actor_manager_.create(&actor_renderer_, &event_manager_);
    ok = ok && map_renderer_.create(d3d_device_,
                                    &scenery_renderer_,
                                    &skinned_mesh_renderer_);
    /*ok = ok &&*/ special_fx_manager_.create(&visual_fx_renderer_,
                                          &sound_manager_);
    /*ok = ok &&*/ event_manager_.create(&special_fx_manager_);

    if (!ok) return false;
  }

  
  ClientDataSerializer::read(
    &reader,
    &client_data_);
  AvatarRaceClientSerializer::read(
    &reader,
    &CopiedList<AvatarRaceClientDescription>(races_));
  AvatarClassClientSerializer::read(
    &reader,
    &CopiedList<AvatarClassClientDescription>(classes_));

  assert(reader.getReadPointOffset()
         == header.images.first_byte_offset);

  

  //   Images
  //===========================================================================
  assert(reader.getReadPointOffset() == header.images.first_byte_offset);
  images_ = new LPDIRECT3DTEXTURE9[header.images.array_entries];
  number_of_images_ = header.images.array_entries;
  for (size_t i = 0; i < header.images.array_entries; ++i) {
    const char* source_data = reader.getReadPoint() + sizeof(unsigned int);
    size_t image_size = 0;
    ImageSerializer::read(
      &reader,
      &PeekArray<unsigned char>(&image_size));

    HRESULT hr = D3DXCreateTextureFromFileInMemory(d3d_device_,
                                                   source_data,
                                                   image_size,
                                                   &images_[i]);
    if (FAILED(hr)) {
      for (size_t j = 0; j < i; ++j) SAFE_RELEASE(images_[j]);
      SAFE_DELETE_ARRAY(images_);
      return false;
    }
  }


  //   Textures
  //===========================================================================
  assert(reader.getReadPointOffset() == header.textures.first_byte_offset);
  for (size_t i = 0; i < header.textures.array_entries; ++i) {
    ClientGameFileTextureData data;
    TextureSerializer::read(
      &reader,
      &data);
    Evidyon::Texture::TextureDescription description;
    assert(data.image_index < header.images.array_entries);
    (description.d3d_texture = images_[data.image_index])->AddRef();
    description.blend_type
      = Evidyon::Texture::TextureDescription::BlendType(data.blend_type);
    description.flip_horizontal = data.flip_horizontal == 0 ? false : true;
    description.flip_vertical   = data.flip_vertical == 0 ? false : true;
    description.animation_type
      = Evidyon::Texture::TextureDescription::AnimationType(data.animation_type);
    memcpy(&description.animation,
           &data.animation,
           sizeof(description.animation));
    renderstate_manager_.addTexture(i, description);
  }


  //   Meshes
  //==========================================================================
  assert(reader.getReadPointOffset() == header.meshes.first_byte_offset);
  for (size_t i = 0; i < header.meshes.array_entries; ++i) {
    Evidyon::Mesh::Mesh mesh;
    MeshSerializer::read(
      &reader,
      &mesh.number_of_subsets);
    mesh.subsets = new Evidyon::Mesh::Mesh::Subset[mesh.number_of_subsets];
    for (size_t subset_index = 0;
                subset_index < mesh.number_of_subsets;
              ++subset_index) {
      size_t offset = reader.getReadPointOffset();
      size_t number_of_vertices = 0, number_of_indices = 0;
      MeshSubsetSerializer::read(
        &reader,
        &PeekArray<Evidyon::Mesh::GeometryVertex>(&number_of_vertices),
        &PeekArray<Evidyon::Mesh::GeometryIndex>(&number_of_indices));
      Evidyon::Mesh::GeometryVertex* vertices
        = new Evidyon::Mesh::GeometryVertex[number_of_indices];
      Evidyon::Mesh::GeometryIndex* indices
        = new Evidyon::Mesh::GeometryIndex[number_of_indices];
      reader.setReadPointOffset(offset);
      MeshSubsetSerializer::read(
        &reader,
        &CopiedArray<Evidyon::Mesh::GeometryVertex>(
          vertices,
          number_of_vertices,
          &number_of_vertices),
        &CopiedArray<Evidyon::Mesh::GeometryIndex>(
          indices,
          number_of_indices,
          &number_of_indices));
      mesh.subsets[subset_index].vertices = vertices;
      mesh.subsets[subset_index].number_of_vertices = number_of_vertices;
      mesh.subsets[subset_index].indices = indices;
      mesh.subsets[subset_index].number_of_indices = number_of_indices;
    }
    mesh_renderer_.addMesh(i, mesh);
  }


  //   VisualFX
  //==========================================================================
  assert(reader.getReadPointOffset() == header.visual_fx.first_byte_offset);
  for (size_t i = 0; i < header.visual_fx.array_entries; ++i) {
    Evidyon::VisualFX::VisualFX description;
    VisualFXSerializer::read(
      &reader,
      &description);
    visual_fx_renderer_.addVisualFX(i, description);
  }

  //   Scenery
  //==========================================================================
  assert(reader.getReadPointOffset() == header.scenery.first_byte_offset);
  for (size_t i = 0; i < header.scenery.array_entries; ++i) {
    Evidyon::Scenery::Scenery description;
    size_t number_of_subsets = 0;
    size_t offset = reader.getReadPointOffset();
    ScenerySerializer::read(
      &reader,
      &description.mesh,
      &PeekArray<Evidyon::Texture::TextureIndex>(&number_of_subsets));
    assert(number_of_subsets ==
           mesh_renderer_.getNumberOfMeshSubsets(description.mesh));
    reader.setReadPointOffset(offset);
    description.subset_textures
      = new Evidyon::Texture::TextureIndex[number_of_subsets];
    ScenerySerializer::read(
      &reader,
      &description.mesh,
      &CopiedArray<Evidyon::Texture::TextureIndex>(
        description.subset_textures,
        number_of_subsets,
        &number_of_subsets),
      &description.local_transform,
      &description.visual_fx,
      &description.visual_fx_offset);
    scenery_renderer_.addScenery(i, description);
  }

  //   Animated Mesh
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.animated_meshes.first_byte_offset);
  for (size_t i = 0; i < header.animated_meshes.array_entries; ++i) {
    using namespace Evidyon::AnimatedMesh;
    AnimatedMeshData description;
    size_t number_of_frames, number_of_bones, number_of_texture_groups;
    size_t offset = reader.getReadPointOffset();
    AnimatedMeshSerializer::read(
      &reader,
      &PeekArray<AnimatedMeshData::Frame>(&number_of_frames),
      &PeekArray<AnimatedMeshData::Bone>(&number_of_bones),
      &number_of_texture_groups);
    description.frames = new AnimatedMeshData::Frame[number_of_frames];
    description.bones = new AnimatedMeshData::Bone[number_of_bones];
    description.texture_groups
      = new AnimatedMeshData::TextureGroup[number_of_texture_groups];
    reader.setReadPointOffset(offset);
    AnimatedMeshSerializer::read(
      &reader,
      &CopiedArray<AnimatedMeshData::Frame>(description.frames,
                                            number_of_frames,
                                            &description.number_of_frames),
      &CopiedArray<AnimatedMeshData::Bone>(description.bones,
                                           number_of_bones,
                                           &description.number_of_bones),
      &description.number_of_texture_groups);
    for (size_t g = 0; g < number_of_texture_groups; ++g) {
      size_t number_of_vertices, number_of_indices;
      offset = reader.getReadPointOffset();
      AnimatedMeshTextureGroupSerializer::read(
        &reader,
        &PeekArray<AnimatedGeometryVertex>(&number_of_vertices),
        &PeekArray<AnimatedGeometryIndex>(&number_of_indices)
        );
      AnimatedGeometryVertex* vertices
        = new AnimatedGeometryVertex[number_of_vertices];
      AnimatedGeometryIndex* indices
        = new AnimatedGeometryIndex[number_of_indices];

      reader.setReadPointOffset(offset);
      AnimatedMeshTextureGroupSerializer::read(
        &reader,
        &CopiedArray<AnimatedGeometryVertex>(
          vertices,
          number_of_vertices,
          &description.texture_groups[g].number_of_vertices),
        &CopiedArray<AnimatedGeometryIndex>(
          indices,
          number_of_indices,
          &description.texture_groups[g].number_of_indices)
        );

      description.texture_groups[g].vertices = vertices;
      description.texture_groups[g].indices = indices;
    }

    // Translate into the renderable form
    RenderableAnimatedMesh renderable_animated_mesh;
    if (!AnimatedMeshUtil_CreateRenderableAnimatedMesh(
        d3d_device_,
        &description,
        &renderable_animated_mesh)) {
      return false;
    }

    // Remove the temporary data
    AnimatedMeshUtil_DeallocateAnimatedMeshData(&description);

    // Copy into the renderer
    animated_mesh_renderer_.addRenderableAnimatedMesh(
      i,
      renderable_animated_mesh);
  }

  //   Animated Mesh Animation
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.animated_mesh_animations.first_byte_offset);
  for (size_t i = 0; i < header.animated_mesh_animations.array_entries; ++i) {
    using namespace Evidyon::AnimatedMesh;
    size_t number_of_total_frames;
    size_t number_of_animated_frames;
    size_t number_of_animated_frame_transforms;
    AnimatedMeshAnimation description;
    size_t offset = reader.getReadPointOffset();
    AnimatedMeshAnimationSerializer::read(
      &reader,
      &number_of_total_frames,
      &PeekArray<unsigned int>(&number_of_animated_frames),
      &PeekArray<AnimatedMeshAnimation::AnimatedFrameTransform>(
        &number_of_animated_frame_transforms));

    unsigned int* animated_frame_to_frame_table 
      = new unsigned int[number_of_animated_frames];
    AnimatedMeshAnimation::AnimatedFrameTransform* animated_frame_transforms
      = new AnimatedMeshAnimation::AnimatedFrameTransform
              [number_of_animated_frame_transforms];
    reader.setReadPointOffset(offset);
    AnimatedMeshAnimationSerializer::read(
      &reader,
      &number_of_total_frames,
      &CopiedArray<unsigned int>(animated_frame_to_frame_table,
                                 number_of_animated_frames,
                                 &description.number_of_animated_frames),
      &CopiedArray<AnimatedMeshAnimation::AnimatedFrameTransform>(
        animated_frame_transforms,
        number_of_animated_frame_transforms,
        &description.number_of_keys));
    description.number_of_keys /= number_of_animated_frames;

    description.number_of_total_frames = number_of_total_frames;

    description.animated_frame_to_frame_table = animated_frame_to_frame_table;
    description.animated_frame_transforms = animated_frame_transforms;

    animated_mesh_renderer_.addAnimatedMeshAnimation(i, description);
  }

  //   Skinned Mesh
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.skinned_meshes.first_byte_offset);
  for (size_t i = 0; i < header.skinned_meshes.array_entries; ++i) {
    using namespace Evidyon::SkinnedMesh;
    Evidyon::SkinnedMesh::SkinnedMesh description;

    size_t number_of_animations;
    size_t number_of_forms;
    size_t offset = reader.getReadPointOffset();
    SkinnedMeshSerializer::read(
      &reader,
      &PeekArray<SkinnedMeshAnimation>(
        &number_of_animations),
      &CopiedList<SkinnedMeshAttachmentPointOffset>(
        (const SkinnedMeshAttachmentPointOffset*)NULL),
      &number_of_forms);

    description.forms = new SkinnedMeshForm[number_of_forms];
    description.animations
      = new SkinnedMeshAnimation[number_of_animations];


    reader.setReadPointOffset(offset);
    SkinnedMeshSerializer::read(
      &reader,
      &CopiedArray<SkinnedMeshAnimation>(
        description.animations,
        number_of_animations,
        &description.number_of_animations),
      &CopiedList<SkinnedMeshAttachmentPointOffset>(
        description.attachment_point_offsets),
      &description.number_of_forms);

    for (size_t form = 0; form < number_of_forms; ++form) {
      size_t number_of_texture_groups;
      offset = reader.getReadPointOffset();
      SkinnedMeshFormSerializer::read(
        &reader,
        &description.forms[form].animated_mesh,
        &PeekArray<Evidyon::Texture::TextureIndex>(&number_of_texture_groups),
        &description.forms[form].transform);

      description.forms[form].texture_group_textures
        = new Evidyon::Texture::TextureIndex[number_of_texture_groups];
      reader.setReadPointOffset(offset);
      SkinnedMeshFormSerializer::read(
        &reader,
        &description.forms[form].animated_mesh,
        &CopiedArray<Evidyon::Texture::TextureIndex>(
          description.forms[form].texture_group_textures,
          number_of_texture_groups,
          &number_of_texture_groups),
        &description.forms[form].transform);

      assert(number_of_texture_groups == 
        animated_mesh_renderer_.numberOfTextureGroups(
          description.forms[form].animated_mesh));
    }

    skinned_mesh_renderer_.addSkinnedMesh(i, description);
  }

  //   Sound
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.sounds.first_byte_offset);
  for (size_t i = 0; i < header.sounds.array_entries; ++i) {
    Evidyon::Sound::Sound description;

    size_t waveform_length;
    size_t offset = reader.getReadPointOffset();
    SoundSerializer::read(
      &reader,
      &PeekArray<unsigned char>(&waveform_length));

    description.waveform = new unsigned char[waveform_length];
    reader.setReadPointOffset(offset);
    SoundSerializer::read(
      &reader,
      &CopiedArray<unsigned char>(
        (unsigned char*)description.waveform,
        waveform_length,
        &description.waveform_size),
      &description.wave_format);

    sound_manager_.addSound(i, description);
  }

  //   Special FX
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.special_fx.first_byte_offset);
  special_fx_manager_.initNumberOfSpecialFX(header.special_fx.array_entries);
  for (size_t i = 0; i < header.special_fx.array_entries; ++i) {
    Evidyon::SpecialFX::SpecialFX description;
    SpecialFXSerializer::read(
      &reader,
      &description);
    special_fx_manager_.initSpecialFX(i, &description);
  }

  //   Special FX Bindings
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.special_fx_bindings.first_byte_offset);
  special_fx_manager_.initNumberOfBindings(
    header.special_fx_bindings.array_entries);
  for (size_t i = 0; i < header.special_fx_bindings.array_entries; ++i) {
    Evidyon::SpecialFX::SpecialFXBinding description;
    SpecialFXBindingSerializer::read(
      &reader,
      &description);
    special_fx_manager_.initBinding(i, &description);
  }

  //   Special FX Emitters
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.special_fx_emitters.first_byte_offset);
  special_fx_manager_.initNumberOfEmitters(
    header.special_fx_emitters.array_entries);
  for (size_t i = 0; i < header.special_fx_emitters.array_entries; ++i) {
    Evidyon::SpecialFX::SpecialFXEmitter description;
    SpecialFXEmitterSerializer::read(
      &reader,
      &description);
    special_fx_manager_.initEmitter(i, &description);
  }

  //   Zone
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.zones.first_byte_offset);
  zones_ = new Evidyon::Zone::Zone[header.zones.array_entries];
  number_of_zones_ = header.zones.array_entries;
  for (size_t i = 0; i < header.zones.array_entries; ++i) {
    ZoneSerializer::read(
      &reader,
      &zones_[i]);
  }

  //   Actor Profile
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.actor_profiles.first_byte_offset);
  actor_renderer_.initNumberOfActorProfiles(
    header.actor_profiles.array_entries);
  for (size_t i = 0; i < header.actor_profiles.array_entries; ++i) {
    Evidyon::Actor::ActorProfile description;
    ActorProfileSerializer::read(
      &reader,
      &description);
    actor_renderer_.initActorProfile(i, &description);
  }

  //   Actor Template
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.actor_templates.first_byte_offset);
  actor_manager_.initNumberOfActorTemplates(
    header.actor_templates.array_entries);
  for (size_t i = 0; i < header.actor_templates.array_entries; ++i) {
    Evidyon::Actor::ActorTemplate description;
    ActorTemplateSerializer::read(
      &reader,
      &description);
    actor_manager_.initActorTemplate(i, &description);
  }


  //   Map
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.maps.first_byte_offset);
  maps_ = new Evidyon::Client::ClientMapData[header.maps.array_entries];
  for (size_t i = 0; i < header.maps.array_entries; ++i) {
    size_t width, height;
    const unsigned char* map_location_table;
    size_t map_location_table_bytes;
    const ClientGameFileMapLocationData* unique_locations;
    size_t number_of_unique_locations;
    ClientMapSerializer::read(
      &reader,
      &width,
      &height,
      &PeekArray<unsigned char>(
        &map_location_table,
        &map_location_table_bytes),
      &PeekArray<ClientGameFileMapLocationData>(
        &unique_locations,
        &number_of_unique_locations));

    assert(map_location_table_bytes % (width * height) == 0);
    if (!maps_[i].create(width,
                         height,
                         map_location_table,
                         map_location_table_bytes / (width * height),
                         unique_locations,
                         number_of_unique_locations)) {
      return false;
    }
  }

  //   Item
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.items.first_byte_offset);
  items_ = new Evidyon::Item::ItemClientDescription[header.items.array_entries];
  number_of_items_ = header.items.array_entries;
  for (size_t i = 0; i < header.items.array_entries; ++i) {
    ItemClientSerializer::read(
      &reader,
      &items_[i]);
  }

  //   Event
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.events.first_byte_offset);
  event_manager_.initNumberOfEvents(header.events.array_entries);
  for (size_t i = 0; i < header.events.array_entries; ++i) {
    Evidyon::Event::Event _event; // underscore to prevent this looking like a keyword in VC2008...
    EventSerializer::read(&reader, &_event);
    event_manager_.initEvent(i, &_event);
  }

  //   Enchantments
  //==========================================================================
  for (size_t i = 0; i < Evidyon::Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    size_t number_of_type;
    size_t offset = reader.getReadPointOffset();
    GameFile::ClientEnchantmentSerializer::read(
      &reader,
      &PeekArray<Event::EventIndex>(&number_of_type));
    reader.setReadPointOffset(offset);
    Event::EventIndex* event_indices
      = new Event::EventIndex[number_of_type];
    GameFile::ClientEnchantmentSerializer::read(
      &reader,
      &CopiedArray<Event::EventIndex>(
        event_indices,
        number_of_type,
        &number_of_type));
    actor_manager_.initEnchantmentType(Evidyon::Magic::EnchantmentType(i),
                                       event_indices,
                                       number_of_type);
  }

  //   Spells
  //==========================================================================
  assert(reader.getReadPointOffset()
         == header.spells.first_byte_offset);
  spells_ = new Evidyon::Spell::SpellClientDescription[header.spells.array_entries];
  number_of_spells_ = header.spells.array_entries;
  for (size_t i = 0; i < header.spells.array_entries; ++i) {
    ClientSpellSerializer::read(
      &reader,
      &spells_[i]);
  }

  //   Spells
  //==========================================================================
  assert(reader.getReadPointOffset()
          == header.geosids.first_byte_offset);
  geosids_.allocate(header.geosids.array_entries);
  for (size_t i = 0; i < header.geosids.array_entries; ++i) {
    Geosid::GeosidClientDescription description;
    ClientGeosidSerializer::read(
      &reader,
      &description);
    geosids_.assignNext(i, description);
  }


  reader.close();

  // GUI
  //==========================================================================

  { // Init the main font
    if (!window_font_.create(d3d_device_,
                             config_.getSetting("Media","FontName","Tahoma"),
                             config_.getSetting("Media","FontSize",10),
                             true)) {
      return false;
    }
    gui_primary_layer_.setTexture(window_font_.getTextureID(),
                                  window_font_.getTexture());
  }

  // The primary GUI rendering layer draws to the fullscreen viewport
  if (!gui_primary_layer_.create(d3d_device_, 3000, 1000)) return false;

  gui_primary_layer_.setFullscreenViewport(0.0f, 1.0f);
  gui_primary_layer_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX,
                                getImage(Evidyon::Client::CLIENT_IMAGE_GUI));

  { // Set up the mouse cursor
    mouse_position_canvas_.absoluteDepth(0.0f);
    mouse_position_canvas_.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    mouse_cursor_canvas_.setSourceCanvas(&mouse_position_canvas_);
    mouse_cursor_canvas_.absoluteWidth(EvidyonGUIDefs::MOUSE_CURSOR_NORMAL.calculateSize().width);
    mouse_cursor_canvas_.absoluteHeight(EvidyonGUIDefs::MOUSE_CURSOR_NORMAL.calculateSize().height);
    mouse_cursor_canvas_.absoluteDepth(0.0f);
    mouse_cursor_canvas_.alignXLeft(-2);
    mouse_cursor_canvas_.alignYTop(-3);
    mouse_cursor_.setSourceCanvas(&mouse_cursor_canvas_);
    mouse_cursor_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    setMouseCursorNormal();
    gui_primary_layer_.registerElement(&mouse_cursor_);

    SetCursor(NULL);
    ShowCursor(FALSE);
  }

  gui_sounds_.assignSounds(
    &sound_manager_,
    client_data_.sounds[Evidyon::Client::CLIENT_SOUND_BUTTON]); // todo: getSound

  return true;
}




//----[  freeResources  ]------------------------------------------------------
void EvidyonClient::freeResources() {
  geosids_.free();
  SAFE_DELETE_ARRAY(zones_);
  if (maps_) {
    for (size_t i = 0; i < number_of_maps_; ++i) {
      maps_[i].destroy();
    }
    SAFE_DELETE_ARRAY(maps_);
  }
  if (images_) {
    for (size_t i = 0; i < number_of_images_; ++i) {
      SAFE_RELEASE(images_[i]);
    }
    SAFE_DELETE_ARRAY(images_);
  }
  SAFE_DELETE_ARRAY(items_);
  number_of_items_ = 0;
  SAFE_DELETE_ARRAY(spells_);
  number_of_spells_ = 0;

  event_manager_.destroy();
  special_fx_manager_.destroy();
  actor_manager_.destroy();
  map_renderer_.destroy();
  actor_renderer_.destroy();
  sound_manager_.destroy();
  animated_mesh_renderer_.destroy();
  skinned_mesh_renderer_.destroy();
  visual_fx_renderer_.destroy();
  scenery_renderer_.destroy();
  light_manager_.destroy();
  mesh_renderer_.destroy();
  renderstate_manager_.destroy();




  special_fx_manager_.destroy();

  gui_primary_layer_.clearAllTextures();
  gui_primary_layer_.destroy();
  window_font_.destroy();

}

