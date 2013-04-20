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
#ifndef __SHARED_EVIDYOND3DRENDERSTATEMANAGER_H__
#define __SHARED_EVIDYOND3DRENDERSTATEMANAGER_H__
#pragma once




#include <d3d9.h>
#include "shared/texture/textureindex.h"
#include "shared/texture/texturedescription.h"
#include <vector>

namespace Evidyon {



  
//----[  EvidyonD3DRenderStateManager  ]---------------------------------------
class EvidyonD3DRenderStateManager {
public:

  enum RenderingPurpose {
    RENDERINGPURPOSE_FVF3DGEOMETRY,
    RENDERINGPURPOSE_FVF3DGEOMETRY_UNLIT,
    RENDERINGPURPOSE_POINTPARTICLES,
    RENDERINGPURPOSE_FVF3DANIMATEDGEOMETRY,
    RENDERINGPURPOSE_INVALID,
  };

public:
  EvidyonD3DRenderStateManager();
  
  // Initializes the rendering manager.  Textures should be added using
  // the addTexture method.
  void create(LPDIRECT3DDEVICE9 d3d_device);

  // Resets the manager and erases all textures
  void destroy();

  // The 'index' parameter is just used to make sure that the texture being
  // added is in the right location.  This method should be called linearly
  // in ascending index order to fill all of the resources.  The texture
  // included in the parameter should be reference-incremented.
  bool addTexture(size_t index, const Texture::TextureDescription& texture);
  void clearTextures();

  // Call this method to initialize the rendering state when starting a scene
  void beginScene();

  // Switches to the given texture
  void setGeometryTexture(Texture::TextureIndex texture_id, bool enable_lighting);

  // Switches to the texture for the given particle type
  void setParticleTexture(Texture::TextureIndex texture_id);

  // Changes to rendering index-blended geometry.
  void setAnimatedGeometryTexture(Texture::TextureIndex texture_id);

  // Clears the current state and resets the device to a state where
  // it can render standard lit 3D FVF geometry using a Z-buffer.
  void resetState();

  // Sets the given texture into the device, resetting the current texture
  // and internal state-monitoring variables.
  void setStage0Texture(LPDIRECT3DTEXTURE9 texture);

  // Used to iterate through the textures to do rendering
  inline size_t numberOfTextures() const { return texture_descriptions_.size(); }

  // Initializes the buffers in the device with SetStreamSource, SetFVF, and
  // SetIndices calls.
  bool setStream0GeometryBuffers(LPDIRECT3DVERTEXBUFFER9 vb,
                                 DWORD fvf,
                                 size_t vertex_stride,
                                 LPDIRECT3DINDEXBUFFER9 ib);

  // Sets the world matrix on the device.  Since this class is passed around so
  // much anyway, it makes sense to provide this device interface instead of
  // requiring classes to keep their own copies of the device just to set the
  // world matrix.
  // Setting index=0 is equivalent to D3DTS_WORLD (the primary world matrix)
  void setWorldMatrix(const D3DMATRIX* matrix, DWORD index);


private:

  // Switches to the given texture
  void changeCurrentTexture(RenderingPurpose purpose,
                            Texture::TextureDescription* texture);

  // Sets the given texture into the device without invalidating the render state
  void internalSetStage0Texture(LPDIRECT3DTEXTURE9 texture);

  // Helper methods to initialize device state
  static void switchToGeometryState(LPDIRECT3DDEVICE9 d3d_device,
                                    bool enable_lighting);
  static void switchToParticleState(LPDIRECT3DDEVICE9 d3d_device);
  static void switchToAnimatedGeometryState(LPDIRECT3DDEVICE9 d3d_device);
  static void setBlendingDisable(LPDIRECT3DDEVICE9 d3d_device);
  static void setBlendingAlphaTest(LPDIRECT3DDEVICE9 d3d_device);
  static void setBlendingAlphaBlend(LPDIRECT3DDEVICE9 d3d_device);
  static void setBlendingLighten(LPDIRECT3DDEVICE9 d3d_device);
  static void setBlendingDarken(LPDIRECT3DDEVICE9 d3d_device);

private:
  LPDIRECT3DDEVICE9 d3d_device_;
  LPDIRECT3DTEXTURE9 current_stage0_texture_;
  Texture::TextureDescription* current_texture_;
  RenderingPurpose current_rendering_purpose_;
  typedef std::vector<Texture::TextureDescription> TextureDescriptions;
  TextureDescriptions texture_descriptions_;

private:
  // Handles rendering geometry buffer state
  DWORD fvf_;
  void* vb0_;
  void* ib_;
};

}


#endif
