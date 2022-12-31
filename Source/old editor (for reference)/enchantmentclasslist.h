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