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
#ifndef __DCXD3DXMATRIX_H__
#define __DCXD3DXMATRIX_H__

#include <d3dx9.h>
#include <dc/float>
#include "dcxvector3.h"


namespace dcx {



  
//----[  dcxD3DXMatrix  ]------------------------------------------------------
class dcxD3DXMatrix : public dcResource<dcxD3DXMatrix> {
public:
  static std::string staticTypeName();
public:
  dcxD3DXMatrix();
  void copy(dcxD3DXMatrix* other);
  dcx::dcxVector3<dc::dcFloat>* getScaling();
  dcx::dcxVector3<dc::dcFloat>* getRotation();
  dcx::dcxVector3<dc::dcFloat>* getTranslation();
  void getTransform(D3DXMATRIX* matrix);

  // Obtains the transformation matrix to use for vertex normals
  void getNormalTransform(D3DXMATRIX* matrix);

  // Obtains the values of the scaling, rotation and translation parameters.
  // The rotation values are in radians.
  void getSRTRadians(float* sx, float* sy, float* sz,
                     float* rx, float* ry, float* rz,
                     float* tx, float* ty, float* tz);

  // This method converts the rotation parameters in radians to the values
  // stored internally, which are in degrees.
  void setSRTRadians(float sx, float sy, float sz,
                     float rx, float ry, float rz,
                     float tx, float ty, float tz);

  // Multiplies the given values into this matrix.  The incoming rotations
  // are in radians.
  void combineSRTRadians(float sx, float sy, float sz,
                         float rx, float ry, float rz,
                         float tx, float ty, float tz);

protected:

  // These factors are applied in the order they are declared
  dcx::dcxVector3<dc::dcFloat> scaling_, rotation_, translation_; 
};

}



#endif