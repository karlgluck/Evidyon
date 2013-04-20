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
#ifndef __VISUALFXPARTICLE_H__
#define __VISUALFXPARTICLE_H__
#pragma once



#ifndef D3DCOLOR_DEFINED
typedef unsigned long D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

namespace Evidyon {
namespace VisualFX {

//----[  VisualFXParticle  ]---------------------------------------------------
struct VisualFXParticle {
  float x, y, z;
  float point_size;
  D3DCOLOR color;
};

#define D3DFVF_EVIDYON_VISUALFXPARTICLE (D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_DIFFUSE)

}
}


#endif