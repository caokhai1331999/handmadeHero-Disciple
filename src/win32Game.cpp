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

#include "win32Game.h"

void OpenConsole() {
    AllocConsole();                             // Allocate a new console
    freopen("CONOUT$", "w", stdout);            // Redirect printf to console
    // freopen("CONOUT$", "w", stderr);            // Redirect stderr
    // freopen("CONIN$", "r", stdin);              // Redirect stdin (optional)
}

void GetWindowDimension(Platform_Properties* Game_Platform) {
    GetClientRect(Game_Platform->Window, &(Game_Platform->ClientRect));
    Dimens.Width = Game_Platform->ClientRect.right - Game_Platform->ClientRect.left;
    Dimens.Height= Game_Platform->ClientRect.bottom - Game_Platform->ClientRect.top;
    Game_Platform->BitmapWidth = Dimens.Width;
    Game_Platform->BitmapHeight = Dimens.Height;
}

void Win32ResizeDIBSection(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* OBuffer, int Width, int Height) {
\
    if(Game_Platform->BitmapMemoryForDirectBlit) {
        VirtualFree(Game_Platform->BitmapMemoryForDirectBlit, 0, MEM_RELEASE);
    }
    // NOTE: The BitmapWidth change every time we resize the window
    Game_Platform->BitmapWidth = Width;
    Game_Platform->BitmapHeight = Height;
    //OBuffer->Pitch = OBuffer->BytesPerPixel * Game_Platform->BitmapWidth;
    Game_Platform->Pitch = 4 * Game_Platform->BitmapWidth;
    
    Game_Platform->Bitmapinfo.bmiHeader.biSize = sizeof(Game_Platform->Bitmapinfo.bmiHeader);
    Game_Platform->Bitmapinfo.bmiHeader.biWidth = Game_Platform->BitmapWidth;
    Game_Platform->Bitmapinfo.bmiHeader.biHeight = -Game_Platform->BitmapHeight;
    Game_Platform->Bitmapinfo.bmiHeader.biPlanes = 1;
    Game_Platform->Bitmapinfo.bmiHeader.biBitCount = 32;
    Game_Platform->Bitmapinfo.bmiHeader.biCompression = BI_RGB;
                     
    Game_Platform->BitmapMemorySize = OBuffer->BytesPerPixel*(Game_Platform->BitmapWidth*Game_Platform->BitmapHeight);

    if(!Game_Platform->GLImageRendered){
        if(Game_Platform->BitmapMemory) {
            VirtualFree(Game_Platform->BitmapMemory, 0, MEM_RELEASE);
        }
        Game_Platform->BitmapMemory = VirtualAlloc(0 ,Game_Platform->BitmapMemorySize ,MEM_COMMIT, PAGE_READWRITE);
    }

    Game_Platform->BitmapMemoryForDirectBlit = VirtualAlloc(0 ,Game_Platform->BitmapMemorySize ,MEM_COMMIT, PAGE_READWRITE);

}

//void RenderSplendidGradient(Win32_Front_Buffer* OBuffer, imagee_content* BMPContent, int XOffset, int YOffset) {

void RenderSplendidGradient(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* OBuffer, Win32_Front_Buffer* FBuffer, imagee_content* BMPContent) {
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    int32 BlitWidth =  BMPContent->Width;
    int32 BlitHeight = BMPContent->Height;
    int32 WidthOffset = 0;
    int32 ImagePitch = 4 * BlitWidth;    

    int32 Height;
    int32 Width;

    if (OBuffer!=NULL && Game_Platform!=NULL){
        Height = Game_Platform->BitmapHeight;
        Width =  Game_Platform->BitmapWidth;
    } else {
        Height = FBuffer->BitmapHeight;
        Width =  FBuffer->BitmapWidth;        
    }
    
    //BUG right here
    if(BlitWidth > Width){
        WidthOffset = BlitWidth - Width;
        BlitWidth = Width;
    }

    if(BlitHeight > Height){
        BlitHeight = Height;
    }
    
    // We take memory from BitmapMemory of main Bufer to write on it
    uint8* Row;
    uint8* DirectRow;

    if (OBuffer!=NULL){
        Row = ((uint8 *)Game_Platform->BitmapMemory);
    } else {
        Row = ((uint8 *)FBuffer->BitmapMemory);
    }

    if (OBuffer!=NULL){
        if(Game_Platform->BitmapMemoryForDirectBlit != NULL){
            DirectRow = ((uint8 *)Game_Platform->BitmapMemoryForDirectBlit);
        } else {
            printf("Bitmap Memory for direct blit is empty\n");
        }
    } else {
        DirectRow = ((uint8 *)FBuffer->BitmapMemoryForDirectBlit);
    }
    
    //Change the image row order upside down
    //uint8* imageRow = (uint8*)BMPContent->ImageContent;
    //uint8* imageRowForDirectBlit = (uint8*)BMPContent->ImageContent;
//// ???? What todo if the image is bigger than the 
    //imageRowForDirectBlit += 4*((BlitHeight - 1) * BlitWidth);

    //for (int32 Y{0}; Y < Height; Y++) {
        //uint32* DirectPixel = (uint32 *)DirectRow;        
        //if(Y == BlitHeight){
            //imageRowForDirectBlit += 4*((BlitHeight - 1) * BlitWidth);            
        //}
        //uint32* imagePixelForDirect = (uint32* )imageRowForDirectBlit;

        //for(int32 X{0}; X < Width; X++) {

//NOTE: For Gradient version
    for (int Y = 0; Y < Game_Platform->BitmapHeight; Y++) {
        uint32* Pixel = (uint32 *)Row;
        //uint32* imagePixel = (uint32* )imageRow;
        for(int X = 0; X < Game_Platform->BitmapWidth; X++) {

            uint8 Blue = X;
            uint8 Green = Y;

            //NOTE: AA RR GG BB()
            // Because I limit the size of Pixels so it can not add Green color to its storage
            *Pixel++ = (Green << 8|Blue);
            // NOTE: How to turn pixels order from bottom up to top down
            // Pixel :
            // ImagePointer :
            // Why Pixel appear in uint8 not uint32
            //if(X >= BlitWidth){
                //*DirectPixel++ = 0xffffffff;
            //} else {
                //*DirectPixel++ = *imagePixelForDirect++;
            //}
        }
        // Instead of manually move row pointer every y axis (by add it to the pitch)
        // we just need to reuse the Pixel pointer pass it to row where it was already moved
        // And this is for passing directly to the window (RIGHT)
        //imageRowForDirectBlit-=ImagePitch;
// For direct blit
        //if(OBuffer != NULL){
            //DirectRow+=OBuffer->Pitch;
        //} else {
            //DirectRow+=FBuffer->Pitch;            
        //}
        ////NOTE: This incidentally produce right pixel order
    }

    //if(!OBuffer->GLImageRendered){    
    //for (int32 Y{0}; Y < Height; Y++) {
        //uint32* Pixel = (uint32 *)Row;
        //uint32* imagePixel = (uint32* )imageRow;
        //for(int32 X{0}; X < Width; X++) {
            //*Pixel++ = *imagePixel++;
        //}
        //// NOTE: This order is for passing to OpenGL
        //imageRow+=ImagePitch;
//For GL
        //if(OBuffer != NULL){
            //Row+=OBuffer->Pitch;
        //} else {
            //Row+=FBuffer->Pitch;            
        //}
        ////NOTE: This incidentally produce right pixel order
        //OBuffer->GLImageRendered = true;
    //}
    //}

}

void ShowGlyphs(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* OBuffer, Glyph_Map* map){
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    for(Glyph_Property* const &glyph: map->Glyph_list){

        int32 BlitWidth =  glyph->w;
        int32 BlitHeight = glyph->h;

        int32 WidthOffset = 0;
        int32 ImagePitch = 4 * BlitWidth;    

        int32 Height;
        int32 Width;

        if (OBuffer!=NULL){
            Height = Game_Platform->BitmapHeight;
            Width =  Game_Platform->BitmapWidth;
        }
    
        //BUG right here
        if(BlitWidth > Width){
            WidthOffset = BlitWidth - Width;
            BlitWidth = Width;
        }

        if(BlitHeight > Height){
            BlitHeight = Height;
        }
    
        // We take memory from BitmapMemory of main Bufer to write on it
        uint8* Row;
        uint8* DirectRow;

        if (OBuffer!=NULL){
            Row = ((uint8 *)Game_Platform->BitmapMemory);
        }

        if (OBuffer!=NULL){
            if(Game_Platform->BitmapMemoryForDirectBlit != NULL){
                DirectRow = ((uint8 *)Game_Platform->BitmapMemoryForDirectBlit);
            } else {
                printf("Bitmap Memory for direct blit is empty\n");
            }
        }
    
        //Change the image row order upside down
        //uint8* imageRow = (uint8*)glyph->bitmap;
        uint8* imageRowForDirectBlit = (uint8*)glyph->upside_down_bitmap;
//// ???? What todo if the image is bigger than the 
                               imageRowForDirectBlit += 4*((BlitHeight - 1) * BlitWidth);

        for (int32 Y{0}; Y < Height; Y++) {
            uint32* DirectPixel = (uint32 *)DirectRow;        
            if(Y == BlitHeight){
                imageRowForDirectBlit += 4*((BlitHeight - 1) * BlitWidth);            
            }
            uint32* imagePixelForDirect = (uint32* )imageRowForDirectBlit;

            for(int32 X{0}; X < Width; X++) {
            
                // Why Pixel appear in uint8 not uint32
                if(X >= BlitWidth){
                    *DirectPixel++ = 0xffffffff;
                } else {
                    *DirectPixel++ = *imagePixelForDirect--;
                }
            }
        }
    }
}


void Win32DisplayBufferWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* OBuffer ) {

    //int ScannedLine;

    StretchDIBits(
        DeviceContext,
        0,0,Game_Platform->BitmapWidth, Game_Platform->BitmapHeight, // Source rectangle
        0,0,WindowWidth, WindowHeight,                 // Destination Rectangle
        // const VOID* lpBits,
        Game_Platform->BitmapMemoryForDirectBlit,
        &Game_Platform->Bitmapinfo,
        DIB_RGB_COLORS,
        SRCCOPY);    

    //Game_Platform->BitmapMemoryForDirectBlit!=NULL?printf("Memory for direct blit was not NULL but screen still being black\n"):printf("Memory Pool is empty\n");
    
/*
     Why Flickering???
     CAUSE: the pixel drawing fx in the window/app loop
*/
}

bool InitOpenGL(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* OBuffer, Win32_Front_Buffer* FBuffer, imagee_content* bmpContent){
    // first device context gotten from current window
    // printf("Start to init OpenGL\n");
        // Create the pixel format features

// Dummy window and context here =====================================
// Source - https://stackoverflow.com/q/45937728
// Posted by D.G. Redd, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-10, License - CC BY-SA 3.0

    test_platform.reloadGLFuncPointer = reload_gl_function_pointer;
    assert(test_platform.reloadGLFuncPointer);
    
    GetWindowDimension(Game_Platform);
    Win32ResizeDIBSection(Game_Platform, OBuffer, Dimens.Width, Dimens.Height);
    
WNDCLASSW wcDummy = {0};
wcDummy.lpfnWndProc     = +[](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){return DefWindowProcW(hWnd, message, wParam, lParam);};
wcDummy.hInstance       = GetModuleHandle(0);
wcDummy.hbrBackground   = (HBRUSH)(COLOR_BACKGROUND);
wcDummy.lpszClassName   = L"Dummy";
wcDummy.style           = CS_OWNDC;

if(!RegisterClassW(&wcDummy))
{
  DWORD errorCode = GetLastError();
  char buffer[256] = {};
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer,
                 sizeof(buffer), NULL);
  printf("%s\n", buffer);
  return false;
}

std::wstring title = L"dummy window for wgl beforehand init";

HWND windowDummy = CreateWindowW(wcDummy.lpszClassName, title.c_str(), WS_DISABLED, 0, 0, 640, 480, 0, 0, wcDummy.hInstance, NULL);

if(windowDummy == NULL)
{
  printf("dummy Window for wgl init is NULL\n");
  DWORD errorCode = GetLastError();
  char buffer[256] = {};
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer,
                 sizeof(buffer), NULL);
  printf("%s\n", buffer);
}

//if (!GetProcessId(NULL)) {
  //ErrorExit();
  //return false;
//}



PIXELFORMATDESCRIPTOR pfdDummy =
{
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    24,
    8,
    0, 0, 0, 0, 0, 0
};

HDC dummyDrawingContext = GetDC(windowDummy);

INT pixelFormatDummy = ChoosePixelFormat(dummyDrawingContext, &pfdDummy);
SetPixelFormat(dummyDrawingContext, pixelFormatDummy, &pfdDummy);

Game_Platform->glData.defaultContext = wglCreateContext(dummyDrawingContext);

wglMakeCurrent(dummyDrawingContext, Game_Platform->glData.defaultContext);

//Succeed make enable wgl
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;

if(wglGetCurrentContext() != NULL)
{

wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress(
    "wglGetExtensionsStringARB");

}
else
    return false;

//if (wglGetExtensionsStringARB != nullptr) {
  //GLint64 numExtensions;
  //glGetInteger64v(GL_NUM_EXTENSIONS, &numExtensions);
  //std::cout << "Available Extensions:\n";
  //for (GLint64 i = 0; i < numExtensions; ++i) {
    //const GLubyte *extensionName = glGetStringi(GL_EXTENSIONS, i);
//
    //std::cout << "\n\t" << (const char *)extensionName;
//
    //if (std::strcmp((const char *)extensionName, "WGL_ARB_create_context") ==
        //0) {
wglCreateContextAttribsARB =    
          (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress(
              "wglCreateContextAttribsARB");
    //}
  //}
//} else
// return false;
// Delete dummy here
if (wglCreateContextAttribsARB) {
    printf("Succeed get wglCreateContextAttribsARB function pointer\n");
    wglDeleteContext(Game_Platform->glData.defaultContext);
    ReleaseDC(windowDummy, dummyDrawingContext);
    DestroyWindow(windowDummy);
} else {
    printf("wglCreateContextAttribsARB is NULL\n");    
}

// succeed enable wgl extension
// Working window and context

HDC windowDC = GetDC(Game_Platform->Window);
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
        desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
        desiredPixelFormat.nVersion =  1;
        desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
        desiredPixelFormat.cColorBits = 32;
        desiredPixelFormat.cAlphaBits = 8;
        desiredPixelFormat.cDepthBits = 24;
        desiredPixelFormat.cStencilBits = 8;
        desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;

        // Assign to an index
        int suggestedPixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);

        // Create a format from that index
        PIXELFORMATDESCRIPTOR suggestedPixelFormat;
        bool32 initTexture = false;
        DescribePixelFormat(
            windowDC,
            suggestedPixelFormatIndex,
            sizeof(suggestedPixelFormat),
            &suggestedPixelFormat);

        // Then set pixel format
        if ( SetPixelFormat(
                 windowDC,
                 suggestedPixelFormatIndex,
                 &suggestedPixelFormat)
             )
        {

            // Then init it
            // Time to create texture
            // Create texture
            // Bind it
            // and set some parameter                

            // NOTE: Failed right at the beginning
            bool success = false;
            // Next create OPENGL context
             //Game_Platform->glData.openglRC = wglCreateContext(windowDC);

            const GLint attribList[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                        3,
                                        WGL_CONTEXT_MINOR_VERSION_ARB,
                                        3,
                                        WGL_CONTEXT_PROFILE_MASK_ARB,
                                        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                        0};

            Game_Platform->glData.openglRC = wglCreateContextAttribsARB(windowDC, NULL, attribList);

            //printf("%p\n",GetProcAddress(LoadLibraryA("opengl32.dll"), "glGetString"));

            //======================================================================
            if(wglMakeCurrent(windowDC, Game_Platform->glData.openglRC)){
        // NOTE: Failed right at the beginning
        // success = gladLoadGL((GLADloadfunc)wglGetProcAddress);

#ifdef ON_LITTLE_BEAST
                success = gladLoadGLLoader((GLADloadproc)GetAnyGLFuncAddress);
#else 
                success = gladLoadGLLoader((GLADloadproc)wglGetProcAddress);
#endif
                assert(success);
                
        if(success)
                {
                    //OpenConsole();
                    //printf("GLAD load successfully\n");
                    printf("VERSION: %s", glGetString(GL_VERSION));
                    printf("Renderer: %s\n", glGetString(GL_RENDERER));;

                    const float Vertices[] = {
                   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,// 0
                    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,// 1
                    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                   
                   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,// 2
                   -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                   -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,// 3
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,// 4
                    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,// 5
                    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
                };

                static const GLfloat fullvertices[] = {
                    //BACK FACE
                   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,// one stride  
                    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,     
                   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                    //FRONT FACE
                   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
                    // LEFT FACE
                   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                    // RIGHT FACE
                    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                    // BOTTOM FACE
                   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
                    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
                    // TOP FACE
                   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
                    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
                    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,    
                   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f 
                };

                static const unsigned int cubeIndices[] = {
                    0, 1, 2, 2, 4, 0,//0
                    6, 7, 8, 7, 6, 11,
                    12, 13, 14, 13, 12, 17,//2
                    18, 19, 20, 20, 22, 18,
                    24, 25, 26, 25, 24, 29,//4
                    30, 31, 32, 32, 34, 30
                };

                
                static const unsigned int planeIndices[] = {
//Even though the vertex 1, 0 will be reused but we have to feed them name for opengl just like this
                    0, 1, 2, 2, 4, 0
                };

                static const float PlaneVertices[] = {
                    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
                  // x,    y,     z
/*
                    1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
                   -1.0f, -1.0f,  1.0f,  1.0f, 1.0f,
                    1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
                                       
                    1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
                   -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
                   -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,*/

                    1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
                   -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 
                   -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,

                   -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,                   
                    1.0f,  1.0f, 1.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 1.0f, 0.0f
                };
                
                static const GLfloat g_color_buffer_data[] = {
                    0.583f,  0.771f,  0.014f,
                    0.609f,  0.115f,  0.436f,
                    0.327f,  0.483f,  0.844f,
                    0.822f,  0.569f,  0.201f,
                    0.435f,  0.602f,  0.223f,
                    0.310f,  0.747f,  0.185f,
                    0.597f,  0.770f,  0.761f,
                    0.559f,  0.436f,  0.730f,
                    0.359f,  0.583f,  0.152f,
                    0.483f,  0.596f,  0.789f,
                    0.559f,  0.861f,  0.639f,
                    0.195f,  0.548f,  0.859f,
                    0.014f,  0.184f,  0.576f,
                    0.771f,  0.328f,  0.970f,
                    0.406f,  0.615f,  0.116f,
                    0.676f,  0.977f,  0.133f,
                    0.971f,  0.572f,  0.833f,
                    0.140f,  0.616f,  0.489f,
                    0.997f,  0.513f,  0.064f,
                    0.945f,  0.719f,  0.592f,
                    0.543f,  0.021f,  0.978f,
                    0.279f,  0.317f,  0.505f,
                    0.167f,  0.620f,  0.077f,
                    0.347f,  0.857f,  0.137f,
                    0.055f,  0.953f,  0.042f,
                    0.714f,  0.505f,  0.345f,
                    0.783f,  0.290f,  0.734f,
                    0.722f,  0.645f,  0.174f,
                    0.302f,  0.455f,  0.848f,
                    0.225f,  0.587f,  0.040f,
                    0.517f,  0.713f,  0.338f,
                    0.053f,  0.959f,  0.120f,
                    0.393f,  0.621f,  0.362f,
                    0.673f,  0.211f,  0.457f,
                    0.820f,  0.883f,  0.371f,
                    0.982f,  0.099f,  0.879f
                };

                glGenVertexArrays(1, &Game_Platform->glData.VAOs);
                glGenBuffers(1, &Game_Platform->glData.VBO);
                glBindVertexArray(Game_Platform->glData.VAOs);
                glBindBuffer(GL_ARRAY_BUFFER, Game_Platform->glData.VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(fullvertices), &fullvertices, GL_STATIC_DRAW);
                
                // CUBE position in NDC
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);

                //Normal
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)3);

                //TextCoord
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)6);

                // Mere Color
                glGenBuffers(1, &Game_Platform->glData.ColorVBO);
                glBindBuffer(GL_ARRAY_BUFFER, Game_Platform->glData.ColorVBO);                
                glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), &g_color_buffer_data, GL_STATIC_DRAW);

                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
                GLuint cubeIndices_;
                glGenBuffers(1, &cubeIndices_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndices_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW);

//  index, size, type, .., stride, pointer
//========================================================================
// FOR PLANE
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glGenVertexArrays(1, &Game_Platform->glData.PlaneVAOs);
                glGenBuffers(1, &Game_Platform->glData.PlaneVBO);

                glBindVertexArray(Game_Platform->glData.PlaneVAOs);       
                glBindBuffer(GL_ARRAY_BUFFER, Game_Platform->glData.PlaneVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float)*sizeof(PlaneVertices), &PlaneVertices, GL_STATIC_DRAW);

                 //Position
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

                 //Normal
                //glEnableVertexAttribArray(1);
                //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
                //
                 //TEXTURE COOR
                glEnableVertexAttribArray(1);                
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
//
                //Color
                glBindBuffer(GL_ARRAY_BUFFER, Game_Platform->glData.ColorVBO);                
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

                GLuint planeindices;
                glGenBuffers(1, &planeindices);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeindices);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), &planeIndices, GL_STATIC_DRAW);

                printf("hmm... Size of Vertices:%d\n", (int)(sizeof(PlaneVertices)/sizeof(float)));

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);


                // NOTE: To here we done assigned CubeVerticles data to VAOs and VBO
                // We will call bindbuffer/vertexArray whenever before glDrawArray
// NOTE: We delve into Buffer drawing later!!!!
                //===============================================================
                
                //printf("Succeed create OpenGL Context\n");
                //Game_Platform->glData.textureHandle = (unsigned int*)malloc(sizeof(unsigned int));

                // NOTE: Found it: The temptexture is local to this fx so its
                // data and address turn to null after the fx called

                //glGenTextures(1, &Game_Platform->glData.textureHandle);
                //glBindTexture(GL_TEXTURE_2D, Game_Platform->glData.textureHandle );
// Game_Platform->glData.textureHandle is the name of the texture
                //last argument This is where point to the image data
                // Why this doesn't work
                if(FBuffer->BitmapHeight != Game_Platform->BitmapHeight){
                    FBuffer->BitmapHeight = Game_Platform->BitmapHeight;
                }
                if(FBuffer->BitmapWidth != Game_Platform->BitmapWidth){
                    FBuffer->BitmapWidth = Game_Platform->BitmapWidth;
                }
                glViewport(0, 0, FBuffer->BitmapWidth, FBuffer->BitmapHeight);

                // Deprecated
 //glEnable(GL_TEXTURE_2D);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);

                glCullFace(GL_FRONT);
                glDepthFunc(GL_LESS);
                // GL 4.3+
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                
                glClearColor(0.179f, 0.179f, 0.179f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

                const GLubyte* ver = glGetString(GL_VERSION);
                if (ver)
                    printf("OpenGL version: %s\n", ver);
                else
                  printf("glGetString(GL_VERSION) returned NULL\n");

                }
                else
                {

                    DWORD ErrorContent = GetLastError();
                    LPVOID ErrorMsgBuffer;
                    if (FormatMessage(
                            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                            FORMAT_MESSAGE_FROM_SYSTEM |
                            FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,
                            ErrorContent,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPTSTR) &ErrorMsgBuffer,
                            0, NULL) == 0){
                        printf("Failed to format message\n");
                    } else

                    {
                        printf("Could n't initialized GLAD first try, Error: %s\n", (char* )ErrorMsgBuffer);                                        

                    }                
                }

            } else {
                // TODO: Diagnostic
                printf("Failed to init OpenGl\n");            
                return false;
            };   
        } else {
            printf("Failed to init OpenGl\n");
            return false;
        }        

        glViewport(Game_Platform->ClientRect.left, Game_Platform->ClientRect.top, Game_Platform->BitmapWidth, Game_Platform->BitmapHeight);
        ReleaseDC(Game_Platform->Window, windowDC);
        return true;
}


void GameUpdateAndRender(Game_Memory* Memory, imagee_content* BMPContent ,Game_Input* Input, Game_State* State, Win32_Front_Buffer* OBuffer,  Game_Sound_OutPut* SoundBuffer, HDC DeviceContextt){

    if(!Memory->IsInitialized){
        State->Hz = 256;
         State->BlueOffset = 0;
         State->GreenOffset = 0;
    }
    
    Game_Controller_Input* Input0 = &Input->Controller[0];
    
    if(Input0->IsAnalog){
    State->Hz = (int)(128.0f*(Input0->EndX));
    State->BlueOffset = (int)(4.0f*(Input0->EndY));        
    } else {
        
    }
    
    if(Input0->Down.EndedDown){
        State->GreenOffset += 1;
    }



    // The flickering bug is due to thes Swapbuffer inside the app
    // loop

    // Display on the screen
    //Win32DisplayBufferWindow(DeviceContext, Dimens.Width, Dimens.Height, OBuffer);
    //SwapBuffers(DeviceContextt);    
    // Display on the screen

    //NOTE: I did this the wrong way
    //RenderSplendidGradient(OBuffer, BMPContent, 0, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //SwapBuffers(DeviceContextt);    

    //if(glGetError() != GL_NO_ERROR){
        //printf("OpenGL Error: %d\n", glGetError());
    //};

    // Display on the screen
    // The glitching sound driven me nearly crazy so I decided to turn it off
    //GameOutPutSound(SoundBuffer, State->Hz);
}

void copyBufferData(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, Win32_Front_Buffer* ScreenBuffer){
    
    ScreenBuffer->BitmapWidth = ScreenBuffer->BitmapWidth!=Game_Platform->BitmapWidth?Game_Platform->BitmapWidth:printf("W didn't change\n");
    ScreenBuffer->BitmapHeight = ScreenBuffer->BitmapHeight!=Game_Platform->BitmapHeight?Game_Platform->BitmapHeight:printf("H didn't change\n");
    ScreenBuffer->Pitch = ScreenBuffer->Pitch!=Game_Platform->Pitch?Game_Platform->Pitch:printf("Pitch didn't change\n");

    // Why if I don't pass this type of data the app will collapse as the conflict of memory
    if(!ScreenBuffer->GLDataPassed){
        PassGLData(&Game_Platform->glData, &ScreenBuffer->glData);
        ScreenBuffer->GLDataPassed = true;
    }

    if(ScreenBuffer->BitmapMemory != Game_Platform->BitmapMemory && Game_Platform->BitmapMemory!=NULL){
        ScreenBuffer->BitmapMemory = Game_Platform->BitmapMemory;
    }

    //if(ScreenBuffer->shaders_list != BackBuffer->shaders_list && BackBuffer->shaders_list.size()>0){
        //;
    //}
        //ScreenBuffer->Bitmapinfo = Game_Platform->Bitmapinfo;
//
        //if(ScreenBuffer->BitmapHandle != Game_Platform->BitmapHandle && Game_Platform->BitmapHandle != NULL){
        //ScreenBuffer->BitmapHandle = Game_Platform->BitmapHandle;
    //}
    //ScreenBuffer->Window = BackBuffer->Window;
}

void displayBufferData(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, Win32_Front_Buffer* FrontBuffer){
    printf("=====================================\n");
    printf("                 |  BackBuffer | FrontBuffer\n");
    printf("VAOS             |  %d         | %d\n", Game_Platform->glData.VAOs, FrontBuffer->glData.VAOs);
    printf("PlaneVAOS        |  %d         | %d\n", Game_Platform->glData.PlaneVAOs, FrontBuffer->glData.PlaneVAOs);
    printf("VBO              |  %d         | %d\n", Game_Platform->glData.VBO, FrontBuffer->glData.VBO);
    printf("ColorVBO         |  %d         | %d\n", Game_Platform->glData.ColorVBO, FrontBuffer->glData.ColorVBO);
    printf("PlaneVBO         |  %d         | %d\n", Game_Platform->glData.PlaneVBO, FrontBuffer->glData.PlaneVBO);
    printf("TextureID        |  %d         | %d\n", Game_Platform->glData.textureHandle, FrontBuffer->glData.textureHandle);
    //printf("ProgramID        |  %d         | %d\n", Game_Platform->glData.ProgramIDs[0], FrontBuffer->glData.ProgramIDs[0]);
    //printf("ProgramID        |  %d         | %d\n", Game_Platform->glData.ProgramIDs[1], FrontBuffer->glData.ProgramIDs[1]);
    printf("Memory Address   |0x%x |0x%x \n", FrontBuffer->BitmapMemory, Game_Platform->BitmapMemory);
    printf("DirectMem Address|0x%x size:%d |       \n", FrontBuffer->BitmapMemoryForDirectBlit, Game_Platform->BitmapMemoryForDirectBlit);
    printf("=====================================\n");
}

// void LoadTileMap(){
//     Tile
// }
void APIENTRY MessageCallback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              const GLchar* message,
                              const void* userParam) {
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
              << " type = 0x" << std::hex << type
              << ", severity = 0x" << severity
              << ", message = " << message << std::endl;
}

//CALLBACK 
LRESULT CALLBACK MainWindowCallBack(HWND Window, UINT Message, WPARAM Wparam, LPARAM Lparam) {

  LRESULT result;
  bool fDraw = false;
  POINT ptPrevious = {};
  switch (Message) {

  case WM_CREATE: {
    printf("On Window creating stage\n");
  } break;
  // What is DeviceContext for in this case??
  // NOTE: Whenever the window is resized, this function capture the size
  // of the new window and update a new proper DIB for that
  // DIB is a table where store BIT color infor
  case WM_SIZE: {

      if (first_size) {
          first_size = false;
      } else {
          GetWindowDimension(&Game_Platform);
          printf("Client Rect left:%d top:%d right:%d bottom:%d\n", (int)Game_Platform.ClientRect.left, (int)Game_Platform.ClientRect.top, (int)Game_Platform.ClientRect.right, (int)Game_Platform.ClientRect.bottom);
          Win32ResizeDIBSection(&Game_Platform, &BackBuffer, Dimens.Width, Dimens.Height);
          if (!Game_Platform.transferNeed) {
              Game_Platform.transferNeed = true;
          }

          if((GetKeyState(VK_CONTROL) & (1 << 15)) > 0){
              //ShowGlyphs(&BackBuffer, &Glyph_Map);
              HDC currentDC = GetDC(Window);
              HDC tempDC;
              RenderSplendidGradient(nullptr, &BackBuffer, nullptr, nullptr);
              Win32DisplayBufferWindow(tempDC, 0, 0, &Game_Platform, &BackBuffer);
              SwapBuffers(tempDC);
              printf("Change to display glyphs\n");
          }else{
              glViewport(Game_Platform.ClientRect.left, Game_Platform.ClientRect.top, Game_Platform.BitmapWidth, Game_Platform.BitmapHeight);
          }

          OutputDebugStringA("WM_SIZE\n");
      }

  } break;

  case WM_CLOSE: {
    GlobalRunning = false;
    OutputDebugStringA("WM_CLOSE\n");
  } break;

      case WM_KEYUP:{

          bool IsUp = ((Lparam & (1 << 31)) != 1);
          bool WasUP = ((Lparam & (1 << 30)) == 0);
          bool WasDown = ((Lparam & (1 << 30)) == 1);

          uint32 vkCode = Wparam;
          
          if (vkCode == 'J') {
              // Actually the front vec is at the back of the camera
              // State.BlueOffset+= 10;
                  //if(is_moving)
                  //is_moving = !is_moving;

              printf("j is released\n");
          }

          else if (vkCode == VK_UP) {
              // Actually the front vec is at the back of the camera
              // State.BlueOffset+= 10;
                  //if(is_moving)
                  //is_moving = !is_moving;
//
              test_vampire_motion.object_speed.current_states = IDLE;
              printf("Up is released\n");
          }

          else if (vkCode == VK_DOWN) {
              // Actually the front vec is at the back of the camera
              // State.BlueOffset+= 10;
                  //if(is_moving){
                      //is_moving = !is_moving;
                  //}
              test_vampire_motion.object_speed.current_states = IDLE;
              printf("DOWN is released\n");
          }

          else if (vkCode == VK_LEFT) {
              // Actually the front vec is at the back of the camera
              // State.BlueOffset+= 10;
              test_vampire_motion.object_speed.current_states = IDLE;
              printf("LEFT is released\n");
              //if(is_moving)
                //is_moving = !is_moving;
//
          }

          else if (vkCode == VK_RIGHT) {
              // Actually the front vec is at the back of the camera
              // State.BlueOffset+= 10;
              //if(is_moving)
                //is_moving = !is_moving;
              test_vampire_motion.object_speed.current_states = IDLE;
              printf("RIGHT is released\n");
          }          

          //else if (vkCode == VK_TAB) {
              //if (SoundOutPut.hz == 128) {
                  //SoundOutPut.hz = 256;
              //} else if (SoundOutPut.hz == 256) {
                  //SoundOutPut.hz = 512;
              //} else {
                  //SoundOutPut.hz = 128;
              //}
              //char Output[256];
              //sprintf(Output, "TAB button hitted, Current Hert is: %d\n",
                      //SoundOutPut.hz);
              //SoundOutPut.WavePeriod = SoundOutPut.SamplePerSecond / SoundOutPut.hz;
//
              //OutputDebugStringA("TAB button hitted");
          //}
//          
      }break;

 case WM_KEYDOWN: {
    bool IsDown = ((Lparam & (1 << 31)) == 0);
    bool WasDown = ((Lparam & (1 << 30)) == 1);
    bool WasUp = ((Lparam & (1 << 30)) == 0);

    uint32 vkCode = Wparam;
    printf("current face: %d, previous face: %d\n", test_vampire_motion.current_face, test_vampire_motion.previous_face);

    if (IsDown) {
      if (vkCode == VK_UP) {
        // Actually the front vec is at the back of the camera
        // State.BlueOffset+= 10;
            //if (WasUp) {
                //dancing_vampire_core = glm::translate(dancing_vampire_core, glm::vec3(0.0f, 0.0f, 5.0f * DelayedRatio));
          if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          test_vampire_motion.previous_face =           test_vampire_motion.current_face;

          test_vampire_motion.current_face = FORWARD;          

          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;
          test_vampire_motion.object_speed.current_states = WALK_FORWARD;
                //if(!is_moving)
                    //is_moving = !is_moving;

                printf("Up is hit\n");
            //}
      }

      else if (vkCode == VK_DOWN) {
        // Actually the front vec is at the back of the camera
        // State.BlueOffset+= 10;
            //if (WasUp) {
                //dancing_vampire_core = glm::translate(dancing_vampire_core, glm::vec3(0.0f, 0.0f, -(5.0f * DelayedRatio)));
                //if(!is_moving)
                    //is_moving = !is_moving;
          if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          test_vampire_motion.previous_face =           test_vampire_motion.current_face;

          test_vampire_motion.current_face = BACKWARD;

          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;
          test_vampire_motion.object_speed.current_states = WALK_BACKWARD;
          printf("DOWN is HIT\n");
            //}

      }

      else if (vkCode == VK_LEFT) {
        // Actually the front vec is at the back of the camera
        // State.BlueOffset+= 10;
            //if (WasUp) {
                //dancing_vampire_core = glm::translate(dancing_vampire_core, glm::vec3(-(5.0f * DelayedRatio), 0.0f, 0.0f));
                //if(!is_moving)
                    //is_moving = !is_moving;
          if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          test_vampire_motion.previous_face =           test_vampire_motion.current_face;

          test_vampire_motion.current_face = LEFT;

          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;
          test_vampire_motion.object_speed.current_states = WALK_LEFT;
                printf("LEFT is HIT\n");
        //}
      }

      else if (vkCode == VK_RIGHT) {
        // Actually the front vec is at the back of the camera
        // State.BlueOffset+= 10;
            //if (WasUp) {
                //dancing_vampire_core = glm::translate(dancing_vampire_core, glm::vec3(5.0f * DelayedRatio, 0.0f, 0.0f));
                //if(!is_moving)
                    //is_moving = !is_moving;
          if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          test_vampire_motion.previous_face =           test_vampire_motion.current_face;

          test_vampire_motion.current_face = RIGHT;

          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;
          test_vampire_motion.object_speed.current_states = WALK_RIGHT;
          printf("RIGHT is HIT\n");
            //}
      }
        //====================================================
      else if (vkCode == 'W') {
          // Actually the front vec is at the back of the camera
          // State.BlueOffset+= 10;
          if((GetKeyState(VK_CONTROL) & (1 << 15)) > 0){
              Win32ResizeDIBSection(&Game_Platform, &BackBuffer, Dimens.Width, Dimens.Height);
              //ShowGlyphs(&BackBuffer, &Glyph_Map);
              HDC currentDC = GetDC(Window);
              Win32DisplayBufferWindow(currentDC, 0, 0, &Game_Platform, &BackBuffer);
              SwapBuffers(currentDC);
              ReleaseDC(Game_Platform.Window, currentDC);
              printf("Change to display glyphs\n");
          }else{
              BackBuffer.camera.Position +=
                  glm::normalize(BackBuffer.camera.Direction) *
                  (float)BackBuffer.camera.speed;
              if (!WasDown) {
                  printf("W is HIT\n");
              }
          }
      }

      else if (vkCode == 'S') {
          if((GetKeyState(VK_CONTROL) & (1 << 15)) > 0){
              Game_Platform.SwitchCamera = !Game_Platform.SwitchCamera;
          }
          
        State.GreenOffset += 10;
        BackBuffer.camera.Position -=
            glm::normalize(BackBuffer.camera.Direction) *
            (float)BackBuffer.camera.speed;
        if (!WasDown) {
            printf("S is HIT\n");
        }
      }

      else if (vkCode == 'A') {
        // XOffset -= 10;
        OutputDebugStringA("Left Button :");
        // if(WasDown) {
        //  Not Camera front and up
        BackBuffer.camera.Position -=
            glm::normalize(
                glm::cross(BackBuffer.camera.Direction, BackBuffer.camera.Up)) *
            (float)BackBuffer.camera.speed;
        // OutputDebugStringA(" Was Down");
        // }
        if (!WasDown) {
            printf("A is HIT\n");
        }
      }

      else if (vkCode == 'L') {
        // XOffset -= 10;
        OutputDebugStringA("L Button :");
        // if(WasDown) {
        if (!Load_Lib) {
            Load_Lib = true;
        }
        // OutputDebugStringA(" Was Down");
        // }
        if (!WasDown) {
            printf("L is HIT\n");
        }
      }

      else if (vkCode == 'D') {
        BackBuffer.camera.Position +=
            glm::normalize(
                glm::cross(BackBuffer.camera.Direction, BackBuffer.camera.Up)) *
            (float)BackBuffer.camera.speed;
        if (!WasDown) {
            printf("D is HIT\n");
        }
        // XOffset += 10;
      }

      else if (vkCode == 'J') {

          //if(!is_moving)
              //is_moving = !is_moving;
          //if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          //test_vampire_motion.previous_face =           test_vampire_motion.current_face;
//
          //test_vampire_motion.current_face = FORWARD;
            test_vampire_motion.pre_pos = glm::vec3(test_vampire_motion.position[3]);
          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;

              if(test_vampire_motion.object_speed.current_states != JUMP_FORWARD)
              test_vampire_motion.object_speed.current_states = JUMP_FORWARD;


          if (!WasDown) {
            printf("J is HIT\n");
        }
        // XOffset += 10;
      }

      else if (vkCode == 'K') {

          //if(!is_moving)
              //is_moving = !is_moving;
          //if(test_vampire_motion.previous_face != test_vampire_motion.current_face)
          //test_vampire_motion.previous_face =           test_vampire_motion.current_face;
//
          //test_vampire_motion.current_face = BACKWARD;
//
            test_vampire_motion.pre_pos = glm::vec3(test_vampire_motion.position[3]);
          test_vampire_motion.object_speed.previous_states = test_vampire_motion.object_speed.current_states;

              if(test_vampire_motion.object_speed.current_states != JUMP_BACKWARD)
              test_vampire_motion.object_speed.current_states = JUMP_BACKWARD;

          if (!WasDown) {
            printf("J is HIT\n");
        }
        // XOffset += 10;
      }

      else if (vkCode == VK_SPACE) {

          BackBuffer.camera.Position += BackBuffer.camera.Up * (float)BackBuffer.camera.speed;

          if (!WasDown) {
            printf("Space is HIT\n");
        }
        // XOffset += 10;
      }

      else if (vkCode == VK_SHIFT) {
        BackBuffer.camera.Position -=
            BackBuffer.camera.Up * (float)BackBuffer.camera.speed;
        if (!WasDown) {
            printf("Left Shift is HIT\n");
        }
        // XOffset += 10;
      }

      else if (vkCode == VK_CONTROL) {

        if (!showMsPF) {
            showMsPF = true;
        }
        printf("Right Shift is HIT\n");
        // XOffset += 10;
      }

      else if (vkCode == VK_BACK) {
       BackBuffer.camera.Direction =
            glm::vec3(-4.0f, 4.0f, 0.0f) - BackBuffer.camera.Position;

        BackBuffer.camera.mouse.LastX = BackBuffer.camera.mouse.xPos;
        BackBuffer.camera.mouse.LastY = BackBuffer.camera.mouse.yPos;

        BackBuffer.camera.mouse.MouseXOffset = 0;
        BackBuffer.camera.mouse.MouseYOffset = 0;

        printf("Direction X is %f\n", BackBuffer.camera.Direction.x);
        printf("Direction Y is %f\n", BackBuffer.camera.Direction.y);

        BackBuffer.camera.Yaw = glm::degrees(
            glm::acos(glm::clamp(BackBuffer.camera.Direction.x, -1.0f, 1.0f)));
        BackBuffer.camera.Pitch = glm::degrees(
            glm::acos(glm::clamp(BackBuffer.camera.Direction.y, -1.0f, 1.0f)));

        printf("Yaw is %f\n", BackBuffer.camera.Yaw);
        printf("Pitch is %f\n", BackBuffer.camera.Pitch);

        if (BackBuffer.camera.Yaw > 360.0f) {
          BackBuffer.camera.Yaw -= 360.0f;
        }

        if (BackBuffer.camera.Pitch > 120.0f) {
          BackBuffer.camera.Yaw -= 120.0f;
        }

        printf("Back to point at the backpack\n");

        std::cout << "Direction is: "
                  << glm::to_string(BackBuffer.camera.Direction) << std::endl;

        WINDOWPLACEMENT windowstatus = {};
        windowstatus.length = sizeof(WINDOWPLACEMENT);

        if (GetWindowPlacement(Window, &windowstatus)) {
          printf("Window position(x, y) is: %d %d\n",
                 windowstatus.rcNormalPosition.left,
                 windowstatus.rcNormalPosition.top);
        } else {
          printf("Failed to get window status\n");
        }

        SetCursorPos(
            BackBuffer.camera.mouse.xPos + windowstatus.rcNormalPosition.left,
            BackBuffer.camera.mouse.yPos + windowstatus.rcNormalPosition.top);
        // XOffset += 10;
      }
    }

    if (!BackBuffer.camera.moved) {
      // std::cout<<"Camera Position
      // is"<<glm::to_string(BackBuffer.camera.Position)<<std::endl;
      // std::cout<<"Camera Direction
      // is"<<glm::to_string(BackBuffer.camera.Direction)<<std::endl;
      BackBuffer.camera.moved = true;
    }

    // if(vkCode == VK_LEFT) {
    //
    // OutputDebugStringA("Left Button :");
    // if(IsDown) {
    // OutputDebugStringA(" Is Down");
    //
    // }
    // OutputDebugStringA("\n");
    // }

    if (vkCode == VK_ESCAPE) {
      if (GlobalRunning) {
        GlobalRunning = false;
      }
    }
  } break;

  case WM_MOUSEWHEEL: {
    float wheelPos = GET_WHEEL_DELTA_WPARAM(Wparam) * 0.1f;
    // printf("Wheel delta: %d\n", (GET_WHEEL_DELTA_WPARAM(Wparam)));
    BackBuffer.camera.fov -= wheelPos * BackBuffer.camera.speed;

    if (BackBuffer.camera.fov < 1.0f) {
      BackBuffer.camera.fov = 1.0f;
    }

    if (BackBuffer.camera.fov > 45.0f) {
      BackBuffer.camera.fov = 45.0f;
    }
    printf("Mouse Wheel is rolling, Wheel: %f, fov: %f\n", wheelPos,
           BackBuffer.camera.fov);

    if (!BackBuffer.camera.mouse.Wheeled) {
      BackBuffer.camera.mouse.Wheeled = true;
    }

  } break;

  case WM_MOUSELEAVE: {
      GetWindowDimension(&Game_Platform);
      
    //if (BackBuffer.camera.mouse.LastX != BackBuffer.BitmapWidth / 2) {
      //BackBuffer.camera.mouse.LastX = BackBuffer.BitmapWidth / 2;
    //}

    //if (BackBuffer.camera.mouse.LastY != BackBuffer.BitmapHeight / 2) {
      //BackBuffer.camera.mouse.LastY = BackBuffer.BitmapHeight / 2;
    //}

    // printf("Mouse Pos X: %d, Y: %d\n", BackBuffer.camera.mouse.LastX,
    // BackBuffer.camera.mouse.LastY);
    if (BackBuffer.camera.mouse.moved) {
      // std::cout<<"Camera Position
      // is"<<glm::to_string(BackBuffer.camera.Position)<<std::endl;
      // std::cout<<"Camera Direction
      // is"<<glm::to_string(BackBuffer.camera.Direction)<<std::endl;
      BackBuffer.camera.mouse.moved = false;
    }

  } break;

  case WM_LBUTTONDOWN: {
    uint32 vkCode = Wparam;
    // if(vkCode == VK_LBUTTON) {
    if (!BackBuffer.camera.focusCenter) {
        if(!BackBuffer.camera.focusCenter)
      BackBuffer.camera.focusCenter = !BackBuffer.camera.focusCenter;
    }
    printf("Mouse LButton is HIT\n");
    //}
    // return 0;
  } break;

  case WM_LBUTTONUP: {
    // uint32 vkCode = Wparam;
    // if(vkCode == VK_LBUTTON) {
    if (BackBuffer.camera.focusCenter) {
      BackBuffer.camera.focusCenter = !BackBuffer.camera.focusCenter;
    }
    printf("Mouse LButton is released\n");
    //}
    // return 0;
  } break;

  case WM_MOUSEHOVER: {
      uint32 vkCode = Wparam;
      if(BackBuffer.camera.focusCenter){
          if (TrackMouseEvent(BackBuffer.camera.mouse.mouseEvent)) {
              printf("Mouse event is being tracked\n");
          } else {
              printf("Can not track Mouse event\n");
          };
      }
  } break;

  case WM_MOUSEMOVE: {
      uint32 vkCode = Wparam;
      //if(vkCode = MK_LBUTTON){
      if(BackBuffer.camera.focusCenter){
          if((GetKeyState(VK_CONTROL) & (1 << 15)) > 0){
              // NOTE: grasp the whole perspective and move it instead of the camera
              if(!BackBuffer.camera.move_perspective_instead)
                  BackBuffer.camera.move_perspective_instead = true;
          }else{
              if(BackBuffer.camera.move_perspective_instead)
                  BackBuffer.camera.move_perspective_instead = false;
          }

          BackBuffer.camera.mouse.xPos = GET_X_LPARAM(Lparam);
          BackBuffer.camera.mouse.yPos = GET_Y_LPARAM(Lparam);

          int gapX = BackBuffer.camera.mouse.LastX - BackBuffer.camera.mouse.xPos;
          int gapY = BackBuffer.camera.mouse.LastY - BackBuffer.camera.mouse.yPos;

          // reset last x and y
          if(gapX > 100 || gapX < -100)
          BackBuffer.camera.mouse.LastX = GET_X_LPARAM(Lparam);
          if(gapY > 100 || gapY < -100)
          BackBuffer.camera.mouse.LastY = GET_Y_LPARAM(Lparam);

          if (!BackBuffer.camera.mouse.moved) {
              BackBuffer.camera.mouse.moved = true;
          }
      }
    // return 0L;
      //}
  } break;

  case WM_SYSKEYDOWN: {
    uint32 vkCode = Wparam;
    bool AltkeyisDown = ((Lparam & (1 << 29)) != 0);
    // & is and operator that is the produce 1 if two bit is 1
    if ((vkCode == VK_F4) && AltkeyisDown) {
      GlobalRunning = false;
    }
    OutputDebugStringA("WM_SYSKEYDOWN\n");
  } break;

  case WM_SYSKEYUP: {
    OutputDebugStringA("WM_SYSKEYUP\n");
  } break;

  case WM_DESTROY: {
    GlobalRunning = false;
    PostQuitMessage(0);
    OutputDebugStringA("WM_DESTROY\n");
  } break;

  default: {
    OutputDebugStringA("DEFAULT\n");
    result = DefWindowProcA(Window, Message, Wparam, Lparam);
  } break;
  }
  // return 0L;
  return result;
}


void PassGLData(OpenGLData* BackData, OpenGLData* FrontData)
{
    if( FrontData->VAOs != BackData->VAOs && !isNull(&BackData->VAOs)){
            FrontData->VAOs = BackData->VAOs;
        }

        if(FrontData->VBO != BackData->VBO && !isNull(&BackData->VBO)){
            FrontData->VBO = BackData->VBO;
        }

        if(FrontData->ColorVBO != BackData->ColorVBO && !isNull(&BackData->ColorVBO)){
            FrontData->ColorVBO = BackData->ColorVBO;
        }

        //if(FrontData->ProgramID != BackData->ProgramID && !isNull(&BackData->ProgramID)){
            //FrontData->ProgramID = BackData->ProgramID;
        //}

        if(FrontData->VAOs != BackData->VAOs && !isNull(&BackData->VAOs)){
            FrontData->VAOs = BackData->VAOs;
        }

        if(FrontData->PlaneVAOs != BackData->PlaneVAOs && !isNull(&BackData->PlaneVAOs)){
            FrontData->PlaneVAOs = BackData->PlaneVAOs;
        }

        if(FrontData->PlaneVBO != BackData->PlaneVBO && !isNull(&BackData->PlaneVBO)){
            FrontData->PlaneVBO = BackData->PlaneVBO;
        }

        if(FrontData->textureHandle != BackData->textureHandle && !isNull(&BackData->textureHandle)){
            FrontData->textureHandle = BackData->textureHandle; 
        }

}
bool isNull(unsigned int* member){
    if(member == 0x00){
        return true;
    }
    return false;
}


void ErrorExit() 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    if (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL) == 0) {
        MessageBox(NULL, TEXT("FormatMessage failed"), TEXT("Error"), MB_OK);
        ExitProcess(dw);
    }

    MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    ExitProcess(dw); 
}

void ResetGLState(Win32_OffScreen_Buffer* BackBuffer, Platform_Properties* Game_Platform){

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEBUG_OUTPUT);
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    HDC windowContext = GetDC(Game_Platform->Window);
    wglMakeCurrent(windowContext, Game_Platform->glData.defaultContext);
    wglDeleteContext(Game_Platform->glData.openglRC);
};


void InitCamera(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer){

    glm::vec3 Position = glm::vec3(-3.0f, -10.0f, -12.0f);
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Right =  glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    //set camera view here
    //std::cout<<"View matrix from camera: "<<glm::to_string(BackBuffer.camera.view)<<std::endl;

    BackBuffer->camera = Camera(Game_Platform->BitmapWidth, Game_Platform->BitmapHeight, Position, Front, Right, Up);    
    BackBuffer->camera.fov = 45.0f;
};

void Set_Projection_View(Win32_OffScreen_Buffer* BackBuffer, Platform_Properties* Game_Platform){
    for(const GLuint& shader:Game_Platform->glData.ProgramIDs){
        glUseProgram(shader);
        setMat4(shader, "projection", !Game_Platform->SwitchCamera?BackBuffer->camera.projection:BackBuffer->camera_set[0]->projection);
        setMat4(shader, "view", !Game_Platform->SwitchCamera?BackBuffer->camera.view:BackBuffer->camera_set[0]->view);
    };
    glUseProgram(0);
        //basic_shader_->setMat4("projection", BackBuffer.camera.projection);
};


void CalDelayedRatio(float* DelayedRatio, Clock_Set* Time_Set, Win32_OffScreen_Buffer* BackBuffer){
    
    //if (RatioCalculated) {
    *DelayedRatio = (float)(Time_Set->MsPerFrame/Time_Set->StandardMSperFrame);
    *DelayedRatio>0.0f?BackBuffer->camera.speed = (2.5f * (*DelayedRatio)):BackBuffer->camera.speed = (2.5f  * 0.17f);                
    //RatioCalculated = false;
    //}                    
};

WNDCLASSEXA SetUpWindowClass(Platform_Properties* Game_Platform, HINSTANCE Instance){
    WNDCLASSEXA WindowClass = {};
  WindowClass.cbSize = sizeof(WNDCLASSEXA);
  WindowClass.style = CS_HREDRAW|CS_VREDRAW;
  //HERE main windowcallback main job
  //WindowClass.lpfnWndProc = BackBuffer->wndproc;
  WindowClass.lpfnWndProc = MainWindowCallBack;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "First Game Window Class";
  Win32ResizeDIBSection(Game_Platform, &BackBuffer, Dimens.Height, Dimens.Width);
  return WindowClass;
};

void CleanUpandExit(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, Glyph_Map* map){
    
    glDeleteVertexArrays(1, &Game_Platform->glData.VAOs);
    glDeleteVertexArrays(1, &Game_Platform->glData.PlaneVAOs);

    glDeleteBuffers(1, &Game_Platform->glData.VBO);
    glDeleteBuffers(1, &Game_Platform->glData.ColorVBO);
    glDeleteBuffers(1, &Game_Platform->glData.PlaneVBO);

    for(B_shader_program* shader: BackBuffer->shaders_list){
        delete shader;
        shader = nullptr;
    }
    BackBuffer->shaders_list.clear();

    for(Camera* camera: BackBuffer->camera_set){
        delete camera;
        camera = nullptr;
    }
    BackBuffer->camera_set.clear();

    for(Glyph_Property* const &glyph: map->Glyph_list){
        if(glyph->upside_down_bitmap){
            //VirtualFree(map->upside_down_bitmap, 0, MEM_RELEASE);
            free(glyph->upside_down_bitmap);
            delete glyph->upside_down_bitmap;
            glyph->upside_down_bitmap = nullptr;
        }
    }


    ResetGLState(BackBuffer, Game_Platform);
};
