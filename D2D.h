﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,04.08.2019</created>
/// <changed>ʆϒʅ,06.08.2019</changed>
// ********************************************************************************

#ifndef D2D_H
#define D2D_H


#include <wrl/client.h> // Windows and COM wrappers (calls to DirectX)
#include <d2d1_1.h> // DirectX includes
#pragma comment (lib, "d2d1.lib") // linkage to the 'd2d1' library
#include <dwrite_1.h> // DirectX includes
#pragma comment (lib, "dwrite.lib") // linkage to the 'dwrite' library


#include "Core.h"
#include "D3D.h"


class Direct2D
{
  friend class TheCore;
  friend class Direct3D;
private:
  TheCore* theCore; // pointer to DirectX core application

  Microsoft::WRL::ComPtr<IDWriteFactory1> writeFactory; // DirectWrite factory
  Microsoft::WRL::ComPtr<ID2D1Factory1> factory; // Direct2D factory
  Microsoft::WRL::ComPtr<ID2D1Device> dev; // Direct2D device
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> devCon; // Direct2D device context

  bool initialized; // true if the initialization was successful
public:
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushYellow; // brushes
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushWhite;
  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBlack;

  Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormatFPS; // text formats
  Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayoutFPS; // text layouts

  Direct2D ( TheCore* ); // creation of the device and its context
  void createBitmapRenderTarget ( void ); // bitmap render target creation (same back buffer in use for Direct3D)
  void initializeTextFormats ( void ); // different formats initialization
  const bool& isInitialized (); // get the initialized state
  void printFPS ( void ); // FPS to screen
  void shutdown ( void ); // destruction preparations
};


#endif // !D2D_H