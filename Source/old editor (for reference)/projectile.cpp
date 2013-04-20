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
