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

