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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once

#include "stdafx.h"
#include "image.h"
#include <dc/table>
#include <dc/boolean>


/**
* A texture references an image and gives it a few interesting modifications (motion, for ex.)
*/
class Texture : public dc::dcGenericResource
{
public:

  /**
   * Contains raw information needed to display a preview of this resource
   */
  struct Preview
  {
      /// The resource that created this preview
      Texture* texture;

      /// The image with which this texture is associated
      Image::Preview* imagePreview;

      /// Whether to enable alpha-testing
      bool enableAlphaTesting;

      /// Whether to enable alpha-blending
      bool enableAlphaBlending;
  };


public:

  /**
   * Initializes this class
   */
  Texture();

  /**
   * Sets up the preview for this texture.  This method will output an error if the image
   * that this texture is associated with could not be found or its index in the image preview
   * array didn't match its index in the image table.  However, these conditions do not cause
   * the method to fail; instead, the imagePreview member will be 'null'.
   * @param imagePreviewArray The array of image previews previously loaded.  This should be a
   *                          direct, in-order loading of the root image table.
   * @param imagePreviews How many images exist in the image preview array
   * @param preview The destination preview element
   * @return Whether or not the preview could be initialized
   */
  bool loadPreview(Image::Preview* imagePreviewArray, size_t imagePreviews, Preview* preview);

  /**
   * Accessor for this class's reference
   */
  dc::dcTable<Image>::Reference* getImageRef() const { return (dcTable<Image>::Reference*)&myImageRef; }

  bool getEnableAlphaTestingValue() const { return myEnableAlphaTestingFlag.getValue(); }
  bool getEnableAlphaBlendingValue() const { return myEnableAlphaBlendingFlag.getValue(); }

  dc::dcBoolean* getFlipHorizontal() { return &myFlipHorizontal; }
  dc::dcBoolean* getFlipVertical() { return &myFlipVertical; }

  bool getFlipHorizontalValue() const { return myFlipHorizontal.getValue(); }
  bool getFlipVerticalValue() const { return myFlipVertical.getValue(); }

  dc::dcBoolean* getEnableAlphaTestingFlag() { return &myEnableAlphaTestingFlag; }
  dc::dcBoolean* getEnableAlphaBlendingFlag() { return &myEnableAlphaBlendingFlag; }

  /**
   * Sorts textures into correct rendering order
   */
  int compareTo(const dc::dcGenericResource* other) const;


public:

  /**
   * Sets up the texture stage indicated on the provided device for rendering, such that if
   * the D3D texture from the referenced image is set up on that stage, then polygons will
   * render with this texture's configuration state.
   * The default implementation sets the alpha testing/blending flags
   */
  virtual HRESULT configureTextureStageState(LPDIRECT3DDEVICE9 pd3dDevice, DWORD stage) = 0;


protected:

  /**
   * This method applies the D3DRS_ALPHABLENDENABLE and D3DRS_ALPHATESTENABLE states to the device,
   * as well as updating the texture transformation matrix for the flip-flags.
   * @param d3dDevice The device to change
   * @param stage The texture stage to affect
   * @param textureTransform The transformation to update
   * @return Whether or not the texture transform matrix was changed
   */
  bool configureBaseState(LPDIRECT3DDEVICE9 d3dDevice, DWORD stage, D3DMATRIX* textureTransform);


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  /// The source image that this texture refers to
  dc::dcTable<Image>::Reference myImageRef;

  /// Whether or not to flip this texture horizontally
  dc::dcBoolean myFlipHorizontal;

  /// Whether or not to flip this texture vertically
  dc::dcBoolean myFlipVertical;

  /// Whether or not to alpha test this texture
  dc::dcBoolean myEnableAlphaTestingFlag;

  /// Whether or not to blend this texture
  dc::dcBoolean myEnableAlphaBlendingFlag;
};




#endif