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
#ifndef __DCXD3DCOLOR_H__
#define __DCXD3DCOLOR_H__

#include <dc/byte>
#include <d3dx9.h>

namespace dcx {

    // Use the DirectCraft basic namespace
    using namespace dc;

    /**
     * Stores a set of three variables of the templated type
     */
    class dcxD3DColor : public dcResource<dcxD3DColor>
    {
        public:

            /**
             * Initializes this resource
             */
            dcxD3DColor()
            {
                member( "R", &myR );
                member( "G", &myG );
                member( "B", &myB );
            }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            dcByte* getR() { return &myR; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            dcByte* getG() { return &myG; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            dcByte* getB() { return &myB; }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            const dcByte* getR() const { return &myR; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            const dcByte* getG() const { return &myG; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            const dcByte* getB() const { return &myB; }

            D3DCOLOR getD3DColor() const {
              return D3DCOLOR_XRGB(myR.getValue(), myG.getValue(), myB.getValue());
            }

            void setD3DColor(D3DCOLOR color) {
              myR.setValue((unsigned char)(0xFF&(color>>16)));
              myG.setValue((unsigned char)(0xFF&(color>> 8)));
              myB.setValue((unsigned char)(0xFF&(color>> 0)));
            }

            COLORREF getRGBColor() const {
              return RGB(myR.getValue(), myG.getValue(), myB.getValue());
            }

            void setRGBColor(COLORREF color) {
              myR.setValue(GetRValue(color));
              myG.setValue(GetGValue(color));
              myB.setValue(GetBValue(color));
            }


        public:

            /**
             * Returns the name of this class type
             */
            static std::string staticTypeName()
            {
                return "D3DColor";
            }


        protected:

            /// Colors
            dcByte myR, myG, myB;
    };

}



#endif
