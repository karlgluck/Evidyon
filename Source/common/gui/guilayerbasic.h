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