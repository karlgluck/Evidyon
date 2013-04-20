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
#ifndef __MAPMASK_H__
#define __MAPMASK_H__
#pragma once



#include <set>
#include <d3d9.h>
#include <dc/filename>
#include <dc/list>
#include <dcx/d3dcolor>
#include "mapmaskcolordata.h"

namespace Evidyon {
namespace World {
namespace Tools {
struct MapCompilationContext;
}
}
}

namespace Evidyon {
namespace World {
namespace Tools {

//----[  MapMask  ]------------------------------------------------------------
// A map mask is a source image that is used to make it easy to paint features
// onto large areas of the map.
class MapMask : public dc::dcResource<MapMask> {
public:
  static std::string staticTypeName() { return "MapMask"; }
  typedef std::set<D3DCOLOR> Colors;

private:
  // // these methods were pulled from MapColorMask and adopted here.  In the
  // // future these should be integrated completely and removed, but for now
  // // I just want to get stuff done.
  bool GetMapMaskInfo(const char* file_name,
                      unsigned int* width,
                      unsigned int* height);
  // When the map mask is loaded, the alpha channel is set to 255
  bool LoadMapMaskData(LPDIRECT3DDEVICE9 d3d_device,
                       const char* file_name,
                       unsigned int width,
                       unsigned int height,
                       D3DCOLOR* data_buffer);
  bool AcquireMapMask(LPDIRECT3DDEVICE9 d3d_device,
                      const char* file_name,
                      unsigned int* width_out,
                      unsigned int* height_out,
                      D3DCOLOR** data_buffer);

public:
  MapMask();
  ~MapMask();

  // Frees all allocated memory
  void destroy();

  // Updates the map mask's in-memory format.  If force_reload is 'true', the
  // map will scan the file on disk again even if an in-memory copy already
  // exists.  Otherwise, if the map has already been loaded, this method just
  // returns 'true' without doing any work.
  bool load(LPDIRECT3DDEVICE9 d3d_device, bool force_reload);

  // Writes the map mask to disk, committing any changes to it.
  void saveChangesToMask(LPDIRECT3DDEVICE9 d3d_device);

  // Called when the mask is loaded to update the colors in the mask.  Any
  // color in color_data_ that doesn't exist anymore is removed; any new
  // ones are added with a default name.
  void updateColorData();

  //// Returns the list of all color names in this mask
  //const dc::dcList<dcx::dcxD3DColor>& getColorNames();

  // Returns the color structure for the given color, or NULL if the color
  // doesn't exist in the list.
  const MapMaskColorData* colorData(D3DCOLOR color) const;

  // Returns the first set of coordinates at which the mask contains the given
  // color.  After calling this method, iterate through the rest of the
  // locations with getNextLocation.  If this method returns 'false', the map
  // does not contain the given color.
  // The color must have its alpha channel set to 0xFF, so use D3DCOLOR_XRGB
  // to generate it.
  bool getFirstLocation(D3DCOLOR color,
                        int* x,
                        int* y);

  // Used to iterate through the map locations containing the given color.
  // This method returns 'false' if the color can no longer be found.
  bool getNextLocation(D3DCOLOR color,
                       int last_x,
                       int last_y,
                       int* x,
                       int* y);

  // Acts like getFirstLocation, but is optimized to only return matches that
  // are within the provided rect area.
  bool getFirstLocationInArea(D3DCOLOR color,
                              const RECT* area,
                              int* x,
                              int* y);

  // Acts like getNextLocation, but is optimized for the case that only matches
  // in a given area are needed.
  bool getNextLocationInArea(D3DCOLOR color,
                             int last_x,
                             int last_y,
                             const RECT* area,
                             int* x,
                             int* y);

  // If either input parameter is less than the corresponding internal value,
  // this method sets that parameter to be equal to the internal one.  This is
  // used when creating the map context to be sure it can hold all of the
  // map layer data.
  void expandDimensionsToContainMask(unsigned int* width,
                                     unsigned int* height);

  // Changes the source file for this map and updates the contents (including
  // the colors) from disk.
  bool setSourceFile(const char* source_file,
                     LPDIRECT3DDEVICE9 d3d_device);

  // Gets a texture for the source file.  If the texture couldn't be loaded,
  // this method returns false.  The texture is loaded into the managed pool
  // pool and it is safe to read data from it.
  // This method is intended to be used in a mask color-selector dialog.
  bool loadSourceFileTexture(LPDIRECT3DDEVICE9 d3d_device,
                             LPDIRECT3DTEXTURE9* d3d_texture);

  // Writes the given color to the mask.  This method has no effect if the
  // mask hasn't been loaded into memory or the x/y coordinates are outside
  // of the valid area of this mask.  Returns the color that was previously
  // at the location.
  D3DCOLOR paint(int x, int y, D3DCOLOR color);

  // Sorts map masks by their priority
  int compareTo(const dc::dcGenericResource* other) const;

  // Changes the priority of this mask
  void setPriority(int priority);

  // Sets color-specific data into the context
  void compile(MapCompilationContext* compilation_context);
  void compile(MapCompilationContext* compilation_context,
               const RECT* area);

private:

  // The file on which this mask is based
  dc::dcFileName source_file_;

  // Allows each color in the mask to be named so that they are easier to
  // manage.
  dc::dcList<MapMaskColorData> color_data_;

  // This mask's priority level; used to sort the masks.
  dc::dcInteger priority_;


// Active data used when the mask has been loaded.
private:

  // Size of the map mask
  unsigned int width_, height_;

  // All of the colors that are present in the map mask
  Colors colors_;
  
  // Array of width_ x height_ colors representing the mask.
  D3DCOLOR* map_mask_;

  // Whether or not the mask has changed since it was loaded.  This flag is set
  // when paint() is called, and reset on save()
  bool modified_;
};

}
}
}

#endif