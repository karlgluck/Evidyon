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
#include "packets.h"
#include <dc/debug>

#include "common/fracture/todo.h"

namespace Evidyon {
namespace Network {
namespace Packet {


//-----------------------------------------------------------------------------
// Name:  encodeMessageType
// Desc:  Writes the network message into the buffer
//-----------------------------------------------------------------------------
char* encodeMessageType(Message message, char* buffer, const char* endOfBuffer)
{
  // Make sure the buffer exists
  if (APP_ERROR(!buffer || (endOfBuffer < buffer))("Invalid buffer provided to encodeMessageType"))
    return 0;

  // Calculate the number of bytes left in the buffer
  size_t bytesLeftInBuffer = endOfBuffer - buffer;

  // Make sure the buffer has enough room in it for this message
  if (APP_ERROR(bytesLeftInBuffer < NETWORK_MESSAGE_SIZE)("encodeMessageType doesn't have enough buffer space"))
    return 0;

  // Write the message into the buffer
  *((unsigned __int8*)(buffer)) = (unsigned __int8)message;
  //memcpy_s(buffer, bytesLeftInBuffer, &message, NETWORK_MESSAGE_SIZE);

  // Return the next available byte
  return buffer + NETWORK_MESSAGE_SIZE;
}



//-----------------------------------------------------------------------------
// Name:  decodeMessageType
// Desc:  Reads the network message type from the packet.  This doesn't do anything to the packet.
//  If the packet is invalid or doesn't contain enough information to read the message, this
//  method returns false.
//-----------------------------------------------------------------------------
bool decodeMessageType(ENetPacket* packet, Message* message)
{
  // Make sure the packet is valid
  if (APP_ERROR(!packet)("'packet' parameter to decodeMessageType is invalid"))
  return false;

  // Decode the message using the overload
  return 0 != decodeMessageType((const char*)packet->data, (const char*)(packet->data + packet->dataLength), message);
}



//-----------------------------------------------------------------------------
// Name:  decodeMessageType
// Desc:  Reads the network message type from the provided buffer
//-----------------------------------------------------------------------------
const char* decodeMessageType(const char* sourceBuffer, const char* endOfSourceBuffer, Message* message)
{
  // Make sure the buffer exists
  if (APP_ERROR(!sourceBuffer || !message || (sourceBuffer + NETWORK_MESSAGE_SIZE > endOfSourceBuffer))
    ("Invalid parameter to decodeMessageType"))
  return 0;

  // Read out the message type
  unsigned char message_byte = *((unsigned char*)sourceBuffer);
  *message = (Message)message_byte;

  // Success
  return sourceBuffer + NETWORK_MESSAGE_SIZE;  
}




//-----------------------------------------------------------------------------
// Name:  encodeString
// Desc:  Takes the indicated string and writes it into the given buffer, and returns a pointer to
//  the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
//  is too small to contain the string.
//-----------------------------------------------------------------------------
char* encodeString(const std::string& in, char* buffer, const char* endOfBuffer)
{
  // Make sure the buffer exists
  if (APP_ERROR(!buffer || (endOfBuffer < buffer))("Invalid destination buffer provided to decodeString"))
  return 0;

  // The length of the source string that we will be putting in the buffer.  Note the difference
  // from the actual number of characters in the string!!  This is because our string includes
  // the null-terminator.
  size_t stringLength = in.length() + 1;

  // Make sure there is enough room in the buffer
  if (APP_ERROR(endOfBuffer < (buffer + (stringLength + sizeof(unsigned long))))
  ("encodeString doesn't have enough buffer space to hold \"%s\"", in.c_str()))
  return 0;

  // Build the header
  unsigned long magicLength = (NETWORK_STRING_MAGICNUMBER<<16)|((unsigned short)(stringLength&0xFFFF));
  magicLength = magicLength ^ NETWORK_STRING_MAGICLENGTHMASK;

  // Write the header
  *((unsigned long*)buffer) = magicLength;
  buffer += sizeof(unsigned long);

  // Write the text, including the terminating null character ('\0')
  memcpy_s(buffer, endOfBuffer - buffer, in.c_str(), stringLength);

  // Success!  Return the next available byte.
  return buffer + stringLength;
}



//-----------------------------------------------------------------------------
// Name:  encodeString
// Desc:  Takes the indicated string and writes it into the given buffer, and returns a pointer to
//  the next unwritten byte in the buffer.  If the return value is null (0) then the buffer
//  is too small to contain the string.
//-----------------------------------------------------------------------------
char* encodeString(const char* in, char* buffer, const char* endOfBuffer)
{
  // Lazy implementation
  return encodeString(std::string(in), buffer, endOfBuffer);
}




//-----------------------------------------------------------------------------
// Name:  decodeString
// Desc:  Reads a string that was encoded with encodeString from the given buffer
//-----------------------------------------------------------------------------
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, std::string* outString)
{
  // Make sure the buffer exists
  if (APP_ERROR(!sourceBuffer || (endOfSourceBuffer < (sourceBuffer + NETWORK_STRING_PADDING_BYTES)))
    ("Invalid source buffer provided to decodeString"))
  return 0;

  // Read the header
  unsigned long magicLength = *((unsigned long*)sourceBuffer);
  magicLength = magicLength ^ NETWORK_STRING_MAGICLENGTHMASK;
  unsigned short magicNumber  = (unsigned short)((magicLength >> 16) & 0xFFFF);
  unsigned short stringLength = (unsigned short)(magicLength & 0xFFFF);

  // Advance past the header
  sourceBuffer += sizeof(unsigned long);

  // Make sure this string is valid
  if (APP_WARNING(magicNumber != NETWORK_STRING_MAGICNUMBER)("Magic number of string wasn't matched during decodeString") ||
  APP_WARNING((endOfSourceBuffer - sourceBuffer) < stringLength)("String length indicates a string longer than the source buffer") ||
  APP_WARNING(sourceBuffer[stringLength-1] != '\0')("String was not null-terminated at the expected length"))
  return 0;

  // Read the string from the buffer
  todo("karl","this method of assignment fails if there is a \\0 somewhere within the string");
  // TODO: this method of assignment fails if there is a \0 somewhere within the string
  // itself (but this doesn't *really* matter).  The bigger issue is that invalid characters
  // get copied; there is no "alphabet" of valid characters.
  outString->assign(sourceBuffer);

  // Success!  Return the next available byte.
  return sourceBuffer + stringLength;
}




//-----------------------------------------------------------------------------
// Name:  decodeString
// Desc:  Reads a string that was encoded with encodeString from the given buffer
//-----------------------------------------------------------------------------
const char* decodeString(const char* sourceBuffer, const char* endOfSourceBuffer, char* outBuffer, size_t outBufferSize)
{
  // Confirm parameters.  The source parameters are managed by the submethod call
  if (APP_ERROR(!outBuffer ||
                 (outBufferSize <= 1) || // need at LEAST space for a null-terminator
                ((endOfSourceBuffer - sourceBuffer) < NETWORK_STRING_PADDING_BYTES))("Invalid destination buffer to decodeString"))
  return 0;

  // Lazy implementation
  std::string outString;
  const char* retval = decodeString(sourceBuffer, endOfSourceBuffer, &outString);

  // Check the return value
  if (!retval) return retval;

  // Output the string
  size_t stringLength = outString.length();
  size_t out_buffer_char_size = outBufferSize - 1;
  strncpy_s(outBuffer,
            outBufferSize,
            outString.c_str(),
            min(stringLength, out_buffer_char_size));

  // Return the value from the submethod
  return retval;
}



//-----------------------------------------------------------------------------
// Name:  encodeMessageOnlyPacket
// Desc:  Creates a packet with no extra data that holds only a message
//-----------------------------------------------------------------------------
ENetPacket* encodeMessageOnlyPacket(Message message)
{
  // Allocate a packet for this message
  ENetPacket* packet = enet_packet_create(0, NETWORK_MESSAGE_SIZE, ENET_PACKET_FLAG_RELIABLE);
  char* packetBuffer = 0;

  // If we obtained the packet, write information into it
  if (packet)
  {
  packetBuffer = (char*)packet->data;
  const char* packetBufferEnd = packetBuffer + packet->dataLength;

  // Write the packet
  packetBuffer = encodeMessageType(message, packetBuffer, packetBufferEnd);
  }

  // Make sure compilation succeeded
  if (APP_ERROR(!packetBuffer)("Couldn't compile packet during encodeMessageOnlyPacket for %lu", message))
  {
  // Free the memory for the packet
  enet_packet_destroy(packet);

  // Fail
  return 0;
  }

  // Success
  return packet;
}


//-----------------------------------------------------------------------------
// Name:  decodeMessageOnlyPacket
// Desc:  Ensures that the packet in the parameter represents a message of the provided type
//-----------------------------------------------------------------------------
bool decodeMessageOnlyPacket(ENetPacket* packet, Message messageToConfirm)
{
  // Make sure the packet is valid
  if (APP_ERROR(!packet)("invalid parameter to decodeMessageOnlyPacket"))
  return false;

  // Obtain pointers to packet data
  const char* buffer = (const char*)packet->data;
  const char* endOfBuffer = (const char*)packet->data + packet->dataLength;

  // Make sure that the message type is correct
  Message message;
  if (!(buffer = decodeMessageType(buffer, endOfBuffer, &message)) ||
  message != messageToConfirm)
  return false;

  // Provide feedback about processing
  if (buffer != endOfBuffer) DEBUG_WARNING("decodeMessageOnlyPacket message (%lu) contained extra data", messageToConfirm);

  // Success
  return true;
}










bool createPacket(bool reliable, Message message, size_t payload_size,
      ENetPacket** packet_out, char** start, const char** end) {
  ENetPacket* packet = enet_packet_create(0,
            NETWORK_MESSAGE_SIZE + payload_size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  if (!packet) return false;
  char* buffer = (char*)packet->data;
  const char* buffer_end = buffer + packet->dataLength;
  buffer = encodeMessageType(message, buffer, buffer_end);
  if (!buffer) {
  enet_packet_destroy(packet);
  return false;
  }
  *packet_out = packet;
  *start = buffer;
  *end = buffer_end;
  return true;
}

bool validateAndTrimPacket(ENetPacket* packet, char* next_unwritten_byte) {
  if (!packet || !next_unwritten_byte) {
    enet_packet_destroy(packet);
    return false;
  }
  enet_packet_resize(packet, next_unwritten_byte  - (char*)packet->data);
  return true;
}

bool openPacket(ENetPacket* packet,
         Message message,
         const char** payload,
         const char** end) {
  if (!packet || !payload || !end) return false;

  const char* buffer = (const char*)packet->data; // parse out the buffer pointers
  const char* buffer_end = buffer + packet->dataLength;

  Message message_encoded; // check to make sure the message is correct
  if (!(buffer = decodeMessageType(buffer, buffer_end, &message_encoded)) ||
        message_encoded != message)
  return false;

  *payload = buffer;  // assign output parameters
  *end = buffer_end;

  return true;
}

//
//MD5Hash& MD5Hash::operator = (const MD5Hash& rhs) {
//#define COPY(base)  data_[4*base+0] = rhs.data_[4*base+0]; data_[4*base+1] = rhs.data_[4*base+1];\
//                    data_[4*base+2] = rhs.data_[4*base+2]; data_[4*base+3] = rhs.data_[4*base+3];
//  COPY(0);
//  COPY(1);
//  COPY(2);
//  COPY(3);
//#undef COPY
//  return *this;
//}

}
}
}