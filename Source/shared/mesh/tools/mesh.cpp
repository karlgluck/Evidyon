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

