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
#include "enchantment.h"
#include "../shared/client-editor/clientenchantmentdescription.h"
#include "../shared/server-editor/magicdescription.h"


Enchantment::Enchantment() {
  member("Description", &description_);
  member("Visual FX", &visual_fx_);
}


void Enchantment::compileForClient(
    Evidyon::ClientEnchantmentDescription* description) const {
  description->visual_fx = visual_fx_.getReferencedResourceIndex();
}


void Enchantment::setProperties(int class_index,
                                unsigned int type_bits,
                                unsigned int inv_class_mask) {
  class_index_ = class_index;
  type_bits_ = type_bits;
  inv_class_mask_ = inv_class_mask;
}


void Enchantment::fillMagicDescriptionStageEffect(
    Evidyon::MagicDescriptionStageEffect* enchantment_effect) {
  enchantment_effect->enchantment_class_index = class_index_;
  enchantment_effect->enchantment_type_bits = type_bits_;
  enchantment_effect->enchantment_inv_class_mask = inv_class_mask_;
}


void Enchantment::reset() {
  description_.setValue("");
  visual_fx_.setGenericReferencedResource(NULL);
}


std::string Enchantment::staticTypeName() {
  return "Enchantment";
}

