#ifndef __EVIDYON_GAMEFILE__
#define __EVIDYON_GAMEFILE__

// THE STUFF IN HERE IS JUST STUCK IN TO MAKE THE GAME
// COMPILE IN ITS CURRENT STATE--IT WOULD REALLY BE GOOD
// TO GET RID OF IT!

namespace Evidyon {

/**
 * Defines different states that an actor can be in
 */
enum ActorState
{
    ACTORSTATE_DEAD,
    ACTORSTATE_NORMAL,
//    ACTORSTATE_WALKING,
    ACTORSTATE_SPIRITFORM,
//    ACTORSTATE_BLOCKING,

    ACTORSTATE_COUNT,
    ACTORSTATE_INVALID,
};




/**
 * Shared between editor and server; defines the different things that an AI character can be doing
 */
enum AICharacterBehavior
{
    /// This behavior prevents the character from doing anything
    BEHAVIOR_SLEEP,
    
    /// The character looks for something to attack
    BEHAVIOR_HUNTTARGET,
    
    /// The character wanders around and only attacks if attacked first
    BEHAVIOR_WANDER_DEFENSIVE,
};

}

#endif