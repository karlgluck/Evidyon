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
#include "evidyonguimodule_bazaar.h"
#include "evidyonguimodule_bazaar_createlistingprompt.h"
//#include "actiontargeter.h"
#include "client/actionqueue.h"
#include "useractionqueue.h"
#include "shared/item/itemclientdescription.h"



namespace Evidyon {



void EvidyonGUIModule_Bazaar_CreateListingPrompt::create(
    GUICanvas* source_canvas,
    EvidyonGUIModule_Bazaar* owner,
    GUIFont* prompt_font) {

  bazaar_ = owner;

  addComponent(&prompt_text_);
  addComponent(&ok_);
  addComponent(&cancel_);
  addComponent(&panel_);
  addComponent(&input_);

  panel_.setFont(prompt_font);
  panel_.setText("Create Listing");
  {
    GUICanvas* canvas = panel_.getCanvas();
    canvas->setSourceCanvas(source_canvas);
    canvas->alignXCenter(0);
    canvas->alignYCenter(0);
    canvas->absoluteWidth(400);
    canvas->absoluteHeight(300);
    canvas->absoluteDepth(0.2f);
  }

  prompt_text_.textLabel()->setFont(prompt_font);
  {
    GUICanvas* canvas = prompt_text_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignXLeft(5);
    canvas->alignYTop(5);
    canvas->relativeWidth(-10);
    canvas->absoluteHeight(50);
    canvas->relativeDepth(-0.01f);
  }

  
  input_.configure(prompt_font, // font
                   false,       // is password?
                   true,        // restrict to numbers?
                   0xFFFFFFFF); // color = white
  {
    GUICanvas* canvas = input_.getCanvas();
    canvas->setSourceCanvas(prompt_text_.getCanvas());
    canvas->alignXCenter(0);
    canvas->alignYBottomOutside(5);
    canvas->relativeWidth(-10);
    canvas->absoluteHeight(35);
    canvas->relativeDepth(0.0f);
  }
  
  ok_.setFont(prompt_font);
  ok_.setText("Create");
  ok_.enable();
  {
    GUICanvas* canvas = ok_.getCanvas();
    canvas->setSourceCanvas(cancel_.getCanvas());
    canvas->alignXCenter(0);
    canvas->alignYTopOutside(-10);
    canvas->relativeWidth(0);
    canvas->relativeHeight(0);
    canvas->relativeDepth(0.0f);
  }

  cancel_.setFont(prompt_font);
  cancel_.setText("Cancel");
  cancel_.enable();
  {
    GUICanvas* canvas = cancel_.getCanvas();
    canvas->setSourceCanvas(panel_.getCanvas());
    canvas->alignXCenter(0);
    canvas->relativeWidth(-30);
    canvas->absoluteHeight(35);
    canvas->alignYBottom(-10);
    canvas->relativeDepth(-0.1f);
  }
}


void EvidyonGUIModule_Bazaar_CreateListingPrompt::destroy() {
  hide();
  clearComponents();
}

void EvidyonGUIModule_Bazaar_CreateListingPrompt::setListingPrompt(
    const char* item_name,
    const ClientAvatarInventoryItem* item,
    const Item::ItemClientDescription* description) {

  item_ = item;

  std::string text = "Sell the ";
  text.append(item_name);
  if (item->quantity > 1) text.append("s");
  text.append(" for:");
  prompt_text_.textLabel()->setText(text.c_str());

  input_.getMouseSentinel()->obtainMouseFocus();
  input_.setText("");

  obtainFocus();
}

void EvidyonGUIModule_Bazaar_CreateListingPrompt::update(ActionTargeter* action_targeter) {
  int price = atoi(input_.getText());
  char list_button_text[128];
  sprintf_s(list_button_text,
            128,
            "List (costs %lu gold)",
            BazaarCommission(price, bazaar_->getNumberOfOwnListedItems()));
  ok_.setText(list_button_text);

  if (cancel_.pollPushed()) {
    bazaar_->showCreateListing();
  }
  if (ok_.pollPushed() && item_ != NULL) {
    Action_BazaarCreateListing* create_listing
      = Client::ActionQueue::add<Action_BazaarCreateListing>();
    create_listing->listing_price = price;
    create_listing->inventory_index = item_->inventory_index;
    //UserEventBazaarCreateListing* create_listing = action_targeter->bazaarCreateListing();
    //create_listing->listing_price_ = price;
    //create_listing->inventory_index_ = item_->inventory_index;
    bazaar_->changeCategory(CATEGORY_OWNITEMS);
  }
}

}


