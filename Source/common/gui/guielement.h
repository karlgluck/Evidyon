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
#ifndef __GUIELEMENT_H__
#define __GUIELEMENT_H__
#pragma once



class GUILayer;
class GUIGenericCanvas;
struct GUIRect;

/**
 * A GUI element is something that creates geometry in a GUILayer during rendering
 */
class GUIElement
{
  public:

    /**
     * Initializes the class
     */
    GUIElement();

    /**
     * Sets the canvas that this class uses for position and size references
     */
    void setSourceCanvas(GUIGenericCanvas* sourceCanvas);

    /** 
     * Sets the canvas outside of which this picture element will not draw
     */
    void setClippingArea(const GUIRect* clippingArea);

    /**
     * Gets the clipping area canvas
     */
    const GUIRect* getClippingArea();


  public:

    /**
     * Updates this element
     */
    virtual void advance(double currentTime, double timeSinceLastUpdate);

    /**
     * Renders this element into the given layer
     */
    virtual bool render(GUILayer* layer);

    /**
     * This method queries the element for the texture that it uses when rendering.  It
     * is invoked by the GUILayer class when the element is registered with the GUI.
     */
    virtual int getTexture() = 0;


  protected:

    /// This defines the rendering target for this element
    GUIGenericCanvas* mySourceCanvas;

    /// Defines the area into which to clip this element; if it doesn't require
    /// clipping, this is set to null.
    const GUIRect* myClippingArea;
};

#endif