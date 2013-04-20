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