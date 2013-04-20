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
#ifndef __ENCHANTMENTCLASSIMPL_H__
#define __ENCHANTMENTCLASSIMPL_H__
#pragma once

#include "enchantment.h"
#include "enchantmentclass.h"
#include "../shared/client-editor/clientenchantmentdescription.h"

template <const int BITS> class EnchantmentClassImpl :
public dc::dcResource<EnchantmentClassImpl<BITS>, EnchantmentClass> {
public:
  static const int TYPES_IN_CLASS = (1<<BITS)-1;

public:
  EnchantmentClassImpl() {
    // this constructor should not normally be used; however,
    // it is necessary that it be implemented for importing.
    for (int i = 0; i < TYPES_IN_CLASS; ++i) {
      member(EnchantmentClass::stringOfEnchantmentClassBits(BITS) + std::string(" enchantment #") +
             EnchantmentClass::stringOfNumber(i),
            &enchantments_[i]);
    }
  }

  EnchantmentClassImpl(const std::string& class_name,
                       int first_enchantment_index,
                       int first_bit, bool visible) {
    first_enchantment_index_ = first_enchantment_index;
    first_bit_ = first_bit;
    for (int i = 0; i < TYPES_IN_CLASS; ++i) {
      member(EnchantmentClass::stringOfEnchantmentClassBits(BITS) + std::string(" enchantment #") +
             EnchantmentClass::stringOfNumber(i),
            &enchantments_[i]);
      unsigned int inv_class_mask, type_bits;
      if (visible) {
        inv_class_mask = ~(((~(unsigned)0)>>((sizeof(unsigned int)*8)-BITS))<<first_bit);
        type_bits = ((unsigned)(i+1))<<first_bit;
      } else {
        inv_class_mask = ~(unsigned)0;
        type_bits = 0;
      }
      enchantments_[i].setProperties(first_enchantment_index + i,
                                     type_bits, inv_class_mask);
    }
  }

  virtual int getEnchantmentTypesInClass() const {
    return TYPES_IN_CLASS;
  }

  virtual int getBitFieldSize() const {
    return BITS;
  }

  virtual void compileForClient(Evidyon::ClientEnchantmentClassDescription* description) {
    description->mask.first_bit = first_bit_;
    description->mask.type_mask = ~((~(unsigned)0L)<<BITS); // create 'BITS' 1s starting at LSB
    description->first_enchantment_index = first_enchantment_index_;
  }

  virtual Enchantment* getEnchantment(unsigned int index) {
    return index < TYPES_IN_CLASS ? &enchantments_[index] : NULL;
  }

public:
  static std::string staticTypeName() {
    return std::string("EnchantmentVisualizationClass") +
           EnchantmentClass::stringOfEnchantmentClassBits(BITS);
  } 

private:
  int first_enchantment_index_, first_bit_;
  Enchantment enchantments_[TYPES_IN_CLASS];
};


#endif