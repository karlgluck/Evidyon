//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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