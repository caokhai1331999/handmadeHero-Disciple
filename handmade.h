#if !defined(HANDMADE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */

/*
  TODO: Services that the platform layer apply to the game
 */

    
/*
  NOTE: Services that the platform layer provide to the game layer
 */


struct Game_OffScreen_Buffer{  
    // BITMAPINFO Bitmapinfo;
    // HBITMAP BitmapHandle;
    void* BitmapMemory;
    int BitmapWidth;
    int BitmapHeight;
    int Pitch;
    const int BytesPerPixel = 4;
};


struct Game_Sound_OutPut{  
    int16 SamplePerSecond;
    int SampleCounts;
    int16* Samples;
};


// TODO: Allow the sample offset here for more robust platform options

internal void GameOutputSound(Game_Sound_OutPut* SecondSoundBuffer, int Hz);

internal void RenderSplendidGradient(Game_OffScreen_Buffer* OBuffer, int XOffset, int YOffset);

void GameUpdateAndRender(Game_OffScreen_Buffer* OBuffer, Game_Sound_OutPut* SoundBuffer);

#define HANDMADE_H
#endif
