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
#include <windows.h>
#include <memory>
#include <math.h>
#include "common/fixedpoint/fixedpoint.h"
#include <assert.h>
#include "common/number.h"
#include "common/randint32.h"
#define DC_ENABLE_DEBUGGING
#include <dc/debug>
#include <dcx/win32>
#include <d3dx9math.h>
#include <dcx/bitmask>
#include "common/sqlite/sqlite3.h"
#include "common/randf.h"
//
//typedef apc::call<EnchantmentType,
//apc::copied_array<MagicIndex, unsigned short>> Type;
//
//Type::serialize(i, Type::param2::from_stl_container(enchantment_magics_));
//
//void setEnchantments(EnchantmentType enchantment, MagicIndex* indices, size_t number_of_indices) {
//}
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")

enum Message {
  MESSAGE_DEBUG_1,
  MESSAGE_DEBUG_2,
};

class Tasklet {
public:
  virtual DWORD route(Message message) = 0;
};

class DemoTasklet : public Tasklet {
public:
  void debug(unsigned int value1, unsigned int value2) {
    //DEBUG_INFO("value = %lu, %lu", value1, value2);
  }
  void debug2(unsigned int value1, unsigned int value2) {
    //DEBUG_INFO("nope = %lu, %lu", value1, value2);
  }

  virtual DWORD route(Message message) {
    DWORD rc;
    switch (message) {
    case MESSAGE_DEBUG_1: __asm { mov ecx, dword ptr [DemoTasklet::debug] } break;
    case MESSAGE_DEBUG_2: __asm { mov ecx, dword ptr [DemoTasklet::debug2] } break;
    default:
      __asm mov ecx, 0
    }
    __asm mov rc, ecx
    return rc;
  }
};



#include <setjmp.h>

namespace apc {

// (no wrapper) invocation parameters:  (Type value)

// invocation parameters: const char* characters, size_t length
template <typename apc_array> struct auto_string {
  auto_string(const char* ch) : characters(ch), length(strlen(ch)) {}
  auto_string(const std::string& str) : characters(str.c_str()), length(str.length()) {}
  const char* characters;
  size_t length;
};

// an array whose elements can just be directly copied into the storage buffer
//  invocation parameters: (_E* elements, size_t length)
template <typename _E, typename _S, const size_t _LIMIT> struct copied_array {
  typedef auto_string<copied_array<_E,_S,_LIMIT>> auto_string;
  typedef _E Element;
  typedef _S Size;
  static const size_t LIMIT = _LIMIT;
  static const size_t BYTE_LIMIT = sizeof(_E) * _LIMIT;
  copied_array(const Element* e, Size s) : elements(e), size(s) {}
  copied_array(auto_string& rhs) : elements(rhs.characters), size(rhs.length) {}
  const Element* elements;
  Size size;
};

// invocation parameters:  (_S* structure)
template <typename _S> struct pointer {
  typedef _S Structure;
  pointer(Structure* s) : structure(s) {}
  Structure* structure;
};

// invocation parameters:  ()
struct __void { explicit __void() {} __void(__void&) {} private: void operator =(__void&); };

template <typename _V> char* __serialize(jmp_buf err_jmp, char* buffer, _V v) {
  *((_V*)buffer) = v;
  return buffer + sizeof(v);
}

template <> char* __serialize(jmp_buf err_jmp, char* buffer, __void) {
  return buffer;
}

template <typename _S> char* __serialize(jmp_buf err_jmp, char* buffer, pointer<_S> p1) {
  memcpy(buffer, p1.structure, sizeof(_S));
  return buffer + sizeof(_S);
}

template <typename _E, typename _S, const size_t _LIMIT> char* __serialize(jmp_buf err_jmp, char* buffer, copied_array<_E,_S,_LIMIT> p1) {
  *((_S*)buffer) = p1.size;
  buffer += sizeof(_S);
  size_t bytes = p1.size * sizeof(_E);
  if (p1.LIMIT < p1.size) longjmp(err_jmp, -1);
  memcpy(buffer, p1.elements, bytes);
  return buffer + bytes;
}

template <typename _V> struct __deserialize {
  __deserialize(jmp_buf err_jmp, const char** data, const char* data_end, char** args) {
    const char* data_ptr = *data;
    const char* data_next = data_ptr + sizeof(_V);
    if (data_next > data_end) longjmp(err_jmp, -1);
    char* args_ptr = *args;
    *(_V*)(args_ptr) = *(_V*)(data_ptr);
    *args = args_ptr + sizeof(_V);
    *data = data_ptr + sizeof(_V);
  }
};

template <typename _S> struct __deserialize<pointer<_S>> {
  __deserialize(jmp_buf err_jmp, const char** data, const char* data_end, char** args) {
    const char* data_ptr = *data;
    const char* data_next = data_ptr + sizeof(_S);
    if (data_next > data_end) longjmp(err_jmp, -1);
    char* args_ptr = *args;
    *(_S**)(args_ptr) = (_S*)(data_ptr);
    *args = args_ptr + sizeof(_S*);
    *data = data_ptr + sizeof(_S);
  }
};

template <typename _E, typename _S, const size_t _LIMIT> struct __deserialize<copied_array<_E,_S,_LIMIT>> {
  __deserialize(jmp_buf err_jmp, const char** data, const char* data_end, char** args) {
    const char* data_ptr = *data;
    char* args_ptr = *args;
    size_t elements = *(_S*)(data_ptr);
    data_ptr += sizeof(_S);
    size_t bytes = elements * sizeof(_E);
    const char* data_next = data_ptr + bytes;
    if ((data_next > data_end) || (_LIMIT < elements)) longjmp(err_jmp, -1);
    *(_E**)(args_ptr) = (_E*)(data_ptr);
    args_ptr += sizeof(_E*);
    *(size_t*)(args_ptr) = elements;
    *args = args_ptr + sizeof(size_t);
    *data = data_next;
  }
};

template <> struct __deserialize<__void> {
  __deserialize(jmp_buf err_jmp, const char** data, const char* data_end, char**args) {}
};


// Used to calculate the minimum number of bytes that would create a valid apc
template <typename _T> struct __min_bytes { static const size_t bytes = sizeof(_T); };
template <> struct __min_bytes<__void> { static const size_t bytes = 0; };
template <typename _S> struct __min_bytes<pointer<_S>> { static const size_t bytes = sizeof(_S); };
template <typename _E, typename _S, const size_t _LIMIT> struct __min_bytes<copied_array<_E,_S,_LIMIT>> { static const size_t bytes = sizeof(_S); };


// Used to calculate the maximum number of bytes used by an apc's message buffer
template <typename _T> struct __max_bytes { static const size_t bytes = sizeof(_T); };
template <> struct __max_bytes<__void> { static const size_t bytes = 0; };
template <typename _S> struct __max_bytes<pointer<_S>> { static const size_t bytes = sizeof(_S); };
template <typename _E, typename _S, const size_t _LIMIT> struct __max_bytes<copied_array<_E,_S,_LIMIT>> { static const size_t bytes = _LIMIT * sizeof(_E) + sizeof(_S); };


// Used to calculate the number of bytes needed for the arguments
template <typename _T> struct __args_bytes { static const size_t bytes = sizeof(_T); };
template <> struct __args_bytes<__void> { static const size_t bytes = 0; };
template <typename _S> struct __args_bytes<pointer<_S>> { static const size_t bytes = sizeof(_S*); };
template <typename _E, typename _S, const size_t _LIMIT> struct __args_bytes<copied_array<_E,_S,_LIMIT>> { static const size_t bytes = sizeof(_E*) + sizeof(size_t); };



template <typename _T, typename _F> void __invoke_thiscall(_T* this_ptr, _F function, const void* args, size_t args_bytes) {
  //DWORD rc;
  __asm {
    mov ecx, args_bytes
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    mov ecx, this_ptr
    call [function]
    //mov rc, eax
    //add esp, sz
  }
}

template <typename _F> void __invoke_cdecl(_F function, const void* args, size_t args_bytes) {
  //DWORD rc;
  __asm {
    mov ecx, args_bytes
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    call [method]
    //mov rc, eax
    //add esp, sz
  }
}

template <typename _F> void __invoke_stdcall(_F function, const void* args, size_t args_bytes) {
  //DWORD rc;
  __asm {
    mov ecx, args_bytes
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    call [method]
    //mov rc, eax
    add esp, sz
  }
}


template <typename _00 = __void, typename _01 = __void, typename _02 = __void, typename _03 = __void,
          typename _04 = __void, typename _05 = __void, typename _06 = __void, typename _07 = __void,
          typename _08 = __void, typename _09 = __void, typename _0A = __void, typename _0B = __void,
          typename _0C = __void, typename _0D = __void, typename _0E = __void, typename _0F = __void>
struct call {
  typedef _00 t00; typedef _01 t01; typedef _02 t02; typedef _03 t03;
  typedef _04 t04; typedef _05 t05; typedef _06 t06; typedef _07 t07;
  typedef _08 t08; typedef _09 t09; typedef _0A t0A; typedef _0B t0B;
  typedef _0C t0C; typedef _0D t0D; typedef _0E t0E; typedef _0F t0F;

  static const size_t min_bytes // immediately discard call if its buffer length is < this value
    = __min_bytes<_00>::bytes + __min_bytes<_01>::bytes + __min_bytes<_02>::bytes + __min_bytes<_03>::bytes + 
      __min_bytes<_04>::bytes + __min_bytes<_05>::bytes + __min_bytes<_06>::bytes + __min_bytes<_07>::bytes + 
      __min_bytes<_08>::bytes + __min_bytes<_09>::bytes + __min_bytes<_0A>::bytes + __min_bytes<_0B>::bytes + 
      __min_bytes<_0C>::bytes + __min_bytes<_0D>::bytes + __min_bytes<_0E>::bytes + __min_bytes<_0F>::bytes;
  static const size_t max_bytes // reserve this much space for this call's buffer
    = __max_bytes<_00>::bytes + __max_bytes<_01>::bytes + __max_bytes<_02>::bytes + __max_bytes<_03>::bytes + 
      __max_bytes<_04>::bytes + __max_bytes<_05>::bytes + __max_bytes<_06>::bytes + __max_bytes<_07>::bytes + 
      __max_bytes<_08>::bytes + __max_bytes<_09>::bytes + __max_bytes<_0A>::bytes + __max_bytes<_0B>::bytes + 
      __max_bytes<_0C>::bytes + __max_bytes<_0D>::bytes + __max_bytes<_0E>::bytes + __max_bytes<_0F>::bytes;
  static const size_t args_bytes // reserve this much space for the function-call arguments
    = __args_bytes<_00>::bytes + __args_bytes<_01>::bytes + __args_bytes<_02>::bytes + __args_bytes<_03>::bytes + 
      __args_bytes<_04>::bytes + __args_bytes<_05>::bytes + __args_bytes<_06>::bytes + __args_bytes<_07>::bytes + 
      __args_bytes<_08>::bytes + __args_bytes<_09>::bytes + __args_bytes<_0A>::bytes + __args_bytes<_0B>::bytes + 
      __args_bytes<_0C>::bytes + __args_bytes<_0D>::bytes + __args_bytes<_0E>::bytes + __args_bytes<_0F>::bytes;

  // Writes the parameters for the method call into the buffer.  Will generate compile errors
  // if the wrong number of arguments is provided.  Be sure that data is at least max_bytes
  // in length.  Returns the number of bytes written.  Returns false on error; otherwise,
  // sets *bytes to the amount written into the buffer.
  static bool serialize(char* data, size_t* bytes,
          _00 p00 = __void(), _01 p01 = __void(), _02 p02 = __void(), _03 p03 = __void(),
          _04 p04 = __void(), _05 p05 = __void(), _06 p06 = __void(), _07 p07 = __void(),
          _08 p08 = __void(), _09 p09 = __void(), _0A p0A = __void(), _0B p0B = __void(),
          _0C p0C = __void(), _0D p0D = __void(), _0E p0E = __void(), _0F p0F = __void()) {
      jmp_buf err_jmp;
      if (0 != setjmp(err_jmp)) return false; // used instead of exceptions to handle errors
      char* start = data;
      data = __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, data, p00), p01), p02), p03);
      data = __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, data, p04), p05), p06), p07);
      data = __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, data, p08), p09), p0A), p0B);
      data = __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, __serialize(err_jmp, data, p0C), p0D), p0E), p0F);
      *bytes = data - start;
      return true;
  }

  // Reads the call from the data buffer, and writes out the arguments so that the method
  // can be invoked.  Returns false if the buffer did not contain enough data or an array
  // was corrupted (length > upper limit); otherwise, *args_bytes_out is set to the number
  // of bytes used in the arguments.  Be sure the args buffer is at least args_bytes in
  // length.
  static bool deserialize(const char* data, size_t bytes, char* args, size_t* args_bytes_out) {
    jmp_buf err_jmp;
    if (bytes < min_bytes || (0 != setjmp(err_jmp))) return false;
    const char* data_ptr = data;
    const char* data_end = data + bytes;
    char* args_ptr = args;
    __deserialize<_00>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_01>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_02>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_03>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_04>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_05>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_06>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_07>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_08>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_09>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0A>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0B>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0C>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0D>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0E>(err_jmp, &data_ptr, data_end, &args_ptr);
    __deserialize<_0F>(err_jmp, &data_ptr, data_end, &args_ptr);
    *args_bytes_out = args_ptr - args;
    return true;
  }

  template <typename _T, typename _F>
      static void invoke_thiscall(_T* this_ptr, _F function, const char* data, size_t bytes, char* args_buffer) {
    size_t args_bytes;
    if (deserialize(data, bytes, args_buffer, &args_bytes)) {
      __invoke_thiscall(this_ptr, function, args_buffer, args_bytes);
    }
  }

  template <typename _F>
      static void invoke_stdcall(_F function, const char* data, size_t bytes, char* args_buffer) {
    size_t args_bytes;
    if (deserialize(data, bytes, args_buffer, &args_bytes)) {
      __invoke_stdcall(this_ptr, function, args_buffer, args_bytes);
    }
  }

  template <typename _F>
      static void invoke_cdecl(_F function, const char* data, size_t bytes, char* args_buffer) {
    size_t args_bytes;
    if (deserialize(data, bytes, args_buffer, &args_bytes)) {
      __invoke_cdecl(this_ptr, function, args_buffer, args_bytes);
    }
  }

};

}

struct Check {
  void check(char* string, size_t k) {
    char str[128];
    memcpy(str, string, k);
    str[k] = '\0';
    DEBUG_INFO("%s, %lu", str, k);
    //DEBUG_INFO("f = %f", f);
  }
  DWORD getCheck() {
    DWORD rc;
    __asm {
      mov ecx, dword ptr[Check::check]
      mov rc, ecx
    }
    return rc;
  }
};


struct DebugFunc {
  static const size_t MSG = 5;
  typedef apc::call<apc::copied_array<char, size_t, 256>, float> apc;
};

const char* string = "typedef apc::call<int, 5, apc::copied_array<char, size_t, 256>, float> DebugCallIn;typedef apc::call<int, 5, apc::copied_array<char, size_t, 256>, float> DebugCallOut;";

struct EvidyonAPC {
  Message message;
  char* data;
  size_t bytes;
};

struct DelayedAPC {
  DWORD delivery_time;
  EvidyonAPC call_data;
};

typedef apc::call<apc::copied_array<char, size_t, 256>> DebugCall;

DelayedAPC delayed_apc;

void attack(DWORD delay_ms, const std::string& text) {
  delayed_apc.delivery_time = GetTickCount() + delay_ms;
  //char* data = new char[DebugCall::bytes];
  //delayed_apc.bytes = DebugCall::serialize(data, DebugCall::t00::auto_string(text));
  //delayed_apc.message = MESSAGE_DEBUG_1;
  size_t args = DebugCall::args_bytes;
  args++;
}

//void Route(EvidyonAPC* call) {
//  char args_buffer[512];
//  switch (call->message) {
//  case MESSAGE_DEBUG_1: DebugCall::invoke_thiscall(&player_, &Player::debug1, call->data, call->bytes, args_buffer);
//  }
//}


#define FOO_DEBUG( format, ... ) = { ## __VA_ARGS__ }
#define FOO_FOO(format, args) = { args }

unsigned long MultPack(unsigned long p, unsigned long v, unsigned long o) {
  return (p * o + v);
}

unsigned long MultUnpack(unsigned long p, unsigned long o) {
  unsigned long lv = p % o;
  DEBUG_INFO("%lu", lv);
  return p / o;
}

//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  int a[] = {6,2,3,4};
  int b[] = {6,4,9,5};
  unsigned long f =0;
  f = MultPack(f, a[0], b[0]);
  f = MultPack(f, a[1], b[1]);
  f = MultPack(f, a[2], b[2]);
  f = MultPack(f, a[3], b[3]);
  MultUnpack(MultUnpack(MultUnpack(MultUnpack(f, b[3]), b[2]), b[1]), b[0]);
  return 0;
  attack(100, "hi there");

//FOO_DEBUG("wtf pk %i %i", 5, 10);
  //int a[] FOO_DEBUG("hi there", 5, 10, 15, 25);
  //int b[] FOO_FOO("hallo", (5, 10, 25, 30));

  char buffer[512];

  typedef apc::call<apc::copied_array<char, size_t, 256>> DebugCallIn;
  typedef apc::call<apc::copied_array<char, size_t, 256>> DebugCallOut;

  //int wtf[] = { 1, 2, 3, 4 };


  size_t bytes;
  DebugCallIn::serialize(
    buffer,
    &bytes,
    DebugCallIn::t00::auto_string("str"));


  char packet[512];

  char args[512];


  Check c;
  DebugCallOut::invoke_thiscall(&c, c.getCheck(), buffer, bytes, args);
  //call_member(
  //  &c,
  //  &Check::check,
  //  args,
  //  DebugCall::deserialize(buffer, args));

  return 0;
}

