/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include <iostream>
#include "Windows.h"
#include "stdint.h"
#include "xinput.h"

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

// NOTE: This is all about calling the function in the Xinput.h without the noticing from the
// compiler
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex,XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
// NOTE: The second line will be expand out to be like this :
// typedef x_input_get_state(DWORD dwUserIndex,XINPUT_STATE *pState)
// This is to turn on the compiler strict type checking
// And to DECLARE A FUNCTION SIGNATURE AS A TYPE
// for example: x_input_get_state _XinputgetState()
X_INPUT_GET_STATE(XinputGetStateStub){
    return (0);
}
// NOTE: But the rules of C does not allow this(x_input_get_state _XinputGetStateStub(){//do something;})
// so we use this for function pointer
global_variable x_input_get_state* XinputGetState_  = XinputGetStateStub;
// So finally we have a pointer name XinputGetState point to the function
// XinputGetStateStub(DWORD ....) which basically X_INPUT_GET_STATE() function
#define XInputGetState XinputGetState_
// This one is to replace the XinputGetState which already been called in Xinput.h
// with the XinputGetState

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex,XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XinputSetStateStub){
    return (0);
}
global_variable x_input_set_state* XinputSetState_  = XinputSetStateStub;
#define XInputSetState XinputSetState_

internal void
win32LoadXInput(void){
    HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
    if(XInputLibrary){
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XinputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XinputSetState");
    }
}

global_variable bool  Running;
global_variable HWND Window;
global_variable RECT ClientRect;
global_variable HDC DeviceContext;
global_variable int  XOffset{0}, YOffset{0};

const global_variable int Height{720};
const global_variable int Width{1280};

struct Win32_Offscreen_Buffer{  
    BITMAPINFO Bitmapinfo;
    void* BitmapMemory;
    HBITMAP BitmapHandle;
    int BitmapWidth;
    int BitmapHeight;
    const int BytesPerPixel{4};
}BackBuffer;

struct win32Dimension{
    int Height{720};
    int Width{1280};
}Dimens;

void GetWindowDimension(HWND Window){
    GetClientRect(Window, &ClientRect);
    Dimens.Width = ClientRect.right - ClientRect.left;
    Dimens.Height= ClientRect.bottom - ClientRect.top;
}

// TODO: Now time to move on to input/output section 

void RenderSplendidGradient(Win32_Offscreen_Buffer* OBuffer,int XOffset, int YOffset){
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    int Width = OBuffer->BitmapWidth;
    int Height = OBuffer->BitmapHeight;
    
    int Pitch = OBuffer->BytesPerPixel*OBuffer->BitmapWidth;
    uint8* Row = (uint8 *)OBuffer->BitmapMemory;
    
    for (int Y{0}; Y < Height; Y++){
        uint32* Pixel = (uint32 *)Row;
        for(int X{0}; X < Width; X++){
            uint8 Blue = ( X + XOffset);
            uint8 Green = ( Y + YOffset);
            // NOTE: AA RR GG BB()
            // Because I limit the size of Pixels so it can not add Green color to its storage
            *Pixel++ = ( (Green<<8) | Blue);
        }
        // Instead of manually move row pointer every y axis (by add it to the pitch)
        // we just need to reuse the Pixel pointer pass it to row where it was already moved
        Row = (uint8 *)Pixel;
    }        
}

internal void Win32ResizeDIBSection(Win32_Offscreen_Buffer* OBuffer, int Width, int Height){
    // Create a storage for memory first
    // using virtualAlloc
    // then store bitmap in it
    
    if(OBuffer->BitmapMemory){
        VirtualFree(OBuffer->BitmapMemory, 0, MEM_RELEASE);
    }
    // NOTE: The BitmapWidth change every time we resize the window
    OBuffer->BitmapWidth = Width;
    OBuffer->BitmapHeight = Height;
    
    int BitMapMemorySize;
    // 4 bits per pixel and there are total Width*Height pixels

    OBuffer->Bitmapinfo.bmiHeader.biSize = sizeof(OBuffer->Bitmapinfo.bmiHeader);
    OBuffer->Bitmapinfo.bmiHeader.biWidth = OBuffer->BitmapWidth;
    // NOTE: Don't know why I have to leave negative height here
    OBuffer->Bitmapinfo.bmiHeader.biHeight = -OBuffer->BitmapHeight;
    OBuffer->Bitmapinfo.bmiHeader.biPlanes = 1;
    OBuffer->Bitmapinfo.bmiHeader.biBitCount = 32;
    OBuffer->Bitmapinfo.bmiHeader.biCompression = BI_RGB;
             
    BitMapMemorySize = OBuffer->BytesPerPixel*(OBuffer->BitmapWidth*OBuffer->BitmapHeight);
    OBuffer->BitmapMemory = VirtualAlloc(0 ,BitMapMemorySize ,MEM_COMMIT, PAGE_READWRITE);
}

// TODO: Cause of unintentionally deletion of code
// first thing first make window and then create the
// receive and translate message DONE!
// Then animate back buffer using createDIBSection and strechDIBit

// NOTE: Keep in mind that try to all what you need to release back to memory
// in a total thing so that I can release it in aggregate

internal void Win32DisplayBufferWindow(HDC DeviceContext, RECT* ClientRect, Win32_Offscreen_Buffer* OBuffer, int X, int Y, int Width, int Height){
    // the Source and the Destination rectangle means
    
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    
    StretchDIBits(
        DeviceContext,
        0,0,OBuffer->BitmapWidth, OBuffer->BitmapHeight, // Source rectangle
        0,0,WindowWidth, WindowHeight,                 // Destination Rectangle
        // const VOID* lpBits,
        OBuffer->BitmapMemory,
        &OBuffer->Bitmapinfo,
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
            // NOTE: Maybe the underlying cause of the unchanging bit is lay on the
            // update the dc and the window resize section
            DeviceContext = GetDC(Window);
            GetWindowDimension(Window);
            // RECT ClientRect;
            // NOTE: Whenever the window is resized, this function capture the size
            // of the new window and update a new proper DIB for that
            // DIB is a table where store BIT color infor
            Win32ResizeDIBSection(&BackBuffer, Dimens.Width, Dimens.Height);
            Win32DisplayBufferWindow(DeviceContext, &ClientRect, &BackBuffer, 0,0, Dimens.Width, Dimens.Height);
            OutputDebugStringA("WM_SIZE\n");
        }break;
        
        case WM_CLOSE:
        {
            Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        }break;

        case WM_KEYDOWN:
        {            
            bool IsDown = ((Lparam &(1 << 31)) == 0);
            uint32 vkCode = Wparam;
                 if(vkCode == VK_LEFT){
                
                    OutputDebugStringA("Left Button :");
                    if(IsDown){                    
                        OutputDebugStringA(" Is Down");
                    }
                    OutputDebugStringA("\n");
                }            
        }break;

        case WM_SYSKEYDOWN:
        {            
            OutputDebugStringA("WM_SYSKEYDOWN\n");            
        }break;

        case WM_SYSKEYUP:
        {            
            OutputDebugStringA("WM_SYSKEYUP\n");            
        }break;

        case WM_KEYUP:
        {
            uint32 vkCode = Wparam;
            // NOTE: This is whether bit 30 or 0 (never 1).
            // So if it is bit 30 it is down 
            bool WasDown = ((Lparam &(1 << 30)) != 0);
            bool IsDown = ((Lparam &(1 << 31)) == 0);            
            if (WasDown != IsDown){

                if(vkCode == VK_UP){
                    YOffset -= 10;
                }

                else if(vkCode == VK_DOWN){
                    YOffset += 10;
                }

                else if(vkCode == VK_LEFT){
                    XOffset -= 10;
                    OutputDebugStringA("Left Button :");
                    if(WasDown){                    
                        OutputDebugStringA(" Was Down");
                    }
                    OutputDebugStringA("\n");
                }

                else if(vkCode == VK_RIGHT){
                    XOffset += 10;
                }                
            }
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
            DeviceContext = BeginPaint(Window, &Paint);

            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            
            int width = Paint.rcPaint.right - Paint.rcPaint.left;
            int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            GetWindowDimension(Window);
            // local_persist DWORD Operation = WHITENESS;

            // if (Operation == WHITENESS){
            //     Operation = BLACKNESS;
            // }else {
            //     Operation = WHITENESS;
            // }
            
            Win32DisplayBufferWindow(DeviceContext, &ClientRect, &BackBuffer, X, Y, width, height);
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
        
        Window = CreateWindowExA(
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
        
        if(Window){
            Running = true;
            while(Running){
                MSG Message;
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)){
                    if (Message.message == WM_QUIT){
                        Running = false;
                    }
                    DispatchMessage(&Message);
                    TranslateMessage(&Message);
                }
                
                // NOTE: The update window function must afoot outside the getting
                // message block and inside the running block
                for(DWORD DeviceIndex{0}; DeviceIndex < XUSER_MAX_COUNT;
                    DeviceIndex++)
                {
                    XINPUT_STATE ControllerState;
                    
                    if(XinputGetState_(DeviceIndex, &ControllerState) == ERROR_SUCCESS){
                        // NOTE: The controller is plugged in
                        XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;
                                               
                        bool Up = (Pad->wButtons &XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad->wButtons &XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad->wButtons &XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad->wButtons &XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start = (Pad->wButtons &XINPUT_GAMEPAD_START);
                        bool Back = (Pad->wButtons &XINPUT_GAMEPAD_BACK);
                        bool LThumb = (Pad->wButtons &XINPUT_GAMEPAD_LEFT_THUMB);
                        bool RThumb = (Pad->wButtons &XINPUT_GAMEPAD_RIGHT_THUMB);
                        bool LShoulder = (Pad->wButtons &XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RShoulder = (Pad->wButtons &XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool B = (Pad->wButtons &XINPUT_GAMEPAD_B);
                        bool A = (Pad->wButtons &XINPUT_GAMEPAD_A);
                        bool X = (Pad->wButtons &XINPUT_GAMEPAD_X);
                        bool Y = (Pad->wButtons &XINPUT_GAMEPAD_Y);

                        int16 StickX = Pad->sThumbLX;
                        int16 StickY = Pad->sThumbLY;

                        // if (Up){
                        //     YOffset -= 4;
                        //     OutputDebugStringA("Control Pad Up button triggered\n");
                        // }
                        
                    } else {
                        // NOTE: The controller is not available
                    };
                    
                }
                XINPUT_VIBRATION Vibration;
                Vibration.wLeftMotorSpeed = 350;
                Vibration.wRightMotorSpeed = 350;
                XInputSetState(0, &Vibration);
                RenderSplendidGradient(&BackBuffer, XOffset, YOffset);
                DeviceContext = GetDC(Window);

                GetWindowDimension(Window);
                    
                Win32DisplayBufferWindow(DeviceContext, &ClientRect, &BackBuffer, 0, 0, Dimens.Width, Dimens.Height);
                ReleaseDC(Window, DeviceContext);
                // TODO: Somehow the function didn't receive the increase offset var
                // to create the animation and somehow there is only one color that is blue
                if(Message.message != WM_KEYDOWN && Message.message != WM_KEYUP)
                {
                    XOffset++;
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
