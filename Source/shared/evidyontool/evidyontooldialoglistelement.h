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
#ifndef __EVIDYONTOOLDIALOGLISTELEMENT_H__
#define __EVIDYONTOOLDIALOGLISTELEMENT_H__
#pragma once



#include <dc/dcresource.h>

namespace Evidyon {
namespace Tools {



  
//----[  EvidyonToolDialogListElement  ]---------------------------------------
class EvidyonToolDialogListElement {
public:
  EvidyonToolDialogListElement(dc::dcGenericResource* resource,
                               int indentation);
  const std::string& name() const;
  bool hasChildren() const;
  size_t numberOfChildren() const;
  std::list<EvidyonToolDialogListElement*> createChildren() const;
  dc::dcAction* getActions() const;
  int performAction(dc::dcActionFunction function, void* userData);
  std::string toString() const;
  bool expanded() const;
  void toggleExpanded();
  int indentation() const;
  bool isChildOf(EvidyonToolDialogListElement* element);
  dc::dcGenericResource* getResource();

private:
  dc::dcGenericResource* resource_;
  int indentation_;
  bool expanded_;
};

}
}



#endif