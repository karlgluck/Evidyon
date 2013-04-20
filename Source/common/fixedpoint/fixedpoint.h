/*
Copyright (c) 2007, Markus Trenkwalder

All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.

* Neither the name of the library's copyright owner nor the names of its 
  contributors may be used to endorse or promote products derived from this 
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  This code has been modified from the original by Karl Gluck, starting on
  July 26 2009 for use in Evidyon.
*/

#ifndef FIXEDP_CLASS_H_INCLUDED
#define FIXEDP_CLASS_H_INCLUDED

#ifdef _MSC_VER
#pragma once
#endif

#include "fixed_func.h"
#include <assert.h>

namespace FixedPoint {

// The template argument p in all of the following functions refers to the 
// fixed point precision (e.g. p = 8 gives 24.8 fixed point functions).
template <int p> struct Fixed;
template <int p> struct FixedNoConstructor;

//----[  FixedNoConstructor  ]-------------------------------------------------
// This is a storage type for unions
// ex. usage that will compile!
//  union {
//    FixedNoConstructor<16> fix; // using Fixed<16> won't compile
//    float flt;
//  } u;
//  Fixed<16> value(u.flt);
//  Fixed<16> value(u.fix);
template <int p>
struct FixedNoConstructor {
  int32_t int_value;
  void set(const Fixed<p>& rhs) { int_value = rhs.int_value; }
};

//----[  Fixed  ]--------------------------------------------------------------
template <int p>
struct Fixed {
	int32_t int_value;

  Fixed() {}
  Fixed(const FixedNoConstructor<p>& x) : int_value(x.int_value) {}
	/*explicit*/ Fixed(int32_t i) : int_value(i << p) {}
	/*explicit*/ Fixed(float f) : int_value(float2fix<p>(f)) {}
	/*explicit*/ Fixed(double f) : int_value(double2fix<p>((double)f)) {}

	Fixed& operator += (Fixed r) { int_value += r.int_value; return *this; }
	Fixed& operator -= (Fixed r) { int_value -= r.int_value; return *this; }
	Fixed& operator *= (Fixed r) { int_value = fixmul<p>(int_value, r.int_value); return *this; }
	Fixed& operator /= (Fixed r) { int_value = fixdiv<p>(int_value, r.int_value); return *this; }
	/*
	Fixed& operator *= (int32_t r) { int_value *= r; return *this; }
	Fixed& operator /= (int32_t r) { int_value /= r; return *this; }
	*/
	Fixed operator - () const { Fixed x; x.int_value = -int_value; return x; }
	Fixed operator + (Fixed r) const { Fixed x = *this; x += r; return x;}
	Fixed operator - (Fixed r) const { Fixed x = *this; x -= r; return x;}
	Fixed operator * (Fixed r) const { Fixed x = *this; x *= r; return x;}
	Fixed operator / (Fixed r) const { Fixed x = *this; x /= r; return x;}
	
	bool operator == (Fixed r) const { return int_value == r.int_value; }
	bool operator != (Fixed r) const { return !(*this == r); }
	bool operator <  (Fixed r) const { return int_value < r.int_value; }
	bool operator >  (Fixed r) const { return int_value > r.int_value; }
	bool operator <= (Fixed r) const { return int_value <= r.int_value; }
	bool operator >= (Fixed r) const { return int_value >= r.int_value; }

	Fixed operator + (int32_t r) const { Fixed x = *this; x += r; return x;}
	Fixed operator - (int32_t r) const { Fixed x = *this; x -= r; return x;}
	Fixed operator * (int32_t r) const { Fixed x = *this; x *= r; return x;}
	Fixed operator / (int32_t r) const { Fixed x = *this; x /= r; return x;}

  float toFloat() const { return (float)int_value / (1 << p); }
  int32_t wholePart() const { return int_value >> p; }
  float fractionPart() const { return (float)(int_value & (~(int32_t(-1) << p))) / (1 << p); }
};


// For use in unions
typedef int32_t FixedIntValue;

// This is a common type
typedef Fixed<16> Fixed16;

// Specializations for use with plain integers
template <int p>
inline Fixed<p> operator + (int32_t a, Fixed<p> b)
{ return b + a; }

template <int p>
inline Fixed<p> operator - (int32_t a, Fixed<p> b)
{ return -b + a; }

template <int p>
inline Fixed<p> operator * (int32_t a, Fixed<p> b)
{ return b * a; }

template <int p>
inline Fixed<p> operator / (int32_t a, Fixed<p> b)
{ Fixed<p> r(a); r /= b; return r; }

// math functions
// no default implementation

template <int p>
inline Fixed<p> sin(Fixed<p> a);

template <int p>
inline Fixed<p> cos(Fixed<p> a);

template <int p>
inline Fixed<p> sqrt(Fixed<p> a);

template <int p>
inline Fixed<p> rsqrt(Fixed<p> a);

template <int p>
inline Fixed<p> inv(Fixed<p> a);

// added by Karl
template <int p>
inline Fixed<p> square(Fixed<p> a) {
	Fixed<p> t;
  t.int_value = (int32_t)(((int64_t)a.int_value * (int64_t)a.int_value) >> p);
  return t;
}



// modified by Karl
// http://lab.polygonal.de/2007/05/10/bitwise-gems-fast-integer-math/
template <int p>
inline Fixed<p> abs(Fixed<p> a) {
  a.int_value = 
    (a.int_value ^ (a.int_value >> 31)) - (a.int_value >> 31);
  return a;
}

// added by Karl
template <int p>
inline Fixed<p> abs_neg(Fixed<p> a) {
  a.int_value = 
    (a.int_value >> 31) - (a.int_value ^ (a.int_value >> 31));
  return a;
}

// added by Karl.  in debug mode, fast methods will assert when they don't work
// http://graphics.stanford.edu/~seander/bithacks.html#IntegerMinOrMax
// called minfix/maxfix to avoid collision with min/max
template <int p>
inline Fixed<p> minfix(Fixed<p> a, Fixed<p> b) {
  Fixed<p> t;
  // r = y ^ ((x ^ y) & -(x < y)); // min(x, y)
  t.int_value = b.int_value ^ ((a.int_value ^ b.int_value) & -(a.int_value < b.int_value));
  return t;
}
template <int p>
inline Fixed<p> maxfix(Fixed<p> a, Fixed<p> b) {
  Fixed<p> t;
  // r = x ^ ((x ^ y) & -(x < y)); // max(x, y)
  t.int_value = a.int_value ^ ((a.int_value ^ b.int_value) & -(a.int_value < b.int_value));
  return t;
}
template <int p>
inline Fixed<p> minfast(Fixed<p> a, Fixed<p> b) {
  Fixed<p> t;
  // r = y + ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // min(x, y)
  t.int_value
    = b.int_value + ((a.int_value - b.int_value) & ((a.int_value - b.int_value) >> (sizeof(a.int_value) * 8 - 1)));
  assert(t.int_value == minfix(a,b).int_value);
  return t;
}
template <int p>
inline Fixed<p> maxfast(Fixed<p> a, Fixed<p> b) {
  Fixed<p> t;
  // r = x - ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // max(x, y)
  t.int_value = a.int_value - ((a.int_value - b.int_value) & ((a.int_value - b.int_value) >> (sizeof(a.int_value) * 8 - 1)));
  assert(t.int_value == maxfix(a,b).int_value);
  return t;
}

// added by Karl
// from http://dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
template <int p>
inline Fixed<p> atan2(Fixed<p> y, Fixed<p> x) {
#define ANGLE_COEF_3  Fixed<p>(0.1963)
#define ANGLE_COEF_1  Fixed<p>(-0.9817)
#define ANGLE_COEF_0  Fixed<p>(3.1415926535/4)

  Fixed<p> angle;
  if (x >= 0) {
    Fixed<p> ay = abs_neg(y);
    // y == -x -> divide-by-zero without this check
    Fixed<p> r = (x == ay) ? 1 : ((x + ay) / (x - ay));
    angle = (ANGLE_COEF_3 * r * r * r) + (ANGLE_COEF_1 * r) + ANGLE_COEF_0;
  } else {
    Fixed<p> ay = abs(y);
    // prevent x == y from divide-by-zero
    Fixed<p> r = (x == ay) ? 1 : ((x + ay) / (ay - x));
    angle = (ANGLE_COEF_3 * r * r * r) + (ANGLE_COEF_1 * r) + 3 * ANGLE_COEF_0;
  }

  return y < 0 ? -angle : angle;

#undef ANGLE_COEF_3
#undef ANGLE_COEF_1
#undef ANGLE_COEF_0
}

// specializations for 16.16 format

template <>
inline Fixed<16> sin(Fixed<16> a)
{
	Fixed<16> r;
	r.int_value = fixsin16(a.int_value);
	return r;
}

template <>
inline Fixed<16> cos(Fixed<16> a)
{
	Fixed<16> r;
	r.int_value = fixcos16(a.int_value);
	return r;
}


template <>
inline Fixed<16> sqrt(Fixed<16> a)
{
	Fixed<16> r;
	r.int_value = fixsqrt16(a.int_value);
	return r;
}

template <>
inline Fixed<16> rsqrt(Fixed<16> a)
{
	Fixed<16> r;
	r.int_value = fixrsqrt16(a.int_value);
	return r;
}

template <>
inline Fixed<16> inv(Fixed<16> a)
{
	Fixed<16> r;
	r.int_value = fixinv<16>(a.int_value);
	return r;
}

// The multiply accumulate case can be optimized.
template <int p>
inline Fixed<p> multiply_accumulate(
	int count, 
	const Fixed<p> *a,
	const Fixed<p> *b)
{
	long long result = 0;
	for (int i = 0; i < count; ++i)
		result += static_cast<long long>(a[i].int_value) * b[i].int_value;
	Fixed<p> r;
	r.int_value = static_cast<int>(result >> p);
	return r;
}

} // end namespace FixedPoint

#endif

