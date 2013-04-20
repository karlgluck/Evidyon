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
////-----------------------------------------------------------------------------
////                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
////   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
////      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
////                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
////-----------------------------------------------------------------------------
//#ifndef __EVIDYONRESOURCES_H__
//#define __EVIDYONRESOURCES_H__
//
//
//#include <dc/list>
//#include <dc/table>
//#include "shared/image/tools/image.h"
//#include "shared/texture/tools/texture.h"
//#include "shared/evidyond3drenderstatemanager.h"
//
//
//namespace Evidyon {
//
//
//  
////----[  EvidyonResources  ]---------------------------------------------------
//class EvidyonResources : public dc::dcResource<EvidyonResources> {
//public:
//  EvidyonResources();
//  void clear();
//
//  void loadAllImages(LPDIRECT3DDEVICE9 d3d_device,
//                     bool force_reload_all);
//
//  // Fills the render-state manager with the most recent set of
//  // textures/images.
//  void initializeTextures(LPDIRECT3DDEVICE9 d3d_device,
//                          bool force_reload_images,
//                          EvidyonD3DRenderStateManager* renderstate_manager);
//
//public:
//
//  // list accessors (for methods that set references)
//  const dc::dcGenericResource::Array& getImages() { return images_.getMembers(); }
//
//private:
//  dc::dcTable<Image::Tools::Image> images_;
//  dc::dcTable<Texture::Tools::Texture> textures_;
//};
//
//}
//
//#endif