#ifndef __EVIDYON_NETWORK_H__
#define __EVIDYON_NETWORK_H__
#pragma once



namespace Evidyon {
namespace Network {
static const unsigned short SERVER_PORT = 12601;

static const float SLOW_SYNC_PERIOD = 1.0f / 4.0f;
static const float FAST_SYNC_PERIOD = 1.0f / 10.0f;

// 'float' is packed into 16 bits; this many bits are for places after the decimal
static const unsigned int FLOAT_PACK_DECIMAL_BITS = 5;
static const float FLOAT_UNPACK_MULTIPLIER = 1.0f / 32.0f;

}
}

#endif