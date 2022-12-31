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