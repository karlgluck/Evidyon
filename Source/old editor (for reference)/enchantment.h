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