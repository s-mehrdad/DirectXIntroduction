﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,24.07.2019</created>
/// <changed>ʆϒʅ,31.08.2019</changed>
// ********************************************************************************

#include "Game.h"
#include "Shared.h"


Game::Game ( HINSTANCE& h_instance ) :
  vertexCountTriangles_A ( 0 ), vertexCountLine ( 0 ), vertexCountTriangles_B ( 0 ),
  initialized ( false ), allocated ( false )
{
  try
  {

    // the game framework instantiation
    core = new (std::nothrow) TheCore ( h_instance, this );

    if (!core->isInitialized ())
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Initialization of framework failed!" );
    }

    mappedLine.pData = nullptr;
    mappedLine.RowPitch = 0;

    initialized = true;
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The game is successfully initialized." );

    allocateResources ();

    if (!allocated)
      PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                L"The game resources is successfully allocated." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Game::allocateResources ( void )
{
  try
  {

    allocated = false;

    vertexCountTriangles_A = 9;

    // three triangles
    Vertex triangles_A [] {
      DirectX::XMFLOAT3{-0.95f, 0.0f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f}, // bottom left
      DirectX::XMFLOAT3{-0.9f, -0.12f, 0.0f}, DirectX::XMFLOAT4{0.53f, 0.53f, 0.53f, 1.0f}, // top middle
      DirectX::XMFLOAT3{-1.0f, -0.12f, 0.0f}, DirectX::XMFLOAT4{0.93f, 0.93f, 0.93f, 1.0f}, // bottom right

      DirectX::XMFLOAT3{-0.75f, 0.0f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f},
      DirectX::XMFLOAT3{-0.7f, -0.12f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f},
      DirectX::XMFLOAT3{-0.8f, -0.12f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f},

      DirectX::XMFLOAT3{-0.55f, 0.2f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f},
      DirectX::XMFLOAT3{-0.5f, -0.12f, 0.0f}, DirectX::XMFLOAT4{00.53f, 0.53f, 0.53f, 1.0f},
      DirectX::XMFLOAT3{-0.6f, -0.12f, 0.0f}, DirectX::XMFLOAT4{0.93f, 0.93f, 0.93f, 1.0f}
    };
    unsigned long trianglesIndices_A [] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    // buffer description
    D3D10_BUFFER_DESC vertexBufferDescT_A;
    vertexBufferDescT_A.ByteWidth = sizeof ( Vertex ) * ARRAYSIZE ( triangles_A ); // buffer size
    vertexBufferDescT_A.Usage = D3D10_USAGE_DEFAULT; // default: only GPC can read and write
    vertexBufferDescT_A.BindFlags = D3D10_BIND_VERTEX_BUFFER; // how to bound to graphics pipeline
    vertexBufferDescT_A.CPUAccessFlags = 0; // CPU access
    //descBufferTriangle.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    vertexBufferDescT_A.MiscFlags = 0; // for now
    //descBufferTriangle.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    // data, with which the buffer is initialized
    D3D10_SUBRESOURCE_DATA vertexDataTriangles_A = {
      triangles_A, // pointer to data in system memory (copy to GPU)
      0, // distance between the lines of the texture in bytes (not needed for vertex buffer)
      0 }; // distance between the depth levels in bytes (not needed for vertex buffer)

    // vertex buffer: purpose: maintain system and video memory
    // note E_OUTOFMEMORY: self-explanatory
    if (FAILED ( core->d3d->device->CreateBuffer ( &vertexBufferDescT_A, &vertexDataTriangles_A, &vertexBuffer [0] ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of triangle vertex buffer failed!" );
      return;
    }

    D3D10_BUFFER_DESC indexBufferDescT_A;
    indexBufferDescT_A.ByteWidth = sizeof ( long ) * vertexCountTriangles_A;
    indexBufferDescT_A.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDescT_A.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDescT_A.CPUAccessFlags = 0;
    //descIndexBufferTriangle.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    indexBufferDescT_A.MiscFlags = 0;
    //descIndexBufferTriangle.StructureByteStride = 0;

    D3D10_SUBRESOURCE_DATA indexDateTriangles_A = { trianglesIndices_A, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &indexBufferDescT_A, &indexDateTriangles_A, &indexBuffer [0] ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of triangle vertex buffer failed!" );
      return;
    }



    vertexCountLine = 2;

    // a line
    Vertex line [] {
      DirectX::XMFLOAT3{-.2f, 0.0f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f}, // left position
      DirectX::XMFLOAT3{0.2f, 0.0f, 0.0f}, DirectX::XMFLOAT4{0.13f, 0.13f, 0.13f, 1.0f} // right position
    };
    unsigned long lineIndices [] { 0, 1 };

    D3D10_BUFFER_DESC vertexBufferDescL;
    vertexBufferDescL.ByteWidth = sizeof ( Vertex ) * vertexCountLine;
    vertexBufferDescL.Usage = D3D10_USAGE_DYNAMIC;
    vertexBufferDescL.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDescL.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    //vertexBufferDescL.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    vertexBufferDescL.MiscFlags = 0;
    //vertexBufferDescL.StructureByteStride = 0;

    D3D10_SUBRESOURCE_DATA vertexDateLine = { line, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &vertexBufferDescL, &vertexDateLine, &vertexBuffer [1] ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of line vertex buffer failed!" );
    }

    D3D10_BUFFER_DESC indexBufferDescL;
    indexBufferDescL.ByteWidth = sizeof ( long ) * vertexCountLine;
    indexBufferDescL.Usage = D3D10_USAGE_DYNAMIC;
    indexBufferDescL.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDescL.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    //indexBufferDescL.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    indexBufferDescL.MiscFlags = 0;
    //indexBufferDescL.StructureByteStride = 0;

    D3D10_SUBRESOURCE_DATA indexDateLine = { lineIndices, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &indexBufferDescL, &indexDateLine, &indexBuffer [1] ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of line vertex buffer failed!" );
    }



    texture = new (std::nothrow) Texture<TargaHeader>
      ( core->d3d->device.Get (), "./textures/clouds.tga" ); // a texture file

    if (!texture)
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Instantiation of texture failed!" );
      return;
    }

    vertexCountTriangles_B = 6;

    VertexT triangles_B [] {
      DirectX::XMFLOAT3{-0.4f, -0.4f, 0.0f}, DirectX::XMFLOAT2{0.0f, 1.0f},
      DirectX::XMFLOAT3{-0.4f, 0.0f, 0.0f}, DirectX::XMFLOAT2{0.0f, 0.0f},
      DirectX::XMFLOAT3{0.0f, -0.4f, 0.0f}, DirectX::XMFLOAT2{1.0f, 1.0f},

      DirectX::XMFLOAT3{-0.4f, 0.0f, 0.0f}, DirectX::XMFLOAT2{0.0f, 0.0f},
      DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}, DirectX::XMFLOAT2{1.0f, 0.0f},
      DirectX::XMFLOAT3{0.0f, -0.4f, 0.0f}, DirectX::XMFLOAT2{1.0f, 1.0f}
    };
    unsigned long trianglesIndices_B [] { 0, 1, 2, 3, 4, 5 };

    D3D10_BUFFER_DESC vertexBufferDescT_B;
    vertexBufferDescT_B.ByteWidth = sizeof ( VertexT ) * vertexCountTriangles_B;
    vertexBufferDescT_B.Usage = D3D10_USAGE_DEFAULT;
    vertexBufferDescT_B.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDescT_B.CPUAccessFlags = 0;
    //descBufferTriangle.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    vertexBufferDescT_B.MiscFlags = 0;
    //descBufferTriangle.StructureByteStride = 0; // Direct3D 11: structured buffer (the size of each element)

    D3D10_SUBRESOURCE_DATA vertexDataTriangles_B = { triangles_B, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &vertexBufferDescT_B, &vertexDataTriangles_B, &vertexBufferT ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of triangle vertex buffer failed!" );
      return;
    }

    D3D10_BUFFER_DESC indexBufferDescT_B;
    indexBufferDescT_B.ByteWidth = sizeof ( long ) * vertexCountTriangles_B;
    indexBufferDescT_B.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDescT_B.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDescT_B.CPUAccessFlags = 0;
    //descIndexBufferTriangle.MiscFlags = D3D10_RESOURCE_MISC_GDI_COMPATIBLE;
    indexBufferDescT_B.MiscFlags = 0;
    //descIndexBufferTriangle.StructureByteStride = 0;

    D3D10_SUBRESOURCE_DATA indexDateTriangle_B = { trianglesIndices_B, 0, 0 };

    if (FAILED ( core->d3d->device->CreateBuffer ( &indexBufferDescT_B, &indexDateTriangle_B, &indexBufferT ) ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Creation of triangle vertex buffer failed!" );
      return;
    }

    allocated = true;

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


const bool& Game::isReady ( void )
{
  return initialized;
};


const bool Game::run ( void )
{
  try
  {

    MSG msg { 0 }; // a new message structure
    unsigned short counter { 0 };

    core->timer->event ( "reset" ); // reset (start)

    core->d3d->camera->setPosition ( 0.0f, 0.0f, -2.0f ); // set the start view


    // main part (game engine)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Todo: research for a robust game loop:
    // mathematical simulation of time and reality, physics, multithreading
    //
    do // continuous loop
    {

#pragma region peekLoop
      if ((counter % 10) == 0)
      {
        // check and peek (get) window messages already placed in an event queue.
        // note the difference between the below two functions:
        // the get function, once called, actually waits for a message,
        // while peek function allows the normal flow, if there is no message in the message queue.
        while ( // peek loop for messages (empting the message queue)
                PeekMessage (
                  // pointer to a message structure to receive message information
                  &msg,
                  // handle to the window, whose messages is intended,
                  // and NULL as argument allow the retrieve of all messages for any window, which belongs to the current thread.
                  core->appHandle,
                  // to retrieve messages filtered through the introduced range. (both zero retunes all available messages)
                  // first message to last message
                  0, 0,
                  // removal flags specify how the messages are to be handled:
                  // additionally to below introduced argument, PM_NOREMOVE prevents the removal of messages in the queue,
                  // therefore after it is passed, the get function is additionally needed to actually retrieve the messages.
                  PM_REMOVE ))
          //while ( GetMessage ( &msg, NULL, 0, 0 ) ) // not a good one for a game, which needs to deliver 30 F/S
        {
          // translation of the virtual-key messages into character messages
          TranslateMessage ( &msg );
          // dispatching the message to the window procedure function, so the event could be handled appropriately.
          DispatchMessage ( &msg );
          // the evaluated value: exit (using intellisense or MSDN, the possible and obvious messages could be seen)
        }
        counter = 0;
      }
#pragma endregion



      // tick the timer to calculate a frame
      core->timer->tick ();
      // -- fps calculation
      core->frameStatistics ();

      if (!core->paused)
      {

        // -----------------------------------------------------------------------------------------------------------
        // a game loop purpose:
        // -- process inputted data
        // Todo add input processes so far

        render ();


        // -----------------------------------------------------------------------------------------------------------
        // -- output a frame
        core->d3d->present ();


        // -----------------------------------------------------------------------------------------------------------
        // -- update the game universe/logic
        // Note the needed delta time
        update ();

      } else
      {
        // Todo add additional suitable procedure can be done in paused state

        // if there is no suitable processing for paused state, set the engine at hibernation:
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
      }

      counter++;
    } while (running == true);
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    return true;
  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
    return false;
  }
};


void Game::render ( void )
{
  try
  {

    core->d3d->clearBuffers ();


    core->d3d->camera->render ();
    core->d3d->renderMatrices ();


    if (core->d2d && core->debug)
      core->d2d->debugInfos (); // -- fps on screen representation



    // setting the active vertex/pixel shaders (active shader technique)
    core->d3d->device->VSSetShader ( core->d3d->shader->vertexShader.Get () );
    core->d3d->device->PSSetShader ( core->d3d->shader->pixelShader.Get () );

    // setting the active input layout
    core->d3d->device->IASetInputLayout ( core->d3d->shader->inputLayout.Get () );

    // set the active vertex and index buffers (binds an array of vertex/index buffers to input-assembler stage)
    // basically, which vertices to put to graphics pipeline when rendering
    unsigned int strides = sizeof ( Vertex );
    unsigned int offset = 0;
    // fourth parameter: constant array of stride values (one stride for each buffer in the vertex-buffer array)
    // fifth parameter: number of bytes between the first element and the element to use (usually zero)
    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBuffer [0].GetAddressOf (), &strides, &offset );
    // set the active corresponding index buffer in the input assembler
    core->d3d->device->IASetIndexBuffer ( indexBuffer [0].Get (), DXGI_FORMAT_R32_UINT, 0 );

    // set primitive topology (Direct3D has no idea about the mathematical conventions to use)
    // basically how to render the resource (vertex data) to screen
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw indexed vertices, starting from vertex 0
    core->d3d->device->DrawIndexed ( vertexCountTriangles_A, 0, 0 );



    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBuffer [1].GetAddressOf (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( indexBuffer [1].Get (), DXGI_FORMAT_R32_UINT, 0 );
    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );
    core->d3d->device->DrawIndexed ( vertexCountLine, 0, 0 );



    //// setting the active texture
    //core->d3d->device->PSSetShaderResources ( 0, 1, texture->getTexture () );

    core->d3d->device->VSSetShader ( core->d3d->shader->vertexShaderT.Get () );
    core->d3d->device->PSSetShader ( core->d3d->shader->pixelShaderT.Get () );

    core->d3d->device->IASetInputLayout ( core->d3d->shader->inputLayoutT.Get () );

    // setting the active sampler
    core->d3d->device->PSSetSamplers ( 0, 1, core->d3d->shader->samplerState.GetAddressOf () );

    strides = sizeof ( VertexT );

    core->d3d->device->IASetVertexBuffers ( 0, 1, vertexBufferT.GetAddressOf (), &strides, &offset );
    core->d3d->device->IASetIndexBuffer ( indexBufferT.Get (), DXGI_FORMAT_R32_UINT, 0 );

    core->d3d->device->IASetPrimitiveTopology ( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // draw indexed vertices, starting from vertex 0
    core->d3d->device->DrawIndexed ( vertexCountTriangles_B, 0, 0 );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Game::update ( void )
{
  try
  {

    // update line vertex buffer
    HRESULT hR;
    char modeX_1 { 0 };
    char modeY_1 { 0 };
    char modeX_2 { 0 };
    char modeY_2 { 0 };

    // map the data back to system memory using a sub-resource
    // second parameter: what CPU does when GPU is busy
    // note that in Direct3D11 a resource may contain sub-resources (additional parameters of device context method)
    // after the resource is mapped, any change to it is reflected to the vertex buffer.
    hR = vertexBuffer [1]->Map ( D3D10_MAP_WRITE_NO_OVERWRITE, 0, &mappedLine.pData );
    if (FAILED ( hR ))
    {
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                                L"Mapping the resource data failed!" );
    }

    // update the sub-resource:

    // turn the line clockwise
    Vertex* vertexData = reinterpret_cast<Vertex*>(mappedLine.pData);

    static float temp { vertexData->position.x };

    if (vertexData->position.x < (vertexData + 1)->position.x)
    {
      modeX_1 = 1 ;
      modeY_1 = 1 ;
      modeX_2 = -1 ;
      modeY_2 = -1 ;
    } else
      if (vertexData->position.x > ( vertexData + 1 )->position.x)
      {
        modeX_1 = 1;
        modeY_1 = -1;
        modeX_2 = -1;
        modeY_2 = 1;
      }

    if ((vertexData + 1)->position.x < temp)
    {
      temp = vertexData->position.x;
      vertexData->position.x = (vertexData + 1)->position.x;
      (vertexData + 1)->position.x = temp;
      temp = vertexData->position.y;
      vertexData->position.y = (vertexData + 1)->position.y;
      (vertexData + 1)->position.y = temp;
      temp = vertexData->position.x;
    }

    vertexData->position.x += modeX_1 * 0.0001f;
    vertexData->position.y += modeY_1 * 0.0001f;
    (vertexData + 1)->position.x += modeX_2 * 0.0001f;
    (vertexData + 1)->position.y += modeY_2 * 0.0001f;

    // randomize the colour vertices
    float rnd_1 { 0.0f };
    float rnd_2 { 0.0f };
    float rnd_3 { 0.0f };
    rnd_1 = ((rand () % 100) / static_cast<float>(100));
    rnd_2 = ((rand () % 100) / static_cast<float>(100));
    rnd_3 = ((rand () % 100) / static_cast<float>(100));
    vertexData->color.x = (vertexData + 1)->color.x = rnd_1; // red
    vertexData->color.y = (vertexData + 1)->color.y = rnd_2; // green
    vertexData->color.z = rnd_1 = (vertexData + 1)->color.z = rnd_3; // blue

    // validates the pointer of the vertex buffer's resource and enables the GPU's read access upon.
    vertexBuffer [1]->Unmap ();

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};


void Game::shutdown ( void )
{
  try
  {

    initialized = false;
    unsigned long rC { 0 };
    //HRESULT hR;
    rC = indexBuffer [0].Reset ();
    rC = indexBuffer [1].Reset ();
    rC = vertexBuffer [0].Reset ();
    rC = vertexBuffer [1].Reset ();
    texture->Release ();
    rC = indexBufferT.Reset ();
    rC = vertexBufferT.Reset ();
    if (core)
    {
      core->shutdown ();
      delete core;
    }

    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The Game is successfully shut down." );

  }
  catch (const std::exception& ex)
  {
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread",
                                              Converter::strConverter ( ex.what () ) );
  }
};
