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
#include "guitexture.h"



//-----------------------------------------------------------------------------
GUITexture::GUITexture() {
  d3d_texture_ = NULL;
}


//-----------------------------------------------------------------------------
GUITexture::~GUITexture() {
  release();
}


//-----------------------------------------------------------------------------
void GUITexture::acquire(LPDIRECT3DTEXTURE9 d3d_texture) {
  release();
  if (d3d_texture != NULL)
  (d3d_texture_ = d3d_texture)->AddRef();
}


//-----------------------------------------------------------------------------
void GUITexture::release() {
  if (d3d_texture_) d3d_texture_->Release();
  d3d_texture_ = NULL;
}


//-----------------------------------------------------------------------------
void GUITexture::initStage0Texture(LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetTexture(0, d3d_texture_);
}


//-----------------------------------------------------------------------------
void GUITexture::cleanStage0Texture(LPDIRECT3DDEVICE9 d3d_device) {
  d3d_device->SetTexture(0, NULL);
}


//-----------------------------------------------------------------------------
GUISize GUITexture::getSize() {
  D3DSURFACE_DESC desc;
  d3d_texture_->GetLevelDesc(0, &desc);
  GUISize size = { desc.Width, desc.Height };
  return size;
}


//-----------------------------------------------------------------------------
bool GUITexture::contains(LPDIRECT3DTEXTURE9 d3d_texture) {
  return d3d_texture_ == d3d_texture;
}