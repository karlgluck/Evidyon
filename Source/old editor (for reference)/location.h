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
#ifndef __LOCATION_H__
#define __LOCATION_H__
#pragma once

/*
template <typename T> dc::dcObjectWrapper : public dc::dcResource<dcGenericResource,dcObjectWrapper<T>>
{
    public:

    protected:

        T* myReference;
};*/


/**
 * This class references a preexisting raw value (int) somewhere in its parent class and allows
 * that value to be edited as if it were a DirectCraft object.
 */
class dcIntegerWrapper : public dc::dcResource<dcIntegerWrapper>
{
    public:

        typedef int BasicType;

    public:

        dcIntegerWrapper();

        dcIntegerWrapper(int* reference);

        void setValue(int value);

        int& getValue() const;

        bool save(dc::dcStreamOut* stream);
        bool load(dc::dcStreamIn* stream);

        virtual void serialize(std::string* string);
        virtual void interpret(const char* string);


    public:

        /**
         * Gets the string representation of this class
         */
        std::string toString() const;


    protected:

        int* myReference;
};


#include <dc/integer>


//
///**
// * Holds the integral location of something in the game; this corresponds to a square division on
// * the map.
// */
//class Location : public dc::dcObject<Location,std::pair<int,int>>
//{
//    public:
//
//        /// The coordinate type as a resource
//      typedef dc::dcInteger CoordinateType;
//
//        /// The coordinate type as a built-in C++ type
//        typedef CoordinateType::BasicType BasicCoordinateType;
//
//    public:
//
//        /**
//         * Initializes the class
//         */
//        Location();
//
//        /**
//         * X-member accessor
//         */
//        inline BasicCoordinateType getX() const { return myValue.first; }
//
//        /**
//         * X-member set
//         */
//        inline void setX(BasicCoordinateType x) { myValue.first = x; }
//
//        /**
//         * Y-member accessor
//         */
//        inline BasicCoordinateType getY() const { return myValue.second; }
//
//        /**
//         * Y-member set
//         */
//        inline void setY(BasicCoordinateType y) { myValue.second = y; }
//
//
//    public:
//
//        bool save(dc::dcStreamOut* stream);
//        bool load(dc::dcStreamIn* stream);
//
//        /**
//         * Gets the string representation of this class
//         */
//        std::string toString() const;
//
//
//    protected:
//
//        /**
//         * Method implemented by the derived class to compare the class
//         * to another resource that is guaranteed to be valid and of
//         * the same type.
//         * @see compareTo
//         */
//        int compareValue(const BasicType& otherValue) const;
//
//
//    public:
//
//        /**
//         * The name of this class type
//         */
//        static std::string staticTypeName();
//
//
//    protected:
//
//        dc::dcIntegerWrapper myX;
//        dc::dcIntegerWrapper myY;
//};



/**
 * Holds the integral location of something in the game; this corresponds to a square division on
 * the map.
 */
class Location : public dc::dcResource<Location>
{
    public:

        /// The coordinate type as a resource
      typedef dc::dcInteger CoordinateType;

        /// The coordinate type as a built-in C++ type
        typedef CoordinateType::BasicType BasicCoordinateType;

        typedef std::pair<BasicCoordinateType, BasicCoordinateType> BasicType;

    public:

        /**
         * Initializes the class
         */
        Location();

        inline BasicType getValue() const {
          return BasicType(getX(), getY());
        }

        inline void setValue(const BasicType& value) {
          setX(value.first);
          setY(value.second);
        }

        /**
         * X-member accessor
         */
        inline BasicCoordinateType getX() const { return myX.getValue(); }

        /**
         * X-member set
         */
        inline void setX(BasicCoordinateType x) { myX.setValue(x); }

        /**
         * Y-member accessor
         */
        inline BasicCoordinateType getY() const { return myY.getValue(); }

        /**
         * Y-member set
         */
        inline void setY(BasicCoordinateType y) { myY.setValue(y); }


    public:


        /**
         * Gets the string representation of this class
         */
        std::string toString() const;

      virtual int compareTo( const dc::dcGenericResource* other ) const;


    protected:


    public:

        /**
         * The name of this class type
         */
        static std::string staticTypeName();


    protected:

      dc::dcInteger myX;
      dc::dcInteger myY;
};


#endif