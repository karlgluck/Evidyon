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
#ifndef __EVIDYONGUIMODULE_BAZAAR_CREATELISTINGPROMPT_H__
#define __EVIDYONGUIMODULE_BAZAAR_CREATELISTINGPROMPT_H__
#pragma once

#include "common/gui/guimodule.h"
#include "evidyonguititledpanel.h"
#include "evidyonguieditabletextline.h"


namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}

namespace Evidyon {

struct ClientAvatarInventoryItem;
class EvidyonGUIModule_Bazaar;



class EvidyonGUIModule_Bazaar_CreateListingPrompt : public GUIModule {
public:

  void create(GUICanvas* source_canvas,
              EvidyonGUIModule_Bazaar* owner,
              GUIFont* prompt_font);

  void destroy();

  void setListingPrompt(const char* item_name,
                        const ClientAvatarInventoryItem* item,
                        const Item::ItemClientDescription* description);

  void update(ActionTargeter* action_targeter);


private:
  EvidyonGUIModule_Bazaar* bazaar_;
  const ClientAvatarInventoryItem* item_;
  EvidyonGUIButton ok_, cancel_;
  EvidyonGUITitledPanel panel_;
  GUITextLabelComponent prompt_text_;
  EvidyonGUIEditableTextLine input_;
};


}


#endif