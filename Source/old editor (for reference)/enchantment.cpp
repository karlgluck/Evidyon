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

