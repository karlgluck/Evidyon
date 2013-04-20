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
#include "evidyond3drenderstatemanager.h"
#include <dc/debug>
#include "common/safemacros.h"
#include <d3dx9.h>

namespace Evidyon {


  
//----[  EvidyonD3DRenderStateManager  ]-------------------------------------
EvidyonD3DRenderStateManager::EvidyonD3DRenderStateManager() {
  d3d_device_ = NULL;
  current_stage0_texture_ = NULL;
  current_texture_ = NULL;
  current_rendering_purpose_ = RENDERINGPURPOSE_INVALID;
  fvf_ = 0;
  vb0_ = NULL;
  ib_ = NULL;
}



//----[  create  ]-----------------------------------------------------------
void EvidyonD3DRenderStateManager::create(LPDIRECT3DDEVICE9 d3d_device) {
  destroy();
  if (d3d_device) (d3d_device_ = d3d_device)->AddRef();
}


  
//----[  destroy  ]----------------------------------------------------------
void EvidyonD3DRenderStateManager::destroy() {
  resetState();
  clearTextures();
  SAFE_RELEASE(d3d_device_);
}



//----[  addTexture  ]-------------------------------------------------------
bool EvidyonD3DRenderStateManager::addTexture(
    size_t index,
    const Texture::TextureDescription& texture) {
  CONFIRM(texture_descriptions_.size() == index) else return false;
  texture_descriptions_.push_back(texture);
  return true;
}





//----[  clearTextures  ]------------------------------------------------------
void EvidyonD3DRenderStateManager::clearTextures() {
  for (TextureDescriptions::iterator i = texture_descriptions_.begin();
     i != texture_descriptions_.end(); ++i) {
    SAFE_RELEASE(i->d3d_texture);
  }
  texture_descriptions_.clear();
  SAFE_RELEASE(current_stage0_texture_);
}


//----[  beginScene  ]---------------------------------------------------------
void EvidyonD3DRenderStateManager::beginScene() {
  LPDIRECT3DDEVICE9 d3d_device = d3d_device_;
  if (d3d_device == NULL) return;
  
  //static int v = int(D3DTOP_MODULATEALPHA_ADDCOLOR);
  //static bool down = false;
  //if (GetAsyncKeyState(VK_SPACE)) {
  //  if (!down) {
  //    down = true;
  //    v = (v%26)+1;
  //  }
  //} else {
  //  down = false;
  //}
  //d3d_device->SetTextureStageState(0, D3DTSS_COLOROP,
  //  GetAsyncKeyState(VK_LSHIFT) ? D3DTOP_SELECTARG2 : v);

  d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

  D3DMATERIAL9 mtrl;
  ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
  mtrl.Diffuse.r = 1.0f; mtrl.Ambient.r = 1.0f;
  mtrl.Diffuse.g = 1.0f; mtrl.Ambient.g = 1.0f;
  mtrl.Diffuse.b = 1.0f; mtrl.Ambient.b = 1.0f;
  mtrl.Diffuse.a = 1.0f; mtrl.Ambient.a = 1.0f;
  d3d_device->SetMaterial(&mtrl);
  d3d_device->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
  d3d_device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x08);
  d3d_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

  //d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
  //d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  //d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  //d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
  //d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
  //d3d_device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

  vb0_ = NULL;
  ib_ = NULL;
  fvf_ = 0;
  current_rendering_purpose_ = RENDERINGPURPOSE_INVALID;
  SAFE_RELEASE(current_stage0_texture_);
  current_texture_ = NULL;
}



//----[  setGeometryTexture  ]-------------------------------------------------
void EvidyonD3DRenderStateManager::setGeometryTexture(
    Texture::TextureIndex texture_id,
    bool enable_lighting) {
  if (texture_id < texture_descriptions_.size()) {
    changeCurrentTexture(enable_lighting ? RENDERINGPURPOSE_FVF3DGEOMETRY
                                         : RENDERINGPURPOSE_FVF3DGEOMETRY_UNLIT,
                         &texture_descriptions_.at(texture_id));
  } else {
    changeCurrentTexture(RENDERINGPURPOSE_FVF3DGEOMETRY, NULL);
  }
}



//----[  setParticleTexture  ]-------------------------------------------------
void EvidyonD3DRenderStateManager::setParticleTexture(
    Texture::TextureIndex texture_id) {
  if (texture_id < texture_descriptions_.size()) {
    changeCurrentTexture(RENDERINGPURPOSE_POINTPARTICLES,
                         &texture_descriptions_.at(texture_id));
  } else {
    changeCurrentTexture(RENDERINGPURPOSE_POINTPARTICLES, NULL);
  }
}



//----[  setAnimatedGeometryTexture  ]-----------------------------------------
void EvidyonD3DRenderStateManager::setAnimatedGeometryTexture(
    Texture::TextureIndex texture_id) {
  if (texture_id < texture_descriptions_.size()) {
    changeCurrentTexture(RENDERINGPURPOSE_FVF3DANIMATEDGEOMETRY,
                         &texture_descriptions_.at(texture_id));
  } else {
    changeCurrentTexture(RENDERINGPURPOSE_FVF3DANIMATEDGEOMETRY, NULL);
  }
}


//----[  resetState  ]---------------------------------------------------------
void EvidyonD3DRenderStateManager::resetState() {
  if (d3d_device_) {
    switchToGeometryState(d3d_device_, false);
    setBlendingDisable(d3d_device_);
    changeCurrentTexture(RENDERINGPURPOSE_FVF3DGEOMETRY_UNLIT, NULL);
    d3d_device_->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    d3d_device_->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3d_device_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  } else {
    setStage0Texture(NULL);
  }
}





//----[  setStage0Texture  ]---------------------------------------------------
void EvidyonD3DRenderStateManager::setStage0Texture(LPDIRECT3DTEXTURE9 texture) {
  current_texture_ = NULL;
  current_rendering_purpose_ = RENDERINGPURPOSE_INVALID;
  internalSetStage0Texture(texture);
}






//----[  setGeometryBuffers  ]-------------------------------------------------
bool EvidyonD3DRenderStateManager::setStream0GeometryBuffers(
    LPDIRECT3DVERTEXBUFFER9 vb,
    DWORD fvf,
    size_t vertex_stride,
    LPDIRECT3DINDEXBUFFER9 ib) {
  if (fvf != fvf_) {
    d3d_device_->SetFVF(fvf);
    fvf_ = fvf;
  }
  if (vb != vb0_) {
    d3d_device_->SetStreamSource(0, vb, 0, vertex_stride);
    vb0_ = static_cast<void*>(vb);
  }
  if (ib != ib_) {
    d3d_device_->SetIndices(ib);
    ib_ = static_cast<void*>(ib);
  }
  return true;
}







//----[  setWorldMatrix  ]-----------------------------------------------------
void EvidyonD3DRenderStateManager::setWorldMatrix(const D3DMATRIX* matrix,
                                                  DWORD index) {
  if (matrix == NULL) {
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    d3d_device_->SetTransform(D3DTS_WORLDMATRIX(index), &mat);
  } else {
    d3d_device_->SetTransform(D3DTS_WORLDMATRIX(index), matrix);
  }
}










//----[  internalSetStage0Texture  ]-------------------------------------------
void EvidyonD3DRenderStateManager::internalSetStage0Texture(LPDIRECT3DTEXTURE9 texture) {
  if (current_stage0_texture_ != texture) {
    SAFE_RELEASE(current_stage0_texture_);
    if (d3d_device_) {
      current_stage0_texture_ = texture;
      d3d_device_->SetTexture(0, texture);
      if (texture) texture->AddRef();
    }
  }
}



//----[  changeCurrentTexture  ]-----------------------------------------------
void EvidyonD3DRenderStateManager::changeCurrentTexture(
    RenderingPurpose purpose,
    Texture::TextureDescription* texture) {
  using namespace Evidyon::Texture;

  if (current_texture_ == texture) return;
  LPDIRECT3DDEVICE9 d3d_device = d3d_device_;
  if (d3d_device == NULL) return;

  bool rendering_purpose_changed = current_rendering_purpose_ != purpose;
  if (rendering_purpose_changed) {
    current_rendering_purpose_ = purpose;
    switch (purpose) {
      default: DEBUG_INFO("invalid rendering purpose; selecting geometry as default");
      case RENDERINGPURPOSE_FVF3DGEOMETRY:
        switchToGeometryState(d3d_device, true); break;
      case RENDERINGPURPOSE_FVF3DGEOMETRY_UNLIT:
        switchToGeometryState(d3d_device, false); break;
      case RENDERINGPURPOSE_POINTPARTICLES:
        switchToParticleState(d3d_device); break;
      case RENDERINGPURPOSE_FVF3DANIMATEDGEOMETRY:
        switchToAnimatedGeometryState(d3d_device); break;
    }
  }

  // If this texture is invalid, we don't have to do anything else.
  if (texture == NULL) { internalSetStage0Texture(NULL); return; }

  // Switch to the new texture
  internalSetStage0Texture(texture->d3d_texture);

  switch (texture->blend_type) {
    case TextureDescription::DISABLE_BLENDING: setBlendingDisable(d3d_device); break;
    case TextureDescription::ALPHATESTONLY: setBlendingAlphaTest(d3d_device); break;
    case TextureDescription::ALPHABLEND: setBlendingAlphaBlend(d3d_device); break;
    case TextureDescription::LIGHTEN: setBlendingLighten(d3d_device); break;
    case TextureDescription::DARKEN: setBlendingDarken(d3d_device); break;
  }


  TextureDescription::AnimationType texture_animation_type = texture->animation_type;
  if ((texture_animation_type == TextureDescription::STATIC) &&
      (!texture->flip_horizontal && !texture->flip_vertical)) {
    // turn off texture transformations
    d3d_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
  } else {
    D3DXMATRIXA16 mat;
    D3DXMatrixIdentity(&mat);
    double time = GetTickCount() / 1000.0;  // TODO: better timing :D
    switch (texture_animation_type) {
      case TextureDescription::FRAME_SEQUENCE: {
        int frames_wide = texture->animation.frame_sequence.frames_wide;
        int frames_high = texture->animation.frame_sequence.frames_wide;
        int total_frames = frames_wide * frames_high;

        // Calculate the current frame
        int frame = ((int)floorf(time * texture->animation.frame_sequence.speed)) % total_frames;

        // Generate the projection matrix to put this frame in the [0,0] [1,1] box.  This requires
        // a scaling and translation.
        mat._11 =  1.0f / frames_wide;
        mat._22 =  1.0f / frames_high;
        mat._31 = -1.0f / frames_wide * (frame % frames_wide);
        mat._32 = -1.0f / frames_high * ((int)(frame / frames_wide));
      } break;

      case TextureDescription::CIRCLING: {
        float radius = texture->animation.circling.radius;
        float angle = time * texture->animation.circling.speed;
        mat._31 = radius * cosf(angle);
        mat._32 = radius * sinf(angle);
      } break;

      case TextureDescription::SLIDING: {
        // Build the translation matrix for moving the texture in a line
        const float SCALING = 10.0f;
        mat._31 = fmodf(texture->animation.sliding.dx * time / SCALING, 1.0f);
        mat._32 = fmodf(texture->animation.sliding.dy * time / SCALING, 1.0f);
      } break;
    }

    // Mirror the texture if necessary
    if (texture->flip_horizontal) mat._11 *= -1.0f;
    if (texture->flip_vertical)   mat._22 *= -1.0f;

    // Initialize the texture transformation
    d3d_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    d3d_device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + 0), &mat);
  }

  // This texture has now been initialized on the device
  current_texture_ = texture;
}





//----[  switchToGeometryState  ]----------------------------------------------
void EvidyonD3DRenderStateManager::switchToGeometryState(
    LPDIRECT3DDEVICE9 d3d_device,
    bool enable_lighting) {
  d3d_device->SetRenderState(D3DRS_LIGHTING, enable_lighting ? TRUE : FALSE);
  d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  d3d_device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
  if (enable_lighting) {
    d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
    d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    d3d_device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
  } else {
    d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    d3d_device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
  }
}



//----[  switchToParticleState  ]----------------------------------------------
void EvidyonD3DRenderStateManager::switchToParticleState(
    LPDIRECT3DDEVICE9 d3d_device) {
#define FtoDW(f) (*(reinterpret_cast<DWORD*>(&(dummy_float=f))))
  float dummy_float;
  d3d_device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);  // turn on point sprites
  d3d_device->SetRenderState(D3DRS_POINTSCALEENABLE,  TRUE);  // allow sprites to be scaled with distance
  d3d_device->SetRenderState(D3DRS_POINTSIZE,   FtoDW(0.1f)); // float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
  d3d_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.0f)); // float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
  d3d_device->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(128.0f));
  d3d_device->SetRenderState(D3DRS_POINTSCALE_A,  FtoDW(1.0f));  // Default 1.0
  d3d_device->SetRenderState(D3DRS_POINTSCALE_B,  FtoDW(0.0f));  // Default 0.0
  d3d_device->SetRenderState(D3DRS_POINTSCALE_C,  FtoDW(0.0f));  // Default 0.0
#undef FtoDW
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
  d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  d3d_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
  d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE); // particles shouldn't be fogged
  d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
  d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
  d3d_device->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}





//----[  switchToAnimatedGeometryState  ]--------------------------------------
void EvidyonD3DRenderStateManager::switchToAnimatedGeometryState(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_LIGHTING, TRUE);
  d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  d3d_device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  d3d_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS);
  d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
  d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
  d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
  d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
  d3d_device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
}


//----[  setBlendingDisable  ]-------------------------------------------------
void EvidyonD3DRenderStateManager::setBlendingDisable(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
  d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
  d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}



//----[  setBlendingAlphaTest  ]-----------------------------------------------
void EvidyonD3DRenderStateManager::setBlendingAlphaTest(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
}



//----[  setBlendingAlphaBlend  ]----------------------------------------------
void EvidyonD3DRenderStateManager::setBlendingAlphaBlend(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
}



//----[  setBlendingLighten  ]-------------------------------------------------
void EvidyonD3DRenderStateManager::setBlendingLighten(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
  d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}



//----[  setBlendingDarken  ]--------------------------------------------------
void EvidyonD3DRenderStateManager::setBlendingDarken(
    LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
  d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
  d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
  d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  d3d_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}


}

