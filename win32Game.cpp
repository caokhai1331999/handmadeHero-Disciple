/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include <iostream>
#include "Windows.h"

using namespace std;

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

global_variable bool  Running{true};
global_variable BITMAPINFO Bitmapinfo;
global_variable void* BitmapMemory;
global_variable HBITMAP BitmapHandle;
const global_variable int BytesPerPixel{4};
const global_variable HWND Window;

struct win32Dimension{
    const int Height{720};
    const int Width{1280};
    HDC DeviceContext;    
}Dimens;

void RenderSplendidGradient(int XOffset = 0, int YOffset = 0){
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    int Pitch = 4*Dimens.Width;
    uint8* Row = (uint8 *)BitmapMemory;
    
    for (int y{0}; y < Dimens.Height; y++){
        uint32* Pixel = (uint32 *)Row;
        for(int x{0}; x < Dimens.Width; x++){
            uint8 Green = ( x + XOffset);
            uint8 Red = ( y + YOffset);
            // NOTE: AA RR GG BB()
            *Pixel++ = (uint8) ( (Red<<8)|Green);
        }
        Row += Pitch;
    }        
}

internal void Win32ResizeDIBSection(int XOffset, int YOffset){
    // Create a storage for memory first
    // using virtualAlloc
    // then store bitmap in it
    
    if(BitmapMemory){
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }
    
    int BitmapWidth, BitmapHeight;
    BitmapWidth = Dimens.Width;
    BitmapHeight = Dimens.Height;
    int BitMapMemorySize;
    // 4 bits per pixel and there are total Width*Height pixels

    Bitmapinfo.bmiHeader.biSize = sizeof(Bitmapinfo.bmiHeader);
    Bitmapinfo.bmiHeader.biWidth = Dimens.Width;
    Bitmapinfo.bmiHeader.biHeight = Dimens.Height;
    Bitmapinfo.bmiHeader.biPlanes = 1;
    Bitmapinfo.bmiHeader.biBitCount = 32;
    Bitmapinfo.bmiHeader.biCompression = BI_RGB;
             
    BitMapMemorySize = BytesPerPixel*(BitmapWidth*BitmapHeight);
    BitmapMemory = VirtualAlloc(0 ,BitMapMemorySize ,MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    
    RenderSplendidGradient(0, 0);
}
// TODO: Cause of unintentionally deletion of code
// first thing first make window and then create the
// receive and translate message DONE!
// Then animate back buffer using createDIBSection and strechDIBit


// NOTE: Keep in mind that try to all what you need to release back to memory
// in a total thing so that I can release it in aggregate

internal void win32UpdateWindow(HWND window, int X, int Y, int Width, int Height){
    StretchDIBits(
        Dimens.DeviceContext,
        0,0,Dimens.Width,Dimens.Height,       // Destination Rectangle
        X,Y,Width,Height,       // Source rectangle
        // const VOID* lpBits,
        BitmapMemory,
        &Bitmapinfo,
        DIB_RGB_COLORS,
        SRCCOPY
);    
}

LRESULT CALLBACK MainWindowCallBack(
  HWND Window,
  UINT Message,
  WPARAM Wparam,
  LPARAM Lparam    
)
{
    LRESULT result;
    switch(Message){
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(0, 0);
            // DIB is a table where store BIT color infor
            OutputDebugStringA("WM_SIZE\n");
        }break;
        
        case WM_CLOSE:
        {
            Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        }break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");            
        }break;

        case WM_DESTROY:
        {
            Running = false;
            PostQuitMessage(0);
            OutputDebugStringA("WM_DESTROY\n");            
        }break;

        case WM_PAINT:            
        {
            PAINTSTRUCT Paint;
            Dimens.DeviceContext = BeginPaint(Window, &Paint);

            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int width = Paint.rcPaint.right - Paint.rcPaint.left;
            int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            local_persist DWORD Operation = WHITENESS;

            if (Operation == WHITENESS){
                Operation = BLACKNESS;
            }else {
                Operation = WHITENESS;
            }
            
            win32UpdateWindow(Window, X, Y, width, height);
            EndPaint(Window, &Paint);
            OutputDebugStringA("WM_PAINT\n");
        }break;
        
        default:
        {
            OutputDebugStringA("DEFAULT\n");
            result = DefWindowProc(Window, Message, Wparam, Lparam);
        }break;
    }
    return result;
}

int CALLBACK WinMain
(HINSTANCE Instance,
 HINSTANCE hInstPrev,
 PSTR cmdline,
 int cmdshow)
{
    WNDCLASSA WindowClass = {};
    // NOTE: This is where receiving the message to change
    // for any change in window

    WindowClass.lpfnWndProc = MainWindowCallBack;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "First Game Window Class";
  
    if(RegisterClassA(&WindowClass)){
        
        HWND WindowHandle = CreateWindowExA(
            // NOTE: The window didn't show up is because the first argument
            // is not proper still have alot to do
            0,
            WindowClass.lpszClassName,
            "win32GameWithoutEngine",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance ,
            0);
        int XOffset{0}, YOffset{0};
        
        if(WindowHandle){
            while(Running){
                MSG Message;
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)){
                    if (Message.message == WM_QUIT){
                        Running = false;
                    }
                    DispatchMessage(&Message);
                    TranslateMessage(&Message);
                    // Win32ResizeDIBSection(XOffset, YOffset);
                    // win32UpdateWindow(Window, 0, 0, Dimens.Width, Dimens.Height);
                }
            }
        }else{
        // TODO: Logging
        }
    } else {
        // TODO: Logging
    }   
    return (0);
}
