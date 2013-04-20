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

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dc_d.lib")
#pragma comment(lib, "dcx_d.lib")



/*
message units have flags set for message classes that they should be passed,
and they just ignore messages that they don't need from those classes--that way
it's both efficient & easy to implement.

ex. problem to solve:
  - add a new element to update with a given period/resolution/priority
  - allow it to remove itself from the scheduler in response to a message?
*/

void timer(unsigned int source_id,
           unsigned int target_id,
           unsigned int delay,
           unsigned int message_type,
           const void* parameters,
           size_t bytes) {
  
}

void __cdecl rpc_burst(float x, float z) {
  DEBUG_INFO("%f %f", x, z);
}

__declspec(naked) DWORD getEIP() {
  __asm {
    mov eax, dword ptr [esp]
    ret
  }
}

//
//void burst(float x, float z) {
//  invoke_rpc<float, float>(NETMSG_GEOSID_BURST);
//}
//
//template <typename A, typename B> void invoke_rpc(Message type) {
//}
//
//void server_burst(void* client, float x, float z) {
//  // do the burst here
//}
//
//void recv_rpc(Message type, void* stack_data) {
//  switch (type) {
//  case NETMSG_GEOSID_BURST: server_burst(this, x, z);
//  }
//}

enum Function {
  FUNCTION_DEBUG,
};

// tasklets are entirely event-driven
//  they can pass messages to one-another, or broadcast to a class of tasklets
//  the "switch palace" distributes messages and manages tasklet lifetime
// the function call stuff makes invoking tasklet commands as easy as invoking
//  simple functions; you call the function, it packs the parameters and
//  away you go.

struct FunctionCall {
  FunctionCall(Function f) : function(f) {}
  Function function;
  const void* parameters;
  size_t size;
};


void debug(int index, int* indices, size_t number) {
  FunctionCall* call = new FunctionCall(FUNCTION_DEBUG);
  //FunctionCall* call = RPC::Call(FUNCTION_DEBUG);
  // typedef TaskletSerializer<unsigned char, Array<unsigned short, unsigned char>>
  //  DebugSerializer;
  // DebugSerializer
  //    ::write(call, index, &CopiedArray<int>(indices, number));
  // SwitchPalace::send(call);
}

void rpc_debug(void* client, int index, int* indices, size_t number) {
}

DWORD unpack(FunctionCall* call, void* args, DWORD* sz) {
  switch (call->function) {
  case FUNCTION_DEBUG:
    //DebugSerializer::read<int, Array<int*, size_t>>(call, args, sz);
    __asm mov eax, dword ptr [rpc_debug]
    __asm ret
  }
  return 0;
}

void call(FunctionCall* call) {
  char buffer[512];
  const void* args = buffer;
  //*((void**)args) = 0; // client address
  DWORD sz;
  DWORD func = unpack(call, buffer + sizeof(void*), &sz);
  DWORD rc;
  __asm {
    mov rc, ecx
    mov ecx, sz
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    call [func]
    mov rc, eax
    add esp, sz
  }
}

void function(const void* args, size_t sz) {
  DWORD rc;
  //rc = *(DWORD*)rpc_burst;
  __asm 
    mov ecx, dword ptr [rpc_burst]
  __asm {
    mov rc, ecx
    mov ecx, sz
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    call [rc]
    mov rc, eax
    add esp, sz
  }
}


struct Class {
  //void member_fn(unsigned int a, unsigned int b) {
  //  DEBUG_INFO("%lu %lu", a, b);
  //}
  void member_fn(float* v, size_t len) {
    DEBUG_INFO("len = %lu, v1 = %.02f", len, v[0]);
  }
};

template <typename _1> void create_member_call(_1 p1) {
}


struct array_size {
  array_size(size_t size) : value(size) {}
  size_t value;
};

template <typename _t> struct copy {
  copy(_t* p) : ptr(p) {}
  _t* ptr;
};

__forceinline char* serialize(char* ptr, array_size t) {
  return ptr;
}

template <typename _t> __forceinline char* serialize(char* ptr, _t t) {
  *(_t*)ptr = t;
  return ptr + sizeof(_t);
}

template <typename _t> __forceinline char* serialize(char* ptr, _t* t) {
  return ptr;
}

template <typename _t> __forceinline char* serialize(char* ptr, copy<_t> t) {
  size_t bytes = sizeof(_t);
  memcpy(ptr, t.ptr, bytes);
  return ptr + bytes;
}

// array
template <typename _1> __forceinline char* serialize(char* ptr, _1* p1, array_size p2) {
  *(size_t*)ptr = p2.value;
  ptr += sizeof(size_t);
  size_t bytes = p2.value * sizeof(*p1);
  memcpy(ptr, p1, bytes);
  return ptr + bytes;
}


template <typename _1, typename _2> __forceinline char* serialize(char* ptr, _1 p1, _2 p2) {
  return serialize(ptr, p1);
}

// array-type _t (int* -> _t = int)
template <typename _t> void deserialize(char** data, char** args) {
  char* data_ptr = *data;
  char* args_ptr = *args;
  size_t elements = *(size_t*)(data_ptr);
  data_ptr += sizeof(size_t);
  *(_t**)(args_ptr) = (_t*)(data_ptr);
  args_ptr += sizeof(_t*);
  *(size_t*)(args_ptr) = elements;
  *args = args_ptr + sizeof(size_t);
  *data = data_ptr + sizeof(size_t) + elements * sizeof(_t);
}

// each raw type must have its own specialization
template <> void deserialize<unsigned int>(char** data, char** args) {
  *(unsigned int*)(*args) = *(unsigned int*)(*data);
  (*args) += sizeof(unsigned int);
  (*data) += sizeof(unsigned int);
}

// each raw type must have its own specialization
template <> void deserialize<array_size>(char** data, char** args) {
}

//template <> void deserialize<array_size>(char** data, char** args) {
//}


template <typename T, typename F> void call_member(T* this_ptr, F func, const void* args, size_t sz) {
  DWORD rc;
  __asm {
    mov rc, ecx
    mov ecx, sz
    mov esi, args
    sub esp, ecx
    mov edi, esp
    shr ecx, 2
    rep movsd
    mov ecx, this_ptr
    call [func]
    mov rc, eax
    //add esp, sz
  }
}

template <typename _1, typename _2, typename T, typename F> void call_member(T* this_ptr, F func, const char* data, size_t size) {
  char args[512];
  char* data_ptr = (char*)data;
  char* args_ptr = args;
  deserialize<_1>(&data_ptr, &args_ptr);
  deserialize<_2>(&data_ptr, &args_ptr);

  call_member(this_ptr, func, (const void*)args, args_ptr - args);
}


template <typename _1, typename _2> void create_member_call(_1 p1, _2 p2) {
  char buffer[512];
  char* ptr = buffer;
  ptr = serialize(ptr, p1, p2);
  ptr = serialize(ptr, p2);

  Class c;
  call_member<_1,_2>(&c, &Class::member_fn, buffer, 0);
}

template <typename _1, typename _2, typename _3> void create_member_call(_1 p1, _2 p2, _3 p3) {
  char buffer[512];
  char* ptr = buffer;
  ptr = serialize(ptr, p3, p2);
  ptr = serialize(ptr, p2, p1);
  ptr = serialize(ptr, p1);
}


class Tasklet {
  virtual DWORD getMessageFunction() = 0;
};

struct TestGetOwnProcAddr : Tasklet {
  DWORD functest() {
    return 3;
  }
  virtual DWORD getMessageFunction() {
    DWORD rc;
    __asm {
      mov ecx, dword ptr [TestGetOwnProcAddr::functest]
      mov rc, ecx
    }
    return rc;
  }
};


//----[  WinMain  ]------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  float params[] = { 1.0f, 10.0f };
  unsigned int f = 3;
  create_member_call(params, array_size(2));

  TestGetOwnProcAddr inst;
  void* pthis = 0;
  DWORD fn = inst.getMessageFunction(&pthis);
  pthis;

  //create_member_call(0x0000FFFFU, 0xF00DBAADU);

  ////function((void*)params, sizeof(float)*2);
  //void (Class::*addr)(float, float) = &Class::member_fn;
  //DWORD fn;
  //__asm {
  //  mov ecx, addr
  //  mov fn, ecx
  //}
  //Class c;
  //call_member(&c, &Class::member_fn, (const void*)params, sizeof(float)*2);
  //invoke_member_fn((void*)&c, fn, (const void*)params, sizeof(float)*2);
}


/*

code a database that writes the code templates for you

so how are hand-coded parts brought in?
  - must be in independent files
  - could be snippets copied in at "assembly" time
  - or functions imported somehow

ex. code

  serialize


*/