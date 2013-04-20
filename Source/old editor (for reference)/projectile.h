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
#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__
#pragma once

#include <dc/float>
#include <dc/table>
#include <dc/list>
#include "visualfx.h"
#include "wavsound.h"

namespace Evidyon {

struct ServerProjectileDescription;
struct ClientProjectileDescription;

}


// Defines a type of projectile
class Projectile : public dc::dcResource<Projectile> {
public:
  Projectile();
  void compileForServer(Evidyon::ServerProjectileDescription* projectile_description) const;
  void compileForClient(Evidyon::ClientProjectileDescription* projectile_description) const;

public:
  static std::string staticTypeName();

private:
  dc::dcFloat range_;
  dc::dcFloat speed_;
  dc::dcTable<VisualFX>::Reference carrier_fx_, termination_fx_;
  dc::dcList<WAVSound>::Reference carrier_sound_, termination_sound_;
};






#endif

