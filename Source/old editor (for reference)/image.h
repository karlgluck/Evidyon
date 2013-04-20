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
#ifndef __IMAGE_H__
#define __IMAGE_H__
#pragma once


#define D3DTEX_MANAGED      0x01 /* create in managed pool */
#define D3DTEX_MIPMAP       0x02 /* generate a mipmap chain */
#define D3DTEX_SYSTEMMEM    0x04 /* create in system memory; overrides D3DTEX_MANAGED */

#include <d3d9.h>

bool SegmentImage(LPDIRECT3DTEXTURE9 texture,
                  int subimage_width,
                  int subimage_height,
                  int rows,
                  int columns,
                  int selected_index,
                  LPDIRECT3DTEXTURE9* segment);

/**
* An image is an abstract representation of something that can produce an IDirect3DTexture9
* object filled with color information.  Right now, only D3DXImage, which loads various image
* formats from the disk, exists.  In the future, derived classes could create images using
* formulas or create new images using mixing techniques.
*/
class Image : public dc::dcGenericResource
{
public:

  /**
   * Contains raw information needed to display a preview of this resource
   */
  struct Preview
  {
      /// The resource that created this preview
      Image* image;

      /// The texture for the image
      LPDIRECT3DTEXTURE9 d3dTexture;
  };

public:

  /**
   * Initializes this resource
   */
  Image();

  /**
   * Generates a preview for this image
   * @param pd3dDevice Device onto which to load the D3D texture
   * @param preview Destination for the preview data
   * @return Whether or not the preview could be loaded
   */
  bool loadPreview(LPDIRECT3DDEVICE9 pd3dDevice, Preview* preview);


public:

  /**
   * Loads the image denoted by this class using the specified device
   * @param pd3dDevice Device onto which to load the texture
   * @param options Combination of one or more D3DTEX_* flags detailing the texture format
   * @param ppd3dTexture Return location for texture interface
   * @return Result code
   */
  virtual bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const = 0;


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();
};




#endif