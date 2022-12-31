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
#include "mesh.h"


namespace Evidyon {
namespace Mesh {
namespace Tools {




  
//----[  Mesh  ]---------------------------------------------------------------
Mesh::Mesh() {
  member("Scaling", &scaling_);
  member("Rotation", &rotation_);
  member("Translation", &translation_); 
  scaling_.getX()->setValue(1.0f);
  scaling_.getY()->setValue(1.0f);
  scaling_.getZ()->setValue(1.0f);
}



//----[  getTransform  ]-------------------------------------------------------
void Mesh::getTransform(D3DXMATRIX* matrix) {
  D3DXMATRIX s, r, t;
  D3DXMatrixScaling(&s,
                    scaling_.getX()->getValue(),
                    scaling_.getY()->getValue(),
                    scaling_.getZ()->getValue());
  D3DXMatrixRotationYawPitchRoll(&r,
                                 D3DXToRadian(rotation_.getY()->getValue()),
                                 D3DXToRadian(rotation_.getX()->getValue()),
                                 D3DXToRadian(rotation_.getZ()->getValue()));
  D3DXMatrixTranslation(&t,
                        translation_.getX()->getValue(),
                        translation_.getY()->getValue(),
                        translation_.getZ()->getValue());
  D3DXMatrixMultiply(matrix, &s, &r);
  D3DXMatrixMultiply(matrix, matrix, &t);
}


//----[  getSRT  ]-------------------------------------------------------------
void Mesh::getSRT(float* sx, float* sy, float* sz,
                  float* rx, float* ry, float* rz,
                  float* tx, float* ty, float* tz) {
  *sx = scaling_.getX()->getValue();
  *sy = scaling_.getY()->getValue();
  *sz = scaling_.getZ()->getValue();
  *rx = D3DXToRadian(rotation_.getX()->getValue());
  *ry = D3DXToRadian(rotation_.getY()->getValue());
  *rz = D3DXToRadian(rotation_.getZ()->getValue());
  *tx = translation_.getX()->getValue();
  *ty = translation_.getY()->getValue();
  *tz = translation_.getZ()->getValue();
}



//----[  combineSRT  ]---------------------------------------------------------
void Mesh::combineSRT(float sx, float sy, float sz,
                      float rx, float ry, float rz,
                      float tx, float ty, float tz) {
  scaling_.getX()->setValue(scaling_.getX()->getValue() * sx);
  scaling_.getY()->setValue(scaling_.getY()->getValue() * sy);
  scaling_.getZ()->setValue(scaling_.getZ()->getValue() * sz);
  rotation_.getX()->setValue(rotation_.getX()->getValue() + D3DXToDegree(rx));
  rotation_.getY()->setValue(rotation_.getY()->getValue() + D3DXToDegree(ry));
  rotation_.getZ()->setValue(rotation_.getZ()->getValue() + D3DXToDegree(rz));
  translation_.getX()->setValue(translation_.getX()->getValue() + tx);
  translation_.getY()->setValue(translation_.getY()->getValue() + ty);
  translation_.getZ()->setValue(translation_.getZ()->getValue() + tz);
}

}
}
}

