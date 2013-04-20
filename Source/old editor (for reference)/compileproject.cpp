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
#include "stdafx.h"
#include "allgameresources.h"
#include "compileproject.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include  "../shared/client-editor/clientspelldescription.h"
#include  "../shared/server-editor/serverspelldescription.h"
#include  "../shared/compiledspecialfx.h"
#include "../shared/server-editor/serverprojectiledescription.h"
#include "../shared/client-editor/clientprojectiledescription.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "actordescriptiontemplatecompiler.h"




//------------------------------------------------------------------------------------------------
// Name:  CompileClientResourceArray
// Desc:  This templated function makes it easy to save an array of resource data.  There must be
//        a corresponding CompileClientResources(T) function.  T must be a pointer type.
//------------------------------------------------------------------------------------------------
template <typename T> bool CompileClientResourceArray(const dc::dcGenericResource::Array& members, dc::dcStreamOut* stream)
{
    // Compile-Time:  make sure the template parameter T is a pointer
    {
        int foo = 0;
        T pointer = (T)&foo;
    }

    // Make sure the parameters are valid
    if (APP_ERROR(!stream)("Invalid parameter to CompileClientResourceArray"))
        return false;

    // Save the number of entries in this array
    size_t count = members.size();
    if (APP_WARNING(!stream->write(&count, sizeof(count))) ("Failed to write size of array")) return false;

    // Iterate through the array
    for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i)
    {
        // Get the entry out of the array
        if (APP_WARNING(!CompileClientResource((T)(*i), stream))("Failed to compile array member %s", (*i)->getName().c_str()))
            return false;
    }

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileServerResourceArray
// Desc:  This templated function makes it easy to save an array of resource data.  There must be
//        a corresponding CompileClientResources(T) function.  T must be a pointer type.
//------------------------------------------------------------------------------------------------
template <typename T> bool CompileServerResourceArray(const dc::dcGenericResource::Array& members, dc::dcStreamOut* stream)
{
    // Compile-Time:  make sure the template parameter T is a pointer
    {
        int foo = 0;
        T pointer = (T)&foo;
    }

    // Make sure the parameters are valid
    if (APP_ERROR(!stream)("Invalid parameter to CompileClientResourceArray"))
        return false;

    // Save the number of entries in this array
    size_t count = members.size();
    if (APP_WARNING(!stream->write(&count, sizeof(count))) ("Failed to write size of array")) return false;

    // Iterate through the array
    for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i)
    {
        // Get the entry out of the array
        if (APP_WARNING(!CompileServerResource((T)(*i), stream))("Failed to compile array member %s", (*i)->getName().c_str()))
            return false;
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcTable<Image>::Element* imageElement, dc::dcStreamOut* stream)
{
    // Get the source type of this image
    const Image* image = imageElement->getImplementation();

    // Let the debug log know what's going on
    DEBUG_INFO("Compiling image %s", imageElement->getName().c_str());

    // Storage location for the loaded texture
    LPDIRECT3DTEXTURE9 d3dTexture = NULL;

    // Get a texture in the managed resource pool without a mipmap chain.  The mipmap chain
    // is unnecessary because we only care about the root image; the system memory pool is used
    // because it is fast and we don't need to ever actually *use* this texture in a D3D call.
    if (image && image->obtainD3DTexture(globalEditor()->getD3DDevice(), D3DTEX_SYSTEMMEM, &d3dTexture))
    {
        // Save the texture into a buffer
        LPD3DXBUFFER pSaveBuffer;
        HRESULT hr;
        hr = D3DXSaveTextureToFileInMemory(&pSaveBuffer, D3DXIFF_PNG, d3dTexture, NULL);

        // Free the texture reference
        SAFE_RELEASE(d3dTexture);

        // If the saving failed, exit
        if (APP_ERROR(FAILED(hr))("D3DXSaveTextureToFileInMemory failed for %s!", imageElement->getName().c_str()))
            return false;

        // Save the byte size of this texture and the contents of the save buffer
        size_t byteSize = pSaveBuffer->GetBufferSize();
        bool failed = !stream->write(&byteSize, sizeof(byteSize)) ||
                      !stream->write(pSaveBuffer->GetBufferPointer(), byteSize);

        // Get rid of the in-memory buffer
        SAFE_RELEASE(pSaveBuffer);

        // If we couldn't save into the stream, exit
        if (APP_ERROR(failed)("Unable to save %lu-byte image into the destination file", byteSize))
            return false;
    }
    else
    {
        // Output a warning
        DEBUG_WARNING("This image is invalid or couldn't be loaded, and can't be saved to the stream");

        // Failed to compile
        return false;
    }

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcTable<Texture>::Element* textureElement, dc::dcStreamOut* stream)
{
    // Get the texture from this element
    const Texture* texture = textureElement->getImplementation();

    // Tell the log what's going on
    DEBUG_INFO("Compiling texture %s", textureElement->getName().c_str());

    // Get the image that this texture references and find its index
    dc::dcTable<Image>::Reference* imageRef = texture->getImageRef();
    dc::dcTable<Image>::Element* referencedElement = imageRef->getReferencedResource();
    size_t index = referencedElement == NULL ? 0xFFFFFFFF : referencedElement->getIndex();

    // Generate the generic flags for this texture
    DWORD flags = (texture->getFlipHorizontalValue() ? 8 : 0) |
                  (texture->getFlipVerticalValue() ? 4 : 0) |
                  (texture->getEnableAlphaTestingValue() ? 2 : 0) |
                  (texture->getEnableAlphaBlendingValue() ? 1 : 0);

    float param1 = 0.0f, param2 = 0.0f;
    if (texture->getTypeID() == dc::dcClass<CirclingTexture>::staticResourceManager()->uniqueID())
    {
        CirclingTexture* circlingTexture = (CirclingTexture*)texture;
        param1 = circlingTexture->getCirclingSpeedValue();
        param2 = circlingTexture->getCirclingMangitudeValue();
        flags |= 16;
    }
    else if (texture->getTypeID() == dc::dcClass<SlidingTexture>::staticResourceManager()->uniqueID())
    {
        SlidingTexture* slidingTexture = (SlidingTexture*)texture;
        param1 = slidingTexture->getMovementSpeedValue();
        param2 = D3DXToRadian(slidingTexture->getMovementAngleValue());
        flags |= 32;
    }
    else if (texture->getTypeID() == dc::dcClass<AnimatedTexture>::staticResourceManager()->uniqueID())
    {
        AnimatedTexture* animatedTexture = (AnimatedTexture*)texture;
        param1 = animatedTexture->getAnimationSpeedValue();
        param2 = 0.0f;

        // Put the frame size in the upper 16 bits
        flags |= (animatedTexture->getFramesWideValue()<<24) | (animatedTexture->getFramesHighValue()<<16) | 64;
    }

    // Save the texture's data
    if (APP_ERROR(!stream->write(&index, sizeof(index)) ||
                   !stream->write(&flags, sizeof(flags)) ||
                   !stream->write(&param1, sizeof(param1)) ||
                   !stream->write(&param2, sizeof(param2)))("Couldn't write texture data"))
        return false;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(Skin::Map::Element* skinMapElement, dc::dcStreamOut* stream)
{
    // Get the key value
    int key = skinMapElement->getKey()->getValue();

    // Get the texture that this map entry references, and find its index
    dc::dcTable<Texture>::Element* referencedElement = skinMapElement->getValue()->getReferencedResource();
    size_t index = referencedElement == NULL ? 0xFFFFFFFF : referencedElement->getIndex();

    // Save the referenced texture index
    if (APP_ERROR(!stream->write(&key, sizeof(key)) ||
                   !stream->write(&index, sizeof(index)))
            ("Failed to write skin map element %s", skinMapElement->getName().c_str()))
        return false;

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<Skin>::Element* skinElement, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling skin %s", skinElement->getName().c_str());

    // Compile the map that this skin uses
    return CompileClientResourceArray<Skin::Map::Element*>(skinElement->getMap()->getMembers(), stream);
}



//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcTable<Mesh>::Element* meshElement, dc::dcStreamOut* stream)
{
    // Get the mesh from this element
    const Mesh* mesh = meshElement->getImplementation();

    // Print this to some sort of log!
    DEBUG_INFO("Compiling mesh %s", meshElement->getName().c_str());

    // Get the mesh's source data
    SubsetGeometry subsetGeometry;
    if (APP_ERROR(!mesh->obtainGeometry(globalEditor()->getD3DDevice(), &subsetGeometry))
            ("obtainGeometry failed for mesh %s during compilation", meshElement->getName().c_str()))
        return false;

    // Holds the return value from this function.  Until we make it all the way through the function,
    // this value indicates failure--that way, by using 'goto' to snap to the cleanup section in case
    // of an error, the function returns false and we don't have to assign false in multiple places.
    bool resultCode = false;

    // Save the number of subsets
    size_t subsets = subsetGeometry.size();
    if (APP_ERROR(!stream->write(&subsets, sizeof(subsets)))("Unable to save subset count"))
        goto COMPILE_RESOURCE_MESH_FAILURE;

    // Save all of the mesh's geometry
    for (SubsetGeometry::iterator i = subsetGeometry.begin(); i != subsetGeometry.end(); ++i)
    {
        // Save the geometry for this subset in the following format:
        //      SubsetIndex     subsetIndex;
        //      DWORD           numVertices;
        //      GeometryVertex  vertexArray[numVertices];
        //      DWORD           numIndices;
        //      GeometryIndex   indexArray[numIndices];
        bool failed = !stream->write(&i->first, sizeof(SubsetIndex)) ||
                      !stream->write(&i->second->NumVertices, sizeof(DWORD)) ||
                      !stream->write(&i->second->NumIndices, sizeof(DWORD)) ||
                      ((i->second->NumVertices > 0) && !stream->write(i->second->pVertices, sizeof(GeometryVertex) * i->second->NumVertices)) ||
                      ((i->second->NumIndices > 0) && !stream->write(i->second->pIndices, sizeof(GeometryIndex) * i->second->NumIndices));

        // Make sure the subset's geometry was saved correctly
        if (APP_ERROR(failed)("Unable to save geometry for subset %lu in mesh %s", (unsigned int)(i->first), meshElement->getName().c_str()))
            goto COMPILE_RESOURCE_MESH_FAILURE;
    }

    // We successfully compiled the mesh
    resultCode = true;

// This label is jumped to when an error occurs
COMPILE_RESOURCE_MESH_FAILURE:

    // Release the mesh geometry
    DeallocateGeometry(&subsetGeometry);

    // Return the result code
    return resultCode;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<Scenery>::Element* sceneryElement, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling scenery %s", sceneryElement->getName().c_str());

    // These are the different references that are going to be saved
    size_t meshRefIndex, skinRefIndex;

    // Get the mesh reference
    {
        dc::dcTable<Mesh>::Element* referencedMeshElement = sceneryElement->getMeshRef()->getReferencedResource();
        meshRefIndex = referencedMeshElement == NULL ? 0xFFFFFFFF : referencedMeshElement->getIndex();
    }

    // Get the skin reference
    {
        dc::dcList<Skin>::Element* referencedSkinElement = sceneryElement->getSkinRef()->getReferencedResource();
        skinRefIndex = referencedSkinElement == NULL ? 0xFFFFFFFF : referencedSkinElement->getIndex();
    }
    // The world matrix
    D3DXMATRIX worldMatrix;

    // Save the transformation matrix
    {
        D3DXMATRIXA16 scale, rotate, translate;
        dcxVector3<dcFloat>* scale_vec3 = sceneryElement->getScaling();
        dcxVector3<dcFloat>* rotate_vec3 = sceneryElement->getRotation();
        dcxVector3<dcFloat>* translate_vec3 = sceneryElement->getTranslation();
        D3DXMatrixScaling(&scale, scale_vec3->getX()->getValue(), scale_vec3->getY()->getValue(), scale_vec3->getZ()->getValue());
        D3DXMatrixRotationYawPitchRoll(&rotate, D3DXToRadian(rotate_vec3->getY()->getValue()),
                                                 D3DXToRadian(rotate_vec3->getX()->getValue()),
                                                 D3DXToRadian(rotate_vec3->getZ()->getValue()));
        D3DXMatrixTranslation(&translate, translate_vec3->getX()->getValue(), translate_vec3->getY()->getValue(), translate_vec3->getZ()->getValue());

        // Combine all of the matrices into the world matrix
        D3DXMatrixMultiply(&worldMatrix, &scale, &rotate);
        D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translate);
    }

    // Save all of the data for this scenery element (reference indices and world matrix)
    if (APP_ERROR(!stream->write(&meshRefIndex, sizeof(meshRefIndex)) ||
                   !stream->write(&skinRefIndex, sizeof(skinRefIndex)) ||
                   !stream->write(&worldMatrix,  sizeof(worldMatrix)))
            ("Failed to save scenery element %s", sceneryElement->getName().c_str()))
        return false;

    // Success
    return true;
}




////------------------------------------------------------------------------------------------------
//// Name:  CompileClientResource
//// Desc:  Writes raw data that the client application can use into the destination stream
////------------------------------------------------------------------------------------------------
//bool CompileClientResource(const dc::dcList<PlayerCharacterRace>::Element* raceElement, dc::dcStreamOut* stream)
//{
//    // Let the log know what we're doing
//    DEBUG_INFO("Compiling race %s for client", raceElement->getName().c_str());
//
//    // Save the name of the race
//    char race[CLIENTGAMEFILE_RACE_STRING_LENGTH];
//    strcpy_s(race, CLIENTGAMEFILE_RACE_STRING_LENGTH, raceElement->getName().c_str());
//    stream->write(race, CLIENTGAMEFILE_RACE_STRING_LENGTH * sizeof(char));
//
//    // Write each stat max
//    for (int i = 0; i < 5; ++i)
//    {
//        unsigned char b;
//
//        switch(i) 
//        {
//            case 0: b = raceElement->getMaxStrengthValue(); break;
//            case 1: b = raceElement->getMaxAgilityValue(); break;
//            case 2: b = raceElement->getMaxConstitutionValue(); break;
//            case 3: b = raceElement->getMaxIntelligenceValue(); break;
//            case 4: b = raceElement->getMaxWisdomValue(); break;
//        }
//
//        // Write this element
//        if (!stream->write(&b, sizeof(unsigned char))) return false;
//    }
//
//    // Success
//    return true;
//}


////------------------------------------------------------------------------------------------------
//// Name:  CompileClientResource
//// Desc:  Writes raw data that the client application can use into the destination stream
////------------------------------------------------------------------------------------------------
//bool CompileClientResource(const dc::dcList<PlayerCharacterClass>::Element* classElement, dc::dcStreamOut* stream)
//{
//    // Let the log know what we're doing
//    DEBUG_INFO("Compiling class %s for client", classElement->getName().c_str());
//
//    // Write the name of the class
//    char classBuffer[CLIENTGAMEFILE_CLASS_STRING_LENGTH];
//    strcpy_s(classBuffer, CLIENTGAMEFILE_CLASS_STRING_LENGTH, classElement->getName().c_str());
//    stream->write(classBuffer, CLIENTGAMEFILE_CLASS_STRING_LENGTH * sizeof(char));
//
//    // Success
//    return true;
//}







//
////------------------------------------------------------------------------------------------------
//// Name:  CompileServerResource
//// Desc:  Writes raw data that the server application can use into the destination stream
////------------------------------------------------------------------------------------------------
//bool CompileServerResource(const dc::dcList<PlayerCharacterRace>::Element* raceElement, dc::dcStreamOut* stream)
//{
//    // Let the log know what we're doing
//    DEBUG_INFO("Compiling race %s for server", raceElement->getName().c_str());
//
//    // Write each stat max
//    for (int i = 0; i < 5; ++i)
//    {
//        unsigned char b;
//
//        switch(i) 
//        {
//            case 0: b = raceElement->getMaxStrengthValue(); break;
//            case 1: b = raceElement->getMaxAgilityValue(); break;
//            case 2: b = raceElement->getMaxConstitutionValue(); break;
//            case 3: b = raceElement->getMaxIntelligenceValue(); break;
//            case 4: b = raceElement->getMaxWisdomValue(); break;
//        }
//
//        // Write this element
//        if (!stream->write(&b, sizeof(unsigned char))) return false;
//    }
//
//    for (int i = 0; i < (int)Avatar::NUMBER_OF_AVATAR_GENDERS; ++i) {
//      int actor_type_template = raceElement->getActorTypeTemplate((Evidyon::AvatarGender)i);
//      if (!stream->write(&actor_type_template, sizeof(actor_type_template))) return false;
//    }
///*
//    // Get the actor type references
//    const dc::dcList<ActorType>::Element* maleActor = raceElement->getActorType(GENDER_MALE);
//    const dc::dcList<ActorType>::Element* femaleActor = raceElement->getActorType(GENDER_FEMALE);
//
//    // Get the actor type indices
//    DWORD maleActorIndex = maleActor ? maleActor->getIndex() : 0xFFFFFFFF;
//    DWORD femaleActorIndex = maleActor ? femaleActor->getIndex() : 0xFFFFFFFF;
//
//    // Write the actor types
//    if (!stream->write(&maleActorIndex, sizeof(DWORD)))  return false;
//    if (!stream->write(&femaleActorIndex, sizeof(DWORD))) return false;
//*/
//    // Success
//    return true;
//}

////------------------------------------------------------------------------------------------------
//// Name:  CompileServerResource
//// Desc:  Writes raw data that the server application can use into the destination stream
////------------------------------------------------------------------------------------------------
//bool CompileServerResource(const dc::dcList<PlayerCharacterClass>::Element* classElement, dc::dcStreamOut* stream)
//{
//    // Let the log know what we're doing
//    DEBUG_INFO("Compiling class %s for server", classElement->getName().c_str());
//
//    // Write the relavent info
//    AvatarClassDescription description;
//    classElement->fillServerDescription(&description);
//    if (APP_ERROR(!stream->write(&description, sizeof(description)))("Couldn't write class data"))
//        return false;
//
//    // Success
//    return true;
//}






//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<Item>::Element* element, dc::dcStreamOut* stream)
{
    ClientItemDescription description;
    if (APP_ERROR(!element->compileForClient(&description) ||
                   !stream->write(&description, sizeof(description)))("Unable to save item %s for client", element->getName().c_str()))
       return false;

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name:  CompileServerResource
// Desc:  
//------------------------------------------------------------------------------------------------
bool CompileServerResource(const dc::dcList<Item>::Element* element, dc::dcStreamOut* stream)
{
    ServerItemDescription description;
    description.type_index = element->getIndex();
    if (APP_ERROR(!element->fillServerDescription(&description) ||
                   !stream->write(&description, sizeof(description)))("Unable to save item %s for server", element->getName().c_str()))
       return false;

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<UnseenSkinnedMeshResource>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling skinned mesh %s for client", element->getName().c_str());

    // Write the skinned mesh info into the stream
    UnseenSkinnedMesh skinnedMesh;
    ZeroMemory(&skinnedMesh, sizeof(skinnedMesh));
    bool failed = APP_ERROR(!element->loadSkinnedMesh(&skinnedMesh))("Couldn't load skinned mesh!") ||
                  APP_ERROR(!saveUnseenSkinnedMesh(stream, &skinnedMesh))("Unable to save skinned mesh");

    // Free the memory
    deallocateUnseenSkinnedMesh(&skinnedMesh);

    // Return the result code
    return !failed;
}



//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<UnseenKeyframeAnimation>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling skinned mesh animation %s for client", element->getName().c_str());

    // Write the animation into the stream
    UnseenSkeletalAnimation animation;
    ZeroMemory(&animation, sizeof(animation));
    bool failed = APP_ERROR(!element->loadAnimation(&animation))("Couldn't load animation") ||
                  APP_ERROR(!saveUnseenSkeletalAnimation(stream, &animation))("Unable to save mesh animation");

    // Free the memory
    deallocateUnseenSkeletalAnimation(&animation);

    // Return the result code
    return !failed;
}






//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<ActorType>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling actor type %s for client", element->getName().c_str());

    ClientActorTypeDescription description;
    if (APP_ERROR(!element->compileForClient(&description))("Couldn't get actor type description"))
        return false;

    return stream->write(&description, sizeof(ClientActorTypeDescription));
}




//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<WAVSound>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling wav sound %s for client", element->getName().c_str());

    if (APP_ERROR(!element->compileForClient(stream))("Couldn't compile WAV sound"))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<Spell>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling spell %s for client", element->getName().c_str());

    Evidyon::ClientSpellDescription description;
    element->compileForClient(&description);
    if (APP_ERROR(!stream->write(&description, sizeof(description)))("Couldn't compile spell"))
        return false;

    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<VisualFXImage>::Element* element, dc::dcStreamOut* stream)
{
  // Let the log know what we're doing
  DEBUG_INFO("Compiling visual FX image %s for client", element->getName().c_str());

  Evidyon::CompiledVisualFXImage compiled;
  element->compileForClient(&compiled);
  if (APP_ERROR(!stream->write(&compiled, sizeof(compiled)))("Couldn't compile"))
      return false;

  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcList<MagicFX>::Element* element, dc::dcStreamOut* stream)
{
  // Let the log know what we're doing
  DEBUG_INFO("Compiling magic FX %s for client", element->getName().c_str());

  Evidyon::CompiledMagicFXParams compiled;
  element->compileForClient(&compiled);
  if (APP_ERROR(!stream->write(&compiled, sizeof(compiled)))("Couldn't compile"))
      return false;
  return true;
}


template <typename Element, typename Description> bool CompileServerResourceStructure(Element* element, dc::dcStreamOut* stream) {
  DEBUG_INFO("Compiling %s", element->getName().c_str());
  Description description;
  element->compileForServer(&description);
  CONFIRM(stream->write(&description, sizeof(description))) else {
    DEBUG_INFO("Couldn't compile %s", element->getName().c_str());
    return false;
  }
  return true;
}


template <typename Element, typename Description> bool CompileClientResourceStructure(Element* element, dc::dcStreamOut* stream) {
  DEBUG_INFO("Compiling %s", element->getName().c_str());
  Description description;
  element->compileForClient(&description);
  CONFIRM(stream->write(&description, sizeof(description))) else {
    DEBUG_INFO("Couldn't compile %s", element->getName().c_str());
    return false;
  }
  return true;
}


//------------------------------------------------------------------------------------------------
// Name:  CompileClientResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileClientResource(const dc::dcTable<VisualFX>::Element* element, dc::dcStreamOut* stream){
  return CompileClientResourceStructure<const VisualFX,
    Evidyon::CompiledVisualFXParams>(element->getImplementation(), stream);
}


//------------------------------------------------------------------------------------------------
// Name:  CompileServerResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileServerResource(const dc::dcTable<Magic>::Element* element, dc::dcStreamOut* stream)
{
    // Let the log know what we're doing
    DEBUG_INFO("Compiling magic %s for server", element->getName().c_str());

    Evidyon::MagicDescription description;
    element->getImplementation()->compileForServer(&description);
    if (APP_ERROR(!stream->write(&description, sizeof(description)))("Couldn't compile magic"))
        return false;

    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  CompileServerResource
// Desc:  Writes raw data that the client application can use into the destination stream
//------------------------------------------------------------------------------------------------
bool CompileServerResource(const dc::dcList<Spell>::Element* element, dc::dcStreamOut* stream)
{
  // Let the log know what we're doing
  DEBUG_INFO("Compiling spell %s for server", element->getName().c_str());

  Evidyon::ServerSpellDescription description;
  element->compileForServer(&description);
  if (APP_ERROR(!stream->write(&description, sizeof(description)))("Couldn't compile spell"))
      return false;

  return true;
}

//----[  CompileServerResource  ]----------------------------------------------
// Writes raw data that the client application can use into the destination
// stream
bool CompileServerResource(const dc::dcList<Projectile>::Element* element,
                           dc::dcStreamOut* stream) {
  return CompileServerResourceStructure
    <const dc::dcList<Projectile>::Element,
     Evidyon::ServerProjectileDescription>(element, stream);
}


//----[  CompileClientResource  ]----------------------------------------------
bool CompileClientResource(const dc::dcList<Projectile>::Element* element,
                           dc::dcStreamOut* stream) {
  return CompileClientResourceStructure
    <const dc::dcList<Projectile>::Element,
     Evidyon::ClientProjectileDescription>(element, stream);
}




//----[  CompileServerResource  ]----------------------------------------------
bool CompileServerResource(const dc::dcTable<LifeformAI>::Element* element,
                           dc::dcStreamOut* stream) {
  return CompileServerResourceStructure
    <const LifeformAI,
     Evidyon::LifeformAIDescription>(element->getImplementation(), stream);
}





//----[  CompileClientResource  ]----------------------------------------------
bool CompileClientResource(const dc::dcList<Quest>::Element* element,
                           dc::dcStreamOut* stream) {
  DEBUG_INFO("Compiling %s", element->getName().c_str());
  Evidyon::Quests::Client::Quest description;
  element->compile(&description);
  CONFIRM(stream->write(&description, sizeof(description))) else {
    DEBUG_INFO("Couldn't compile %s", element->getName().c_str());
    return false;
  }
  return true;
}



//----[  CompileServerResource  ]----------------------------------------------
bool CompileServerResource(const dc::dcList<Quest>::Element* element,
                           dc::dcStreamOut* stream) {
  DEBUG_INFO("Compiling %s", element->getName().c_str());
  Evidyon::Quests::Server::Quest description;
  element->compile(&description);
  CONFIRM(stream->write(&description, sizeof(description))) else {
    DEBUG_INFO("Couldn't compile %s", element->getName().c_str());
    return false;
  }
  return true;
}







//----[  CompileProject  ]-----------------------------------------------------
bool CompileProject(ProjectVolucris* pv,
                    dc::dcStreamOut* clientStream,
                    dc::dcStreamOut* serverStream)
{
  {
    std::list<std::string> names;
    pv->getReservedNames(&names);
    {
      size_t size = names.size();
      serverStream->write(&size, sizeof(size));
    }
    for (std::list<std::string>::iterator i = names.begin(); i != names.end(); ++i) {
      size_t length = i->length();
      serverStream->write(&length, sizeof(length));
      serverStream->write(i->c_str(), length);
    }
  }

  // Sort the textures so that they are ordered in the correct way for rendering
  pv->getImageTable()->sort();
  pv->getTextureTable()->sort();
  //pv->getItemsList()->sort(); // items don't have to be sorted now

  //// Assign type IDs to all of the invalid entries, now that they are being officially compiled
  //{
  //    int nextTypeIndex = 0;
  //    for (dcGenericResource::Array::const_iterator i = pv->getItemsList()->getMembers().begin(); i != pv->getItemsList()->getMembers().end(); ++i)
  //    {
  //        Item* item = (Item*)(*i);
  //        int typeIndex = item->getTypeIndex();
  //        if (typeIndex == INT_MAX)
  //            item->setTypeIndex(nextTypeIndex++);
  //        else
  //            if (nextTypeIndex <= typeIndex)
  //                nextTypeIndex = typeIndex + 1;
  //    }
  //}

  // Compile images
  if (!CompileClientResourceArray<const dc::dcTable<Image>::Element*>(pv->getImageTable()->getMembers(), clientStream))
      return false;

  // Save the static image references
  {
    dc::dcTable<Image>::Element* image;
    size_t index;

    // First image is the font
    image = pv->getFontImage()->getReferencedResource();
    index = image != 0 ? image->getIndex() : 0xFFFFFFFF;
    if (!clientStream->write(&index, sizeof(index))) return false;

    // Second image is the title
    image = pv->getTitleMainImage()->getReferencedResource();
    index = image != 0 ? image->getIndex() : 0xFFFFFFFF;
    if (!clientStream->write(&index, sizeof(index))) return false;

    // Third image is the title without text
    image = pv->getTitleBackgroundImage()->getReferencedResource();
    index = image != 0 ? image->getIndex() : 0xFFFFFFFF;
    if (!clientStream->write(&index, sizeof(index))) return false;

    // Fourth image contains GUI stuff
    image = pv->getGUIImage()->getReferencedResource();
    index = image != 0 ? image->getIndex() : 0xFFFFFFFF;
    if (!clientStream->write(&index, sizeof(index))) return false;

    // Fifth image is the Evidyon symbol
    image = pv->getEvidyonSymbolImage()->getReferencedResource();
    index = image != 0 ? image->getIndex() : 0xFFFFFFFF;
    if (!clientStream->write(&index, sizeof(index))) return false;

    index = pv->getDayNightLightParticleImageImage()->getReferencedResourceIndex();
    if (!clientStream->write(&index, sizeof(index))) return false;
  }

  // Compile textures
  if (!CompileClientResourceArray<const dc::dcTable<Texture>::Element*>(pv->getTextureTable()->getMembers(), clientStream))
      return false;

  // Compile mesh skins
  if (!CompileClientResourceArray<const dc::dcList<Skin>::Element*>(pv->getSkinList()->getMembers(), clientStream))
      return false;

  // Compile the meshes themselves
  if (!CompileClientResourceArray<const dc::dcTable<Mesh>::Element*>(pv->getMeshTable()->getMembers(), clientStream))
      return false;

  // Save the scenery
  if (!CompileClientResourceArray<const dc::dcList<Scenery>::Element*>(pv->getSceneryList()->getMembers(), clientStream))
      return false;


  ActorDescriptionTemplateCompiler actor_description_template_compiler_;

  // Build the character race actor description information
  { // TODO: make this a method!
    const dc::dcGenericResource::Array& races =
      pv->getAvatarRaceList()->getMembers();
    for (dc::dcGenericResource::Array::const_iterator i = races.begin(); i != races.end(); ++i) {
      PlayerCharacterRace* race = (PlayerCharacterRace*)*i;
      CONFIRM(race->compileActorTemplates(&actor_description_template_compiler_)) else return false;
    }
  }
  // Save the player-character information to the client
  CONFIRM(pv->getAvatarRaceList()->compileForClient(clientStream) &&
          pv->getAvatarClassList()->compileForClient(clientStream)) else return false;

  // Save the player-character information to the server
  CONFIRM(pv->getAvatarRaceList()->compileForServer(serverStream) &&
          pv->getAvatarClassList()->compileForServer(serverStream)) else return false;
  if (!pv->getLevelDefinitions()->compileForServer(serverStream))
      return false;

  // Compile sound effects
  if (!CompileClientResourceArray<const dc::dcList<WAVSound>::Element*>(pv->getWAVSoundList()->getMembers(), clientStream))
      return false;

  // Save defined sound effects
  {
    size_t index;
    clientStream->write(&(index = pv->getFootstepSoundResourceIndex()), sizeof(index));
    clientStream->write(&(index = pv->getButtonSoundResourceIndex()), sizeof(index));
  }

  // Compile projectiles
  if (!CompileClientResourceArray<const dc::dcList<Projectile>::Element*>(pv->getProjectiles()->getMembers(), clientStream))
      return false;
  if (!CompileServerResourceArray<const dc::dcList<Projectile>::Element*>(pv->getProjectiles()->getMembers(), serverStream))
      return false;

  // Compile enchantments
  CONFIRM(pv->getEnchantmentClassList()->compileForClient(clientStream)) else return false;

  // Compile spells
  if (!CompileClientResourceArray<const dc::dcList<Spell>::Element*>(pv->getSpellsList()->getMembers(), clientStream)) {
    return false;
  }
  if (!CompileServerResourceArray<const dc::dcTable<Magic>::Element*>(pv->getMagicList()->getMembers(), serverStream)) {
    return false;
  }
  if (!CompileServerResourceArray<const dc::dcList<Spell>::Element*>(pv->getSpellsList()->getMembers(), serverStream)) {
    return false;
  }

  // Save the special effect information
  if (!CompileClientResourceArray<const dc::dcList<VisualFXImage>::Element*>(pv->getVisualFXImagesList()->getMembers(), clientStream))
      return false;
  if (!CompileClientResourceArray<const dc::dcList<MagicFX>::Element*>(pv->getMagicFXList()->getMembers(), clientStream))
      return false;
  if (!CompileClientResourceArray<const dc::dcTable<VisualFX>::Element*>(pv->getVisualFXTable()->getMembers(), clientStream))
      return false;

  // Save defined visual effects
  {
    size_t index;
    clientStream->write(&(index = pv->getPhysicalHitMagicFXResourceIndex()), sizeof(index));
    serverStream->write(&(index = pv->getSpawnMagicFXResourceIndex()), sizeof(index));
  }

  //// Compile the effects list for the client and server
  //if (!CompileClientResourceArray<const dc::dcTable<Magic>::Element*>(pv->getEffectsList()->getMembers(), clientStream))
  //    return false;
  //if (!CompileServerResourceArray<const dc::dcTable<Magic>::Element*>(pv->getEffectsList()->getMembers(), serverStream))
  //    return false;

  // Compile the skinned meshes, animations and actor types
  if (!CompileClientResourceArray<const dc::dcList<UnseenSkinnedMeshResource>::Element*>(pv->getSkinnedMeshesList()->getMembers(), clientStream))
      return false;
  if (!CompileClientResourceArray<const dc::dcList<UnseenKeyframeAnimation>::Element*>(pv->getSkinnedMeshAnimationsList()->getMembers(), clientStream))
      return false;
  if (!CompileClientResourceArray<const dc::dcList<ActorType>::Element*>(pv->getActorTypesList()->getMembers(), clientStream))
      return false;

  int monster_level_rarity_boost[RARITY_SCALE_SIZE];
  { // the server's item manager loads everything in this scope
    // Compile the item list for the client and server
    if (!CompileClientResourceArray<const dc::dcList<Item>::Element*>(pv->getItemsList()->getMembers(), clientStream))
        return false;
    if (!CompileServerResourceArray<const dc::dcList<Item>::Element*>(pv->getItemsList()->getMembers(), serverStream))
        return false;

    // compile the treasure generation and level-boost settings for the server.  the
    {
      TreasureGeneration* treasure_generation = pv->getTreasureGeneration();
      CONFIRM(treasure_generation->compileForServer(serverStream)) else return false;
      treasure_generation->compileRarityBoost(monster_level_rarity_boost);
    }
  }

  // Compile the lifeform AI types' actor descriptions
  {
    const dc::dcGenericResource::Array& members = pv->getLifeformAITable()->getMembers();
    for (dc::dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i) {
      dc::dcTable<LifeformAI>::Element* element = (dcTable<LifeformAI>::Element*)(*i);
      LifeformAI* lifeform_ai = element->getImplementation();
      lifeform_ai->compileActorTemplates(&actor_description_template_compiler_);
    }
  }

  // Compile Lifeform AI types for the server
  if (!CompileServerResourceArray<const dc::dcTable<LifeformAI>::Element*>(pv->getLifeformAITable()->getMembers(), serverStream))
      return false;


  // Write the actor template descriptions into the streams
  CONFIRM(actor_description_template_compiler_.compileForClient(clientStream)) else return false;
  CONFIRM(actor_description_template_compiler_.compileForServer(serverStream)) else return false;

  // Write the quests into the client and server
  if (!CompileClientResourceArray<const dc::dcList<Quest>::Element*>(pv->getQuests()->getMembers(), clientStream))
    return false;
  if (!CompileServerResourceArray<const dc::dcList<Quest>::Element*>(pv->getQuests()->getMembers(), serverStream))
    return false;


  {// Compile the maps
    dc::dcList<Atlas>* atlases = pv->getAtlasList();
    size_t number_of_maps = atlases->getMembers().size();
    clientStream->write(&number_of_maps, sizeof(number_of_maps));
    serverStream->write(&number_of_maps, sizeof(number_of_maps));

    for (size_t i = 0; i < number_of_maps; ++i) {
      AtlasCompilationContext context;
      Atlas* atlas = static_cast<Atlas*>(atlases->getMembers().at(i));
      if (!atlas->compile(globalEditor()->getD3DDevice(), &context))
          return false;

      int map_width,
          map_height;

      // Compile this map data for the client application
      bool failedCompilingMap = !context.compile(globalEditor()->getD3DDevice(),
                                                 clientStream,
                                                 serverStream,
                                                &map_width,
                                                &map_height);
      context.destroy();  // Get rid of the data in the context

      // Make sure that nothing went wrong
      if (APP_ERROR(failedCompilingMap)("Unable to compile map %lu!", i))
          return false;

      CONFIRM(atlas->compileServerData(globalEditor()->getD3DDevice(),
                                       serverStream,
                                       map_width,
                                       map_height,
                                       monster_level_rarity_boost)) else return false;
    }
  }

  // Success
  return true;
}
