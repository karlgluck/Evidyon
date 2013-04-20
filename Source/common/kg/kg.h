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
#ifndef __COMMON_KG_KG_H__
#define __COMMON_KG_KG_H__
#pragma once

#include "assert.h" // modified version of assert()

// confirm()'s failure behavior can be redirected by #defining a new
// confirm_failed or assert_failed macro.  One reason to do this might be
// sending failures to a log.  If this is done, all safe and non-safe
// confirms are enabled
#if defined(assert_failed)
#define confirm_failed assert_failed
#endif

#if defined(confirm_failed)
#define confirm(expression)   if (!(!(expression) && (confirm_failed(#expression),1))) {}
#else
#if (defined(_DEBUG) || defined(DEBUG))
#define confirm_failed(expression) _wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), __LINE__)
#define confirm(expression)   if (!(!(expression) && (confirm_failed(#expression),1))) {}
#else
// unsafe confirm -> nothing in release mode
#define confirm_failed(expression)
#define confirm(expression)   if (true) {}
#endif
#endif

// unsafe variants
#define CLASSNAME(name) name
#define IFSAFE(expression)
#define IFNOTSAFE(expression) expression
#define SELECTSAFE(a,b) b
#define SAFE 0
#define NOTSAFE 1
#include "array.h"
#include "allocatedarray.h"
#include "charbuffer.h"
#include "idmap.h"
#include "hashtoindextranslator.h"
#undef CLASSNAME
#undef IFSAFE
#undef IFNOTSAFE
#undef SELECTSAFE
#undef SAFE
#undef NOTSAFE



// "safe" confirm -> simple test in release mode; unless confirm_failed is redirected,
// the rhs of && evaluates to 1 and the statement becomes "if (!!(expression)) {}"
#if !(defined(_DEBUG) || defined(DEBUG))
#undef confirm
#define confirm(expression)   if (!!(expression)) {}
//#define confirm(expression)   if (!(!(expression) && (confirm_failed(#expression),1))) {}
#endif


// safe variants
#define CLASSNAME(name) Safe##name
#define IFSAFE(expression) expression
#define IFNOTSAFE(expression)
#define SELECTSAFE(a,b) a
#define SAFE 1  // allows #if SAFE ... #endif
#define NOTSAFE 0
#include "array.h"
#include "allocatedarray.h"
#include "charbuffer.h"
#include "idmap.h"
#include "hashtoindextranslator.h"
#undef CLASSNAME
#undef IFSAFE
#undef IFNOTSAFE
#undef SELECTSAFE
#undef SAFE
#undef NOTSAFE




#endif