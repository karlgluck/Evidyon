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
#define T(letter) typename letter = void
#define T8(f) T(_##f##0), T(_##f##1), T(_##f##2), T(_##f##3), T(_##f##4), T(_##f##5), T(_##f##6), T(_##f##7)
#define T8BEGIN(f) T(_##f##0), T(_##f##1), T(_##f##2), T(_##f##3), T(_##f##4), T(_##f##5), T(_##f##6), T(_##f##7)
#define T8END(f)  T(_##f##0), T(_##f##1), T(_##f##2), T(_##f##3), T(_##f##4), T(_##f##5), T(_##f##6), T(_##f##7)
#define TLIST_64 T8BEGIN(0), T8(1), T8(2), T8(3), T8(4), T8(5), T8(6), T8END(7)


#define TYPEN(base,n) typename __##base##n
#define TYPE0(base)  TYPEN(base,0)
#define TYPE1(base)  TYPEN(base,0), TYPEN(base,1)
#define TYPE2(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2)
#define TYPE3(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2), TYPEN(base,3)
#define TYPE4(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2), TYPEN(base,3), TYPEN(base,4)
#define TYPE5(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2), TYPEN(base,3), TYPEN(base,4), TYPEN(base,5)
#define TYPE6(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2), TYPEN(base,3), TYPEN(base,4), TYPEN(base,5), TYPEN(base,6)
#define TYPE7(base)  TYPEN(base,0), TYPEN(base,1), TYPEN(base,2), TYPEN(base,3), TYPEN(base,4), TYPEN(base,5), TYPEN(base,6), TYPEN(base,7)
#define TYPES_0      
#define TYPES_1      TYPE7(0),
#define TYPES_2      TYPE7(0), TYPE7(1),
#define TYPES_3      TYPE7(0), TYPE7(1), TYPE7(2),
#define TYPES_4      TYPE7(0), TYPE7(1), TYPE7(2), TYPE7(3),
#define TYPES_5      TYPE7(0), TYPE7(1), TYPE7(2), TYPE7(3), TYPE7(4),
#define TYPES_6      TYPE7(0), TYPE7(1), TYPE7(2), TYPE7(3), TYPE7(4), TYPE7(5),
#define TYPES_7      TYPE7(0), TYPE7(1), TYPE7(2), TYPE7(3), TYPE7(4), TYPE7(5), TYPE7(6),
#define TYPES(base,plus) TYPES_##base TYPE##plus(base)


#define W_PARAMN(base,n) __##base##n v##base##n
#define W_PARAM0(base)  W_PARAMN(base,0)
#define W_PARAM1(base)  W_PARAMN(base,0), W_PARAMN(base,1)
#define W_PARAM2(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2)
#define W_PARAM3(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2), W_PARAMN(base,3)
#define W_PARAM4(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2), W_PARAMN(base,3), W_PARAMN(base,4)
#define W_PARAM5(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2), W_PARAMN(base,3), W_PARAMN(base,4), W_PARAMN(base,5)
#define W_PARAM6(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2), W_PARAMN(base,3), W_PARAMN(base,4), W_PARAMN(base,5), W_PARAMN(base,6)
#define W_PARAM7(base)  W_PARAMN(base,0), W_PARAMN(base,1), W_PARAMN(base,2), W_PARAMN(base,3), W_PARAMN(base,4), W_PARAMN(base,5), W_PARAMN(base,6), W_PARAMN(base,7)
#define W_PARAMS_0      
#define W_PARAMS_1      W_PARAM7(0),
#define W_PARAMS_2      W_PARAM7(0), W_PARAM7(1),
#define W_PARAMS_3      W_PARAM7(0), W_PARAM7(1), W_PARAM7(2),
#define W_PARAMS_4      W_PARAM7(0), W_PARAM7(1), W_PARAM7(2), W_PARAM7(3),
#define W_PARAMS_5      W_PARAM7(0), W_PARAM7(1), W_PARAM7(2), W_PARAM7(3), W_PARAM7(4),
#define W_PARAMS_6      W_PARAM7(0), W_PARAM7(1), W_PARAM7(2), W_PARAM7(3), W_PARAM7(4), W_PARAM7(5),
#define W_PARAMS_7      W_PARAM7(0), W_PARAM7(1), W_PARAM7(2), W_PARAM7(3), W_PARAM7(4), W_PARAM7(5), W_PARAM7(6),
#define W_PARAMS(base,plus) W_PARAMS_##base W_PARAM##plus(base)


//#define W_BODYN(base,n) buffer=SerializerWrite(_##base##n(v##base##n), buffer, end);
#define W_BODYN(base,n) buffer=Serializer<_##base##n>::write(v##base##n, buffer, end);
#define W_BODY0(base)  W_BODYN(base,0)
#define W_BODY1(base)  W_BODYN(base,0) W_BODYN(base,1)
#define W_BODY2(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2)
#define W_BODY3(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2) W_BODYN(base,3)
#define W_BODY4(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2) W_BODYN(base,3) W_BODYN(base,4)
#define W_BODY5(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2) W_BODYN(base,3) W_BODYN(base,4) W_BODYN(base,5)
#define W_BODY6(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2) W_BODYN(base,3) W_BODYN(base,4) W_BODYN(base,5) W_BODYN(base,6)
#define W_BODY7(base)  W_BODYN(base,0) W_BODYN(base,1) W_BODYN(base,2) W_BODYN(base,3) W_BODYN(base,4) W_BODYN(base,5) W_BODYN(base,6) W_BODYN(base,7)
#define W_BODY_0      
#define W_BODY_1      W_BODY7(0)
#define W_BODY_2      W_BODY7(0) W_BODY7(1)
#define W_BODY_3      W_BODY7(0) W_BODY7(1) W_BODY7(2)
#define W_BODY_4      W_BODY7(0) W_BODY7(1) W_BODY7(2) W_BODY7(3)
#define W_BODY_5      W_BODY7(0) W_BODY7(1) W_BODY7(2) W_BODY7(3) W_BODY7(4)
#define W_BODY_6      W_BODY7(0) W_BODY7(1) W_BODY7(2) W_BODY7(3) W_BODY7(4) W_BODY7(5)
#define W_BODY_7      W_BODY7(0) W_BODY7(1) W_BODY7(2) W_BODY7(3) W_BODY7(4) W_BODY7(5) W_BODY7(6)
#define W_BODY(base,plus) W_BODY_##base W_BODY##plus(base);


#define R_PARAMN(base,n) __##base##n *v##base##n
#define R_PARAM0(base)  R_PARAMN(base,0)
#define R_PARAM1(base)  R_PARAMN(base,0), R_PARAMN(base,1)
#define R_PARAM2(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2)
#define R_PARAM3(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2), R_PARAMN(base,3)
#define R_PARAM4(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2), R_PARAMN(base,3), R_PARAMN(base,4)
#define R_PARAM5(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2), R_PARAMN(base,3), R_PARAMN(base,4), R_PARAMN(base,5)
#define R_PARAM6(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2), R_PARAMN(base,3), R_PARAMN(base,4), R_PARAMN(base,5), R_PARAMN(base,6)
#define R_PARAM7(base)  R_PARAMN(base,0), R_PARAMN(base,1), R_PARAMN(base,2), R_PARAMN(base,3), R_PARAMN(base,4), R_PARAMN(base,5), R_PARAMN(base,6), R_PARAMN(base,7)
#define R_PARAMS_0      
#define R_PARAMS_1      R_PARAM7(0),
#define R_PARAMS_2      R_PARAM7(0), R_PARAM7(1),
#define R_PARAMS_3      R_PARAM7(0), R_PARAM7(1), R_PARAM7(2),
#define R_PARAMS_4      R_PARAM7(0), R_PARAM7(1), R_PARAM7(2), R_PARAM7(3),
#define R_PARAMS_5      R_PARAM7(0), R_PARAM7(1), R_PARAM7(2), R_PARAM7(3), R_PARAM7(4),
#define R_PARAMS_6      R_PARAM7(0), R_PARAM7(1), R_PARAM7(2), R_PARAM7(3), R_PARAM7(4), R_PARAM7(5),
#define R_PARAMS_7      R_PARAM7(0), R_PARAM7(1), R_PARAM7(2), R_PARAM7(3), R_PARAM7(4), R_PARAM7(5), R_PARAM7(6),
#define R_PARAMS(base,plus) R_PARAMS_##base R_PARAM##plus(base)


#define R_BODYN(base,n) buffer=Serializer<_##base##n>::read(buffer, end, v##base##n);
#define R_BODY0(base)  R_BODYN(base,0)
#define R_BODY1(base)  R_BODYN(base,0) R_BODYN(base,1)
#define R_BODY2(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2)
#define R_BODY3(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2) R_BODYN(base,3)
#define R_BODY4(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2) R_BODYN(base,3) R_BODYN(base,4)
#define R_BODY5(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2) R_BODYN(base,3) R_BODYN(base,4) R_BODYN(base,5)
#define R_BODY6(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2) R_BODYN(base,3) R_BODYN(base,4) R_BODYN(base,5) R_BODYN(base,6)
#define R_BODY7(base)  R_BODYN(base,0) R_BODYN(base,1) R_BODYN(base,2) R_BODYN(base,3) R_BODYN(base,4) R_BODYN(base,5) R_BODYN(base,6) R_BODYN(base,7)
#define R_BODY_0      
#define R_BODY_1      R_BODY7(0)
#define R_BODY_2      R_BODY7(0) R_BODY7(1)
#define R_BODY_3      R_BODY7(0) R_BODY7(1) R_BODY7(2)
#define R_BODY_4      R_BODY7(0) R_BODY7(1) R_BODY7(2) R_BODY7(3)
#define R_BODY_5      R_BODY7(0) R_BODY7(1) R_BODY7(2) R_BODY7(3) R_BODY7(4)
#define R_BODY_6      R_BODY7(0) R_BODY7(1) R_BODY7(2) R_BODY7(3) R_BODY7(4) R_BODY7(5)
#define R_BODY_7      R_BODY7(0) R_BODY7(1) R_BODY7(2) R_BODY7(3) R_BODY7(4) R_BODY7(5) R_BODY7(6)
#define R_BODY(base,plus) R_BODY_##base R_BODY##plus(base);




#define SERIALIZER_READ8(type,base) READ##type##(base,0); \
                                    READ##type##(base,1); \
                                    READ##type##(base,2); \
                                    READ##type##(base,3); \
                                    READ##type##(base,4); \
                                    READ##type##(base,5); \
                                    READ##type##(base,6); \
                                    READ##type##(base,7);
#define SERIALIZER_READ64(type) READ##type##_NOPARAMS();    \
                                SERIALIZER_READ8(type,0); \
                                SERIALIZER_READ8(type,1); \
                                SERIALIZER_READ8(type,2); \
                                SERIALIZER_READ8(type,3); \
                                SERIALIZER_READ8(type,4); \
                                SERIALIZER_READ8(type,5); \
                                SERIALIZER_READ8(type,6); \
                                SERIALIZER_READ8(type,7);


#define SERIALIZER_WRITE8(type,base) WRITE##type##(base,0); \
                                     WRITE##type##(base,1); \
                                     WRITE##type##(base,2); \
                                     WRITE##type##(base,3); \
                                     WRITE##type##(base,4); \
                                     WRITE##type##(base,5); \
                                     WRITE##type##(base,6); \
                                     WRITE##type##(base,7);

#define SERIALIZER_WRITE64(type) WRITE##type##_NOPARAMS();   \
                                SERIALIZER_WRITE8(type,0); \
                                SERIALIZER_WRITE8(type,1); \
                                SERIALIZER_WRITE8(type,2); \
                                SERIALIZER_WRITE8(type,3); \
                                SERIALIZER_WRITE8(type,4); \
                                SERIALIZER_WRITE8(type,5); \
                                SERIALIZER_WRITE8(type,6); \
                                SERIALIZER_WRITE8(type,7);

#define SERIALIZER_READ8(type,base) READ##type##(base,0); \
                                    READ##type##(base,1); \
                                    READ##type##(base,2); \
                                    READ##type##(base,3); \
                                    READ##type##(base,4); \
                                    READ##type##(base,5); \
                                    READ##type##(base,6); \
                                    READ##type##(base,7);
#define SERIALIZER_READ64(type) READ##type##_NOPARAMS();    \
                                SERIALIZER_READ8(type,0); \
                                SERIALIZER_READ8(type,1); \
                                SERIALIZER_READ8(type,2); \
                                SERIALIZER_READ8(type,3); \
                                SERIALIZER_READ8(type,4); \
                                SERIALIZER_READ8(type,5); \
                                SERIALIZER_READ8(type,6); \
                                SERIALIZER_READ8(type,7);


#define SERIALIZER_WRITE8(type,base) WRITE##type##(base,0); \
                                     WRITE##type##(base,1); \
                                     WRITE##type##(base,2); \
                                     WRITE##type##(base,3); \
                                     WRITE##type##(base,4); \
                                     WRITE##type##(base,5); \
                                     WRITE##type##(base,6); \
                                     WRITE##type##(base,7);

#define SERIALIZER_WRITE64(type) WRITE##type##_NOPARAMS();   \
                                SERIALIZER_WRITE8(type,0); \
                                SERIALIZER_WRITE8(type,1); \
                                SERIALIZER_WRITE8(type,2); \
                                SERIALIZER_WRITE8(type,3); \
                                SERIALIZER_WRITE8(type,4); \
                                SERIALIZER_WRITE8(type,5); \
                                SERIALIZER_WRITE8(type,6); \
                                SERIALIZER_WRITE8(type,7);