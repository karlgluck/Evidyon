#ifndef __EVIDYON_MAP_H__
#define __EVIDYON_MAP_H__
#pragma once


namespace Evidyon {

enum MapNavigability
{
  MAPNAV_WALK,
  MAPNAV_WADE,
  MAPNAV_SWIM,
  MAPNAV_PIT,
  MAPNAV_LOWWALL,
  MAPNAV_HIGHWALL,
  MAPNAV_IMPASSABLE,
  MAPNAV_SAFEZONE,

  MAPNAV_COUNT,
};

}

#endif