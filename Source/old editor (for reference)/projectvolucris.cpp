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

#include "stdafx.h"
#include "allgameresources.h"



//------------------------------------------------------------------------------------------------
// Name:  ProjectVolucris
// Desc:  Initializes this class
//------------------------------------------------------------------------------------------------
ProjectVolucris::ProjectVolucris()
{
  member("Volucris", this);
  member("Resource Packs", &myResourcePacks);
  member("Font Image", &myFontImage);
  member("Title Main Image", &myTitleMainImage);
  member("Title Background Image", &myTitleBackgroundImage);
  member("GUI Image", &myGUIImage);
  member("Evidyon Symbol", &myEvidyonSymbolImage);
  member("Day/Night Light Particle", &myDayNightLightParticleImage);
  member("Images", &myImages);
  member("Textures", &myTextures);
  member("Skins", &mySkins);
  member("Meshes", &myMeshes);
  member("Scenery", &myScenery);
  member("Sounds", &myWAVSoundList);
  member("Footstep Sound", &myFootstepSound);
  member("Button Sound", &myButtonSound);
  member("Skinned Meshes", &mySkinnedMeshes);
  member("Skinned Mesh Animations", &mySkinnedMeshAnimations);
  member("Terrain", &myTerrain);
  member("Terrain Rules", &myTerrainRules);
  member("Elevations", &myElevations);
  member("VisualFX Images", &myVisualFXImages);
  member("VisualFX", &myVisualFX);
  member("MagicFX", &myMagicFX);
  member("Physical Hit Magic FX", &myPhysicalHitMagicFX);
  member("Spawn Magic FX", &mySpawnMagicFX);
  member("Projectiles", &myProjectiles);
  member("Enchantments", &myEnchantments);
  member("Magic", &myMagic);
  member("Spells", &mySpells);
  member("Items", &myItems);
  member("Quests", &quests_);
  member("Actor Types", &myActorTypes);
  member("Treasure Generation", &treasure_generation_);
  member("Lifeform AI", &myLifeformAITable);
  member("World Atlases", &myAtlases);
  member("Credits", &myCredits);
  member("Races", &avatar_race_list_);
  member("Classes", &avatar_class_list_);
  member("Level Definitions", &myLevelDefinitions);
}


//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  The name of this resource type
//------------------------------------------------------------------------------------------------
std::string ProjectVolucris::staticTypeName()
{
    return "Project Volucris";
}


//------------------------------------------------------------------------------------------------
// Name:  clear
// Desc:  Erases all of the information in this structure
//------------------------------------------------------------------------------------------------
void ProjectVolucris::clear()
{
    myResourcePacks.clear();
    myFontImage.setGenericReferencedResource(NULL);
    myGUIImage.setGenericReferencedResource(NULL);
    myTitleMainImage.setGenericReferencedResource(NULL);
    myTitleBackgroundImage.setGenericReferencedResource(NULL);
    myEvidyonSymbolImage.setGenericReferencedResource(NULL);
    myDayNightLightParticleImage.setGenericReferencedResource(NULL);
    myImages.clear();
    myTextures.clear();
    mySkins.clear();
    myMeshes.clear();
    myScenery.clear();
    myTerrain.clear();
    myTerrainRules.clear();
    myCredits.clear();
    myTerrain.clear();
    myElevations.clear();
    myAtlases.clear();
    myVisualFXImages.clear();
    myMagicFX.clear();
    myVisualFX.clear();
    myMagic.clear();
    mySpells.clear();
    myProjectiles.clear();
    myEnchantments.reset();
    mySkinnedMeshes.clear();
    mySkinnedMeshAnimations.clear();
    myActorTypes.clear();
    //avatar_race_list_.clear();
    //avatar_class_list_.clear();
    myItems.clear();
    myCredits.clear();
    myItems.clear();
    myLifeformAITable.clear();
    myWAVSoundList.clear();
}



void ProjectVolucris::getReservedNames(std::list<std::string>* names) {
  {
    const dcGenericResource::Array& races = avatar_race_list_.getMembers();
    for (dcGenericResource::Array::const_iterator i = races.begin(); i != races.end(); ++i) {
      names->push_back((*i)->getName());
    }
  }
  {
    const dcGenericResource::Array& classes = avatar_class_list_.getMembers();
    for (dcGenericResource::Array::const_iterator i= classes.begin(); i != classes.end(); ++i) {
      names->push_back((*i)->getName());
    }
  }
  {
    const dcGenericResource::Array& lifeforms = myLifeformAITable.getMembers();
    for (dcGenericResource::Array::const_iterator i = lifeforms.begin(); i != lifeforms.end(); ++i) {
      dc::dcTable<LifeformAI>::Element* lifeform_ai =
        (dc::dcTable<LifeformAI>::Element*)(*i);
      names->push_back(lifeform_ai->getImplementation()->getLifeformName());
    }
  }

}