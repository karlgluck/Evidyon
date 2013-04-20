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
//// File:    aicharactertype.h
////
//// Desc:    
////
////------------------------------------------------------------------------------------------------
//#ifndef __AICHARACTERTYPE_H__
//#define __AICHARACTERTYPE_H__
//
//
///**
// * This is a version of ActorFaction that is valid to use with dc::dcEnum
// */
//enum EditorActorFaction
//{
//    EDITORACTORFACTION_ROGUE,
//    EDITORACTORFACTION_LAWFUL,
//    EDITORACTORFACTION_NEUTRAL,
//    EDITORACTORFACTION_CHAOTIC,
//    EDITORACTORFACTION_EVIL,
//
//    EDITORACTORFACTION_COUNT,
//    EDITORACTORFACTION_INVALID,
//};
//
//
///**
// * Defines a computer-controlled character
// */
//class AICharacterType : public dc::dcResource<AICharacterType>
//{
//    public:
//
//        /**
//         * Initializes this class
//         */
//        AICharacterType();
//
//        /**
//         * Compiles this type into a game-file representation for use by the server
//         */
//        //bool compileForServer(ServerAICharacterTypeDescription* description) const;
//
//        void setActorType(dcList<ActorType>::Element* actorType) { myActorType.setReferencedResource(actorType); }
//
//    public:
//
//        /**
//         * Returns the type name of this class
//         */
//        static std::string staticTypeName();
//
//
//    protected:
//
//        /// The actor that this character uses to represent itself in the world
//        dc::dcList<ActorType>::Reference myActorType;
//
//        /// Defines the kind of targets this actor should acquire
//        dc::dcEnum<EditorActorFaction> myFaction;
//
//        /// How much experience this character gives when killed by a member of the opposing faction
//        dc::dcInteger myExperienceOffset, myExperienceFactor;
//
//        dc::dcInteger myAttackDamageOffset, myAttackDamageFactor;
//        dc::dcFloat myMovementSpeedOffset, myMovementSpeedFactor;
//        dc::dcFloat myAttackDelayOffset, myAttackDelayFactor;
//        dc::dcInteger myHPOffset, myHPFactor;
//
//        dc::dcList<Scenery>::Reference myEquippedItems[ACTORATTACHMENTPOINT_COUNT];
//};
//
//
//#endif