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
#include "avatarrace.h"
#include "shared/actor/actortemplate.h"
#include "shared/actor/tools/actortemplatecompiler.h"
#include "shared/avatar/avatarraceclientdescription.h"
#include "shared/avatar/avatarraceserverdescription.h"
#include <assert.h>
#include <dc/dcbufferstream.h>

namespace Evidyon {
namespace Avatar {
namespace Tools {



  
//----[  AvatarRace  ]---------------------------------------------------------
AvatarRace::AvatarRace() {
  member("Description", &description_);
  for (int i = 0; i < NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
    member(std::string("Level 1 Max ")
            + AvatarAttributeString(AvatarAttribute(i)),
           &attribute_level_1_maxes_[i]);
  }

  for (int g = 0; g < NUMBER_OF_AVATAR_GENDERS; ++g) {
    std::string gender_string = AvatarGenderString(AvatarGender(g));
    member(gender_string + " Actor Type", &gender_actors_[g]);
    for (int i = 0; i < AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++i) {
      char number[16];
      sprintf_s(number, 16, "%i", i+1);
      member(gender_string + std::string(" Hair Style #") + number,
             &hair_styles_[g][i]);
    }
  }
}


//----[  compile  ]------------------------------------------------------------
bool AvatarRace::compile(
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Avatar::AvatarRaceClientDescription* client_description,
    Avatar::AvatarRaceServerDescription* server_description) {

  strncpy_s(client_description->description,
            Avatar::AvatarRaceClientDescription::DESCRIPTION_LENGTH,
            description_.getValue().c_str(),
            Avatar::AvatarRaceClientDescription::DESCRIPTION_LENGTH-1);

  for (int i = 0; i < NUMBER_OF_AVATAR_ATTRIBUTES; ++i) {
    client_description->attribute_level_1_maxes[i]
    = attribute_level_1_maxes_[i].getValue();
    server_description->attribute_level_1_maxes[i]
    = attribute_level_1_maxes_[i].getValue();
  }

  for (int i = 0; i < NUMBER_OF_AVATAR_GENDERS; ++i) {
    Evidyon::SkinnedMesh::SkinnedMeshFormIndex form;
    Evidyon::Tools::ActorProfileElement* actor_profile;
    gender_actors_[i].compile(&actor_profile, &form);
    if (!actor_profile ||
        form == Evidyon::SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX) {
      return false;
    }

    // Compile the template for this lifeform
    Actor::ActorTemplate actor_template;
    actor_profile->compileDefaultTemplate(&actor_template);
    actor_template.alignment
      = Actor::ALIGNMENT_NEUTRAL;
    actor_template.skinned_mesh_form = form;
    strncpy_s(actor_template.name,
              Actor::ActorTemplate::MAX_NAME_LENGTH,
              (std::string(AvatarGenderString(AvatarGender(i)))
                + " "
                + getName()).c_str(),
              Actor::ActorTemplate::MAX_NAME_LENGTH-1);
    actor_template.template_type = Actor::AVATAR_TEMPLATE;
    actor_template.profile_specialization
      = Actor::SPECIALIZE_UNARMED;
    Actor::ActorTemplateIndex actor_template_index
      = actor_template_compiler->addTemplate(&actor_template);
    assert(actor_template_index != Actor::INVALID_ACTOR_TEMPLATE_INDEX);

    // Save this gender's information
    server_description->gender_data[i].actor_template
      = actor_template_index;
    server_description->gender_data[i].actor_profile
      = actor_profile->getIndex();
    server_description->gender_data[i].skinned_mesh_form
      = form;
    for (int j = 0; j < AVATAR_RACE_HAIR_STYLES_PER_GENDER; ++j) {
      server_description->gender_data[i].hair_style[j]
        = hair_styles_[i][j].getReferencedResourceIndex();
    }
  }

  return true;
}




//----[  getForm  ]------------------------------------------------------------
void AvatarRace::getForm(Avatar::AvatarGender gender,
                         Evidyon::Actor::Tools::ActorFormReference* form) {
  dc::dcBufferStream buffer;
  gender_actors_[gender].save(&buffer);
  buffer.seek(0);
  form->load(&buffer);
}


}
}
}

