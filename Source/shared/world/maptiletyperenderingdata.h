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
#ifndef __MAPTILETYPERENDERINGDATA_H__
#define __MAPTILETYPERENDERINGDATA_H__
#pragma once



#include "maptile_data.h"

namespace Evidyon {
namespace World {

//----[  MapTileTypeRenderingData  ]-------------------------------------------
// This class is employed by the main map renderer to assist in rendering each
// unique type of element that can be present on a given map tile.  These
// elements are generated on-the-fly and change frequently with the map, so
// this class assists in optimizing them for rendering.
template <typename DataType> class MapTileTypeRenderingData {
public:

  //----[  MapTileTypeRenderingData  ]-----------------------------------------
  MapTileTypeRenderingData(const MapTile_Data* data) {
    for (int i = 0; i < CACHED_TILES; ++i) {
      rendering_data_[i].data = &data[i];
    }
    reset();
  }

  //----[  getRenderingData  ]-------------------------------------------------
  // Obtains the internal rendering data buffer so that its pointers can be
  // properly initialized.  This method should be called during the constructor
  // of the owner class, since the actual pointer reference should never need
  // to be changed--just the data in the buffers should be modified.
  DataType* getRenderingData() {
    return rendering_data_;
  }

  //----[  sortAndReset  ]-----------------------------------------------------
  void sortAndReset() {

    // Sort the rendering data.  The method defined in the data type should
    // sort the pointers first by comparing visibility (putting invisible
    // entries at the end) then by ascending texture index.
    qsort(rendering_data_,
          CACHED_TILES,
          sizeof(DataType),
          DataType::Sorter);

    // Count the number of entries in the buffer that both have a valid texture
    // and are not invisible.
    DataType* iterator = rendering_data_;
    const DataType* end = iterator + CACHED_TILES;
    while (iterator != end &&
           iterator->texture() != Evidyon::Texture::INVALID_TEXTURE_INDEX &&
           iterator->data->visibility != VISIBILITY_INVISIBLE) {
      ++iterator;
    }
    number_of_renderable_entries_ = iterator - rendering_data_;

    // Reset the rendering state
    reset();
  }

  //----[  reset  ]------------------------------------------------------------
  // Resets the rendering state to the start of the buffer
  void reset() {
    next_texture_index_ = number_of_renderable_entries_ > 0 ?
        rendering_data_[0].texture() : Evidyon::Texture::INVALID_TEXTURE_INDEX;
    next_renderable_entry_ = 0;
  }

  //----[  next  ]-------------------------------------------------------------
  // Gets the next entry that belongs to the texture index, or returns 'false'
  // if no more visible entries with that texture exist.  What to do with the
  // entry is entirely up to the owner class.
  bool next(Texture::TextureIndex texture_index, DataType** entry) {
    if (next_texture_index_ != texture_index) return false;
    size_t next_renderable_entry = next_renderable_entry_;
    *entry = &rendering_data_[next_renderable_entry];
    ++next_renderable_entry;
    next_texture_index_
      = next_renderable_entry < number_of_renderable_entries_ ?
          rendering_data_[next_renderable_entry].texture()
        :
          Texture::INVALID_TEXTURE_INDEX;
    next_renderable_entry_ = next_renderable_entry;
    return true;
  }

private:

  // Whatever kind of rendering data this class holds.  The pointers in the
  // elements here must be initialized manually by the owner class.
  DataType rendering_data_[CACHED_TILES];

  // Information used by next() during rendering.
  Evidyon::Texture::TextureIndex next_texture_index_;
  size_t next_renderable_entry_;

  // Stores the number of renderable entries so that entries that are marked
  // as invisible but have a valid texture entry are not rendered.
  size_t number_of_renderable_entries_;
};

}
}


#endif