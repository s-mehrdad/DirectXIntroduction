﻿
// ===========================================================================
/// <summary>
/// Direct2D.h
/// DirectXIntroduction
/// created by Mehrdad Soleimanimajd on 04.08.2019
/// </summary>
/// <created>ʆϒʅ, 04.08.2019</created>
/// <changed>ʆϒʅ, 04.07.2023</changed>
// ===========================================================================

#ifndef DIRECT2D_H
#define DIRECT2D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d2d1_1.h> // DirectX includes
#pragma comment (lib, "d2d1.lib") // linkage to the 'd2d1' library
#include <dwrite_1.h> // DirectX includes
#pragma comment (lib, "dwrite.lib") // linkage to the 'dwrite' library


#include "Core.h"
#include "Direct3D.h"
#include "Game.h"


// Direct2D wrapper
class Direct2D
{
    friend class TheCore;
    friend class Direct3D;
    friend class Game;
private:
    TheCore* core; // pointer to the framework core

    Microsoft::WRL::ComPtr<IDWriteFactory1> writeFac; // DirectWrite factory
    Microsoft::WRL::ComPtr<ID2D1Factory1> factory; // Direct2D factory
    Microsoft::WRL::ComPtr<ID2D1Device> device; // Direct2D device
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> deviceCon; // Direct2D device context
    Microsoft::WRL::ComPtr<IDXGISurface1> dcBuffer; // Direct2D device context buffer
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> dcBitmap; // render target bitmap (desired properties)

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushYellow; // brushes
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushWhite;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBlack;

    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatFPS; // text formats
    Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatLogs;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutFPS; // text layouts
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutLogs;
    bool textLayoutsDebug;

    bool initialized; // true if initialization was successful
    bool allocated; // true if resources allocation was successful
public:
    Direct2D (TheCore*); // creation of the device
    void allocateResources (void); // Direct2D resources resize/creation
    void initializeTextFormats (void); // different formats initialization
    const bool& isInitialized (); // get the initialized state
    void debugInfos (void); // FPS infos plus logs to screen
};


#endif // !DIRECT2D_H
