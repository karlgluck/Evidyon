#include "shared/evidyon_world.h"


namespace Evidyon {

namespace World {


RegionID regionIDFromPoint(float x, float y) {
  return regionIDFromPoint(
  (int)((x<0.0f)?(x-1.0f):(x)),
  (int)((y<0.0f)?(y-1.0f):(y)));
}

RegionID regionIDFromPoint(float x, float y, short* region_x, short* region_y) {
  return regionIDFromPoint(
  (int)((x<0.0f)?(x-1.0f):(x)),
  (int)((y<0.0f)?(y-1.0f):(y)), region_x, region_y);
}

RegionID regionIDFromPoint(int x, int y) {
  short px = x/Evidyon::World::REGION_SIZE-(x<0?1:0);
  short py = y/Evidyon::World::REGION_SIZE-(y<0?1:0);
  return regionIDFromRegionCoordinates(px, py);
}

RegionID regionIDFromPoint(int x, int y, short* region_x, short* region_y) {
  short px = x/Evidyon::World::REGION_SIZE-(x<0?1:0);
  short py = y/Evidyon::World::REGION_SIZE-(y<0?1:0);
  *region_x = px;
  *region_y = py;
  return regionIDFromRegionCoordinates(px, py);

}

RegionID regionIDFromRegionCoordinates(short region_x, short region_y) {
  RegionID upx = *reinterpret_cast<RegionID*>(&region_x);
  RegionID upy = *reinterpret_cast<RegionID*>(&region_y);
  // the bitmask is VERY IMPORTANT otherwise region IDs for
  // negative coordinates are not unique
  return ((upx<<16)&0xFFFF0000)|(upy&0x0000FFFF);
}

}

}