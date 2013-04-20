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
#ifndef __DETAILED_MAGIC_H__
#define __DETAILED_MAGIC_H__

#include "magic.h"
#include "../shared/server-editor/magicdescription.h"
#include <dc/enum>
#include <dc/float>
#include <dc/boolean>
#include <dc/list>
#include "magicfx.h"
#include "projectile.h"
#include "enchantment.h"

namespace Evidyon {

class DetailedMagicStage : public dc::dcResource<DetailedMagicStage> {
public:
  DetailedMagicStage();
  void compileForServer(MagicDescriptionStage* description);

public:
  static std::string staticTypeName();

private:
  dc::dcEnum<MagicDescriptionStageTargeting::Type> targeting_type_;
  dc::dcInteger targets_;
  dc::dcFloat delay_;
  dc::dcFloat spread_;
  dc::dcEnum<MagicDescriptionStageEffect::Type> effect_type_;
  dc::dcFloat duration_;
  dc::dcFloat float_param_;
  dc::dcInteger int_param_;

  // If the effect is an enchantment, the referenced object is used
  // to fill out enchantment parameters
  dc::dcReference<Enchantment> enchantment_;

  dc::dcList<MagicFX>::Reference aoe_fx_;

  // Depending on the targeting type, one of these is used
  dc::dcList<MagicFX>::Reference per_targeter_magic_fx_;
  dc::dcList<Projectile>::Reference per_targeter_projectile_fx_;
};


// Detailed magic lets the editor specify every part of a
// magic instance's definition by hand.
class DetailedMagic : public dc::dcResource<DetailedMagic, Magic> {
public:
  DetailedMagic();
  virtual void compileForServer(MagicDescription* description);

public:
  static std::string staticTypeName();

private:
  dc::dcList<MagicFX>::Reference magic_fx_;
  dc::dcEnum<ActorAction> casting_action_;
  dc::dcEnum<MagicDescription::PowerType> power_type_;
  dc::dcBoolean freeze_;
  dc::dcFloat movement_delay_;
  dc::dcFloat essential_action_delay_;
  dc::dcFloat balancing_action_delay_;
  DetailedMagicStage stages_[Evidyon::MagicDescription::NUM_STAGES];

private:
  /// This class implements the Magic interface
  static dc::dcClass<DetailedMagic>::Implements<Magic> implements_magic_;
};


}

#endif