
#include <stdarg.h>
#include <memory.h>

/**
 * Writes a structure's raw byte data to the given buffer
 */
template <typename T> char* encodeStructure(char* buffer, const char* endOfBuffer, va_list args_in, va_list* args_out)
{
    if( !buffer || (buffer + sizeof(T) > endOfBuffer) ) return 0;
    memcpy_s( buffer, endOfBuffer - buffer, (const void*)&(va_arg(args_in,T)), sizeof(T) );
    *args_out = args_in;
    return buffer + sizeof(T);
}
/**
 * Reads a structure's raw byte data from the given buffer
 */
template <typename T> const char* decodeStructure(const char* buffer, const char* endOfBuffer, va_list args_in, va_list* args_out )
{
    if( !buffer || (buffer + sizeof(T) > endOfBuffer) ) return 0;
    memcpy_s((void*)(va_arg(args_in,T*)), sizeof(T), buffer, sizeof(T) );
    *args_out = args_in;
    return buffer + sizeof(T);
}


// f - float
// d - double
// c - char
// C - unsigned char
// s - short
// S - unsigned short
// l - long
// L - unsigned long
// r - size of structure (size_t), pointer to structure
// a{CSL} - # of elements in array, pointer to first array elements
char* compileMessageVirtual(const char* mask, char* buffer, const char* end_of_buffer, va_list args_in, va_list* args_out) {
  while (*mask != '\0') {
    va_list args_next;
    switch (*mask) {
      case 'f': buffer = encodeStructure<float>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'd': buffer = encodeStructure<double>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'c': buffer = encodeStructure<char>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'C': buffer = encodeStructure<unsigned char>(buffer, end_of_buffer, args_in, &args_next); break;
      case 's': buffer = encodeStructure<short>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'S': buffer = encodeStructure<unsigned short>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'l': buffer = encodeStructure<long>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'L': buffer = encodeStructure<unsigned long>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'r': {
        size_t size = va_arg(args_in, size_t);
        const void* structure = va_arg(args_in, void*);
        if( buffer && (buffer + size <= end_of_buffer) ) {
          memcpy_s(buffer, end_of_buffer - buffer, structure, size);
          buffer += size;
        }
        args_next = args_in;
      } break;
    }
    if (*mask == 'a') {
      ++mask;
      size_t array_size = 0;
      switch (*mask) {
        case 'C': buffer = encodeStructure<unsigned char>(buffer, end_of_buffer, args_in, &args_next);  array_size = va_arg(args_in,unsigned char);  break;
        case 'S': buffer = encodeStructure<unsigned short>(buffer, end_of_buffer, args_in, &args_next); array_size = va_arg(args_in,unsigned short); break;
        case 'L': buffer = encodeStructure<unsigned long>(buffer, end_of_buffer, args_in, &args_next);  array_size = va_arg(args_in,unsigned long);  break;
      }
      ++mask;
      size_t element_size = 0;
      switch (*mask) {
        case 'f': element_size = sizeof(float); break;
        case 'd': element_size = sizeof(double); break;
        case 'c': element_size = sizeof(char); break;
        case 'C': element_size = sizeof(unsigned char); break;
        case 's': element_size = sizeof(short); break;
        case 'S': element_size = sizeof(unsigned short); break;
        case 'l': element_size = sizeof(long); break;
        case 'L': element_size = sizeof(unsigned long); break;
        case 'r': {
          element_size = va_arg(args_in, size_t);
        } break;
      }
      void* array_pointer = va_arg(args_in,void*);
      size_t bytes = array_size * element_size;
      if( buffer && (buffer + bytes <= end_of_buffer) ) {
        memcpy_s(buffer, end_of_buffer - buffer, array_pointer, bytes);
        buffer += bytes;
      }

      args_next = args_in;
    }
    args_in = args_next;
    ++mask;
  }
  *args_out = args_in;
  return buffer;
}

char* compileMessage(const char* mask, char* buffer, const char* end_of_buffer, ...) {
  va_list args;
  va_start(args, end_of_buffer);
  buffer = compileMessageVirtual(mask, buffer, end_of_buffer, args, &args);
  va_end(args);
  return buffer;
}


// f - float*
// d - double*
// c - char*
// C - unsigned char*
// s - short*
// S - unsigned short*
// l - long*
// L - unsigned long*
// r - size of structure (size_t), pointer to structure buffer
// a{CSL} - pointer to number of entries in buffer, buffer size in entries (size_t)
const char* decompileMessageVirtual(const char* mask, const char* buffer, const char* end_of_buffer, va_list args_in, va_list* args_out) {
  while (*mask != '\0' && buffer) {
    va_list args_next;
    switch (*mask) {
      case 'f': buffer = decodeStructure<float>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'd': buffer = decodeStructure<double>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'c': buffer = decodeStructure<char>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'C': buffer = decodeStructure<unsigned char>(buffer, end_of_buffer, args_in, &args_next); break;
      case 's': buffer = decodeStructure<short>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'S': buffer = decodeStructure<unsigned short>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'l': buffer = decodeStructure<long>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'L': buffer = decodeStructure<unsigned long>(buffer, end_of_buffer, args_in, &args_next); break;
      case 'r': {
        size_t size = va_arg(args_in, size_t);
        void* structure = va_arg(args_in, void*);
        if( buffer && (buffer + size <= end_of_buffer) ) {
          memcpy_s(structure, end_of_buffer - buffer, buffer, size);
          buffer += size;
        }
        args_next = args_in;
      } break;
    }
    if (*mask == 'a') {
      ++mask;
      size_t array_entries = 0;
      switch (*mask) {
        case 'C': { buffer = decodeStructure<unsigned char>(buffer, end_of_buffer, args_in, &args_next);  unsigned char*  array_entries_out = va_arg(args_in,unsigned char*);  array_entries = *array_entries_out; } break;
        case 'S': { buffer = decodeStructure<unsigned short>(buffer, end_of_buffer, args_in, &args_next); unsigned short* array_entries_out = va_arg(args_in,unsigned short*); array_entries = *array_entries_out; } break;
        case 'L': { buffer = decodeStructure<unsigned long>(buffer, end_of_buffer, args_in, &args_next);  unsigned long*  array_entries_out = va_arg(args_in,unsigned long*);  array_entries = *array_entries_out; } break;
      }
      ++mask;
      size_t buffer_size_entries = va_arg(args_in,size_t);
      size_t element_size = 0;
      switch (*mask) {
        case 'f': element_size = sizeof(float); break;
        case 'd': element_size = sizeof(double); break;
        case 'c': element_size = sizeof(char); break;
        case 'C': element_size = sizeof(unsigned char); break;
        case 's': element_size = sizeof(short); break;
        case 'S': element_size = sizeof(unsigned short); break;
        case 'l': element_size = sizeof(long); break;
        case 'L': element_size = sizeof(unsigned long); break;
        case 'r': {
          element_size = va_arg(args_in, size_t);
        } break;
      }
      void* array_pointer = va_arg(args_in,void*);
      if (array_entries > buffer_size_entries) {
        buffer = 0;
      }
      size_t bytes = element_size * array_entries;
      if( buffer && (buffer + bytes <= end_of_buffer) ) {
        memcpy_s(array_pointer, end_of_buffer - buffer, buffer, bytes);
        buffer += bytes;
      }

      args_next = args_in;
    }
    args_in = args_next;
    ++mask;
  }
  *args_out = args_in;
  return buffer;
}

const char* decompileMessage(const char* mask, const char* buffer, const char* end_of_buffer, ...) {
  va_list args;
  va_start(args, end_of_buffer);
  buffer = decompileMessageVirtual(mask, buffer, end_of_buffer, args, &args);
  va_end(args);
  return buffer;
}


void main() {
  char buffer[32];
  char* end = buffer + 32;
  float floatys[10];
  for (int i = 0; i < 10; ++i) floatys[i] = i * 10.0f;
  compileMessage("aCf", buffer, end, (unsigned char)5, floatys );
  unsigned char num;
  float read[5];
  decompileMessage("aCf", buffer, end, &num, (size_t)5, read);
}