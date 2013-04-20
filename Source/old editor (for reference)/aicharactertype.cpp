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
////------------------------------------------------------------------------------------------------
//// File:    aicharactertype.cpp
////
//// Desc:    
////
////------------------------------------------------------------------------------------------------
//#include "stdafx.h"
//#include "allgameresources.h"
//#include "editresourcedialog.h"
//#include "projectvolucriseditor.h"
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  getString
//// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<ActorAnimation>
////------------------------------------------------------------------------------------------------
//template<> const char* dc::dcEnum<EditorActorFaction>::getString(EditorActorFaction value)
//{
//    switch(value)
//    {
//        case EDITORACTORFACTION_ROGUE:      return "Rogue";
//        case EDITORACTORFACTION_LAWFUL:     return "Lawful (Good)";
//        case EDITORACTORFACTION_NEUTRAL:    return "Neutral (Good)";
//        case EDITORACTORFACTION_CHAOTIC:    return "Chaotic (Good)";
//        case EDITORACTORFACTION_EVIL:       return "Evil";
//        default: return 0;
//    }
//}
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name:  staticTypeName
//// Desc:  Implements the class name for dc::dcEnum<ActorAnimation>
////------------------------------------------------------------------------------------------------
//std::string dc::dcEnum<EditorActorFaction>::staticTypeName()
//{
//    return "EditorActorFaction";
//}
//
//
//
//
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: AICharacterType
//// Desc: Initializes this class
////------------------------------------------------------------------------------------------------
//AICharacterType::AICharacterType()
//{
//    member("Actor Type", &myActorType);
//    member("Faction", &myFaction);
//    //member("Default Behavior", &myDefaultBehavior);
//    //member("Alerted Behavior", &myAlertedBehavior);
//    //member("Primary Attack", &myPrimaryAttack);
//    //member("Primary Attack Range", &myPrimaryAttackRange);
//
//    member("Experience", &myExperienceOffset);
//    member("Attack Damage", &myAttackDamageOffset);
//    member("Movement Speed", &myMovementSpeedOffset);
//    member("Attack Delay", &myAttackDelayOffset);
//    member("Hit Points (HP)", &myHPOffset);
//
//    member("Experience - Additional Per Level", &myExperienceFactor);
//    member("Attack Damage - Additional Per Level", &myAttackDamageFactor);
//    member("Movement Speed - Additional Per Level", &myMovementSpeedFactor);
//    member("Attack Delay - Additional Per Level", &myAttackDelayFactor);
//    member("Hit Points (HP) - Additional Per Level", &myHPFactor);
//
//    for (int i = 0; i < (int)ACTORATTACHMENTPOINT_COUNT; ++i)
//    {
//        std::string memberName = std::string("Equipment - ") + dc::dcEnum<ActorAttachmentPoint>::getString((ActorAttachmentPoint)i);
//        member(memberName, &myEquippedItems[i]);
//    }
//}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: compileForServer
//// Desc: Compiles this type into a game-file representation for use by the server
////------------------------------------------------------------------------------------------------
////bool AICharacterType::compileForServer(ServerAICharacterTypeDescription* description) const
////{
////    strncpy_s(description->name, MAX_AICHARACTER_NAME_LENGTH, getName().c_str(), MAX_AICHARACTER_NAME_LENGTH);
////
////    // Get the actor type reference
////    const dc::dcList<ActorType>::Element* actor = myActorType.getReferencedResource();
////    description->actorTypeIndex = actor ? actor->getIndex() : 0xFFFFFFFF;
////
////    // Set the behavior flags
////    description->defaultBehavior = myDefaultBehavior.getValue();
////    description->alertedBehavior = myAlertedBehavior.getValue();
////
////    // Obtain the attack effect
////    const dc::dcTable<Magic>::Element* attack = myPrimaryAttack.getReferencedResource();
////    description->attackEffect = attack ? attack->getIndex() : 0xFFFFFFFF;
////    description->attackRangeSq = myPrimaryAttackRange.getValue()*myPrimaryAttackRange.getValue();
////
////    // Save the attributes of this AI character
////    description->experience.offset = myExperienceOffset.getValue();
////    description->experience.factor = myExperienceFactor.getValue();
////    description->movementSpeed.offset = myMovementSpeedOffset.getValue();
////    description->movementSpeed.factor = myMovementSpeedFactor.getValue();
////    description->attackDamage.offset = myAttackDamageOffset.getValue();
////    description->attackDamage.factor = myAttackDamageFactor.getValue();
////    description->attackDelay.offset = myAttackDelayOffset.getValue();
////    description->attackDelay.factor = myAttackDelayFactor.getValue();
////    description->hp.offset = myHPOffset.getValue();
////    description->hp.factor = myHPFactor.getValue();
////
////    for (int i = 0; i < (int)ACTORATTACHMENTPOINT_COUNT; ++i)
////    {
////        const dc::dcList<Scenery>::Element* scenery = myEquippedItems[i].getReferencedResource();
////        description->equippedItems[i] = scenery ? scenery->getIndex() : 0xFFFFFFFF;
////    }
////
////    // Success
////    return true;
////}
//
//
//
////------------------------------------------------------------------------------------------------
//// Name: staticTypeName
//// Desc: Returns the type name of this class
////------------------------------------------------------------------------------------------------
//std::string AICharacterType::staticTypeName()
//{
//    return "AICharacterType";
//}
//
