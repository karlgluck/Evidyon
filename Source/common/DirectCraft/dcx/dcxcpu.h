//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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