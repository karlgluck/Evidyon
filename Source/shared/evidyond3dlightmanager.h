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
#ifndef __SHARED_EVIDYOND3DLIGHTMANAGER_H__
#define __SHARED_EVIDYOND3DLIGHTMANAGER_H__
#pragma once





#include <d3d9.h>
#include <vector>

namespace Evidyon {



  
//----[  PointLight  ]---------------------------------------------------------
struct PointLight {
  float h, v; // horizontal/vertical map coordinate

  // Light set in the device; x/z are automatically generated, y defaults
  // to zero.  Change the ambient color to modify the color of this light.
  // Do not change the light type.  The size (attenuation) should measured
  // against the post-transformed (i.e. view-space) vertices.
  D3DLIGHT9 d3d_light;
};

  
//----[  EvidyonD3DLightManager  ]---------------------------------------------
class EvidyonD3DLightManager {
public:
  EvidyonD3DLightManager();

  // Initializes this object.  No lights can be allocated unless the device
  // has been initialized correctly.
  bool create(LPDIRECT3DDEVICE9 d3d_device);
  void destroy();

  // Changes the ambient lighting state based on what time of day it is
  void updateAmbientLight(double current_time);
  void setDaylightAmbience();
  void setNightAmbience();

  // Obtains a light.  The most recently allocated light is always visible;
  // however, only 8 lights will be displayed at a time.
  PointLight* allocateLight();
  PointLight* allocateSmallLight(D3DCOLOR color);
  PointLight* allocateBigLight(D3DCOLOR color);
  void freeLight(PointLight* light);

  // Updates, sets and enables 8 lights.  This also sets the diffuse/
  // ambient material sources.
  void beginScene(float viewer_x, float viewer_y);


private:
  LPDIRECT3DDEVICE9 d3d_device_;
  typedef std::vector<PointLight*> Lights;
  Lights lights_;
};


}



#endif