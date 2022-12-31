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