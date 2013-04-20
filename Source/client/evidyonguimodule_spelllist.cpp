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
#include "evidyonguimodule_spelllist.h"
#include "evidyonguitooltipbox.h"
#include "evidyonguikeyboundactionssentinel.h"
#include "shared/spell/spellclientdescription.h"



namespace Evidyon {



  
//----[  EvidyonGUIModule_SpellList  ]-----------------------------------------
EvidyonGUIModule_SpellList::EvidyonGUIModule_SpellList() {
  tooltip_ = NULL;
  button_font_ = NULL;
  spell_descriptions_ = NULL;
  number_of_spells_ = 0;
}




//----[  create  ]-------------------------------------------------------------
void EvidyonGUIModule_SpellList::create(GUICanvas* source_canvas,
                                        GUIFont* button_font,
                                        const Spell::SpellClientDescription* spells,
                                        size_t number_of_spells,
                                        EvidyonGUITooltipBox* tooltip) {
  tooltip_ = tooltip;
  button_font_ = button_font;
  spell_descriptions_ = spells;
  number_of_spells_ = number_of_spells;
  addComponent(&spell_list_);
  addComponent(&label_);

  spell_list_.setLineHeight(25);
  spell_list_.setBackgroundColor(0xA0FFFFFF);

  {
    GUICanvas* canvas = spell_list_.getCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(300);
    canvas->alignXLeft(50);
    canvas->alignYTop(100);
    canvas->absoluteDepth(0.4f);
  }

  {
    GUICanvas* canvas = label_.getCanvas();
    canvas->setSourceCanvas(spell_list_.getCanvas());
    canvas->relativeWidth(0);
    canvas->absoluteHeight(35);
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(5);
    canvas->absoluteDepth(0.4f);
    label_.textLabel()->setFont(button_font);
    label_.textLabel()->setAlignment(GUIALIGN_CENTER);
    label_.textLabel()->setTextColor(0xFFFFFFFF);
    label_.textLabel()->setText("Click to Cast a Spell or Right-Click to Set a Key");
  }

  tooltip_description_.setFont(button_font);
}



//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIModule_SpellList::destroy() {
  clearComponents();
  button_font_ = NULL;
  hide();
}



//----[  sync  ]---------------------------------------------------------------
void EvidyonGUIModule_SpellList::sync(EvidyonGUIKeyBoundActionsSentinel* key_bindings,
                                      const Avatar::SpellAvailabilityMask* spell_mask) {
  GUILayer* layer = hide();
  float position = spell_list_.getScrollPosition();
  spell_list_.clear();
  for (int i = 0; i < number_of_spells_; ++i) {
    if (spell_mask->isBitSet(i)) {
      EvidyonGUIButton* button = spell_list_.button(i);
      button->enable();
      button->setFont(button_font_);
      GUIKey bound_key = key_bindings->getKeyForSpell(i);
      if (bound_key != GUIKEY_NULL) {
        std::string text = spell_descriptions_[i].name;
        text.append(" (");
        text.append(GUIKeyboardKeyName(bound_key));
        text.append(")");
        button->setText(text.c_str());
      } else {
        button->setText(spell_descriptions_[i].name);
      }
      button->getTextLabelElement()->setAlignment(GUIALIGN_LEFT);
    }
  }
  spell_list_.setScrollPosition(position);
  show(layer);
}





//----[  getMouseOverSpell  ]--------------------------------------------------
bool EvidyonGUIModule_SpellList::getMouseOverSpell(const Spell::SpellClientDescription** spell) {
  int id;
  if (spell_list_.getMouseOverButton(&id)) {
    if (spell != NULL) *spell = &spell_descriptions_[id];
    return true;
  } else {
    return false;
  }
}



//----[  getClickedSpell  ]----------------------------------------------------
bool EvidyonGUIModule_SpellList::getClickedSpell(int* spell_type,
                                                 const Spell::SpellClientDescription** spell) {
  int id;
  if (spell_list_.getClickedButton(&id)) {
    if (spell_type != NULL) *spell_type = id;
    if (spell != NULL) *spell = &spell_descriptions_[id];
    return true;
  } else {
    return false;
  }
}





//----[  updateTooltip  ]------------------------------------------------------
void EvidyonGUIModule_SpellList::updateTooltip() {
  if (!tooltip_ || !isActive()) return;
  const Spell::SpellClientDescription* spell;
  if (getMouseOverSpell(&spell)) {
    GUISize size;
    tooltip_description_.setSpell(spell,
                                 &size);
    tooltip_->setContentSize(size);
    if (tooltip_->becomeOwner(this)) {
      tooltip_->showContentAfter(0.0);
      tooltip_->addContent(&tooltip_description_);
    }
  } else {
    tooltip_->releaseOwnership(this);
  }
}





//----[  getCanvas  ]----------------------------------------------------------
GUICanvas* EvidyonGUIModule_SpellList::getCanvas() {
  return spell_list_.getCanvas();
}



//----[  scrollToTop  ]--------------------------------------------------------
void EvidyonGUIModule_SpellList::scrollToTop() {
  spell_list_.setScrollPosition(0.0f);
}


}