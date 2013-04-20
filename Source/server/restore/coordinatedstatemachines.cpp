// flying = not hit by projectiles, can't be flown over
// tall = hit by projectiles, can't be flown over
// normal = hit by projectiles & can be flown over
// short = can't be hit by projectiles
enum ActorSize {
  FLYING_SMALL,
  FLYING_NORMAL,
  FLYING_BIG,
  FLYING_HUGE
  TALL_SMALL,
  TALL_NORMAL,
  TALL_BIG,
  TALL_HUGE,
  NORMAL_SMALL,
  NORMAL_NORMAL,
  NORMAL_BIG,
  NORMAL_HUGE,
  SHORT_SMALL,
  SHORT_NORMAL,
};






//----[  Client  ]-------------------------------------------------------------
// Terminology:
//  Client - all data/meta-data relating to the player, his/her terminal, the
//           network connection.
//  Player - data that interfaces between the person behind the remote client
//           and the Avatar as a life-form.  Interaction that occurs with menus
//           such as the inventory, storage, bazaar, geosids, etc. belongs in
//           the Client (even if it is based on the Player's state)
//  Avatar - the character that the Player controls
class Client {
public:

  // Pre-Player::update()
  //  free play time expired? message, + set grace period
  //  grace period expired? idle log off timer expired? -> leave world
  // Post-Player::update()
  //  send inventory / storage updates
  //  if Player::leavingWorld(), log the player out.
  void update(double time, double time_since_last_update);


private:
  Time::Timer<Time::RESOLUTION_60000_MS> idle_logoff_timer_;
  Time::Timer<Time::RESOLUTION_60000_MS> free_play_time_expires_;
  Time::Timer<Time::RESOLUTION_60000_MS> free_play_time_expires_grace_period_;
  ActorPointer actor_;
  Player player_;
  PlayerRequest player_request_;
  AvatarEnchantments enchantments_;
  AvatarInstance avatar_;
  ClientItemsInterface items_;
  ClientToClientItemTrade trade_;


};