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
#ifndef __EVIDYONPACKETSERIALIZER_H__
#define __EVIDYONPACKETSERIALIZER_H__
#pragma once


#include "../../common/serializer/serializer.h"
#include "../../common/serializer/serializer_begin.h"

namespace Evidyon {
namespace Network {
namespace Packet {
enum Message;
}
}
}



static const unsigned int MAX_PACKET_SIZE = 512;
typedef Evidyon::Network::Packet::Message PacketMessage;
typedef unsigned short TxPacketMessage;

#define WRITEPACKET_NOPARAMS()  static ENetPacket* writePacket(unsigned int flags) { ENetPacket* packet = enet_packet_create(0,sizeof(TxPacketMessage),flags); *((TxPacketMessage*)(packet->data))=MSG; return packet; }
#define WRITEPACKET(base, plus) template <TYPES(base,plus)> static ENetPacket* writePacket(unsigned int flags, W_PARAMS(base,plus)) { char data[MAX_PACKET_SIZE]; char* buffer=data; const char* end = data+MAX_PACKET_SIZE; buffer=SerializerWrite<TxPacketMessage>(MSG,buffer,end); W_BODY(base,plus); if (buffer == 0) return 0; ENetPacket* packet = enet_packet_create(0, buffer-data, flags); memcpy(packet->data, data, packet->dataLength); return packet; }
#define READPACKET_NOPARAMS()   static bool readPacket(ENetPacket* packet) { return (packet->dataLength == sizeof(TxPacketMessage)) && (*((TxPacketMessage*)(packet->data)) == MSG); }
#define READPACKET(base, plus)  template <TYPES(base,plus)> static bool readPacket(ENetPacket* packet, R_PARAMS(base,plus)) { const char* buffer=(const char*)packet->data; const char* end = buffer+packet->dataLength; TxPacketMessage _msg; buffer=SerializerRead<TxPacketMessage>(buffer, end, &_msg); if (!buffer || PacketMessage(_msg) != MSG) return false; R_BODY(base,plus); return buffer != 0; }


template <const PacketMessage MSG, TLIST_64> class EvidyonPacketSerializer {
public: SERIALIZER_READ64(PACKET);
public: SERIALIZER_WRITE64(PACKET);
};

#include "../../common/serializer/serializer_end.h"


#endif