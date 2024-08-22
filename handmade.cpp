/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include "handmade.h"

void RenderSplendidGradient(Game_Offscreen_Buffer* OBuffer, int XOffset, int YOffset) {
    // RR GG BB
    // Row is a pointer to every line of bitmapMemory
    // While pitch is data length of everyline of bitmap
    int Width = OBuffer->BitmapWidth;
    int Height = OBuffer->BitmapHeight;    
    int Pitch = OBuffer->Pitch;
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

internal void GameOutPutSound(Game_Sound_OutPut* SecondSoundBuffer) {
    local_persist real32 tsine = 0;
    local_persist int ToneVolume = 3000;
    local_persist int ToneHz = 256;
    local_persist int WavePeriod = SecondSoundBuffer->SamplePerSecond/ToneHz ;
    // int16 SampleValue = ((RunningSampleIndex++/          (SquareWavePeriod/2))% 2) ? ToneVolume : -ToneVolume;

    for (int SampleIndex{0};
         SampleIndex < SecondSoundBuffer->SampleCounts;
         SampleIndex++){

        int16* SampleOut = SecondSoundBuffer->Samples;
        real32 SineValue = sinf(tsine);            
        int16 SampleValue = (int16)(SineValue * ToneVolume);

        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
        tsine += 2.0f*Pi32* 1.0f/(real32)WavePeriod;                            
    }                                               
}

void GameUpdateAndRender(Game_Offscreen_Buffer* OBuffer, int BlueOffset, int GreenOffset, Game_Sound_OutPut* SecondSoundBuffer){
    GameOutPutSound(SecondSoundBuffer);
    RenderSplendidGradient(OBuffer, BlueOffset, GreenOffset);
}
