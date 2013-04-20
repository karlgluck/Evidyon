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
#include "rescalemeshesdialog.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dc/debug>


//------------------------------------------------------------------------------------------------
// Name:  RescaleMeshesDialog
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
RescaleMeshesDialog::RescaleMeshesDialog()
{
}




//------------------------------------------------------------------------------------------------
// Name:  execute
// Desc:  
//------------------------------------------------------------------------------------------------
int RescaleMeshesDialog::execute(const dc::dcGenericResource::Array* display)
{
    if (display->empty()) return 0;
    HWND hParent = globalEditor()->getTopmostWindowHandle();
    CONFIRM(createD3DDialog(GetModuleHandle(NULL),
                                hParent,
                                0,
                                "Rescale Meshes",
                                800,
                                600,
                                globalEditor()->getD3DDevice())) else return false;
    IDirect3DDevice9* d3dDevice = myD3DDevice;
/*

    // Initialize Direct3D to run on the target window
    IDirect3DDevice9* d3dDevice = globalEditor()->getD3DDevice();
    IDirect3DSwapChain9* d3dSwapChain;

    dcx::dcxD3DDuplicateDeviceForWindow(

    // Create parameters
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.hDeviceWindow = hDlg;
    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    HRESULT hr = d3dDevice->CreateAdditionalSwapChain(&d3dpp, &d3dSwapChain);
    hr = hr;
*/
    //myMouse.initialize(d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
    myMouse.initialize(800, 600);

    // Create a view and projection matrix
    D3DXMATRIXA16 view, proj, world;
    //FLOAT aspect = d3dpp.BackBufferWidth / (FLOAT)d3dpp.BackBufferHeight;
    FLOAT aspect = 800.0f / (FLOAT)600.0f;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, aspect, 1.0f, 100.0f);

    // Create the mesh
    ID3DXMesh* pBoxMesh;
    APP_ERROR(FAILED(D3DXCreateBox(d3dDevice, 1.0f, 1.0f, 1.0f, &pBoxMesh, NULL)))("Failed to create box");

    // Initialize the graphics device for this demo
    d3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

    d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
    d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    //// Set the device to render into this window
    //IDirect3DSurface9* d3dBuffer;
    //d3dSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &d3dBuffer);
    //d3dDevice->SetRenderTarget(0, d3dBuffer);
    //D3DSURFACE_DESC desc;
    //d3dBuffer->GetDesc(&desc);
    //d3dBuffer->Release();

    // Load up all the meshes as D3DX meshes
    struct RescalingMesh
    {
        dcxVector3<dcFloat>* s;
        dcxVector3<dcFloat>* r;
        dcxVector3<dcFloat>* t;
        D3DXMATRIXA16 world;
        LPD3DXMESH d3dMesh;
        Mesh* meshResource;

        void update()
        {
            D3DXMATRIXA16 scale, rotate, translate;
            D3DXMatrixScaling(&scale, s->getX()->getValue(), s->getY()->getValue(), s->getZ()->getValue());
            D3DXMatrixRotationYawPitchRoll(&rotate,
                                           D3DXToRadian(r->getY()->getValue()),
                                           D3DXToRadian(r->getX()->getValue()),
                                           D3DXToRadian(r->getZ()->getValue()));
            D3DXMatrixTranslation(&translate, t->getX()->getValue(), t->getY()->getValue(), t->getZ()->getValue());

            // Combine all of the matrices
            D3DXMatrixMultiply(&world, &scale, &rotate);
            D3DXMatrixMultiply(&world, &world, &translate);
        }
    };
    RescalingMesh* rescalingMeshes = new RescalingMesh[display->size()];
    int meshIndex = 0;
    for (dcGenericResource::Array::const_iterator i = display->begin(); i != display->end(); ++i, ++meshIndex)
    {
        dc::dcTable<Mesh>::Element* element = (dcTable<Mesh>::Element*)(*i);
        Mesh* mesh = element->getImplementation();
        rescalingMeshes[meshIndex].meshResource = mesh;
        rescalingMeshes[meshIndex].s = mesh->getScaling();
        rescalingMeshes[meshIndex].r = mesh->getRotation();
        rescalingMeshes[meshIndex].t = mesh->getTranslation();
        rescalingMeshes[meshIndex].update();

        SubsetGeometry subsetGeometry;
        mesh->obtainSourceGeometry(d3dDevice, &subsetGeometry);

        // Get info from the mesh
        DWORD vertices = 0;
        DWORD indices = 0;

        // Sum all of the geometry data
        for (SubsetGeometry::iterator s = subsetGeometry.begin(); s != subsetGeometry.end(); ++s)
        {
            vertices += s->second->NumVertices;
            indices  += s->second->NumIndices;
        }

        LPD3DXMESH d3dMesh;

        // Create a new mesh
        if (APP_ERROR(FAILED(D3DXCreateMeshFVF(indices / 3, vertices, D3DXMESH_MANAGED|D3DXMESH_32BIT, D3DFVF_GEOMETRYVERTEX, d3dDevice, &d3dMesh)))
            ("Couldn't create the new mesh"))
            continue;

        rescalingMeshes[meshIndex].d3dMesh = d3dMesh;

        // Vertex/index pointers
        GeometryVertex* pVertices;
        DWORD* pIndices;
        DWORD* pAttrib;

        // Save the vertices, indices and attributes
        if (SUCCEEDED(d3dMesh->LockVertexBuffer(0, (VOID**)&pVertices)) &&
            SUCCEEDED(d3dMesh->LockIndexBuffer(0, (VOID**)&pIndices)) /*&&
            SUCCEEDED(d3dMesh->LockAttributeBuffer(0, &pAttrib))*/)
        {
            // Sums the number of vertices/indices/attributes
            DWORD totalVertices = 0;
            DWORD totalIndices = 0;
            DWORD totalAttrib = 0;

            // Write data from each subset
            for (SubsetGeometry::iterator s = subsetGeometry.begin(); s != subsetGeometry.end(); ++s)
            {
                // Copy the vertices
                memcpy(&pVertices[totalVertices], s->second->pVertices, s->second->NumVertices * sizeof(GeometryVertex));

                // Copy the indices (we have to offset the value though)
                for (DWORD k = 0; k < s->second->NumIndices; ++k)
                    pIndices[totalIndices + k] = s->second->pIndices[k] + totalVertices;
/*
                // Set the attribute elements for the indices
                for (DWORD k = 0; k < mesh->Subsets[s].NumIndices / 3; ++k)
                {
                    if (displayMode == 0)
                    {
                        pAttrib[totalAttrib + k] = 0;
                    }
                    else
                    {
                        pAttrib[totalAttrib + k] = mesh->Subsets[s].SubsetIndex;
                    }
                }*/

                // Add to the total values
                totalVertices += s->second->NumVertices;
                totalIndices += s->second->NumIndices;
                //totalAttrib += mesh->Subsets[s].NumIndices / 3;
            }

            // Unlock the buffers
            d3dMesh->UnlockVertexBuffer();
            d3dMesh->UnlockIndexBuffer();
            //d3dMesh->UnlockAttributeBuffer();
        }
    }

    //UpdateWindow(hDlg);
    //ShowWindow(hDlg, SW_SHOW);

    int editIndex = 0;

    bool editing = false;
    float editScale = 10.0f;
    POINTS editDown;

    FLOAT destXCoord = 0.0f, currentXCoord = 0.0f;
    float amp = 0.2f;

    // Go into the main window loop for handling the dialog
    bool running = true;
    while(running && messagePump())//IsWindow(hDlg) && running)
    {/*
        // Handle all of the messages
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // If we get a quit message, exit!
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
                running = false;

            // Clear out the message structure
            ZeroMemory(&msg, sizeof(msg));

        }*/

        myMouse.query();
        myKeyboard.query(0);

        POINTS d, u;
        if (myMouse.getRealClick(dcxWin32Mouse::BUTTON_LEFT, &d, &u))
        {
            if (editing)
            {
                POINTS pt;
                myMouse.getCoordinates(&pt, 0);
                int dx = u.x - d.x;
                int dy = u.y - d.y;
                float dist = sqrtf(dx*dx+dy*dy);

                rescalingMeshes[editIndex].s->getX()->setValue(editScale * dist);
                rescalingMeshes[editIndex].s->getY()->setValue(editScale * dist);
                rescalingMeshes[editIndex].s->getZ()->setValue(editScale * dist);
                rescalingMeshes[editIndex].update();

                editing = false;
            }
            else
            {
                int dx = u.x - d.x;
                int dy = u.y - d.y;
                editScale = rescalingMeshes[editIndex].s->getX()->getValue() / sqrtf(dx*dx + dy*dy);
                editDown = d;
                editing = true;
            }
            /*
            if (d.x < u.x)
            {
                rescalingMeshes[editIndex].s->getX()->setValue(rescalingMeshes[editIndex].s->getX()->getValue() * 1.5f);
                rescalingMeshes[editIndex].s->getY()->setValue(rescalingMeshes[editIndex].s->getY()->getValue() * 1.5f);
                rescalingMeshes[editIndex].s->getZ()->setValue(rescalingMeshes[editIndex].s->getZ()->getValue() * 1.5f);
            }
            else if (d.x > u.x)
            {
                rescalingMeshes[editIndex].s->getX()->setValue(rescalingMeshes[editIndex].s->getX()->getValue() * 0.75f);
                rescalingMeshes[editIndex].s->getY()->setValue(rescalingMeshes[editIndex].s->getY()->getValue() * 0.75f);
                rescalingMeshes[editIndex].s->getZ()->setValue(rescalingMeshes[editIndex].s->getZ()->getValue() * 0.75f);
            }
            else if (editIndex < display->size() - 1)
            {
                editIndex++;
            }

            rescalingMeshes[editIndex].update();*/
        }

        float scaling[3] = {
          rescalingMeshes[editIndex].s->getX()->getValue(),
          rescalingMeshes[editIndex].s->getY()->getValue(),
          rescalingMeshes[editIndex].s->getZ()->getValue(),
        };
        bool update = true;
        if (myKeyboard.keyPressed(KEY_UP)) amp *= 2;
        if (myKeyboard.keyPressed(KEY_DOWN)) amp /= 2;
        if (myKeyboard.keyPressed(KEY_Q)) scaling[0] += amp;
        if (myKeyboard.keyPressed(KEY_A)) scaling[0] -= amp;
        if (myKeyboard.keyPressed(KEY_W)) scaling[1] += amp;
        if (myKeyboard.keyPressed(KEY_S)) scaling[1] -= amp;
        if (myKeyboard.keyPressed(KEY_E)) scaling[2] += amp;
        if (myKeyboard.keyPressed(KEY_D)) scaling[2] -= amp;
        if (myKeyboard.keyPressed(KEY_R)) { scaling[0] += amp; scaling[1] += amp; scaling[2] += amp; }
        if (myKeyboard.keyPressed(KEY_F)) { scaling[0] -= amp; scaling[1] -= amp; scaling[2] -= amp; }
        rescalingMeshes[editIndex].s->getX()->setValue(scaling[0]);
        rescalingMeshes[editIndex].s->getY()->setValue(scaling[1]);
        rescalingMeshes[editIndex].s->getZ()->setValue(scaling[2]);

        if (myKeyboard.keyPressed(KEY_LEFT ) && (editIndex < display->size() - 1)) editIndex++;
        if (myKeyboard.keyPressed(KEY_RIGHT) && (editIndex > 0))                   editIndex--;
        if (myKeyboard.keyPressed(KEY_Z)) {
          rescalingMeshes[editIndex].r->getY()->setValue(rescalingMeshes[editIndex].r->getY()->getValue() - 90);
          update = true;
        }
        if (myKeyboard.keyPressed(KEY_X)) {
          rescalingMeshes[editIndex].r->getY()->setValue(rescalingMeshes[editIndex].r->getY()->getValue() + 90);
          update = true;
        }

        if (update) {
          rescalingMeshes[editIndex].update();
        }

        // Change where the display is located
        destXCoord = 1.0f * editIndex;
        currentXCoord = (destXCoord - currentXCoord) * 0.05f + currentXCoord;
        D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(currentXCoord, 4.0f, 3.0f), &D3DXVECTOR3(currentXCoord, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
        d3dDevice->SetTransform(D3DTS_VIEW, &view);

        d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

        // Render to the window
        if (SUCCEEDED(d3dDevice->BeginScene()))
        {
            D3DXMATRIXA16 wld;
            D3DXMatrixTranslation(&wld, currentXCoord, 0.0f, 0.0f);
            d3dDevice->SetTransform(D3DTS_WORLD, &wld);
            pBoxMesh->DrawSubset(0);

            // Relocate and render each mesh
            for (int i = 0; i < display->size(); ++i)
            {
                D3DXMATRIXA16 final;
                D3DXMatrixTranslation(&final, 1.0f * i, 0.0f, 0.0f);
                D3DXMatrixMultiply(&final, &rescalingMeshes[i].world, &final);
                d3dDevice->SetTransform(D3DTS_WORLD, &final);
                rescalingMeshes[i].d3dMesh->DrawSubset(0);
            }

            // Clear the back-buffer
            d3dDevice->EndScene();
        }

        present();
        //d3dSwapChain->Present(NULL, NULL, NULL, NULL, 0);
        //InvalidateRect(hD3DTarget, NULL, TRUE);
        //SendMessage(hD3DTarget, WM_PAINT, 0, 0);
        //d3dDevice->Present(NULL, NULL, NULL, NULL);
    }

    for (DWORD index = 0; index < display->size(); ++index)
    {
        rescalingMeshes[index].d3dMesh->Release();
    }
    delete[] rescalingMeshes;
    //d3dSwapChain->Release();
    SAFE_RELEASE(pBoxMesh);
    releaseObjects();

    // Success
    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  end
// Desc:  Forces this dialog (and any of its children) to close
//------------------------------------------------------------------------------------------------
void RescaleMeshesDialog::end(INT_PTR result)
{
    //EndDialog(my
}




//------------------------------------------------------------------------------------------------
// Name:  dialogMenuNotification
// Desc:  If a menu item is selected, this method is invoked
//------------------------------------------------------------------------------------------------
INT_PTR RescaleMeshesDialog::dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex)
{
    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  dialogNotification
// Desc:  When something on the dialog is activated, this function is invoked
//------------------------------------------------------------------------------------------------
INT_PTR RescaleMeshesDialog::dialogNotification(HWND hDlg, HWND hControl, WORD ControlID, WORD Notification)
{
    return 0;
}




//------------------------------------------------------------------------------------------------
// Name:  dialogEvent
// Desc:  All messages received by the dialog are passed through here.  The default
//------------------------------------------------------------------------------------------------
INT_PTR RescaleMeshesDialog::dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool ret;
    myKeyboard.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
    myMouse.handleWindowsMessage(hDlg, uMsg, wParam, lParam, &ret);
    if (uMsg == WM_CLOSE)
        DestroyWindow(hDlg);
    return 0;
}


