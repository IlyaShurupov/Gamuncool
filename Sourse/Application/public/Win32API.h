#pragma once

// Windows Header Files:
#include <d2d1.h>
#include <windows.h>

#include "FrameBuff.h"


class SystemHandler {
 public:
  SystemHandler(int Width, int Height);
  ~SystemHandler();

  // Register the win & call methods for instantiating drawing res
  HRESULT Initialize();

  // Get Buffer to write
  FBuff* getFBuff();

  // UserInputs
  void getUserInputs(struct UserInputs* user_inputs);

  // Draw Fbuff.
  void SysOutput();

  void consoletoggle();

  bool active();

  void destroy();

  void getRect(Rect<SCR_UINT>& rect);
  void setRect(Rect<SCR_UINT>& rect);

  void SetIcon(std::string stricon);

 private:

  // The windows procedure.
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

 public:
  bool close = false;

 private:
  HICON hWindowIcon = NULL;
  HICON hWindowIconBig = NULL;
  MSG msg;
  HDC hdcMem;
  HWND m_hwnd;
  FBuff* buff;
  ID2D1Factory* m_pDirect2dFactory;
};