#ifndef __EVIDYON_ENCHANTMENTS_H__
#define __EVIDYON_ENCHANTMENTS_H__
#pragma once



namespace Evidyon {

namespace Enchantments {
static const int NUMBER_OF_ENCHANTMENT_CLASSES = 9;
static const int ENCHANTMENT_CLASS_BITS[NUMBER_OF_ENCHANTMENT_CLASSES] = {
  1, 1,       // = 2 * 1 = 2 bits
  1, 1,       // = 2 * 1 = 2 bits
  2, 2, 2,    // = 3 * 2 = 6 bits
  3, 3,       // = 2 * 3 = 6 bits
              // total   = 16 bits
};


static const char* ENCHANTMENT_CLASS_NAMES[NUMBER_OF_ENCHANTMENT_CLASSES] = {
  "Speed",
  "Strength",
  "Armor",
  "Magic Defense",
  "5",
  "6",
  "7",
  "8",
  "9",
};

static const int ENCHANTMENTS_FROM_BITS[4] = { 0,
                                               (1<<1)-1,
                                               (1<<2)-1,
                                               (1<<3)-1 };

static const int TOTAL_ENCHANTMENT_TYPES = 4*1 + 3*3 + 2 * 7;/*
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[0]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[1]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[2]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[3]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[4]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[5]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[6]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[7]] +
  ENCHANTMENTS_FROM_BITS[ENCHANTMENT_CLASS_BITS[8]];*/

inline unsigned int EnchantmentInvClassMask(int class_bits, int first_bit_index) {
  return ~(((~(unsigned int)0)>>((sizeof(unsigned int)*8)-class_bits))<<first_bit_index);
}

inline unsigned int EnchantmentTypeBits(unsigned int enchantment_index, int class_first_bit_index) {
  return ((unsigned)(enchantment_index + 1)) << class_first_bit_index;
}


}


}

#endif