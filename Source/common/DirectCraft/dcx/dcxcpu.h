//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#ifndef __DCXCPU_H__
#define __DCXCPU_H__



namespace dcx {

    /**
     * Defines basic features of the CPU
     */
    struct dcxCPUFeatures
    {
        unsigned fpu        : 1;    // FPU (Processor is newer than a 386/486)
        unsigned mmx        : 1;    // MMX
        unsigned amd_mmx    : 1;    // AMD MMX Extensions
        unsigned amd_3dnow  : 1;    // AMD 3DNow!
        unsigned amd_e3dnow : 1;    // AMD 3DNow! Extensions
        unsigned sse        : 1;    // SSE (Intel Streaming SIMD Extensions)
        unsigned sse2       : 1;    // SSE2
        unsigned htt        : 1;    // HyperThreading Technology
    };

    /**
     * Obtains the operating features of the CPU using the CPUID assembler instruction
     * @param features Output mask defining which features the CPU supports
     * @return Whether or not the CPU supports CPUID (if false, this method can't be used)
     */
    bool dcxGetCPUFeatures( dcxCPUFeatures* features );

}



#endif