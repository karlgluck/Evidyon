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
#ifndef __COLORKEY_H__
#define __COLORKEY_H__
#pragma once


/**
 * Stores an RGB color key, which is used by the Image class and its derivatives to make a color
 * in the file being loaded transparent.
 */
class ColorKey : public dc::dcResource<ColorKey>
{
public:

  /**
   * Sets up this color key with the provided name
   */
  ColorKey();

  /**
   * Determines whether or not this color key is enabled
   */
  bool isEnabled() const;

  /**
   * Obtains the COLORREF-formatted color of this color key
   */
  COLORREF getRGBColor() const;

  /**
   * Gets the D3DCOLOR_XRGB-formatted color of this color key
   */
  D3DCOLOR getD3DColor() const;

  /**
   * Sets the color of this colorkey by a COLORREF value
   */
  void setRGBColor(COLORREF color);

  /**
   * Enables or disables this color key
   */
  void enable(bool en);


public:

  /**
   * Gets the name of this class (needed for DC library)
   */
  static std::string staticTypeName();


protected:

  /// Whether or not the color key is enabled
  dc::dcBoolean myEnableFlag;

  /// The color of this color key
  dcxVector3<dcByte> myColor;
};




#endif