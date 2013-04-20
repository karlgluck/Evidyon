//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#ifndef __EVIDYONGUIMODULE_SPELLLIST_H__
#define __EVIDYONGUIMODULE_SPELLLIST_H__
#pragma once



#include "evidyonguiscrollablebuttonlist.h"
#include "common/gui/guimodule.h"
#include "shared/avatar/evidyon_avatar.h"
#include "evidyonguispelldescription.h"
#include "common/gui/guitextlabelcomponent.h"

namespace Evidyon {
namespace Spell {
struct SpellClientDescription;
}
}

class GUIFont;


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_SpellList : public GUIModule {
public:
  EvidyonGUIModule_SpellList();
  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const Spell::SpellClientDescription* spells,
              size_t number_of_spells,
              EvidyonGUITooltipBox* tooltip);
  void destroy();
  void sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings, const Avatar::SpellAvailabilityMask* spell_mask);
  bool getMouseOverSpell(const Spell::SpellClientDescription** spell);
  bool getClickedSpell(int* spell_type,
                       const Spell::SpellClientDescription** spell);
  void updateTooltip();

  GUICanvas* getCanvas();

  void scrollToTop();

private:
  GUIFont* button_font_;
  EvidyonGUIScrollableButtonList spell_list_;
  const Spell::SpellClientDescription* spell_descriptions_;
  size_t number_of_spells_;

  GUITextLabelComponent label_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUISpellDescription tooltip_description_;
};

}


#endif