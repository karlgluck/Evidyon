//-----------------------------------------------------------------------------
//                 _,_ _, _  _, __, __, _, _    _, ___ _,_ __, _  _,  _,
//   Copyright     | | |\ | (_  |_  |_  |\ |   (_   |  | | | \ | / \ (_ 
//      (c)        | | | \| , ) |   |   | \|   , )  |  | | |_/ | \ / , )
//                 `~' ~  ~  ~  ~~~ ~~~ ~  ~    ~   ~  `~' ~   ~  ~   ~ 
//-----------------------------------------------------------------------------
#ifndef __COMMON_KG_ASSERT_H__
#define __COMMON_KG_ASSERT_H__
#pragma once


// This file provides a drop-in replacement for assert().
//
//  find all:       #include <assert.h>
//  replace with:   #include <kg/assert.h>
//
// Default behavior causes assert() to behave normally.
//
// If you #define assert_failed, all assert() and confirm() conditions that
// fail will magically invoke this macro instead of the default assert,
// even in release mode!  This must be a single function (not an expression).
// Make use of the __LINE__, __FILE__ and __FUNCTION__ macros for logging!
//#define assert_failed(expression_str)\
//  OutputDebugString(__FILE__ " [ASSERTION FAILED]  "##expression_str##"\n")


#include <assert.h>

#if defined(assert_failed)
#undef assert
#define assert(expression) (void)((!!(expression)) || (assert_failed(#expression), 0))
#endif


#endif