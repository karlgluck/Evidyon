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
#ifndef __GUILAYERBASIC_H__
#define __GUILAYERBASIC_H__
#pragma once



/**
 * Implements a GUILayer using a straightforward index-to-texture approach.
 */
class GUILayerBasic : public GUILayer {
  typedef std::map<int, GUITexture*> TextureMap;

public:
  GUILayerBasic();
  ~GUILayerBasic();

  virtual GUITexture* getTexture(int id);

  // setting a NULL texture is allowed and will behave as if that id simply wasn't mapped
  void setTexture(int id, LPDIRECT3DTEXTURE9 d3d_texture);
  void clearAllTextures();

  // returns whether or not a texture with the given ID was removed
  bool removeTexture(int id);

  // returns the number of textures removed.  will not erase anything if the parameter is NULL.
  int removeTexture(LPDIRECT3DTEXTURE9 d3d_texture);

private:
  TextureMap textures_;
};


#endif