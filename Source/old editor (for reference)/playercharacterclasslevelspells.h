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
#ifndef __PLAYERCHARACTERCLASSLEVELSPELLS_H__
#define __PLAYERCHARACTERCLASSLEVELSPELLS_H__
#pragma once

#include <dc/integer>
#include <dc/list>
#include "spell.h"


class PlayerCharacterClassLevelSpells : public dc::dcCollection<PlayerCharacterClassLevelSpells>
{
    public:

  /**
   * Associates two values with each other
   */
  class Element : public dc::dcResource<Element> {
  public:
    /**
     * Initializes this resource
     */
    Element();

    /**
     * Compares this element with another resource.  Only levels are compared.
     */
    int compareTo(const dc::dcGenericResource* other) const;

    /**
     * Obtains the level at which this element becomes active
     */
    int getLevelValue() const;

    /**
     * Gets the spell that this element references
     */
    const dc::dcList<Evidyon::Spell>::Element* getSpell() const;

    /**
     * Sets this element's contents
     */
    void set(int level, dc::dcList<Evidyon::Spell>::Element* effect);


  public:
    /**
     * The name of this class type
     * @return dc::dcClass name string
     */
    static std::string staticTypeName();


  protected:

    /// The level of this spell
    dc::dcInteger myLevel;

    /// The spell to make available
    dc::dcList<Evidyon::Spell>::Reference mySpell;
  };

  /**
   * Fills out the mask with available spells
   */
  void fillClassSpellAvailabilityMask(Evidyon::SpellAvailabilityMask mask[Evidyon::HIGHEST_DEFINED_LEVEL]) const;

  /**
   * Determines whether or not this collection allows duplicate entries to
   * be held internally.
   * @return Flag value
   */
  bool allowDuplicates() const;

  /**
   * Determines whether or not the derived collection type requires that its
   * entries be kept sorted.
   */
  bool keepSorted() const;


public:

  /**
   * The name of this class type
   * @return dc::dcClass name string
   */
  static std::string staticTypeName();

};


#endif