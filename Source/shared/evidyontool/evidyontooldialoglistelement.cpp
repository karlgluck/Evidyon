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
#include "evidyontooldialoglistelement.h"



namespace Evidyon {
namespace Tools {


  
//----[  EvidyonToolDialogListElement  ]---------------------------------------
EvidyonToolDialogListElement::EvidyonToolDialogListElement(
    dc::dcGenericResource* resource,
    int indentation) {
  resource_ = resource;
  expanded_ = false;
  indentation_ = indentation;
}



//----[  name  ]---------------------------------------------------------------
const std::string& EvidyonToolDialogListElement::name() const {
    return resource_->getName();
}



//----[  hasChildren  ]--------------------------------------------------------
bool EvidyonToolDialogListElement::hasChildren() const {
    return numberOfChildren() > 0;
}



//----[  numberOfChildren  ]---------------------------------------------------
size_t EvidyonToolDialogListElement::numberOfChildren() const {
  return resource_->getMembers().size();
}



//----[  createChildren  ]-----------------------------------------------------
std::list<EvidyonToolDialogListElement*>
EvidyonToolDialogListElement::createChildren() const {
  std::list<EvidyonToolDialogListElement*> list;
  int child_indentation = indentation_ + 1;
  const dc::dcGenericResource::Array& p = resource_->getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = p.begin(); i != p.end(); ++i) {
    list.push_back(new EvidyonToolDialogListElement(*i, child_indentation));
  }

  return list;
}



//----[  getActions  ]---------------------------------------------------------
dc::dcAction* EvidyonToolDialogListElement::getActions() const {
    return resource_->getManager()->getActions();
}



//----[  performAction  ]------------------------------------------------------
int EvidyonToolDialogListElement::performAction(dc::dcActionFunction function,
                                                void* userData) {
    return (*function)(resource_, userData);
}



//----[  toString  ]-----------------------------------------------------------
std::string EvidyonToolDialogListElement::toString() const {
  return resource_->toString();
}




//----[  expanded  ]-----------------------------------------------------------
bool EvidyonToolDialogListElement::expanded() const {
  return expanded_;
}




//----[  toggleExpanded  ]-----------------------------------------------------
void EvidyonToolDialogListElement::toggleExpanded() {
  expanded_ = !expanded_;
}




//----[  EvidyonToolDialogListElement  ]---------------------------------------
int EvidyonToolDialogListElement::indentation() const {
  return indentation_;
}



//----[  isChildOf  ]----------------------------------------------------------
bool EvidyonToolDialogListElement::isChildOf(EvidyonToolDialogListElement* element) {
  return resource_->getOwner() == element->resource_;
}





//----[  getResource  ]--------------------------------------------------------
dc::dcGenericResource* EvidyonToolDialogListElement::getResource() {
  return resource_;
}



}
}
