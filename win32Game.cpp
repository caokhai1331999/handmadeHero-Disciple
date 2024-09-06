/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
/*
  TODO: THIS IS NOT A FINAL PLATFORM LAYER
   - Saved the game location
   - Get a handle to our own executable file
   - Asset loading path
   - Threading (launch a thread)
   - Raw Input (Support multiple keyboards)
   - Sleep/TimeBeginPeriod
   - ClipCursor() (for multiple monitors)
   - FullScreen Support
   - WM_SetCursor ( Control Cursor Visibility)
   - QueryCancelAutoplay
   - WM_ActivateApp ( For when we are not active application)
   - Blit speed improvement (using BitBlt)
   - Hardware Acceleration (OpenGl or Direct3D or Both)
   - Get Keyboard layout (For French layout, international WASD support)

   Just a partial list if you want to get the game in a complete shipping state
   
 */
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <Windows.h>
#include <xinput.h>
#include <DSound.h>
#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include <audioclient.h>


using namespace std;

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

typedef int16_t int16;
typedef int8_t int8;
typedef int32_t int32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef bool bool16;
typedef bool bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef float real32;
typedef double real64;

#include "win32Game.h"

// NOTE: This is all about calling the function in the Xinput.h without the noticing from the compiler
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex,XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
// NOTE: The second line will be expand out to be like this :
// typedef x_input_get_state(DWORD dwUserIndex,XINPUT_STATE *pState)
// This is to turn on the compiler strict type checking
// And to DECLARE A FUNCTION SIGNATURE AS A TYPE
// for example: x_input_get_state _XinputgetState()
X_INPUT_GET_STATE(XinputGetStateStub) {
    return (ERROR_DEVICE_NOT_CONNECTED);
// NOTE: But the rules of C does not allow this(x_input_get_state _XinputGetStateStub() {//do something;})
}
// so we use this for function pointer
global_variable x_input_get_state* XinputGetState_  = XinputGetStateStub;
// So finally we have a pointer name XinputGetState point to the function
// XinputGetStateStub(DWORD ....) which basically X_INPUT_GET_STATE() function
#define XinputGetState XinputGetState_
// This one is to replace the XinputGetState which already been called in Xinput.h
// with the XinputGetState                                                 
// ==================================================================
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex,XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XinputSetStateStub) {
    return (ERROR_DEVICE_NOT_CONNECTED);

}
global_variable x_input_set_state* XinputSetState_  = XinputSetStateStub;
#define XinputSetState XinputSetState_
// ==================================================================

// ==================================================================
// NOTE: 
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS,LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);
// ==================================================================

// ==================================================================
// NOTE: 
#define CO_CREATE_INSTANCE(name) HRESULT name(CLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, IID riid, LPVOID* Enumerator);
typedef CO_CREATE_INSTANCE (Co_Create_Instance);
// ==================================================================

// ==================================================================
// NOTE: 
#define ENUM_AUDIO_ENDPOINTS(name) HRESULT name (EDataFlow dataFlow, DWORD        dwStateMask, LPVOID  FAR * ppv);
typedef ENUM_AUDIO_ENDPOINTS (Enum_Audio_Endpoints);
// ==================================================================

#include "handmade.cpp"

global_variable bool  GlobalRunning;
global_variable HWND Window;
global_variable RECT ClientRect;
global_variable HDC DeviceContext;
// global_variable int  XOffset{0}, YOffset{0};
global_variable Win32_OffScreen_Buffer BackBuffer = {};
global_variable LPDIRECTSOUNDBUFFER GlobalSecondBuffer;

const global_variable int Height{720};
const global_variable int Width{1280};

internal void
win32LoadXInput(void) {
    HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
    if (!XInputLibrary) {
        // TODO: Do a diagnostic
        XInputLibrary = LoadLibrary("xinput1_3.dll");
    }
    // somehow it couldn't find the dll file maybe due to the function or
    // it's just not there therefore I used xinput1_4.dll instead
    if(XInputLibrary) {
        // retrieve the address of the exported function in dll file
        XinputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if (!XinputGetState) {XinputGetState = XinputGetStateStub;}
        XinputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if (!XinputSetState) {XinputSetState = XinputSetStateStub;}
    } else {
        // TODO: Do a diagnostic
    }
}

// ============================================================================
// NOTE: DONE Practice using coreaudio and multimedia api instead of directsound
//Now it's time to write real wave
internal void win32InitCoreAudioSound(HWND window, int32 SamplePerSecond, int32 SecondBufferSize) {
    // NOTE:As the mentor said I have the output the sound ahead of a frame
    // to make it work on time
    
    HMODULE CombaseapiLibrary = LoadLibraryA("combase.dll");
    HMODULE MmdeviceapiLibrary = LoadLibraryA("mmdevice.dll");

    // // NOTE: Load the library
    if (CombaseapiLibrary && MmdeviceapiLibrary) {
        // NOTE: Get access to the IMMDeviceEnumerator api through..
        //NOTE: Seem like I didn't understand shit. The GetProcAddress must work
        //To retrieve the address and I have to assigned to the pointer

        Co_Create_Instance* CoCreateInstance = (Co_Create_Instance* ) GetProcAddress(CombaseapiLibrary, "CoCreateInstance");

        const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
        const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
        IMMDeviceEnumerator* pEnumerator = nullptr;

// Initialize COM library
// NOTE: Create a IMMDeviceEnumerator instance
        if (CoCreateInstance && (SUCCEEDED(CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)pEnumerator)))) {
            // NOTE: pEnumerator is a pointer to the IMMDeviceEnumerator
            // NOTE: Get the IMMDeviceCollection api through IMMDeviceEnumerator::
            IMMDeviceCollection *ppDevicesl = nullptr;
            if (SUCCEEDED(pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &ppDevicesl)))
            {   
                IMMDevice *ppDevice = nullptr;
// NOTE: Get the IMMDevice by calling GetDevice
                if(SUCCEEDED(ppDevicesl->Item(0, &ppDevice))) {
                    // NOTE: Activate the IMMDevice
                    IAudioClient* ppInterface1;
                    if(SUCCEEDED(ppDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)ppInterface1))) {
                        // NOTE: Initialize the IMMDevice
                        WAVEFORMATEX* pFormat;
                        // NOTE: Now set the format                          
                        pFormat->wFormatTag = WAVE_FORMAT_PCM;
                        pFormat->nChannels = 2;
                        pFormat->nSamplesPerSec = SamplePerSecond;
                        pFormat->wBitsPerSample = 16;
                        // NOTE: Basic thing: Product of is result of multiplying
                        pFormat->nBlockAlign = (pFormat->nChannels * pFormat->wBitsPerSample)/8;
                        pFormat->nAvgBytesPerSec = (pFormat->nSamplesPerSec * pFormat->nBlockAlign); 
                        pFormat->cbSize = 0;

                        if (SUCCEEDED(ppInterface1->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 2, 0, pFormat, NULL))) {
                            // NOTE: Buffer that has a one second duration
                            // GetBuffer and ReleaseBuffer is correspond to the lock and unlockBuffer in the directSound one
                        } else {
                            // TODO: Do a diagnoses
                        }
                        IAudioClient* ppInterface2;
                        // If I wasn't wrong the interface stand for buffer
                        // NOTE: Then may be get buffer size
                        if(SUCCEEDED(ppDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)ppInterface2))) {
                
                        } else {
                            // TODO: Do a diagnoses                  
                        }
            
                    }else {
                        // TODO: Do a diagnoses                  
                    }          
                } else {
                    // TODO: Do a diagnoses
                }
            }else{
                // TODO: Do a diagnoses
            }
        
        } else {
            // TODO: Do a diagnostic        
        }

    }
}


// ============================================================================
internal void win32InitDSound(HWND window, int32 SamplePerSecond, int32 SecondBufferSize) {
    // NOTE:As the mentor said I have the output the sound ahead of a frame
    // to make it work on time
    
    // NOTE: Load the library
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (DSoundLibrary) {        
        // NOTE: Create the DSound object - cooperative
        direct_sound_create* DirectSoundCreate = (direct_sound_create* )            GetProcAddress(DSoundLibrary, "DirectSoundCreate");
        LPDIRECTSOUND DirectSound ;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound,
                                                             0))) {
            WAVEFORMATEX WaveFormat;
                    
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = SamplePerSecond;
            WaveFormat.wBitsPerSample = 16;
            // NOTE: Basic thing: Product of is result of multiplying
            WaveFormat.nBlockAlign = (WaveFormat.nChannels *
                                      WaveFormat.wBitsPerSample)/8;
            WaveFormat.nAvgBytesPerSec = (WaveFormat.nSamplesPerSec *
                                          WaveFormat.nBlockAlign); 
            WaveFormat.cbSize = 0;
            
            // ===============================================================
            // NOTE: Primary Buffer
            if(SUCCEEDED(DirectSound->SetCooperativeLevel(window,
                                                          DSSCL_PRIORITY))) {
                // NOTE: Little trick here to clear all the struct member to zero
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;    
                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                
                // NOTE: Create a primary buffer
                if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription,
                                                            &PrimaryBuffer, 0))) {
                    OutputDebugStringA("Primary sound buffer was create successfully/n");                    
                    BufferDescription.dwBufferBytes = 0;
                    
                    if((PrimaryBuffer->SetFormat(&WaveFormat)) == DS_OK) {
                        //NOTE: Or
                        // if(SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))) {
                        OutputDebugStringA("Primary sound buffer was set/n");                        
                        // NOTE: Start it playing
                    }else {
                        // TODO: Do a diagnostic                   
                    }
                }
                        
            } else {
                // TODO: Do a diagnostic
            }
                    
            // =========================================================

            // NOTE: Then the second one
            if(SUCCEEDED(DirectSound->SetCooperativeLevel(window,
                                                          DSSCL_PRIORITY))) {
                // NOTE: Create a secondary buffer
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = 0;
                BufferDescription.dwBufferBytes = SecondBufferSize;                    
                BufferDescription.lpwfxFormat = &WaveFormat;
                                
                if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription,
                                                            &GlobalSecondBuffer, 0))) {
                    OutputDebugStringA("Secondary sound buffer was created successfully/n");                                        
                }
                else {
                    // TODO: Do a diagnostic
                }
                
            } else {
                // TODO: Do a diagnostic
            }
            // ================================================================
        
        } else {
            // TODO: Do a diagnostic
        }
        
    } else {
        // TODO: Do a diagnostic        
    }

}

internal void Win32ClearSoundBuffer(win32_Sound_OutPut* SoundOutPut){
                    VOID* Region1;
                    DWORD Region1Size;
                    VOID* Region2;
                    DWORD Region2Size;

                    if(SUCCEEDED(GlobalSecondBuffer->Lock(0, SoundOutPut->SecondBufferSize, &Region1, &Region1Size,&Region2, &Region2Size,DSBLOCK_FROMWRITECURSOR))){
                        uint8* SampleOut = (uint8* )Region1;
                        // NOTE: Basically what we want to do is remembering where
                        // we were and how many sound we're outputting and able
                        // to lock the buffer at whatever we left off
                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region1Size;
                             SampleIndex++){                            
                            *SampleOut++ = 0;                            
                        }
                        
                        SampleOut = (uint8* )Region2;                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region2Size;
                             SampleIndex++){                            
                            *SampleOut++ = 0;
                        }                                                GlobalSecondBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);        
                    }    
}

internal void Win32FillSoundBuffer(win32_Sound_OutPut* SoundOutPut, DWORD ByteToLock, DWORD ByteToWrite, Game_Sound_OutPut* SoundSourceBuffer){
                    VOID* Region1;
                    DWORD Region1Size;
                    VOID* Region2;
                    DWORD Region2Size;
                    
                    if(SUCCEEDED(GlobalSecondBuffer->Lock(ByteToLock, ByteToWrite, &Region1, &Region1Size,&Region2, &Region2Size,DSBLOCK_FROMWRITECURSOR))){

                        DWORD Region1SampleCounts = Region1Size/SoundOutPut->BytesPerSample;
                        int16* SampleOut = (int16* )Region1;
                        int16* SourceSample = SoundSourceBuffer->Samples;
                        // NOTE: Basically what we want to do is remembering where
                        // we were and how many sound we're outputting and able
                        // to lock the buffer at whatever we left off
                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region1SampleCounts;
                             SampleIndex++){

                            // if (SquareWaveCounter){
                            //     SquareWaveCounter = SquareWavePeriod;
                            // }
                            // NOTE: This formula is to produce square wave
                            // int16 SampleValue = ((RunningSampleIndex++ /            (SquareWavePeriod/2))% 2) ? ToneVolume : -ToneVolume;
                            
                            // real32 SineValue = sinf(SoundOutPut->tsine);            
                            *SampleOut++ = *SourceSample++;
                            *SampleOut++ = *SourceSample++;
                            // --SquareWaveCounter;
                            
                            // NOTE: In order to avoid glitch instead of using
                            // sinewave and then plus it to however far we want
                            // GlobalRunning sampleindex (cause this var change over the time) we store where we are in
                            
                            // SoundOutPut->tsine += 2.0f*Pi32* 1.0f/(real32)SoundOutPut->WavePeriod;
                            ++SoundOutPut->RunningSampleIndex;
                        }
                        
                        SampleOut = (int16* )Region2;
                        DWORD Region2SampleCounts = Region2Size/SoundOutPut->BytesPerSample;                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region2SampleCounts;
                             SampleIndex++){
                            
                            // int16 SampleValue = ((RunningSampleIndex++/          (SquareWavePeriod/2))% 2) ? ToneVolume : -ToneVolume;
                            // real32 SineValue = sinf(SoundOutPut->tsine);
                            *SampleOut++ = *SourceSample++;
                            *SampleOut++ = *SourceSample++;
                            // SoundOutPut->tsine += 2.0f*Pi32* 1.0f/(real32)SoundOutPut->WavePeriod;
                            ++SoundOutPut->RunningSampleIndex;                            
                        }                                                GlobalSecondBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
                    }

}

void GetWindowDimension(HWND Window) {
    GetClientRect(Window, &ClientRect);
    Dimens.Width = ClientRect.right - ClientRect.left;
    Dimens.Height= ClientRect.bottom - ClientRect.top;
}

internal void Win32ResizeDIBSection(Win32_OffScreen_Buffer* OBuffer, int Width, int Height) {
    
    if(OBuffer->BitmapMemory) {
        VirtualFree(OBuffer->BitmapMemory, 0, MEM_RELEASE);
    }
    // NOTE: The BitmapWidth change every time we resize the window
    OBuffer->BitmapWidth = Width;
    OBuffer->BitmapHeight = Height;
    OBuffer->Pitch = OBuffer->BytesPerPixel * OBuffer->BitmapWidth *    OBuffer->BitmapHeight;
    
    int BitMapMemorySize;

    OBuffer->Bitmapinfo.bmiHeader.biSize = sizeof(OBuffer->Bitmapinfo.bmiHeader);
    OBuffer->Bitmapinfo.bmiHeader.biWidth = OBuffer->BitmapWidth;
    OBuffer->Bitmapinfo.bmiHeader.biHeight = -OBuffer->BitmapHeight;
    OBuffer->Bitmapinfo.bmiHeader.biPlanes = 1;
    OBuffer->Bitmapinfo.bmiHeader.biBitCount = 32;
    OBuffer->Bitmapinfo.bmiHeader.biCompression = BI_RGB;
        
             
    BitMapMemorySize = OBuffer->BytesPerPixel*(OBuffer->BitmapWidth*OBuffer->BitmapHeight);
    OBuffer->BitmapMemory = VirtualAlloc(0 ,BitMapMemorySize ,MEM_COMMIT, PAGE_READWRITE);
}

// NOTE: Keep in mind that try to all what you need to release back to memory
// in a total thing so that I can release it in aggregate

internal void ProcessXinputDigitalButton(DWORD XInputButtonState ,Game_Button_State* OldState ,DWORD ButtonBit, Game_Button_State* NewState){

    NewState->EndedDown = ((XInputButtonState & ButtonBit) == ButtonBit);
    NewState->HalfTransitionCount = ((OldState->EndedDown == NewState->EndedDown)? 1 : 0);
       
}

internal void Win32DisplayBufferWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, Win32_OffScreen_Buffer* OBuffer ) {
    
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
    switch(Message) {
        case WM_SIZE:
        {
            DeviceContext = GetDC(Window);
            GetWindowDimension(Window);
            // NOTE: Whenever the window is resized, this function capture the size
            // of the new window and update a new proper DIB for that
            // DIB is a table where store BIT color infor
            Win32ResizeDIBSection(&BackBuffer, Dimens.Width, Dimens.Height);
            // Win32DisplayBufferWindow(DeviceContext, Dimens.Width, Dimens.Height,  &Buffer);
            OutputDebugStringA("WM_SIZE\n");
        }break;
        
        case WM_CLOSE:
        {
            GlobalRunning = false;
            OutputDebugStringA("WM_CLOSE\n");
        }break;

        case WM_KEYDOWN:
        {            
            bool IsDown = ((Lparam &(1 << 31)) == 0);
            uint32 vkCode = Wparam;
            if(vkCode == VK_LEFT) {
                
                OutputDebugStringA("Left Button :");
                if(IsDown) {                    
                    OutputDebugStringA(" Is Down");
                }
                OutputDebugStringA("\n");
            }            
        }break;

        case WM_SYSKEYDOWN:
        {
            uint32 vkCode = Wparam;
            bool AltkeyisDown = ((Lparam &(1 << 29)) != 0);
            if((vkCode == VK_F4) && AltkeyisDown) {
                GlobalRunning = false;
            }                                        
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
            if (WasDown != IsDown) {

                if(vkCode == VK_UP) {
                    // YOffset -= 10;
                }

                else if(vkCode == VK_DOWN) {
                    // YOffset += 10;
                }

                else if(vkCode == VK_LEFT) {
                    // XOffset -= 10;
                    OutputDebugStringA("Left Button :");
                    if(WasDown) {                    
                        OutputDebugStringA(" Was Down");
                    }
                    OutputDebugStringA("\n");
                }

                else if(vkCode == VK_RIGHT) {
                    // XOffset += 10;                    
                }
                
                else if(vkCode == VK_TAB) {
                    if(SoundOutPut.hz == 128){
                        SoundOutPut.hz = 256;
                    } else if (SoundOutPut.hz == 256) {
                        SoundOutPut.hz = 512;
                    } else {
                        SoundOutPut.hz = 128;                        
                    }
                    // char Output[256];
                    // sprintf(Output, "TAB button hitted, Current Hert is: %d\n", SoundOutPut.hz);
                    SoundOutPut.WavePeriod = SoundOutPut.SamplePerSecond/SoundOutPut.hz;
                    
                    OutputDebugStringA("TAB button hitted");                  
                }
            }                
            }break;

        case WM_DESTROY:
        {
            GlobalRunning = false;
            PostQuitMessage(0);
            OutputDebugStringA("WM_DESTROY\n");            
        }break;

        
        case WM_PAINT:            
        {
            PAINTSTRUCT Paint;
            DeviceContext = BeginPaint(Window, &Paint);

            // int X = Paint.rcPaint.left;
            // int Y = Paint.rcPaint.top;
            
            // int width = Paint.rcPaint.right - Paint.rcPaint.left;
            // int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

            GetWindowDimension(Window);
            // local_persist DWORD Operation = WHITENESS;

            // if (Operation == WHITENESS) {
            //     Operation = BLACKNESS;
            // }else {
            //     Operation = WHITENESS;
            // }
            
            Win32DisplayBufferWindow(DeviceContext,Dimens.Width, Dimens.Height, &BackBuffer);
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
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceCounter(&PerfCountFrequencyResult);
    // NOTE: Actually, this the counts per second
    // TODO: Try to find out why the PerfCountFrequency is too large.
    int64 PerfCountFrequency = (int64)(PerfCountFrequencyResult.QuadPart);                
    win32LoadXInput();
    WNDCLASSA WindowClass = {};
    WindowClass.lpfnWndProc = MainWindowCallBack;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "First Game Window Class";
    Win32ResizeDIBSection(&BackBuffer, Dimens.Height, Dimens.Width);

    if(RegisterClassA(&WindowClass)) {
        
        Window = CreateWindowExA(
            // NOTE: The window didn't show up is because the first argument
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
        if(Window) {
            GlobalRunning = true; 
            //NOTE: we create a second buffer last for 2 second with

            // int16* SSamples = nullptr;
            // NOTE: Don't call _alloc in the app loop it cause bug (it doesn't clean up entirely but just barely in the function)

            
            // win32_Sound_OutPut SoundOutPut = {};
            SoundOutPut.SamplePerSecond = 48000;
            SoundOutPut.RunningSampleIndex = 0;
            // SoundOutPut.tsine = 0.0f;
            // SoundOutPut.hz = 128;
            // SoundOutPut.WavePeriod = SoundOutPut.SamplePerSecond/SoundOutPut.hz;
            SoundOutPut.LatencySampleCount = SoundOutPut.SamplePerSecond / 15;
            // SoundOutPut.SquareWaveCount = 0;
            SoundOutPut.ToneVolume = 3500;
            SoundOutPut.BytesPerSample = sizeof(int16)*2;
            // Hert(hz) is cycles per second
            SoundOutPut.SecondBufferSize = 2*SoundOutPut.BytesPerSample*SoundOutPut.SamplePerSecond;
            
            int16* SSamples = (int16* )VirtualAlloc(0 , SoundOutPut.SecondBufferSize ,MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            
            win32InitDSound(Window, SoundOutPut.SamplePerSecond, SoundOutPut.SecondBufferSize);
            Win32ClearSoundBuffer(&SoundOutPut);
            OutputDebugStringA("Sound is playing");

            LARGE_INTEGER LastCounter;
            QueryPerformanceCounter(&LastCounter);
            uint64 LastCycleCounts;
            
            Game_Input Input[2] = {};
            Game_Input* OldInput = &Input[0];
            Game_Input* NewInput = &Input[1];

            LastCycleCounts = __rdtsc();
            
            while(GlobalRunning) {
                MSG Message;
                // NOTE: This is where receiving the message to change
                // for any change in window
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)) {
                    if (Message.message == WM_QUIT) {
                        GlobalRunning = false;
                    }
                    DispatchMessage(&Message);
                    TranslateMessage(&Message);
                }

                int MaxControllerCount = XUSER_MAX_COUNT;
                if( MaxControllerCount > ArrayCount(Input->Controller)) {
                    MaxControllerCount = ArrayCount(Input->Controller);   
                }
                
                // NOTE: The update window function must afoot outside the getting
                // message block and inside the running block
                for(DWORD ControllerIndex{0}; ControllerIndex < MaxControllerCount;
                    ControllerIndex++)
                {
                    XINPUT_STATE ControllerState;
                    
                    if(XinputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) {
                        // NOTE: The controller is plugged in
                        Game_Controller_Input* Old_Controller = &OldInput->Controller[ControllerIndex];
                        Game_Controller_Input* New_Controller = &NewInput->Controller[ControllerIndex];
                        
                        XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;
                        
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> Down ,XINPUT_GAMEPAD_A, &New_Controller-> Down);
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> Right ,XINPUT_GAMEPAD_B, &New_Controller-> Right);
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> Left ,XINPUT_GAMEPAD_X, &New_Controller-> Left);
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> Up ,XINPUT_GAMEPAD_Y, &New_Controller-> Up);
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> LeftShoulder ,XINPUT_GAMEPAD_LEFT_SHOULDER, &New_Controller-> LeftShoulder);
                        ProcessXinputDigitalButton(Pad->wButtons ,&Old_Controller-> Right ,XINPUT_GAMEPAD_RIGHT_SHOULDER, &New_Controller-> RightShoulder);

                        
                        int16 StickX = Pad->sThumbLX;
                        int16 StickY = Pad->sThumbLY;
                        
                        bool B = (Pad->wButtons &XINPUT_GAMEPAD_B);
                        bool A = (Pad->wButtons &XINPUT_GAMEPAD_A);
                        bool X = (Pad->wButtons &XINPUT_GAMEPAD_X);
                        bool Y = (Pad->wButtons &XINPUT_GAMEPAD_Y);

                    } else {
                        // NOTE: The controller is not available
                    };
                        
                    
                }
                XINPUT_VIBRATION Vibration;
                Vibration.wLeftMotorSpeed = 350;
                Vibration.wRightMotorSpeed = 350;
                XinputSetState(0, &Vibration);
                
                // RenderSplendidGradient(&BackBuffer, XOffset, YOffset);

                // ===========================================================
                // NOTE: The writting cursor create data and the play one will pick
                // everyone of them and send to sound card to make sound .
                // One write one read just like a chase between a cat and a mouse.
                // Once you hit play 'cursor position right now you have to stop
                // the writting somewhere otherwise the newly date will overwrite
                // whatever the play cursor want to read

                
                // NOTE: We need to constantly ask the where we are in sound chase
                // and fill properly in the regions
                DWORD PlayCursor;
                DWORD WriteCursor;
                DWORD ByteToLock;
                DWORD ByteToWrite;
                DWORD TargetCursor;
                bool32 SoundIsValid = false ;
                if(SUCCEEDED(GlobalSecondBuffer->GetCurrentPosition(&PlayCursor,            &WriteCursor))) {
                    
                    ByteToLock = (SoundOutPut.RunningSampleIndex* SoundOutPut.BytesPerSample)% SoundOutPut.SecondBufferSize;
                    // % for the secondBufferSize is the move to wrap around the buffer
                    TargetCursor = ((PlayCursor + (SoundOutPut.LatencySampleCount * SoundOutPut.BytesPerSample)) % SoundOutPut.SecondBufferSize);

                    // TODO: Collapse these two loops
                    // The bugs is we didn't catch up the play cursor yet
                    // TODO: Change this to using a lower latency offset from the
                    // playcursor when we actually start having sound effect
                    
                    if(ByteToLock > TargetCursor){
                        // 
                        ByteToWrite = (SoundOutPut.SecondBufferSize - ByteToLock); // Region 1
                        ByteToWrite += TargetCursor;                   // Region 2
                    } else {
                        // when the requested size fit in buffer when there only region 1 is active
                        // In this case,one situation is that The ByteToWrite is 0 and wait for the next turn of the loop                        
                        ByteToWrite = TargetCursor - ByteToLock; // Region 1
                    }                    
                    SoundIsValid  = true; 
                }                

                // NOTE: this function throw this memory on the stack and I know
                // it will go away when it function is done
                Game_Sound_OutPut SoundBuffer = {};
                SoundBuffer.SamplePerSecond = SoundOutPut.SamplePerSecond;
                SoundBuffer.SampleCounts = ByteToWrite/SoundOutPut.BytesPerSample;
                SoundBuffer.Samples = nullptr;
                SoundBuffer.Samples = SSamples;
                
                Game_OffScreen_Buffer ScreenBuffer = {};
                ScreenBuffer.BitmapMemory = BackBuffer.BitmapMemory;
                ScreenBuffer.BitmapWidth = BackBuffer.BitmapWidth;
                ScreenBuffer.BitmapHeight = BackBuffer.BitmapHeight;
                ScreenBuffer.Pitch = BackBuffer.Pitch;
                GlobalSecondBuffer->Play( 0, 0, DSBPLAY_LOOPING);
                // NOTE: Don't know why compiler couldn't find this function
                // implementation after a little remove of few arguments
                
                GameUpdateAndRender(NewInput, &ScreenBuffer, &SoundBuffer);
                
                // TODO: This function just being called once
                

                if (SoundIsValid){
                    // TODO: Devle more about why I had to mod SecondBufferSize
                    // to Byte based on index to create bytetolock

                    Win32FillSoundBuffer(&SoundOutPut, ByteToLock, ByteToWrite, &SoundBuffer);
                }                                                    
                                
                // =============================================================
                if(Message.message != WM_KEYDOWN && Message.message != WM_KEYUP)
                {
                    // XOffset++;
                }
                
                DeviceContext = GetDC(Window);                                    
                // RenderSplendidGradient(&BackBuffer, XOffset, YOffset);
                Win32DisplayBufferWindow(DeviceContext, Dimens.Width, Dimens.Height, &BackBuffer);
                
                LARGE_INTEGER EndCounter;
                QueryPerformanceCounter(&EndCounter);

                uint64 EndCycleCounts;
                EndCycleCounts = __rdtsc();

                //  __rdtsc() is an intrinsict (the one which looked like a function call
                // but it actually a hint to the compiler to a specific dissembly language intstruction)
                //
                // S : Single 
                // I : Instruction
                // M : Multiple
                // D : Data
                
                
                uint64 CyclesElapsed = EndCycleCounts - LastCycleCounts;
                // NOTE: It based on the var type to decide what kind of the substraction to do
                real32 ElapsedCounter = (real32)((real32)(EndCounter.QuadPart) - (real32)(LastCounter.QuadPart));
                real32 McPerFrame = (real32)((real32)CyclesElapsed/(1000.f * 1000.f));
                real32 MsPerFrame = (real32)((1000 * (real32)ElapsedCounter) / (real32)PerfCountFrequency);
                real32 FPS = (real32)((real32)PerfCountFrequency/(real32)ElapsedCounter);
#if 0                
                char Buffer[256];
                // NOTE: The '%' is to decide the format of the next thing to print
                // for example: %d is the 32 bit integer
                sprintf(Buffer, "%f Miliseconds/Frame, %f FPS, %f Mc/f \n ", MsPerFrame, FPS, MsPerFrame);
                OutputDebugStringA(Buffer);
#endif                
                LastCounter = EndCounter;
                LastCycleCounts = EndCycleCounts;
                // MULPD -> real32 ==> 128 bits / 32 bits -> 4 real32 packs per register 
                // MULPS -> real64 ==> 128 bits / 64 bits -> 2 real32 packs per register  
                // ReleaseDC(Window, DeviceContext);

                Game_Input* Temp = NewInput;
                NewInput = OldInput; // ???? still don't understand
                OldInput = Temp;
            }
            
        }else{
            // TODO: Logging
        }
    } else {
        // TODO: Logging
    }   
    return (0);
}
