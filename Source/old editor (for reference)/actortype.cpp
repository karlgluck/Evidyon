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
#include "resource"
#include "stdafx.h"
#include "unseenskinnedmeshresource.h"
#include "unseenkeyframeanimation.h"
#include "colorkey.h"
#include "image.h"
#include "texture.h"
#include "wavsound.h"
#include "skinnedmeshattachmentpoint.h"
#include "actortype.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"



/**
 * 
 */
class EditActorTypeDialog : public dcxD3DDialogController
{
    public:

        /**
         * Initializes the class
         */
        EditActorTypeDialog();

        /**
         * Runs the rescale-meshes dialog
         */
        int execute(ActorType* actorType);

        /**
         * Forces this dialog (and any of its children) to close
         */
        void end(INT_PTR result);


    protected:

        /**
         * If a menu item is selected, this method is invoked
         *   @param hDlg The dialog containing the menu
         *   @param hMenu The menu containing the selected item
         *   @param ItemID The ID of the item that was selected
         *   @param ItemIndex The index of the selected item
         */
        INT_PTR dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex);

        /**
         * When something on the dialog is activated, this function is invoked
         *   @param hWnd Dialog handle
         *   @param hControl Control that spawned the notification
         *   @param ControlID ID of the control that created the notification
         *   @param Notification The event that occurred
         *   @return Return code
         */
        INT_PTR dialogNotification(HWND hDlg, HWND hControl, WORD ControlID, WORD Notification);

        /**
         * All messages received by the dialog are passed through here.  The default
         * implementation of this method will call EndDialog with a value of "0"
         * when the window is closed.
         *   @param hWnd Dialog handle
         *   @param uMsg Message
         *   @param wParam Parameter
         *   @param lParam Parameter
         *   @return Return code
         */
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:

        dcxWin32Mouse myMouse;
        dcxWin32Keyboard myKeyboard;

        float myMeshScaling[3];
        float myMeshTranslation[3];
        float myMeshRotation;
        bool myFlipWindingOrderFlag;
        ActorType* myActorType;

        ActorAnimation myCurrentActionAnimation;
        UnseenSkeletalAnimation myCurrentAnimation;
        float myAnimationSpeed;
        double myAnimationTime;
        float myAdjustedAnimationSecondsPerTick;

        float myActionAnimationSpeedOffsets[ACTORANIMATION_COUNT];
        float myActionAnimationSpeedFactors[ACTORANIMATION_COUNT];

        enum MotionType
        {
            MOTION_STOPPED,
            MOTION_TURNINPLACE,
            MOTION_MOVEINCIRCLE,
            MOTION_MOVEBACKANDFORTH,
        };

        bool mySelectingAttachmentPointsFlag;

        MotionType myMotionType;
        float myMotionSpeed;
};





//------------------------------------------------------------------------------------------------
// Name:  EditActorTypeDialog
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
EditActorTypeDialog::EditActorTypeDialog()
{
    myMotionType = MOTION_STOPPED;
    myMotionSpeed = 0.0f;
    ZeroMemory(myActionAnimationSpeedOffsets, sizeof(myActionAnimationSpeedOffsets));
    ZeroMemory(myActionAnimationSpeedFactors, sizeof(myActionAnimationSpeedFactors));
    ZeroMemory(&myCurrentAnimation, sizeof(myCurrentAnimation));
    myCurrentActionAnimation = ACTORANIMATION_INVALID;
    myAnimationTime = 0.0;
    myAdjustedAnimationSecondsPerTick = 1.0;
    mySelectingAttachmentPointsFlag = false;
}




//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  
//------------------------------------------------------------------------------------------------
int EditActorTypeDialog::execute(ActorType* actorType)
{
    // At this point, all of the data we need to generate in order to display the map to the user has been loaded, so
    // we can go about initializing Direct3D on a dialog window.
    if (APP_ERROR(!createD3DDialog(GetModuleHandle(NULL), globalEditor()->getTopmostWindowHandle(),
        IDR_EDITACTORTYPE_MENU, "Edit Actor", 800, 600, globalEditor()->getD3DDevice()))
        ("Couldn't create a Direct3D dialog to display the world Atlas"))
    {
        // Exit
        return 0;
    }

    // Initialize the render state
    myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    myD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
    myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    // Set up alpha testing
    myD3DDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x08);
    myD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    myD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    myD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Create a box mesh
    LPD3DXMESH boxMesh;
    D3DXCreateBox(myD3DDevice, 1.0f, 1.0f, 1.0f, &boxMesh, 0);


    // Load the skinned mesh
    UnseenSkinnedMesh skinnedMesh;
    const dc::dcList<UnseenSkinnedMeshResource>::Element* skinnedMeshElement = actorType->getSkinnedMesh()->getReferencedResource();
    if (skinnedMeshElement) skinnedMeshElement->loadSkinnedMesh(&skinnedMesh);

    std::map<DWORD,LPDIRECT3DTEXTURE9> textures;

    for (dcGenericResource::Array::const_iterator i = actorType->getMeshTextures()->getMembers().begin(); i != actorType->getMeshTextures()->getMembers().end(); ++i)
    {
        const dc::dcMap<dcInteger,dcTable<Texture>::Reference>::Element* skin = 
            (const dc::dcMap<dcInteger,dcTable<Texture>::Reference>::Element*)(*i);

        const dc::dcTable<Texture>::Element* texture = skin->getValue()->getReferencedResource();
        const dc::dcTable<Image>::Element* image = texture ? texture->getImplementation()->getImageRef()->getReferencedResource() : 0;
        Image::Preview imagePreview;
        LPDIRECT3DTEXTURE9 tex;
        if (image)
        {
            image->getImplementation()->loadPreview(myD3DDevice, &imagePreview);
            tex = imagePreview.d3dTexture;
        }
        else
            tex = 0;

        textures.insert(std::map<DWORD,LPDIRECT3DTEXTURE9>::value_type(skin->getKey()->getValue(), tex));
    }

    D3DXMATRIX* frameMatrices = new D3DXMATRIX[skinnedMesh.info.numberOfFrames];

    UpdateWindow(myMainWindow);
    ShowWindow(myMainWindow, SW_SHOW);

    IsometricCamera camera;
    //camera.rebuild(800, 600, 1.0f);
    camera.rebuildXenimusCamera(800, 600);

    // Initialize the render state
    myD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    myD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
    myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    float amp = 0.2f;
    myActorType = actorType;
    myMeshScaling[0] = actorType->getScalingFactor()->getX()->getValue();
    myMeshScaling[1] = actorType->getScalingFactor()->getY()->getValue();
    myMeshScaling[2] = actorType->getScalingFactor()->getZ()->getValue();
    myMeshTranslation[0] = actorType->getTranslation()->getX()->getValue();
    myMeshTranslation[1] = actorType->getTranslation()->getY()->getValue();
    myMeshTranslation[2] = actorType->getTranslation()->getZ()->getValue();
    myMeshRotation = D3DXToRadian(actorType->getRotation()->getY()->getValue());
    myFlipWindingOrderFlag = false;

    for (int i = 0; i < (int)ACTORANIMATION_COUNT; ++i)
    {
        myActionAnimationSpeedOffsets[i] = myActorType->getSpeedOffset((ActorAnimation)i)->getValue();
        myActionAnimationSpeedFactors[i] = myActorType->getSpeedFactor((ActorAnimation)i)->getValue();
    }

    double lastFrameTime = dcxWin32Clock::getAccurateSystemTime();

    // Go into the main window loop for handling the dialog
    while(messagePump())
    {
        double currentFrameTime = dcxWin32Clock::getAccurateSystemTime();
        double timeSinceLastFrame = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        myMouse.query();
        myKeyboard.query(0);

        myD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);
        myD3DDevice->SetRenderState(D3DRS_CULLMODE, myFlipWindingOrderFlag ? D3DCULL_CW : D3DCULL_CCW);

        if (myKeyboard.keyPressed(KEY_UP)) amp *= 2;
        if (myKeyboard.keyPressed(KEY_DOWN)) amp /= 2;
        if (myKeyboard.keyPressed(KEY_Q)) myMeshScaling[0] += amp;
        if (myKeyboard.keyPressed(KEY_A)) myMeshScaling[0] -= amp;
        if (myKeyboard.keyPressed(KEY_W)) myMeshScaling[1] += amp;
        if (myKeyboard.keyPressed(KEY_S)) myMeshScaling[1] -= amp;
        if (myKeyboard.keyPressed(KEY_E)) myMeshScaling[2] += amp;
        if (myKeyboard.keyPressed(KEY_D)) myMeshScaling[2] -= amp;
        if (myKeyboard.keyPressed(KEY_R)) { myMeshScaling[0] += amp; myMeshScaling[1] += amp; myMeshScaling[2] += amp; }
        if (myKeyboard.keyPressed(KEY_F)) { myMeshScaling[0] -= amp; myMeshScaling[1] -= amp; myMeshScaling[2] -= amp; }
        if (myKeyboard.keyPressed(KEY_LEFT))  { myMeshRotation -= D3DX_PI/2.0f; }
        if (myKeyboard.keyPressed(KEY_RIGHT)) { myMeshRotation += D3DX_PI/2.0f; }
        if (myKeyboard.keyPressed(KEY_Z)) { mySelectingAttachmentPointsFlag = !mySelectingAttachmentPointsFlag; }

        // Render to the window
        if (SUCCEEDED(myD3DDevice->BeginScene()))
        {
            camera.setPosition(0.0f, 0.0f);
            camera.initialize3DRendering(myD3DDevice);

            // Draw the grid
            myD3DDevice->SetFVF(D3DFVF_XYZ);
            myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE); 
            D3DXMATRIX identity;
            D3DXMatrixIdentity(&identity);
            camera.setWorldMatrix(myD3DDevice, &identity);
            for (int y = -10; y <= 10; ++y)
            {
                D3DXVECTOR3 vecs[2] = { D3DXVECTOR3(-10.0f, 0.0f, y), D3DXVECTOR3(10.0f, 0.0f, y) };
                myD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vecs, sizeof(D3DXVECTOR3));
            }
            for (int x = -10; x <= 10; ++x)
            {
                D3DXVECTOR3 vecs[2] = { D3DXVECTOR3(x, 0.0f, -10.0f), D3DXVECTOR3(x, 0.0f, 10.0f) };
                myD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vecs, sizeof(D3DXVECTOR3));
            }
            
            // Create the transformation matrices
            D3DXMATRIX geometric, builder;
            D3DXMatrixScaling(&geometric, myMeshScaling[0], myMeshScaling[1], myMeshScaling[2]);
            //D3DXMatrixRotationY(&builder, myMeshRotation);
            D3DXMatrixRotationYawPitchRoll(&builder, myMeshRotation,
                                                     D3DXToRadian(actorType->getRotation()->getX()->getValue()),
                                                     D3DXToRadian(actorType->getRotation()->getZ()->getValue()));
            D3DXMatrixMultiply(&geometric, &geometric, &builder);
            D3DXMatrixTranslation(&builder, myMeshTranslation[0], myMeshTranslation[1], myMeshTranslation[2]);
            D3DXMatrixMultiply(&geometric, &geometric, &builder);
            D3DXMATRIX motion;
            switch(myMotionType)
            {
                case MOTION_STOPPED:
                    D3DXMatrixIdentity(&motion);
                    break;
                case MOTION_TURNINPLACE:
                    D3DXMatrixRotationY(&motion, dcxWin32Clock::getAccurateSystemTime() * myMotionSpeed);
                    break;
                case MOTION_MOVEINCIRCLE:
                    {
                        D3DXMATRIX rotate;
                        D3DXMATRIX translate;

                        const float distance = 5.0f;
                        float angle = dcxWin32Clock::getAccurateSystemTime() * myMotionSpeed / distance;
                        D3DXMatrixTranslation(&translate, distance * cosf(angle), 0.0f, distance*sinf(angle));

                        D3DXMatrixRotationY(&rotate, -(D3DX_PI/2.0f+angle));

                        D3DXMatrixMultiply(&motion, &rotate, &translate);

                    } break;
            }

            D3DXMATRIX root;
            D3DXMatrixMultiply(&root, &geometric, &motion);

            if (myCurrentActionAnimation != ACTORANIMATION_INVALID)
            {
                myAnimationTime += timeSinceLastFrame * (myMotionSpeed * myActionAnimationSpeedFactors[myCurrentActionAnimation] + myActionAnimationSpeedOffsets[myCurrentActionAnimation]);

                DWORD timeTickIndex = (DWORD)(myAnimationTime / myAdjustedAnimationSecondsPerTick);
                DWORD tickLength = myCurrentAnimation.info.tickLength;

                DWORD lowerKeyframeIndex, upperKeyframeIndex;
                double blendFactor;

                DWORD numberOfKeyframes = myCurrentAnimation.info.numberOfKeyframes;
                DWORD highestKeyframeIndex = numberOfKeyframes - 1;
                DWORD approximateKeyframeIndex = (timeTickIndex / myCurrentAnimation.info.averageTicksPerKeyframe) % numberOfKeyframes;
                DWORD lower = approximateKeyframeIndex;
                while(myCurrentAnimation.keyframes[lower].tick > timeTickIndex && lower > 0) --lower;
                double lowerTime = myCurrentAnimation.keyframes[lower].tick * myAdjustedAnimationSecondsPerTick;
                double upperTime;
                lowerKeyframeIndex = lower;
                if (lower == highestKeyframeIndex)
                {
                    upperKeyframeIndex = 0;
                    upperTime = lowerTime + myCurrentAnimation.info.averageTicksPerKeyframe * myAdjustedAnimationSecondsPerTick;
                }
                else
                {
                    upperKeyframeIndex = lower + 1;
                    upperTime = myCurrentAnimation.keyframes[upperKeyframeIndex].tick * myAdjustedAnimationSecondsPerTick;
                }
                double timeDelta = upperTime - lowerTime;
                blendFactor = (myAnimationTime - lowerTime) / timeDelta;
                if (APP_WARNING(blendFactor < 0.0 || blendFactor > 1.0)("blendFactor invalid")) blendFactor = 1.0;

                myAnimationTime = fmodf(myAnimationTime, myCurrentAnimation.info.tickLength * myAdjustedAnimationSecondsPerTick);


                // Set the transform for each of the frames
                for (int m = 0; m < skinnedMesh.info.numberOfFrames; ++m)
                {
                    if (skinnedMesh.frameHierarchy[m].parentFrameIndex < skinnedMesh.info.numberOfFrames)
                    {
                        DWORD animatedOffset = myCurrentAnimation.frameIndexToAnimatedFrameTransformTable[m];
                        if (animatedOffset != 0xFFFFFFFF)
                        {
                            LPD3DXMATRIX lowerMatrix = &myCurrentAnimation.keyframes[lowerKeyframeIndex].animatedFrameTransforms[animatedOffset];
                            LPD3DXMATRIX upperMatrix = &myCurrentAnimation.keyframes[upperKeyframeIndex].animatedFrameTransforms[animatedOffset];

                            D3DXMATRIXA16 interpolated;
                            { // Interpolate
                              interpolated = upperMatrix->operator -(*lowerMatrix);
                              interpolated *= blendFactor;
                              interpolated += *lowerMatrix;
                            }

                            // Set the matrix
                            D3DXMatrixMultiply(&frameMatrices[m], &interpolated, &frameMatrices[skinnedMesh.frameHierarchy[m].parentFrameIndex]);
                        }
                        else
                        {
                            D3DXMatrixMultiply(&frameMatrices[m], &skinnedMesh.frameHierarchy[m].offset, &frameMatrices[skinnedMesh.frameHierarchy[m].parentFrameIndex]);
                        }

                    }
                    else
                        D3DXMatrixMultiply(&frameMatrices[m], &root, &skinnedMesh.frameHierarchy[m].offset);
                }
            }
            else
            {
                // Just copy the basic hierarchy
                for (int m = 0; m < skinnedMesh.info.numberOfFrames; ++m)
                {
                    if (skinnedMesh.frameHierarchy[m].parentFrameIndex < skinnedMesh.info.numberOfFrames)
                        D3DXMatrixMultiply(&frameMatrices[m], &skinnedMesh.frameHierarchy[m].offset, &frameMatrices[skinnedMesh.frameHierarchy[m].parentFrameIndex]);
                    else
                        D3DXMatrixMultiply(&frameMatrices[m], &root, &skinnedMesh.frameHierarchy[m].offset);
                }
            }

            myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

            if (mySelectingAttachmentPointsFlag)
            {
                myD3DDevice->SetTexture(0, NULL);
                myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

                // Render each of the bones
                for (DWORD i = 0; i < skinnedMesh.info.numberOfBones; ++i)
                { 
                    D3DXMATRIX combined;
                    D3DXMatrixScaling(&combined, 3.0f, 3.0f, 3.0f);
                    D3DXMatrixMultiply(&combined, &combined, &frameMatrices[skinnedMesh.skeleton[i].frameIndex]);
                    camera.setWorldMatrix(myD3DDevice, &combined);
                    boxMesh->DrawSubset(0);
                }
            }
            else
            {

                // Set the bones for this mesh
                for (DWORD i = 0; i < skinnedMesh.info.numberOfBones; ++i)
                {
                    D3DXMATRIX combined;
                    D3DXMatrixMultiply(&combined, &skinnedMesh.skeleton[i].inverseOffset, &frameMatrices[skinnedMesh.skeleton[i].frameIndex]);
                    camera.setWorldMatrix(myD3DDevice, &combined, i);
                }

                // Initialize the vertex blending state
                myD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

                for (std::map<DWORD,LPDIRECT3DTEXTURE9>::iterator i = textures.begin(); i != textures.end(); ++i)
                {
                    myD3DDevice->SetTexture(0, i->second);

                    for (size_t t = 0; t < skinnedMesh.info.numberOfTextureGroups; ++t)
                    {
                        UnseenSkinnedMesh::TextureGroup* textureGroup = &skinnedMesh.textureGroups[t];
                        for (size_t g = 0; g < textureGroup->info.numberOfVertexInfluenceGroups; ++g)
                        {
                            UnseenSkinnedMesh::TextureGroup::VertexInfluenceGroup* influenceGroup = &textureGroup->vertexInfluenceGroups[g];
                            
                            myD3DDevice->SetFVF(influenceGroup->info.d3dVertexFormat);

                            switch(influenceGroup->info.numberOfVertexInfluences)
                            {
                                case 0: myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE); break;
                                case 1: myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS); break;
                                default: myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, (influenceGroup->info.numberOfVertexInfluences-1)); break;
                            }

                            myD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, influenceGroup->info.numberOfVertices, influenceGroup->info.numberOfTriangles, influenceGroup->indices,
                                (D3DFORMAT)influenceGroup->info.d3dIndexFormat, influenceGroup->vertices, influenceGroup->info.vertexSizeInBytes);
                        }
                    }
                }

                myD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
            }

            myD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

            // Draw the scaling of this character
            myD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            myD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
            D3DXMATRIX mx, trans;
            D3DXMatrixTranslation(&trans, 0.0f, -0.5f, 0.0f);
            D3DXMatrixScaling(&mx, myActorType->getRadialSize()->getValue(),
                                   -myActorType->getHeightValue(),
                                    myActorType->getRadialSize()->getValue());
            D3DXMatrixMultiply(&mx, &trans, &mx);
            camera.setWorldMatrix(myD3DDevice, &mx);
            boxMesh->DrawSubset(0);

            myD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
            myD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

            // Render the attachment points
            for (size_t i = 0; i < (size_t)ACTORATTACHMENTPOINT_COUNT; ++i)
            {
                D3DXMATRIX mat;
                int boneIndex = myActorType->getSkinnedMeshAttachmentPoint(i)->getSourceBoneIndexValue();
                if (boneIndex < 0 || boneIndex > skinnedMesh.info.numberOfBones) continue;
                myActorType->getSkinnedMeshAttachmentPoint(i)->generateOffsetMatrix(&mat);
                D3DXMATRIX frameMatrix = frameMatrices[skinnedMesh.skeleton[boneIndex].frameIndex];

                // Reset the scaling of the frame matrix
                D3DXMATRIX invScaling;
                D3DXMatrixScaling(&invScaling, myActorType->getScalingFactor()->getX()->getValue(),
                                                myActorType->getScalingFactor()->getY()->getValue(),
                                                myActorType->getScalingFactor()->getZ()->getValue());
                D3DXMatrixInverse(&invScaling, 0, &invScaling);
                D3DXMatrixMultiply(&mat, &mat, &invScaling);
                D3DXMatrixMultiply(&mat, &mat, &frameMatrix);

                camera.setWorldMatrix(myD3DDevice, &mat);
                boxMesh->DrawSubset(0);
            }

            // Clear the back-buffer
            myD3DDevice->EndScene();
        }

        //d3dSwapChain->Present(NULL, NULL, NULL, NULL, 0);
        CONFIRM(present());
    }

    SAFE_DELETE(frameMatrices);

    for (std::map<DWORD,LPDIRECT3DTEXTURE9>::iterator i = textures.begin(); i != textures.end(); ++i)
        SAFE_RELEASE(i->second);

    SAFE_RELEASE(boxMesh);
    releaseObjects();


    // Success
    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  end
// Desc:  Forces this dialog (and any of its children) to close
//------------------------------------------------------------------------------------------------
void EditActorTypeDialog::end(INT_PTR result)
{
}




//------------------------------------------------------------------------------------------------
// Name:  dialogMenuNotification
// Desc:  If a menu item is selected, this method is invoked
//------------------------------------------------------------------------------------------------
INT_PTR EditActorTypeDialog::dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex)
{
    switch(ItemID)
    {
        case ID_ACTORTYPE_SAVEANDEXIT:

            myActorType->getScalingFactor()->getX()->setValue(myMeshScaling[0]);
            myActorType->getScalingFactor()->getY()->setValue(myMeshScaling[1]);
            myActorType->getScalingFactor()->getZ()->setValue(myMeshScaling[2]);
            myActorType->getTranslation()->getX()->setValue(myMeshTranslation[0]);
            myActorType->getTranslation()->getY()->setValue(myMeshTranslation[1]);
            myActorType->getTranslation()->getZ()->setValue(myMeshTranslation[2]);
            myActorType->getRotation()->getY()->setValue(D3DXToDegree(myMeshRotation));

            for (int i = 0; i < (int)ACTORANIMATION_COUNT; ++i)
            {
                myActorType->getSpeedOffset((ActorAnimation)i)->setValue(myActionAnimationSpeedOffsets[i]);
                myActorType->getSpeedFactor((ActorAnimation)i)->setValue(myActionAnimationSpeedFactors[i]);
            }
            
        case ID_ACTORTYPE_CLOSE:

            EndDialog(hDlg, TRUE);

            break;

        case ID_MESH_ROTATE:
        {
            float rotationDegrees = D3DXToDegree(myMeshRotation);
            if (dcx::dcxWin32PromptNumericInput(hDlg, "Mesh Rotation", "Please enter the Y rotation value in degrees", &rotationDegrees))
                myMeshRotation = D3DXToRadian(rotationDegrees);
        } break;

        case ID_MESH_SCALE:
        {
            float scaling;
            if (dcx::dcxWin32PromptNumericInput(hDlg, "Mesh Scaling", "Please enter a uniform mesh scaling value", &scaling))
            {
                myMeshScaling[0] = scaling;
                myMeshScaling[1] = scaling;
                myMeshScaling[2] = scaling;
            }

        } break;

        case ID_FLIPWINDINGORDER_YES:
        {
            myFlipWindingOrderFlag = true;
        } break;

        case ID_FLIPWINDINGORDER_NO:
        {
            myFlipWindingOrderFlag = false;
        } break;

        case ID_ACTIONANIMATIONS_RUNACTIONANIMATION:
        {
            // Build a prompt and caption
            std::string caption = "Run Action Animation";
            std::string prompt = "Select action to display:";

            // Add entries to the options table
            std::list<std::string> options;
            const char* stringValue = dc::dcEnum<ActorAnimation>::getString((ActorAnimation)0);
            int index = 1;
            for (index = 1; stringValue != 0; ++index)
            {
                options.push_back(stringValue);
                stringValue = dc::dcEnum<ActorAnimation>::getString((ActorAnimation)index);
            }

            // Unload the old animation

            // Get the action to display
            int selection = -1;
            if (dcxWin32PromptForSelection(globalEditor()->getTopmostWindowHandle(), caption.c_str(), prompt.c_str(),
                                            &options, &selection))
            {
                // Load up the animation
                myAnimationTime = 0.0;
                dc::dcList<UnseenKeyframeAnimation>::Element* animation = myActorType->getActionAnimation((ActorAnimation)selection);
                if (animation)
                {
                    if (APP_ERROR(!animation->loadAnimation(&myCurrentAnimation))("Couldn't load animation!"))
                        ZeroMemory(&myCurrentAnimation, sizeof(myCurrentAnimation));
                    else
                    {
                        myCurrentActionAnimation = (ActorAnimation)selection;
                        myAnimationSpeed = 1.0f;
                        myAnimationTime = 0.0;

                        float adjustedSecondsPerTick = myCurrentAnimation.info.secondsPerTick;
                        myAdjustedAnimationSecondsPerTick = adjustedSecondsPerTick;
                    }
                }
            }
            else
            {
                // Stop animating
                deallocateUnseenSkeletalAnimation(&myCurrentAnimation);
                myCurrentActionAnimation = ACTORANIMATION_INVALID;
                ZeroMemory(&myCurrentAnimation, sizeof(myCurrentAnimation));
            }

        } break;

        case ID_ACTIONANIMATIONS_STOPACTIONANIMATION:
        {
            // Stop animating
            deallocateUnseenSkeletalAnimation(&myCurrentAnimation);
            myCurrentActionAnimation = ACTORANIMATION_INVALID;
            ZeroMemory(&myCurrentAnimation, sizeof(myCurrentAnimation));

        } break;

        case ID_ACTIONANIMATIONS_STOPMOVING: myMotionType = MOTION_STOPPED; break;
        case ID_ACTIONANIMATIONS_TURNINPLACESLOWLY: myMotionType = MOTION_TURNINPLACE; break;
        case ID_ACTIONANIMATIONS_MOVEINCIRCLE: myMotionType = MOTION_MOVEINCIRCLE; break;
        case ID_ACTIONANIMATIONS_MOVEBACKANDFORTH: myMotionType = MOTION_MOVEBACKANDFORTH; break;

        case ID_SETMOVEMENTSPEED_SLOWED:    myMotionSpeed = 0.5f; break;
        case ID_SETMOVEMENTSPEED_NORMAL:    myMotionSpeed = 1.0f; break;
        case ID_SETMOVEMENTSPEED_FASTER:    myMotionSpeed = 2.0f; break;
        case ID_SETMOVEMENTSPEED_FASTEST:   myMotionSpeed = 5.0f; break;

        case ID_ACTIONANIMATIONS_SETANIMATIONPERIOD:
        {/*
            if (myCurrentActionAnimation == ACTORANIMATION_INVALID) break;
            float period = myCurrentAnimation.info.secondsPerTick * myCurrentAnimation.info.tickLength;
            if (dcx::dcxWin32PromptNumericInput(hDlg, "Set Animation Period", "Please type the period for this animation:", &period, true))
            {
                myActionAnimationSpeedAdjustments[myCurrentActionAnimation] = period / (myCurrentAnimation.info.tickLength * myCurrentAnimation.info.secondsPerTick);
                myAdjustedAnimationSecondsPerTick = period / myCurrentAnimation.info.tickLength;
            }*/
        } break;

        case ID_ACTIONANIMATIONS_SETANIMATIONSPEEDFACTOR:
        {
            if (myCurrentActionAnimation == ACTORANIMATION_INVALID) break;
            float speedFactor = myActionAnimationSpeedFactors[myCurrentActionAnimation];
            if (dcx::dcxWin32PromptNumericInput(hDlg, "Set Animation Speed Factor", "Please enter an animation speed factor:", &speedFactor, true))
                myActionAnimationSpeedFactors[myCurrentActionAnimation] = speedFactor;
        } break;

        case ID_ACTIONANIMATIONS_SETANIMATIONSPEEDOFFSET:
        {
            if (myCurrentActionAnimation == ACTORANIMATION_INVALID) break;
            float speedFactor = myActionAnimationSpeedOffsets[myCurrentActionAnimation];
            if (dcx::dcxWin32PromptNumericInput(hDlg, "Set Animation Speed Offset", "Please enter an animation speed offset:", &speedFactor, true))
                myActionAnimationSpeedOffsets[myCurrentActionAnimation] = speedFactor;
        } break;
    }

    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  dialogNotification
// Desc:  When something on the dialog is activated, this function is invoked
//------------------------------------------------------------------------------------------------
INT_PTR EditActorTypeDialog::dialogNotification(HWND hDlg, HWND hControl, WORD ControlID, WORD Notification)
{
    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  dialogEvent
// Desc:  All messages received by the dialog are passed through here.  The default
//------------------------------------------------------------------------------------------------
INT_PTR EditActorTypeDialog::dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool ret;
    myKeyboard.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
    myMouse.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
    if (uMsg == WM_CLOSE)
        DestroyWindow(hDlg);
    return 0;
}














//------------------------------------------------------------------------------------------------
// Name:  getString
// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<ActorAttachmentPoint>
//------------------------------------------------------------------------------------------------
template<> const char* dc::dcEnum<ActorAttachmentPoint>::getString(ActorAttachmentPoint value)
{
    switch(value)
    {
        case ACTORATTACHMENTPOINT_HELMET:      return "Helmet";
        case ACTORATTACHMENTPOINT_SHIELD:      return "Shield";
        case ACTORATTACHMENTPOINT_WEAPON:      return "Weapon";
        default: return 0;
    }
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Implements the class name for dc::dcEnum<ActorAnimation>
//------------------------------------------------------------------------------------------------
std::string dc::dcEnum<ActorAttachmentPoint>::staticTypeName()
{
    return "ActorAttachmentPoint";
}



























//------------------------------------------------------------------------------------------------
// Name:  getString
// Desc:  Implements the enum-index-to-string conversion for dc::dcEnum<ActorAnimation>
//------------------------------------------------------------------------------------------------
template<> const char* dc::dcEnum<ActorAnimation>::getString(ActorAnimation value)
{
  switch(value) {
    case ACTORANIMATION_IDLE:       return "Idle";
    case ACTORANIMATION_COMBAT_IDLE: return "Combat Idle";
    case ACTORANIMATION_RUN:        return "Run";
    case ACTORANIMATION_RUN_ATTACK: return "Run Attack";
    case ACTORANIMATION_DIE1:       return "Die 1";
    case ACTORANIMATION_DIE2: return "Die 2";
    case ACTORANIMATION_DIE3: return "Die 3";
    case ACTORANIMATION_DIE4: return "Die 4";
    case ACTORANIMATION_ATTACK1: return "Attack 1";
    case ACTORANIMATION_ATTACK2: return "Attack 2";
    case ACTORANIMATION_ATTACK3: return "Attack 3";
    case ACTORANIMATION_ATTACK4: return "Attack 4";
    case ACTORANIMATION_BLOCK1: return "Block 1";
    case ACTORANIMATION_BLOCK2: return "Block 2";
    case ACTORANIMATION_BLOCK3: return "Block 3";
    case ACTORANIMATION_CASTSPELL1: return "Cast Spell 1";
    case ACTORANIMATION_CASTSPELL2: return "Cast Spell 2";
    case ACTORANIMATION_GOTHIT: return "Got Hit";
    case ACTORANIMATION_MISC1: return "Misc. 1";
    case ACTORANIMATION_MISC2: return "Misc. 2";
    case ACTORANIMATION_MISC3: return "Misc. 3";
    case ACTORANIMATION_MISC4: return "Misc. 4";

    default: return 0;
  }
}




//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Implements the class name for dc::dcEnum<ActorAnimation>
//------------------------------------------------------------------------------------------------
std::string dc::dcEnum<ActorAnimation>::staticTypeName()
{
    return "ActorAnimation";
}




//------------------------------------------------------------------------------------------------
// Name: ActorType
// Desc: Initializes this resource
//------------------------------------------------------------------------------------------------
ActorType::ActorType()
{
    member("Skinned Mesh", &mySkinnedMesh);
    member("Scaling", &myScalingFactor);
    member("Rotation (degrees)", &myRotation);
    member("Translation", &myTranslation);
    member("Radial Size", &myRadialSize);
    member("Height", &height_);
    member("Mesh Textures", &myMeshTextures);

    // Add all of the actions as members
    for (size_t i = 0; i < (size_t)ACTORANIMATION_COUNT; ++i)
    {
        std::string actorAnimation = std::string("\"") + dc::dcEnum<ActorAnimation>::getString((ActorAnimation)i) + "\" ";
        member(actorAnimation + "Animation", &myActionAnimations[i]);
        member(actorAnimation + "Speed Offset", &myActionAnimationSpeedOffsets[i]);
        member(actorAnimation + "Speed Factor", &myActionAnimationSpeedFactors[i]);

        myActionAnimationSpeedOffsets[i].setValue(0.0f);
        myActionAnimationSpeedFactors[i].setValue(1.0f);
    }

    // Add all of the action sounds
    for (size_t i = 0; i < (size_t)ACTORSOUND_COUNT; ++i)
    {
      std::string actorAction = std::string("\"") + dc::dcEnum<Evidyon::ActorSound>::getString((ActorSound)i) + "\" ";
        member(actorAction + " Sound", &myActorSounds[i]);
    }

    // Add the attachment points
    for (size_t i = 0; i < (size_t)ACTORATTACHMENTPOINT_COUNT; ++i)
    {
        std::string attachmentPoint = dc::dcEnum<ActorAttachmentPoint>::getString((ActorAttachmentPoint)i);
        member("Attachment Point - " + attachmentPoint, &myAttachmentPoints[i]);
    }

    // Initialize the scaling factor
    myScalingFactor.getX()->setValue(1.0f);
    myScalingFactor.getY()->setValue(1.0f);
    myScalingFactor.getZ()->setValue(1.0f);
}




//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Writes this actor type's information into the output description structure
//------------------------------------------------------------------------------------------------
bool ActorType::compileForClient(ClientActorTypeDescription* description) const
{
    // Get the skinned mesh reference
    const dc::dcList<UnseenSkinnedMeshResource>::Element* skinnedMeshElement = mySkinnedMesh.getReferencedResource();
    description->skinnedMeshIndex = skinnedMeshElement ? skinnedMeshElement->getIndex() : 0xFFFFFFFF;
    description->height = height_.getValue();

    // Fill in all of the action animations
    for (size_t i = 0; i < (size_t)ACTORANIMATION_COUNT; ++i)
    {
        // Get the animation referenced by this action
        const dc::dcList<UnseenKeyframeAnimation>::Element* animation = myActionAnimations[i].getReferencedResource();

        // Assign the output animation type
        description->actionAnimationIndices[i] = animation ? animation->getIndex() : 0xFFFFFFFF;

        // Assign the animation speed values
        description->actionAnimationSpeedFactors[i] = myActionAnimationSpeedFactors[i].getValue();
        description->actionAnimationSpeedOffsets[i] = myActionAnimationSpeedOffsets[i].getValue();
    }


    const dc::dcMap<dcInteger,dcTable<Texture>::Reference>::Element* skin = 
        (const dc::dcMap<dcInteger,dcTable<Texture>::Reference>::Element*)myMeshTextures.getMembers().front();
    const dc::dcTable<Texture>::Element* texture = skin->getValue()->getReferencedResource();
    description->skinTexture = texture ? texture->getIndex() : 0xFFFFFFFF;

    // Build the actor's root transformation matrix
    D3DXMATRIX build;
    D3DXMatrixScaling(&build, myScalingFactor.getX()->getValue(),
                              myScalingFactor.getY()->getValue(),
                              myScalingFactor.getZ()->getValue());
    D3DXMATRIX rotation;
    D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(myRotation.getY()->getValue()),
                                              D3DXToRadian(myRotation.getX()->getValue()),
                                              D3DXToRadian(myRotation.getZ()->getValue()));
    D3DXMatrixMultiply(&build, &build, &rotation);
    D3DXMATRIX translation;
    D3DXMatrixTranslation(&translation, myTranslation.getX()->getValue(),
                                         myTranslation.getY()->getValue(),
                                         myTranslation.getZ()->getValue());
    D3DXMatrixMultiply(&description->rootTransform, &build, &translation);

    // Save the size of this actor
    description->sizeRadiusSq = myRadialSize.getValue() * myRadialSize.getValue();

    // Save the action sounds
    for (size_t i = 0; i < (size_t)ACTORSOUND_COUNT; ++i)
    {
        const dc::dcList<WAVSound>::Element* element = myActorSounds[i].getReferencedResource();
        description->soundIndices[i] = element ? element->getIndex() : 0xFFFFFFFF;
    }

    // Save the attachment points
    for (size_t i = 0; i < (size_t)ACTORATTACHMENTPOINT_COUNT; ++i)
    {
        description->attachmentPointBoneMatrices[i] = myAttachmentPoints[i].getSourceBoneIndexValue();

        D3DXMATRIX attachmentOffset;
        myAttachmentPoints[i].generateOffsetMatrix(&attachmentOffset);

        // Build the inverse scaling matrix and multiply it in, so that the attachment offset
        // isn't affected by the mesh's scale
        D3DXMATRIX invScaling;
        D3DXMatrixScaling(&invScaling, myScalingFactor.getX()->getValue(),
                                       myScalingFactor.getY()->getValue(),
                                       myScalingFactor.getZ()->getValue());
        D3DXMatrixInverse(&invScaling, 0, &invScaling);
        D3DXMatrixMultiply(&description->attachmentPointTransforms[i], &attachmentOffset, &invScaling);
    }

    // Success
    return true;
}



//------------------------------------------------------------------------------------------------
// Name: runEditDialog
// Desc: Shows a dialog that lets the user visually set the scale of this actor
//------------------------------------------------------------------------------------------------
void ActorType::runEditDialog()
{
    EditActorTypeDialog edit;
    edit.execute(this);
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Gets the string name of this resource type
//       @return Class name
//------------------------------------------------------------------------------------------------
std::string ActorType::staticTypeName()
{
    return "ActorType";
}

