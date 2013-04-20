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
#include "shared/magic/tools/magic_branching.h"
#include "shared/magic/tools/magiccompiler.h"
#include "shared/magic/magic_branching.h"
#include "shared/event/event_targetedspecialfx.h"
#include "shared/event/tools/eventcompiler.h"


namespace Evidyon {
namespace Magic {
namespace Tools {



  
//----[  Magic_Branching_Stage  ]----------------------------------------------
Magic_Branching_Stage::Magic_Branching_Stage() {
  member("Special FX", &special_fx_);
  member("Special FX Duration", &special_fx_duration_);
  member("Targets", &targets_);
  member("Radius", &radius_);
  member("Delay", &delay_);
  member("Minimum Damage", &min_damage_);
  member("Maximum Damage", &max_damage_);
}


//----[  compile  ]------------------------------------------------------------
void Magic_Branching_Stage::compile(Evidyon::Event::Tools::EventCompiler* event_compiler,
                                    Evidyon::Magic::Magic_Branching_Stage* stage) {
  Event::Event_TargetedSpecialFX event_description;
  event_description.special_fx = special_fx_.getReferencedResourceIndex();
  event_description.duration = special_fx_duration_.getValue();
  stage->event_index = event_compiler->add(&event_description);
  stage->targets = targets_.getValue();
  stage->radius_sq = radius_.getValue() * radius_.getValue();
  stage->delay = delay_.getValue();
  stage->base_damage = min_damage_.getValue();
  int damage_range = max_damage_.getValue() - stage->base_damage;
  stage->random_damage = damage_range > 1 ? damage_range : 1;
}



dc::dcClass<Magic_Branching>::Implements<Evidyon::Magic::Tools::Magic> Magic_Branching::implementsMagic;


//----[  Magic_Branching  ]----------------------------------------------------
Magic_Branching::Magic_Branching() {
  member("Stage 1", &stages_[0]);
  member("Stage 2", &stages_[1]);
  member("Stage 3", &stages_[2]);
  member("Find Enemies", &find_enemies_);
  member("Find Friends", &find_friends_);
}



//----[  compile  ]------------------------------------------------------------
MagicIndex Magic_Branching::compile(MagicCompiler* compiler) {
  Evidyon::Magic::Magic_Branching description;
  stages_[0].compile(compiler->eventCompiler(), &description.stage[0]);
  stages_[1].compile(compiler->eventCompiler(), &description.stage[1]);
  stages_[2].compile(compiler->eventCompiler(), &description.stage[2]);
  description.find_enemies = find_enemies_.getValue();
  description.find_friends = find_friends_.getValue();
  return compiler->add(&description);
}



}
}
}

