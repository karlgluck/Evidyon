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
#ifndef __D3DXIMAGE_H__
#define __D3DXIMAGE_H__
#pragma once


                  

/**
* A D3DXImage can load most common image-file formats from the file system.
*/
class D3DXImage : public dc::dcResource<D3DXImage,Image>
{
public:

  /**
   * Initializes this resource
   */
  D3DXImage();


public:

  /**
   * Loads the image denoted by this class using the specified device.  A device pointer
   * is made a parameter so that even when this class is not being edited, the method can
   * still be used to load the texture.
   *   @param pd3dDevice Device onto which to load the texture
   *   @param options Options used when creating the destination texture
   *   @param ppd3dTexture Return location for texture interface
   *   @return Result code
   */
  bool obtainD3DTexture(LPDIRECT3DDEVICE9 pd3dDevice, DWORD options, LPDIRECT3DTEXTURE9* ppd3dTexture) const;

  /**
   * Gets the source file name for this image
   * @return Pointer to the internal file name object
   */
  dc::dcFileName* getFileName();

  /**
   * Gets the color key object this image uses
   * @return Pointer to the internal color key object
   */
  ColorKey* getColorKey();

  /**
   * Gets the integer representing the number of sub-image columns
   */
  dc::dcInteger* getSubImageColumns();

  /**
   * Gets the integer representing the number of sub-image rows
   */
  dc::dcInteger* getSubImageRows();

  /**
   * Gets the integer for which sub-image is selected
   */
  dc::dcInteger* getSubImage();

  /**
   * Sets up the sub-image for this image.  This makes the class only obtain a piece of a texture.
   * @param rows How many rows of sub-images
   * @param columns How many columns of sub-images
   * @param selection Which image to select (numbered starting at zero and in LTR, TTB reading order)
   */
  void setSubImage(int rows, int columns, int selection);


public:

  /**
   * Gets the string name of this resource type
   * @return Class name
   */
  static std::string staticTypeName();


protected:

  /// The file from which to load the image
  dc::dcFileName mySourceFile;

  /// The image's color-key information
  ColorKey myColorKey;

  /// The number of columns of sub-images in the file
  dc::dcInteger mySubImageColumns;

  /// The number of rows of sub-images in the file
  dc::dcInteger mySubImageRows;

  /// Which of the sub-images to designate as this image
  dc::dcInteger mySubImage;


protected:

  /// This class implements the Image interface
  static dc::dcClass<D3DXImage>::Implements<Image> implementsImage;
};



#endif