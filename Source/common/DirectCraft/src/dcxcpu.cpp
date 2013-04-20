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
//-----------------------------------------------------------------------------
// File:    dcxcpu.h
// Desc:    Reads a mask that defines the features supported by the CPU
//-----------------------------------------------------------------------------
#include "../dcx/dcxcpu.h"
#include "../dc/debug"



using namespace dcx;



//-----------------------------------------------------------------------------
// Name:  dcxGetCPUFeatures
// Desc:  Obtains the operating features of the CPU using the CPUID assembler instruction
//-----------------------------------------------------------------------------
bool dcx::dcxGetCPUFeatures( dcxCPUFeatures* features ) {
  int hasCPUID = 0;
  unsigned long standardFeatures = 0; // Standard IA features
  unsigned long extendedFeatures = 0; // Extended AMD style features

    char manufacturer[13];

	// Use CPUID to get features of the processor
  manufacturer[0] = 0;
	manufacturer[12] = 0;
  __asm {
    /* this code was removed because:
        1. I'm not sure where it came from
        2. I can't be certain that I have the right to redistribute it
        3. Evidyon no longer uses this method

       the code involved detecting & using the cpuid assembler instruction
       to poll the cpu's type and determine which extensions are available
    */
	}

  DEBUG_INFO( "CPU type: \"%s\"", manufacturer );

	if( APP_WARNING( !hasCPUID )( "CPU doesn't support CPUID instruction" ) ) return false;

	// FPU (Floating Point Unit)
  features->fpu = 1 & (standardFeatures >> 0);
  APP_WARNING( !features->fpu )( "CPU doesn't have a floating-point unit!" );

  // Detect features
  features->mmx = 1 & (standardFeatures >> 23);       // MMX (MultiMedia eXtensions)
  features->amd_mmx = 1 & (extendedFeatures >> 22);   // AMD MMX Extensions
  features->amd_3dnow = 1 & (extendedFeatures >> 31); // AMD 3DNow!
  features->amd_e3dnow = 1 & (extendedFeatures >> 30);// AMD 3DNow! Extensions
  features->sse = 1 & (standardFeatures >> 25);       // SSE
  features->sse2 = 1 & (standardFeatures >> 26);      // SSE2
  features->htt = 1 & (standardFeatures >> 28);       // HyperThreading Technology

  // Succcess
	return true;
}
