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
#include "evidyond3dlightmanager.h"
#include "common/safemacros.h"
#include <math.h> // for fabsf


namespace Evidyon {

//----[  EvidyonD3DLightManager  ]---------------------------------------------
EvidyonD3DLightManager::EvidyonD3DLightManager() {
  d3d_device_ = NULL;
}



//----[  create  ]-------------------------------------------------------------
bool EvidyonD3DLightManager::create(LPDIRECT3DDEVICE9 d3d_device) {
  destroy();
  (d3d_device_ = d3d_device)->AddRef();
  return true;
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonD3DLightManager::destroy() {
  lights_.clear();
  SAFE_RELEASE(d3d_device_)
}

//----[  updateAmbientLight  ]-------------------------------------------------
void EvidyonD3DLightManager::updateAmbientLight(double current_time) {
  BYTE b = (BYTE)fabsf(cosf((current_time)/(60.0*5)) * 255.0);
  if (b > 0xC0) b = 0xC0;
  if (b < 0x75) b = 0x75;
  d3d_device_->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(b,b,b));
}

#define D3DCOLOR_NOON D3DCOLOR_XRGB(60,60,40)
#define D3DCOLOR_DAY  D3DCOLOR_XRGB(30,40,40)

//----[  setDaylightAmbience  ]------------------------------------------------
void EvidyonD3DLightManager::setDaylightAmbience() {
  BYTE r = 30, g = 40, b = 40;
  d3d_device_->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(0,r,g,b));
  //d3d_device_->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0,0,0));
}



//----[  setNightAmbience  ]---------------------------------------------------
void EvidyonD3DLightManager::setNightAmbience() {
  d3d_device_->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0,0,0));
}



//----[  allocateLight  ]------------------------------------------------------
PointLight* EvidyonD3DLightManager::allocateLight() {
  PointLight* light = new PointLight;
  ZeroMemory(light, sizeof(light));
  light->d3d_light.Type = D3DLIGHT_POINT;
  lights_.push_back(light);
  return light;
}



//----[  allocateSmallLight  ]-------------------------------------------------
PointLight* EvidyonD3DLightManager::allocateSmallLight(D3DCOLOR color) {
  PointLight* light = allocateLight();
  //light->d3d_light.Ambient.a = 0.0f;//((color>>24)&0xFF) / 255.0f;
  //light->d3d_light.Ambient.r = ((color>>16)&0xFF) / 255.0f;
  //light->d3d_light.Ambient.g = ((color>> 8)&0xFF) / 255.0f;
  //light->d3d_light.Ambient.b = ((color>> 0)&0xFF) / 255.0f;
  //light->d3d_light.Diffuse.a = 0.0f;
  //light->d3d_light.Diffuse.r = 0.0f;
  //light->d3d_light.Diffuse.g = 0.0f;
  //light->d3d_light.Diffuse.b = 0.0f;
  light->d3d_light.Diffuse.a = 1.0f;//((color>>24)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.r = ((color>>16)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.g = ((color>> 8)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.b = ((color>> 0)&0xFF) / 255.0f;
  light->d3d_light.Ambient.a = 0.0f;
  light->d3d_light.Ambient.r = 0.0f;
  light->d3d_light.Ambient.g = 0.0f;
  light->d3d_light.Ambient.b = 0.0f;
  light->d3d_light.Attenuation0 = 1.0f;
  light->d3d_light.Attenuation1 = 0.01f;
  light->d3d_light.Attenuation2 = 0.005f;
  light->d3d_light.Range = 100.0f;
  return light;
}



//----[  allocateBigLight  ]---------------------------------------------------
PointLight* EvidyonD3DLightManager::allocateBigLight(D3DCOLOR color) {
  PointLight* light = allocateLight();
  light->d3d_light.Diffuse.a = 1.0f;//((color>>24)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.r = ((color>>16)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.g = ((color>> 8)&0xFF) / 255.0f;
  light->d3d_light.Diffuse.b = ((color>> 0)&0xFF) / 255.0f;
  light->d3d_light.Ambient.a = 0.0f;
  light->d3d_light.Ambient.r = 0.0f;
  light->d3d_light.Ambient.g = 0.0f;
  light->d3d_light.Ambient.b = 0.0f;
  light->d3d_light.Attenuation0 = 1.0f;
  light->d3d_light.Attenuation1 = 0.01f;
  light->d3d_light.Attenuation2 = 0.001f;
  light->d3d_light.Range = 1000.0f;
  return light;
}



//----[  freeLight  ]----------------------------------------------------------
void EvidyonD3DLightManager::freeLight(PointLight* light) {
  for (Lights::iterator i = lights_.begin(); i != lights_.end(); ++i) {
    if (light == *i) {
      SAFE_DELETE(light);
      lights_.erase(i);
      break;
    }
  }
}



//----[  beginScene  ]---------------------------------------------------------
void EvidyonD3DLightManager::beginScene(float viewer_x, float viewer_y) {
  // TODO: enable lights based on distance to viewer
  int i = 7, number_of_lights = lights_.size();
  while (i >= number_of_lights) {
    d3d_device_->LightEnable(i, FALSE);
    --i;
  }
  while (i >= 0) {
    PointLight* light = lights_.at(i);
    light->d3d_light.Position.x = light->h;
    light->d3d_light.Position.z = light->v;
    d3d_device_->SetLight(i, &light->d3d_light);
    d3d_device_->LightEnable(i, TRUE);
    --i;
  }

  //D3DLIGHT9 sunlight;
  //sunlight.Type = D3DLIGHT_DIRECTIONAL;
  //BYTE b = 255 * fabsf(sinf(GetTickCount()/10000.0f));
  //D3DCOLOR color = D3DCOLOR_ARGB(255,b,b,b);
  //sunlight.Ambient.a = ((color>>24)&0xFF) / 255.0f;
  //sunlight.Ambient.r = ((color>>16)&0xFF) / 255.0f;
  //sunlight.Ambient.g = ((color>> 8)&0xFF) / 255.0f;
  //sunlight.Ambient.b = ((color>> 0)&0xFF) / 255.0f;
  //sunlight.Diffuse.a = 0.0f;
  //sunlight.Diffuse.r = 0.0f;
  //sunlight.Diffuse.g = 0.0f;
  //sunlight.Diffuse.b = 0.0f;
  //sunlight.Direction.x = 0.0f;//sinf(GetTickCount()/3000.0f);
  //sunlight.Direction.y = -1.0f;//cosf(GetTickCount()/3000.0f);
  //sunlight.Direction.z = 0.0f;
  //sunlight.Range = 1000.0f;

  //d3d_device_->SetLight(7, &sunlight);
  //d3d_device_->LightEnable(7, TRUE);

  // Set up lighting to affect geometry
  //d3d_device_->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
  //d3d_device_->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
}


}