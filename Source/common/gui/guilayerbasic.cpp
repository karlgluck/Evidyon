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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guitexture.h"
#include "guilayer.h"
#include "guilayerbasic.h"



//-----------------------------------------------------------------------------
GUILayerBasic::GUILayerBasic() {
}


//-----------------------------------------------------------------------------
GUILayerBasic::~GUILayerBasic() {
  clearAllTextures();
}


//-----------------------------------------------------------------------------
GUITexture* GUILayerBasic::getTexture(int id) {
  TextureMap::iterator i = textures_.find(id);
  if (i == textures_.end()) return NULL;
  return i->second;
}


//-----------------------------------------------------------------------------
void GUILayerBasic::setTexture(int id, LPDIRECT3DTEXTURE9 d3d_texture) {
  if (d3d_texture != NULL) {
    GUITexture* gui_texture = getTexture(id);
    if (!gui_texture) {
      gui_texture = new GUITexture();
      textures_.insert(TextureMap::value_type(id, gui_texture));
    }
    gui_texture->acquire(d3d_texture);
  } else {
    removeTexture(id);
    textures_.insert(TextureMap::value_type(id, NULL));
  }
}


//-----------------------------------------------------------------------------
void GUILayerBasic::clearAllTextures() {
  for (TextureMap::iterator i = textures_.begin(); i != textures_.end(); ++i) {
    GUITexture* gui_texture = i->second;
    if (gui_texture) {
      gui_texture->release();
      delete gui_texture;
    }
  }
  textures_.clear();
}

//-----------------------------------------------------------------------------
bool GUILayerBasic::removeTexture(int id) {
  TextureMap::iterator i = textures_.find(id);
  if (i == textures_.end()) return false;
  GUITexture* gui_texture = i->second;
  if (gui_texture) {
    gui_texture->release();
    delete gui_texture;
  }
  textures_.erase(i);
  return true;
}

//-----------------------------------------------------------------------------
int GUILayerBasic::removeTexture(LPDIRECT3DTEXTURE9 d3d_texture) {
  TextureMap::iterator next;
  int number_erased = 0;
  if (d3d_texture == NULL) return 0; // don't erase NULL textures this way!

  for (TextureMap::iterator i = textures_.begin(); i != textures_.end(); i = next) {
    next = i; ++next;
    GUITexture* gui_texture = i->second;
    if (gui_texture == NULL) continue;
    if (gui_texture->contains(d3d_texture)) {
      gui_texture->release();
      delete gui_texture;
      textures_.erase(i);
      ++number_erased;
    }
  }

  return number_erased;
}
