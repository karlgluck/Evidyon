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
#include "geosidcompiler.h"
#include "shared/geosid/geosidclientdescription.h"
#include "shared/geosid/geosidserverdescription.h"

namespace Evidyon {
namespace Geosid {
namespace Tools {



  
//----[  GeosidCompiler  ]-----------------------------------------------------
GeosidCompiler::GeosidCompiler(Magic::Tools::MagicCompiler* magic_compiler,
                               LifeformAI::Tools::LifeformAICompiler* lifeform_compiler) {
  assert(magic_compiler);
  assert(lifeform_compiler);
  magic_compiler_ = magic_compiler;
  lifeform_compiler_ = lifeform_compiler;
}


//----[  clear  ]--------------------------------------------------------------
void GeosidCompiler::clear() {
  server_.clear();
  client_.clear();
}


//----[  add  ]----------------------------------------------------------------
Evidyon::Geosid::GeosidIndex GeosidCompiler::add(
    const GeosidServerDescription& server,
    const GeosidClientDescription& client) {
  GeosidServerDescription* server_desc;
  size_t server_index = server_.add(server, &server_desc);
  GeosidClientDescription* client_desc;
  size_t client_index = client_.add(client, &client_desc);
  assert(server_index == client_index);
  server_desc->index = (Geosid::GeosidIndex)server_index;
  client_desc->index = server_desc->index;
  return (Geosid::GeosidIndex)server_index;
}



//----[  numberOfGeosids  ]----------------------------------------------------
size_t GeosidCompiler::numberOfGeosids() const {
  assert(server_.objects.size() == client_.objects.size());
  return server_.objects.size();
}


//----[  compileServer  ]------------------------------------------------------
void GeosidCompiler::compileServer(size_t index, GeosidServerDescription* desc) {
  server_.copyOut(index, desc);
}


//----[  compileClient  ]------------------------------------------------------
void GeosidCompiler::compileClient(size_t index, GeosidClientDescription* desc) {
  client_.copyOut(index, desc);
}



//----[  magicCompiler  ]------------------------------------------------------
Magic::Tools::MagicCompiler* GeosidCompiler::magicCompiler() {
  assert(magic_compiler_);
  return magic_compiler_;
}



//----[  lifeformCompiler  ]---------------------------------------------------
LifeformAI::Tools::LifeformAICompiler* GeosidCompiler::lifeformCompiler() {
  assert(lifeform_compiler_);
  return lifeform_compiler_;
}


}
}
}

