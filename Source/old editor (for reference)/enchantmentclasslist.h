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
#ifndef __ENCHANTMENTCLASSLIST_H__
#define __ENCHANTMENTCLASSLIST_H__
#pragma once

#include <dc/string> // for dc::dcResource/dcStream
//class EnchantmentClass;
#include "../shared/evidyon_enchantments.h"
#include "enchantment.h"

//static const int NUMBER_OF_ENCHANTMENT_CLASSES = 10;

class EnchantmentClassList : public dc::dcResource<EnchantmentClassList> {
  //static const int TWO_BIT_CLASSES = 4;   // = 4*2 = 8 bits
  //static const int THREE_BIT_CLASSES = 2; // = 2*3 = 6 bits
  //static const int TOTAL_CLASS_BITS = 16;
  //static const int VISIBLE_ONE_BIT_CLASSES = 2;//TOTAL_CLASS_BITS - (TWO_BIT_CLASSES*2 + THREE_BIT_CLASSES*3);
  //static const int TOTAL_CLASSES = 10;
  //static const int INVISIBLE_ONE_BIT_CLASSES = 2;//TOTAL_CLASSES - (VISIBLE_ONE_BIT_CLASSES +
  //                                               //                 TWO_BIT_CLASSES +
  //                                               //                 THREE_BIT_CLASSES);
public:
  EnchantmentClassList();
  ~EnchantmentClassList();

  bool compileForClient(dc::dcStreamOut* stream);

  void reset();

public:
  static std::string staticTypeName();

private:
  //EnchantmentClass* classes_[TOTAL_CLASSES];
  Enchantment enchantments_[Evidyon::Enchantments::TOTAL_ENCHANTMENT_TYPES];
};



#endif