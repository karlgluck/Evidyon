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
// Provide all of the debugging definitions
#include "../dc/dcinternal_begin.h"
#include "../dc/dcstream.h"
#include "../dc/dcfilestream.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// compile with: /EHsc /MT
#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <process.h>
#include <locale.h>
#include <locale>
#include <windows.h>



using namespace dc;


/*
//-----------------------------------------------------------------------------
// Name:  dcStaticLogger
// Desc:  Gets the global DirectCraft logging object
//-----------------------------------------------------------------------------
dcLogger* dcStaticLogger()
{
    static dcLogger logger( "#dc-output", true );
    return &logger;
}


struct dcStartup
{
    dcStartup()
    {
        dcStaticLogger()->println( dcLogger::BLACK|dcLogger::BOLD, "Using DirectCraft Library v0.1.0 written by Karl Gluck" );
    }
};

/// Executes code when the application starts
static dcStartup startup;*/


/*
//-----------------------------------------------------------------------------
// Name:  dcOutputDebugStringF
// Desc:  Outputs a string to the debug window using printf-style formatting
//-----------------------------------------------------------------------------
void dcOutputDebugStringF( unsigned long textStyle, const char* message, ... )
{
    char buffer[1024];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( buffer, sizeof(buffer), sizeof(buffer), message, args );
    buffer[sizeof(buffer)-1] = '\0';
    va_end( args );

    // Write to the file, etc
    dcStaticLogger()->print( textStyle, buffer );
}


//-----------------------------------------------------------------------------
// Name:  OutputDebugStringFBlack
// Desc:  Outputs a string to the debug window using printf-style formatting
//-----------------------------------------------------------------------------
void dc::dcOutputDebugStringFBlack( const char* message, ... )
{
    char buffer[1024];

    // Compile the message to the string
    va_list args;
    va_start( args, message );
    _vsnprintf_s( buffer, sizeof(buffer), sizeof(buffer), message, args );
    buffer[sizeof(buffer)-1] = '\0';
    va_end( args );

    // Write to the file, etc
    dcStaticLogger()->print( dcLogger::BLACK, buffer );
}
*/
//
//struct dcMemoryAllocation
//{
//    /// The actual memory that was allocated by the call to malloc
//    void* mallocMemory;
//
//    /// How many bytes were allocated
//    size_t bytes;
//};
//
//
//struct dcMemoryAllocator
//{
//    /// The line on which the allocation occurred
//    unsigned long line;
//
//    /// The file name to search for
//    char file[256];
//
//    /// The memory pointers that have been allocated
//    dcMemoryAllocation** allocationMemoryArray;
//
//    /// Number of allocation array entries
//    size_t allocationArrayEntries;
//
//    /// How large the array is
//    size_t allocationArraySize;
//
//    /// How many allocations have been made at this file/line
//    unsigned int allocations;
//};
//
///**
// * Compares two memory structures to each other
// */
//int dcCompareMemoryPointers( void* context, const void* p1, const void* p2 )
//{ 
//    // Cast the pointers
//    dcMemoryAllocator* m1 = (dcMemoryAllocator*)p1;
//    dcMemoryAllocator* m2 = (dcMemoryAllocator*)p2;
//
//    // Compare line numbers
//    if( m1->line < m2->line ) return -1;
//    if( m1->line > m2->line ) return +1;
//
//    // Compare file strings
//    int file = strcmp( m1->file, m2->file );
//    if( file != 0 ) return file;
//
//    // Equal
//    return 0;
//}
//
///**
// * Helps with memory management
// */
//struct dcMemoryManager
//{
//    /// Global instance of the memory manager
//    static dcMemoryManager* singleton;
//
//    // How many elements in the list are being used
//    unsigned int myElementCount;
//
//    // How many elements are allocated
//    size_t myElementPointersArraySize;
//
//    // The list of memory elements
//    dcMemoryAllocator** myElementPointers;
//
//    /**
//     * Initializes the class
//     */
//    void construct()
//    {
//        myElementCount = 0;
//        myElementPointersArraySize = 0;
//        myElementPointers = 0;
//    }
//
//    /**
//     * Erases allocated class data
//     */
//    void destruct()
//    {
//        dcStaticLogger()->printFormatted( dcLogger::ERR, "Error !!!  Disabled memory detection because it's causing problems...\n" );
//        return;
//
//        // Erase each of the element pointers
//        if( myElementPointers )
//        {
//            // Free each pointer
//            for( unsigned int i = 0; i < myElementCount; ++i )
//            {
//                size_t unfreedAllocations = myElementPointers[i]->allocationArrayEntries;
//
//                // Print a message if something went wrong
//                if( unfreedAllocations != 0 )
//                    dcStaticLogger()->printFormatted( dcLogger::ERR,
//                                                      "%s(%u): MEMORY LEAK DETECTED (%u allocations unfreed)\n"
//                                                      "=====================================================\n",
//                                                      myElementPointers[i]->file,
//                                                      myElementPointers[i]->line,
//                                                      myElementPointers[i]->allocationArrayEntries );
//
//                // Erase any unfreed allocations
//                for( unsigned int j = 0; j < unfreedAllocations; ++j )
//                {
//                    // Get this allocation object
//                    dcMemoryAllocation* allocation = myElementPointers[i]->allocationMemoryArray[j];
//
//                    // Output information about the allocation that wasn't freed
//                    dcStaticLogger()->printFormatted( dcLogger::ERR,
//                                                      "[%3u]:  %u bytes @ 0x%X\n", j,
//                                                      allocation->bytes, allocation->mallocMemory );
//
//                    // Free the allocated memory for the user
//                    free( allocation->mallocMemory );
//                    free( allocation );
//                }
//
//                // Close the allocated memory section
//                if( unfreedAllocations != 0 )
//                    dcStaticLogger()->print( dcLogger::ERR,
//                                             "=====================================================\n" );
//
//                // Erase the memory for the container
//                free( myElementPointers[i] );
//            }
//
//            // Erase the array
//            free( myElementPointers );
//        }
//
//        // Reset structure sizes
//        myElementPointersArraySize = 0;
//        myElementCount = 0;
//    }
//
//    /**
//     * Generates memory
//     */
//    void* create( size_t size, unsigned long line, const char* file )
//    {
//        // Search for this element in the array
//        dcMemoryAllocator* memory = generateElement( line, file );
//
//        // If we could get an element, do allocation processing on it.  If not, too bad.
//        if( memory )
//        {
//            // Reallocate the memory array, if necessary
//            if( memory->allocationArrayEntries + 1 >= memory->allocationArraySize )
//            {
//                // Calculate a new buffer size that is double the current size, padded a little bit
//                size_t newArraySize = (size_t)((memory->allocationArraySize + 1) * 2);
//
//                // Create a new buffer 
//                dcMemoryAllocation** newArray = (dcMemoryAllocation**)malloc( sizeof(dcMemoryAllocation*) * newArraySize );
//                if( !newArray )
//                {
//                    // Close this class
//                    destruct();
//
//                    // Failure; just perform an unchecked allocation
//                    return malloc( size );
//                }
//
//                // If there was anything in our current buffer, save it to the new one
//                if( memory->allocationMemoryArray != 0 )
//                {
//                    memcpy( newArray, memory->allocationMemoryArray, sizeof(dcMemoryAllocation*) * memory->allocationArraySize );
//                    free( memory->allocationMemoryArray );
//                }
//
//                // Set the buffer pointer
//                memory->allocationMemoryArray = newArray;
//
//                // Save the new size
//                memory->allocationArraySize = newArraySize;
//            }
//
//            // Allocate the requested memory for whatever new() is making
//            void* mallocMemory = malloc( size );
//
//            // Get the memory for a container to hold this allocated memory
//            dcMemoryAllocation* allocation = (dcMemoryAllocation*)malloc( sizeof(dcMemoryAllocation) );
//
//            // Initialize the allocation
//            allocation->mallocMemory = mallocMemory;
//            allocation->bytes = size;
//
//            // Add this allocated memory entry to the end of the list
//            memory->allocationMemoryArray[memory->allocationArrayEntries++] = allocation;
//
//            // Increment the allocation count
//            memory->allocations++;
//
//            // Return the allocated memory
//            return mallocMemory;
//        }
//        else
//            return malloc( size );
//    }
//
//    /**
//     * Deallocates memory
//     */
//    void destroy( void* memory )
//    {
//        // Do a linear search for this entry
//        for( unsigned int i = 0; i < myElementCount; ++i )
//        {
//            // Get the element pointer
//            dcMemoryAllocator* element = myElementPointers[i];
//
//            // Get the allocation count for this element
//            size_t allocationArrayEntries = element->allocationArrayEntries;
//
//            // Look through all of the allocations
//            for( unsigned int j = 0; j < allocationArrayEntries; ++j )
//            {
//                // Check to see if this allocation matches
//                if( element->allocationMemoryArray[j]->mallocMemory == memory )
//                {
//                    // Decrement the number of allocations
//                    allocationArrayEntries--;
//
//                    // Free the memory for this element
//                    free( element->allocationMemoryArray[j] );
//
//                    // Swap down the highest element from the list and
//                    // remove that highest element
//                    element->allocationMemoryArray[j] = element->allocationMemoryArray[allocationArrayEntries];
//                    element->allocationMemoryArray[allocationArrayEntries] = 0;
//
//                    // Save the new allocation count
//                    element->allocationArrayEntries = allocationArrayEntries;
//
//                    //dcStaticLogger()->printFormatted( dcLogger::INFO, "Memory from %s(%u) freed\n", element->file, element->line );
//
//                    // Exit the search loops
//                    goto endLoop;
//                }
//            }
//        }
//
//endLoop:
//
//        // Erase this block of memory
//        free( memory );
//    }
//
//    /**
//     * Obtains an element that represents the given allocation, whether it's already
//     * in the list or not.
//     */
//    dcMemoryAllocator* generateElement( unsigned long line, const char* file )
//    {
//        // Used to keep track of a memory pointer
//        dcMemoryAllocator* memory = 0;
//
//        // First, look for the memory element in the list using a binary search
//        if( myElementPointers )
//        {
//            // Build a search key
//            dcMemoryAllocator key;
//            key.line = line;
//            strcpy_s( key.file, sizeof(key.file), file );
//
//            // Do a binary search for the element
//            memory = (dcMemoryAllocator*)bsearch_s( &key, myElementPointers, myElementCount,
//                                                     sizeof(dcMemoryAllocator*), dcCompareMemoryPointers, 0 );
//        }
//
//        // If we could find the entry, return it
//        if( memory ) return memory;
//
//        // Reallocate if it is necessary
//        if( myElementCount + 1 >= myElementPointersArraySize )
//        {
//            // Calculate a new buffer size that is double the current size, padded a little bit
//            size_t newArraySize = (size_t)((myElementPointersArraySize + 1) * 2);
//
//            // Create a new buffer 
//            dcMemoryAllocator** newArray = (dcMemoryAllocator**)malloc( sizeof(dcMemoryAllocator*) * newArraySize );
//            if( !newArray )
//            {
//                // Close this class
//                destruct();
//
//                // Failure
//                return 0;
//            }
//
//            // If there was anything in our current buffer, save it to the new one
//            if( myElementPointers != 0 )
//            {
//                memcpy( newArray, myElementPointers, sizeof(dcMemoryAllocator*) * myElementPointersArraySize );
//                free( myElementPointers );
//            }
//
//            // Set the buffer pointer
//            myElementPointers = newArray;
//
//            // Save the new size
//            myElementPointersArraySize = newArraySize;
//        }
//
//        // Create a new element
//        memory = (dcMemoryAllocator*)malloc( sizeof(dcMemoryAllocator) );
//        if( !memory ) { destruct(); return 0; }
//
//        // Give it some attributes
//        memory->line = line;
//        strcpy_s( memory->file, sizeof(memory->file), file );
//        memory->allocationArrayEntries = 0;
//        memory->allocationArraySize = 5;
//        memory->allocationMemoryArray = new dcMemoryAllocation*[memory->allocationArraySize];
//        if( !memory->allocationMemoryArray )
//        {
//            free( memory );
//            destruct();
//            return 0;
//        }
//        memory->allocations = 0;
//        // todo: add more attributes
//
//        // Add this memory to the list
//        myElementPointers[myElementCount++] = memory;
//
//        // Make sure the list is sorted
//        qsort_s( myElementPointers, myElementCount, sizeof(dcMemoryAllocator*), dcCompareMemoryPointers, 0 );
//
//        // Return this element
//        return memory;
//    }
//
//    /**
//     * Allocates an instance of the memory manager, if necessary, and returns whether
//     * or not the "singleton" member of this class is valid.
//     */
//    static bool spawn()
//    {
//        struct MemorySpawnController
//        {
//            // Flag to keep track of whether or not the controller exists.  This flag
//            // prevents a new manager from being created while destructors are being
//            // called when the program is exiting.
//            bool spawned;
//
//            /**
//             * Initializes the spawn controller
//             */
//            MemorySpawnController()
//            {
//                spawned = false;
//            }
//
//            /**
//             * Deallocates the internal memory manager
//             */
//            ~MemorySpawnController()
//            {
//                if( dcMemoryManager::singleton )
//                {
//                    dcMemoryManager::singleton->destruct();
//                    free( dcMemoryManager::singleton );
//                    dcMemoryManager::singleton = 0;
//                }
//            }
//        };
//
//        // Create an instance of the memory spawn controller when this
//        // method is called for the first time
//        static MemorySpawnController controller;
//
//        // Allocate the singleton for the memory manager, if necessary
//        if( !controller.spawned )
//        {
//            // Set the singleton pointer
//            singleton = (dcMemoryManager*)malloc( sizeof( dcMemoryManager ) );
//
//            // Change the spawn flag and initialize the manager if
//            // we successfully allocated the memory
//            if( singleton )
//            {
//                singleton->construct();
//                controller.spawned = true;
//            }
//        }
//
//        // If the singleton for the memory manager exists, return 'true'
//        return singleton != 0;
//    }
//
//    /**
//     * Allocates managed memory for the DirectCraft library
//     * @param size How much memory to allocate
//     * @param line The line on which the memory allocation call was made
//     * @param file The file from which the allocation call originated
//     * @return A pointer to the desired amount of memory, or 0 if it could not be obtained
//     */
//    static void* allocate( size_t size, unsigned long line, const char* file )
//    {
//        if( spawn() )
//            return singleton->create( size, line, file );
//        else
//            return malloc( size );
//    }
//
//    /**
//     * Frees memory
//     */
//    static void deallocate( void* memory )
//    {
//        if( singleton )
//            return singleton->destroy( memory );
//        else
//            free( memory );
//    }
//};
//
//// Reset the singleton value until the first call to operator new
//dcMemoryManager* dcMemoryManager::singleton = 0;
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void* operator new( size_t size, unsigned long line, const char* file )
//{
//    return dcMemoryManager::allocate( size, line, file );
//
//}
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void* operator new[]( size_t size, unsigned long line, const char* file )
//{
//    return dcMemoryManager::allocate( size, line, file );
//}
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void operator delete( void* memory )
//{
//    dcMemoryManager::deallocate( memory );
//}
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void operator delete[]( void* memory )
//{
//    dcMemoryManager::deallocate( memory );
//}
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void operator delete( void* memory, unsigned long line, const char* file )
//{
//    dcMemoryManager::deallocate( memory );
//}
//
//
////-----------------------------------------------------------------------------
//// Name:  
//// Desc:  
////-----------------------------------------------------------------------------
//void operator delete[]( void* memory, unsigned long line, const char* file )
//{
//    dcMemoryManager::deallocate( memory );
//}