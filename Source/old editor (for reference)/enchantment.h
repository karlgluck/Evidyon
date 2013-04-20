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
#ifndef __ENCHANTMENT_H__
#define __ENCHANTMENT_H__
#pragma once

#include <dc/table>
#include "visualfx.h"

namespace Evidyon {
struct ClientEnchantmentDescription;
struct MagicDescriptionStageEffect;
}

class Enchantment : public dc::dcResource<Enchantment> {
public:
  Enchantment();
  void compileForClient(
      Evidyon::ClientEnchantmentDescription* description) const;
  void setProperties(int class_index,
                     unsigned int type_bits,
                     unsigned int inv_class_mask);

  // Sets the enchantment-specific parameters in the stage effect
  void fillMagicDescriptionStageEffect(
    Evidyon::MagicDescriptionStageEffect* enchantment_effect);

  // Zeroes out this class
  void reset();

  unsigned int getInvClassMask() const { return inv_class_mask_; }
  unsigned int getTypeBits() const { return type_bits_; }

public:
  static std::string staticTypeName();

private:
  dc::dcString description_; // totally internal for the editor
  dc::dcTable<VisualFX>::Reference visual_fx_; // for client

  // for the magic descriptions on the server
  int class_index_;
  unsigned int type_bits_;
  unsigned int inv_class_mask_;
};


#endif