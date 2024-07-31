/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include <iostream>
#include <cmath>
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

typedef bool bool16;
typedef bool bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef float real32;
typedef double real64;

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


global_variable bool  Running;
global_variable HWND Window;
global_variable RECT ClientRect;
global_variable HDC DeviceContext;
global_variable int  XOffset{0}, YOffset{0};
global_variable LPDIRECTSOUNDBUFFER GlobalSecondBuffer;

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


struct win32_Sound_OutPut{
    int SamplePerSecond;
    int BytesPerSample;
    // Hert(hz) is cycles per second
    int32 SecondBufferSize;
    uint32 RunningSampleIndex;
    int hz;
    int WavePeriod;
    int SquareWaveCount;
    int ToneVolume;
    // Sample per cycle is SquareWave Period    
};

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
            //         WAVEFORMATEX WaveFormat;                                
        
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

internal void Win32FillSoundBuffer(win32_Sound_OutPut* SoundOutPut, DWORD ByteToLock, DWORD ByteToWrite){
                    VOID* Region1;
                    DWORD Region1Size;
                    VOID* Region2;
                    DWORD Region2Size;
                    
                    if(SUCCEEDED(GlobalSecondBuffer->Lock(ByteToLock, ByteToWrite, &Region1, &Region1Size,&Region2, &Region2Size,DSBLOCK_FROMWRITECURSOR))){
                      
                        int16* SampleOut = (int16* )Region1;
                        DWORD Region1SampleCount = Region1Size/SoundOutPut->BytesPerSample;

                        // NOTE: Basically what we want to do is remembering where
                        // we were and how many sound we're outputting and able
                        // to lock the buffer at whatever we left off
                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region1SampleCount;
                             SampleIndex++){
                            // if (SquareWaveCounter){
                            //     SquareWaveCounter = SquareWavePeriod;
                            // }
                            // NOTE: This formula is to produce square wave
                            // int16 SampleValue = ((RunningSampleIndex++ /            (SquareWavePeriod/2))% 2) ? ToneVolume : -ToneVolume;
                            real32 t = 2.0f*Pi32* (real32)SoundOutPut->RunningSampleIndex/(real32)SoundOutPut->WavePeriod;
                            real32 SineValue = sinf(t);            
                            int16 SampleValue = (int16)(SineValue * SoundOutPut->ToneVolume);
                            *SampleOut++ = SampleValue;
                            *SampleOut++ = SampleValue;
                            // --SquareWaveCounter;
                            ++SoundOutPut->RunningSampleIndex;
                        }
                        
                        SampleOut = (int16* )Region2;                        
                        DWORD Region2SampleCount = Region2Size/SoundOutPut->BytesPerSample;                        
                        for (DWORD SampleIndex{0};
                             SampleIndex < Region2SampleCount;
                             SampleIndex++){
                            
                            // int16 SampleValue = ((RunningSampleIndex++/          (SquareWavePeriod/2))% 2) ? ToneVolume : -ToneVolume;
                            real32 t = 2.0f*Pi32* (real32)SoundOutPut->RunningSampleIndex/(real32)SoundOutPut->WavePeriod;                            
                            real32 SineValue = sinf(t);            
                            int16 SampleValue = (int16)(SineValue * SoundOutPut->ToneVolume);

                            *SampleOut++ = SampleValue;
                            *SampleOut++ = SampleValue;                         ++SoundOutPut->RunningSampleIndex;                            
                        }                                                GlobalSecondBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
                    }

}

void GetWindowDimension(HWND Window) {
    GetClientRect(Window, &ClientRect);
    Dimens.Width = ClientRect.right - ClientRect.left;
    Dimens.Height= ClientRect.bottom - ClientRect.top;
}

// TODO: Now time to move on to input/output section 

void RenderSplendidGradient(Win32_Offscreen_Buffer* OBuffer,int XOffset, int YOffset) {
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    int Width = OBuffer->BitmapWidth;
    int Height = OBuffer->BitmapHeight;    
    int Pitch = OBuffer->BytesPerPixel*OBuffer->BitmapWidth;
    uint8* Row = (uint8 *)OBuffer->BitmapMemory;
    
    for (int Y{0}; Y < Height; Y++) {
        uint32* Pixel = (uint32 *)Row;
        for(int X{0}; X < Width; X++) {
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

internal void Win32ResizeDIBSection(Win32_Offscreen_Buffer* OBuffer, int Width, int Height) {
    // Create a storage for memory first
    // using virtualAlloc
    // then store bitmap in it
    
    if(OBuffer->BitmapMemory) {
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

internal void Win32DisplayBufferWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, Win32_Offscreen_Buffer* OBuffer ) {
    // the Source and the Destination rectangle means
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
            // NOTE: Maybe the underlying cause of the unchanging bit is lay on the
            // update the dc and the window resize section
            DeviceContext = GetDC(Window);
            GetWindowDimension(Window);
            // RECT ClientRect;
            // NOTE: Whenever the window is resized, this function capture the size
            // of the new window and update a new proper DIB for that
            // DIB is a table where store BIT color infor
            Win32ResizeDIBSection(&BackBuffer, Dimens.Width, Dimens.Height);
            Win32DisplayBufferWindow(DeviceContext, Dimens.Width, Dimens.Height,  &BackBuffer);
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
                Running = false;
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
                    YOffset -= 10;
                }

                else if(vkCode == VK_DOWN) {
                    YOffset += 10;
                }

                else if(vkCode == VK_LEFT) {
                    XOffset -= 10;
                    OutputDebugStringA("Left Button :");
                    if(WasDown) {                    
                        OutputDebugStringA(" Was Down");
                    }
                    OutputDebugStringA("\n");
                }

                else if(vkCode == VK_RIGHT) {
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
    win32LoadXInput();
    WNDCLASSA WindowClass = {};
    // NOTE: This is where receiving the message to change
    // for any change in window
    WindowClass.lpfnWndProc = MainWindowCallBack;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "First Game Window Class";
  
    if(RegisterClassA(&WindowClass)) {
        
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
        
        if(Window) {
            Running = true;            
            //NOTE: we create a second buffer last for 2 second with
            // sample per second is 4800 and byte per sample is sizeof(int16)

            win32_Sound_OutPut SoundOutPut = {};
            SoundOutPut.SamplePerSecond = 48000;
            SoundOutPut.RunningSampleIndex = 0;
            SoundOutPut.hz = 256;
            SoundOutPut.WavePeriod = SoundOutPut.SamplePerSecond/SoundOutPut.hz;
            // SoundOutPut.SquareWaveCount = 0;
            SoundOutPut.ToneVolume = 3500;
            SoundOutPut.BytesPerSample = sizeof(int16)*2;
            // Hert(hz) is cycles per second
            SoundOutPut.SecondBufferSize = 2*SoundOutPut.BytesPerSample*SoundOutPut.SamplePerSecond;
            //NOTE: SomeHow the RunningSampleIndex failed to init as 0            
            win32InitDSound(Window, SoundOutPut.SamplePerSecond, SoundOutPut.SecondBufferSize);
            Win32FillSoundBuffer(&SoundOutPut, 0, SoundOutPut.SecondBufferSize);
            GlobalSecondBuffer->Play( 0, 0, DSBPLAY_LOOPING);
            OutputDebugStringA("Sound is playing");
            
            // bool32 SoundIsPlaying = false;                                        
            // NOTE: I don't know should I do this if else stuff or not!!!!
           
            while(Running) {
                MSG Message;
                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)) {
                    if (Message.message == WM_QUIT) {
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
                    
                    if(XinputGetState(DeviceIndex, &ControllerState) == ERROR_SUCCESS) {
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

                        XOffset = StickX >> 12;
                        YOffset = StickY >> 12;
                        // if (Up) {
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
                XinputSetState(0, &Vibration);
                RenderSplendidGradient(&BackBuffer, XOffset, YOffset);

                // ===========================================================
                // NOTE: The writting cursor create data and the play one will pick
                // everyone of them and send to sound card to make sound .
                // One write one read just like a chase between a cat and a mouse.
                // Once you hit play 'cursor position right now you have to stop
                // the writting somewhere otherwise the newly date will overwrite
                // whatever the play cursor want to read

                // NOTE: The purpose of the lock function is to create a safe area
                //where the previous written data is proctected from overwriting
                //play buffer one
                
                // NOTE: This part create manually the sound bit by bit not loading them from any file source                

                DWORD PlayCursor;
                DWORD WriteCursor;

                // NOTE: Still don't know why this happened
                if(SUCCEEDED(GlobalSecondBuffer->GetCurrentPosition(&PlayCursor,            &WriteCursor))) {
                    DWORD ByteToLock = (SoundOutPut.RunningSampleIndex* SoundOutPut.BytesPerSample)% SoundOutPut.SecondBufferSize;
                
                    DWORD ByteToWrite;
                    // TODO: Collapse these two loops
                    // The bugs is we didn't catch up the play cursor yet
                    if (ByteToLock == PlayCursor){
                        // BUGS Lies here
                        // SomeHow ByteToWrite always equal to SecondBufferSize
                        // Cause the compare operator happens before PlayCursor
                        // increment
                        ByteToWrite = 0; // Glitching problem found
                        // Uninitialized ByteToWrite var
                    }
                    else if(ByteToLock > PlayCursor){
                        // Exclude out the lock area and then increment
                        // by play cursor
                        ByteToWrite = (SoundOutPut.SecondBufferSize - ByteToLock); // Region 1
                        ByteToWrite += PlayCursor;                   // Region 2
                    } else {
                        // In this case, The ByteToWrite is 0 and wait for the
                        // next turn of the loop
                        ByteToWrite = PlayCursor - ByteToLock;
                    }
                    Win32FillSoundBuffer(&SoundOutPut, ByteToLock, ByteToWrite);
                }                                                    
                                
                // =============================================================
                if(Message.message != WM_KEYDOWN && Message.message != WM_KEYUP)
                {
                    XOffset++;
                }
                
                DeviceContext = GetDC(Window);                                    
                GetWindowDimension(Window);                
                Win32DisplayBufferWindow(DeviceContext, Dimens.Width, Dimens.Height, &BackBuffer);
                ReleaseDC(Window, DeviceContext);
                
                // TODO: Somehow the function didn't receive the increase offset var
                // to create the animation and somehow there is only one color that is blue
            }
            
        }else{
            // TODO: Logging
        }
    } else {
        // TODO: Logging
    }   
    return (0);
}
