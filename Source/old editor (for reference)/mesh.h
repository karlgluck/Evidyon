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
#ifndef __MESH_H__
#define __MESH_H__
#pragma once


/**
* Generic interface to a mesh type
*/
class Mesh : public dc::dcGenericResource
{
public:

  /**
   * Contains raw information needed to display a preview of this resource
   */
  struct Preview
  {
      /// The resource that created this preview
      Mesh* mesh;

      /// The D3DX mesh that can be used to render this geometry
      LPD3DXMESH d3dxMesh;
  };


public:

  /**
   * Initializes this class
   * @param name The name of this resource
   */
  Mesh();

  /**
   * Loads a preview of this mesh into the provided structure
   * @param d3dDevice Device to use during the loading of the mesh (temporary usage)
   * @param preview Destination preview structure
   * @return Whether or not the preview could be loaded
   */
  bool loadPreview(LPDIRECT3DDEVICE9 d3dDevice, Preview* preview);

  /**
   * Loads the geometry specified by this mesh class in its final form.  This means that
   * any source transformations (such as scaling, rotation, or translation) and subset
   * remapping will be applied.
   *   @param pd3dDevice The source device (if necessary)
   *   @param subsetGeometry The destination for the mesh's subset geometry
   *   @return Result code
   */
  bool obtainGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const;

  /**
   * Loads the geometry for this mesh and parses it to determine the scaling factor required
   * to make the largest dimension 1.0 in object-space.
   *   @param pd3dDevice The source device (if necessary)
   *   @param scalingFactor Destination for the scaling factor value
   *   @return Whether or not the method succeeded
   */
  bool getUnityScalingFactor(LPDIRECT3DDEVICE9 pd3dDevice, float* scalingFactor) const;

  /**
   * Obtains an editable reference to the internal rotation structure
   */
  dcxVector3<dcFloat>* getRotation() const;

  /**
   * Obtains an editable reference to the internal scaling structure
   */
  dcxVector3<dcFloat>* getScaling() const;

  /**
   * Obtains an editable reference to the internal translation structure
   */
  dcxVector3<dcFloat>* getTranslation() const;


public:

  /**
   * Loads the geometry held by this class into the destination set.  Note that the
   * returned set is indexed by subset; also, the data that is returned MUST be editable.
   * This means that by editing whatever comes out, the original data loaded by the
   * class must not change.
   *   @param pd3dDevice The source device (if necessary)
   *   @param subsetGeometry The destination for the mesh's subset geometry
   *   @return Result code
   */
  virtual bool obtainSourceGeometry(LPDIRECT3DDEVICE9 pd3dDevice, SubsetGeometry* subsetGeometry) const = 0;


public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  /// Changes one subset in the mesh to another
  dc::dcMap<dcIndex32,dcIndex32> mySubsetRemap;

  /// Transforms the raw geometry of this mesh
  dcxVector3<dcFloat> myRotation;
  dcxVector3<dcFloat> myScaling;
  dcxVector3<dcFloat> myTranslation;
};



#endif