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
#include "enchantmentclasslist.h"
#include "enchantmentclass.h"
#include "enchantmentclassimpl.h"
#include "../shared/client-editor/clientenchantmentdescription.h"

#include <dc/debug>



EnchantmentClassList::EnchantmentClassList() {
  int member_index = 0;
  int enchantment_index = 0;
  int first_bit = 0;
  for (int i = 0; i < Evidyon::Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    int bits = Evidyon::Enchantments::ENCHANTMENT_CLASS_BITS[i];
    int enchantments = Evidyon::Enchantments::ENCHANTMENTS_FROM_BITS[bits];
    for (int j = 0; j < enchantments; ++j) {
      member(std::string(Evidyon::Enchantments::ENCHANTMENT_CLASS_NAMES[i]) +
             std::string(" #") +
             EnchantmentClass::stringOfNumber(j),
            &enchantments_[enchantment_index]);
      enchantments_[enchantment_index].setProperties(i,
          Evidyon::Enchantments::EnchantmentTypeBits(j, first_bit),
          Evidyon::Enchantments::EnchantmentInvClassMask(bits, first_bit));
      ++enchantment_index;
    }
    first_bit += bits;
  }
  /*
  int member_index = 0;
  int first_enchantment_index = 0;
  int first_bit = 0;

#define CREATE_ENCHANTMENT_CLASSES(number, bits, group_name, visible) \
  for (int i = 0; i < number; ++i) { \
    char name_buffer[8]; _ltoa_s(member_index+1, name_buffer, 8, 10); \
    EnchantmentClass* new_class = new EnchantmentClassImpl<bits>(name_buffer,\
        first_enchantment_index, first_bit, visible);\
    first_bit += new_class->getBitFieldSize();\
    first_enchantment_index += new_class->getEnchantmentTypesInClass();\
    classes_[member_index++] = new_class;\
    std::string name = std::string("Class ") + name_buffer + group_name;\
    member(name, new_class); }

  CREATE_ENCHANTMENT_CLASSES(VISIBLE_ONE_BIT_CLASSES, 1, " 1-bit", true);
  CREATE_ENCHANTMENT_CLASSES(TWO_BIT_CLASSES, 2, " 2-bit", true);
  CREATE_ENCHANTMENT_CLASSES(THREE_BIT_CLASSES, 3, " 3-bit", true);
  CREATE_ENCHANTMENT_CLASSES(INVISIBLE_ONE_BIT_CLASSES, 1, " 1-bit (Invisible)", false);

  ASSERT(member_index == TOTAL_CLASSES);*/
}


EnchantmentClassList::~EnchantmentClassList() {
  //for (int i = 0; i < TOTAL_CLASSES; ++i) {
  //  delete classes_[i];
  //  classes_[i] = NULL;
  //}
}

bool EnchantmentClassList::compileForClient(dc::dcStreamOut* stream) {
  {
    size_t number_of_classes = Evidyon::Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES;
    CONFIRM(stream->write(&number_of_classes,
                          sizeof(number_of_classes))) else return false;
  }

  // Write out the classes
  int first_enchantment_index = 0, first_bit = 0;
  for (int i = 0; i < Evidyon::Enchantments::NUMBER_OF_ENCHANTMENT_CLASSES; ++i) {
    Evidyon::ClientEnchantmentClassDescription enchantment_class;
    enchantment_class.first_enchantment_index = first_enchantment_index;
    enchantment_class.mask.first_bit = first_bit;
    unsigned int bits = Evidyon::Enchantments::ENCHANTMENT_CLASS_BITS[i];
    enchantment_class.mask.type_mask = ~((~(unsigned)0L)<<bits); // create 'bits' 1s starting at LSB
    CONFIRM(stream->write(&enchantment_class,
                          sizeof(enchantment_class))) else return false;
    first_bit += bits;
    first_enchantment_index += Evidyon::Enchantments::ENCHANTMENTS_FROM_BITS[bits];
  }

  // Write out the enchantments themselves
  size_t total_enchantments = Evidyon::Enchantments::TOTAL_ENCHANTMENT_TYPES;
  stream->write(&total_enchantments, sizeof(total_enchantments));
  for (int i = 0; i < Evidyon::Enchantments::TOTAL_ENCHANTMENT_TYPES; ++i) {
    Evidyon::ClientEnchantmentDescription description;
    enchantments_[i].compileForClient(&description);
    CONFIRM(stream->write(&description,
                          sizeof(description))) else return false;
  }

  return true;
  //{
  //  size_t number_of_classes = TOTAL_CLASSES;
  //  CONFIRM(stream->write(&number_of_classes,
  //                        sizeof(number_of_classes))) else return false;
  //}

  //// Write out the classes
  //size_t total_enchantments = 0;
  //for (int i = 0; i < TOTAL_CLASSES; ++i) {
  //  total_enchantments += classes_[i]->getEnchantmentTypesInClass();
  //  Evidyon::ClientEnchantmentClassDescription enchantment_class;
  //  classes_[i]->compileForClient(&enchantment_class);
  //  CONFIRM(stream->write(&enchantment_class,
  //                        sizeof(enchantment_class))) else return false;
  //}

  //// Write out the enchantments themselves
  //stream->write(&total_enchantments, sizeof(total_enchantments));
  //for (int i = 0; i < TOTAL_CLASSES; ++i) {
  //  Evidyon::ClientEnchantmentDescription description;
  //  int class_enchantments = classes_[i]->getEnchantmentTypesInClass();
  //  for (int j = 0; j < class_enchantments; ++j) {
  //    Enchantment* enchantment = classes_[i]->getEnchantment((unsigned)j);
  //    enchantment->compileForClient(&description);
  //    CONFIRM(stream->write(&description,
  //                          sizeof(description))) else return false;
  //  }
  //}

  //return true;
}

void EnchantmentClassList::reset() {
  for (int i = 0; i < Evidyon::Enchantments::TOTAL_ENCHANTMENT_TYPES; ++i) {
    enchantments_[i].reset();
  }
  /*
  for (int i = 0; i < TOTAL_CLASSES; ++i) {
    int class_enchantments = classes_[i]->getEnchantmentTypesInClass();
    for (int j = 0; j < class_enchantments; ++j) {
      Enchantment* enchantment = classes_[i]->getEnchantment((unsigned)j);
      enchantment->reset();
    }
  }*/
}

std::string EnchantmentClassList::staticTypeName() {
  return "EnchantmentClassList";
}