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
#include "projectile.h"
#include "../shared/server-editor/serverprojectiledescription.h"
#include "../shared/client-editor/clientprojectiledescription.h"
#include "../shared/evidyon_magic.h"



Projectile::Projectile() {
  member("Range", &range_);
  member("Carrier SpecialFX", &carrier_fx_);
  member("Carrier Sound", &carrier_sound_);
  member("Termination SpecialFX", &termination_fx_);
  member("Termination Sound", &termination_sound_);
}

void Projectile::compileForServer(Evidyon::ServerProjectileDescription* projectile_description) const {
  projectile_description->duration = range_.getValue() / MAGIC_PROJECTILE_SPEED;
}

void Projectile::compileForClient(Evidyon::ClientProjectileDescription* projectile_description) const {
  projectile_description->duration = range_.getValue() / MAGIC_PROJECTILE_SPEED;
  projectile_description->carrier_fx = carrier_fx_.getReferencedResourceIndex();
  projectile_description->termination_fx = termination_fx_.getReferencedResourceIndex();
  projectile_description->carrier_sound = carrier_sound_.getReferencedResourceIndex();
  projectile_description->termination_sound = termination_sound_.getReferencedResourceIndex();
}

std::string Projectile::staticTypeName() {
  return "Projectile";
}
