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
#ifndef __GUITEXTURE_H__
#define __GUITEXTURE_H__
#pragma once



#include <d3d9.h>
#include "guisize.h"




//----[  GUITexture  ]---------------------------------------------------------
class GUITexture {
public:
  GUITexture();
  ~GUITexture();
  void acquire(LPDIRECT3DTEXTURE9 d3d_texture);
  void release();
  void initStage0Texture(LPDIRECT3DDEVICE9 d3d_device);
  void cleanStage0Texture(LPDIRECT3DDEVICE9 d3d_device);
  GUISize getSize();
  bool contains(LPDIRECT3DTEXTURE9 d3d_texture);

private:
  LPDIRECT3DTEXTURE9 d3d_texture_;
};



#endif