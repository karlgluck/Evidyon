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
#ifndef __CLIENT_INLINE_H__
#define __CLIENT_INLINE_H__
#pragma once




//----[  getPeer  ]------------------------------------------------------------
NetworkPeer* Client::getPeer() {
  return peer_;
}



//----[  isValid  ]------------------------------------------------------------
bool Client::isValid() const {
  return peer_ && (state_ != INVALID);
}



//----[  getLoggedIntoAccountID  ]---------------------------------------------
unsigned int Client::getLoggedIntoAccountID() const {
  return (state_ == LOGGED_INTO_ACCOUNT ||
          state_ == ENTERING_WORLD ||
          state_ == LOGGED_INTO_CHARACTER) ? account_id_ : 0;
}



//----[  getLoggedIntoCharacterID  ]-------------------------------------------
unsigned int Client::getLoggedIntoCharacterID() const {
  return state_ == LOGGED_INTO_CHARACTER ? character_id_ : 0;
}






//----[  getActor  ]-----------------------------------------------------------
const ActorPointer* Client::getActor() const {
  return &actor_;
}



//----[  hasCurrency  ]--------------------------------------------------------
bool Client::hasCurrency(CurrencyType amount) const {
  return avatar_.money >= amount;
}



//----[  giveCurrency  ]-------------------------------------------------------
void Client::giveCurrency(CurrencyType amount) {
  avatar_.money += amount;
}



//----[  takeCurrency  ]-------------------------------------------------------
CurrencyType Client::takeCurrency(CurrencyType amount) {
  CurrencyType own_currency = avatar_.money;
  if (own_currency <= amount) return 0;
  own_currency -= amount;
  avatar_.money = own_currency;
  return amount;
}



//----[  isEvil  ]-------------------------------------------------------------
bool Client::isEvil() const {
  switch (getAlignmentClass()) {
  default:
  case Actor::ALIGNMENT_ADMIN:
  case Actor::ALIGNMENT_GUARDIAN:
  case Actor::ALIGNMENT_CHAMPION:
  case Actor::ALIGNMENT_GOOD:
  case Actor::ALIGNMENT_NEUTRAL:
    return false;
  case Actor::ALIGNMENT_EVIL:
  case Actor::ALIGNMENT_DESTROYER:
  case Actor::ALIGNMENT_DREADLORD:
    return true;
  }
}



//----[  isInIntroductoryLevels  ]---------------------------------------------
bool Client::isInIntroductoryLevels() const {
  // admin chars are always in the intro levels
  return isAdmin() || avatar_.level < 4; // level 1-4 are introductory
}



//----[  attackedBy  ]---------------------------------------------------------
bool Client::attackedBy(const Client* source_client) const {
  // TODO: this relies on knowing that the history length is 6
  return attacker_history_[0].source_client == source_client ||
         attacker_history_[1].source_client == source_client ||
         attacker_history_[2].source_client == source_client ||
         attacker_history_[3].source_client == source_client ||
         attacker_history_[4].source_client == source_client ||
         attacker_history_[5].source_client == source_client;
}


//----[  getAggressorClientCharacterID  ]--------------------------------------
unsigned int Client::getAggressorClientCharacterID(const Client* source_client) const {
#define LINE(i) ((attacker_history_[i].source_client == source_client) \
                        ? attacker_history_[i].character_id : 0)
  return LINE(0) | LINE(1) | LINE(2) | LINE(3) | LINE(4) | LINE(5);
#undef LINE
}




//----[  inCombat  ]-----------------------------------------------------------
bool Client::inCombat() const {
#define LINE(i) (attacker_history_[i].source_client ? \
                 attacker_history_[i].source_client->attackedBy(this) : false)
  return LINE(0) || LINE(1) || LINE(2) || LINE(3) || LINE(4) || LINE(5);
#undef LINE
}



//----[  mostFavorableZoneRules  ]---------------------------------------------
Zone::ZoneRules Client::mostFavorableZoneRules() const {
  Zone::ZoneRules safest = getLocalZoneRules();
  Zone::ZoneRules check;
#define LINE(i)\
  if (attacker_history_[i].source_client) {\
  check = attacker_history_[i].source_client->getLocalZoneRules();\
  if (check > safest) safest = check; }
  LINE(0); LINE(1); LINE(2); LINE(3); LINE(4); LINE(5); 
#undef LINE
  return safest;
}



//----[  isAdmin  ]------------------------------------------------------------
bool Client::isAdmin() const {
  return is_admin_;
}


#endif