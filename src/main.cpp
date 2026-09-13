/*======================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
  ================================================================================*/
#include <ctime>
#include <cstdlib>
#include "SoundMaker.h"
#include "Tile.h"
#include "Light.h"
//#include "handmade.h"
#include "physics.h"
#include "animator.h"
#include "entity.h"

// So the when we call just the name of the function we have the pointer of that function

int CALLBACK WinMain
(HINSTANCE Instance,
 HINSTANCE hInstPrev,
 PSTR cmdline,
 int cmdshow)
{
    //Game_Platform.wndproc = MainWindowCallBack;

    Clock_Set TimeSet = {};
    imagee_content* BMPContent = nullptr;

    QueryPerformanceFrequency(&(TimeSet.PerfCountFrequencyResult));

    float ColorOffset = 0.0f;
    Glyph_Map Glyphs_Map = {};    

    bool32 collided_  = false;
    bool32 along1 = true;  
    bool32 along2 = true;  
    bool32 short_color_change_ = false;

    std::string test_string = " HE ";
    std::vector<texture_group>texture_collection;
    texture_collection.reserve(10);
    win32LoadXInput();

  WNDCLASSEXA WindowClass = SetUpWindowClass(&Game_Platform, Instance);
    //HWND Window = {};

  
  // NOTE: I forgot to init window
  HMODULE AniLib = {};
  
  TimeSet.PerfCountFrequency = (int64)(TimeSet.PerfCountFrequencyResult.QuadPart);  

  updateCa UpdateCamera = NULL;
  //setup_pointlight__ setup_pointlight = NULL;
  //Set_Light_ Set_environmental_light = NULL;
  move_object_ move_ = NULL;
  InitCamera__ InitCamera_ = NULL;
  AutoAdjustCameraPos__ AutoAdjustCameraPos = NULL;
  Load_Textures_for_OpenGL__ Load_Textures_for_OpenGL = NULL;
  //LoadFont__ LoadFont = NULL;
  //DrawFont__ DrawFont = NULL;
  //setUpUBO__ setUpUBO = NULL;

      //AniTimeUpdater AniUpdate_;

  Animation* danceAnimation = nullptr;
  Animator* animator = nullptr;
  int delay_count = 0;
  Camera* Chosen_Camera;
  std::vector<glm::mat4>*Transform_ = nullptr;

  if (RegisterClassExA(&WindowClass)) {

    Game_Platform.Window = CreateWindowExA(
        // NOTE: The window didn't show up is because the first argument
        WS_EX_APPWINDOW, WindowClass.lpszClassName, "win32GameWithoutEngine",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

    if (Game_Platform.Window) {

      OpenConsole();
      int displayCount = ShowCursor(true);
      printf("display count: %d\n", displayCount);
      printf("counter per 1s: %I64d\n",  TimeSet.PerfCountFrequency);

      if (!GlobalRunning) {
        GlobalRunning = true;
      }

      HDC DeviceContext = GetDC(Game_Platform.Window);
      int refreshRate = GetDeviceCaps(DeviceContext, VREFRESH);
      ReleaseDC(Game_Platform.Window, DeviceContext);

      if(first_announce){
          if(AniLib!=NULL)
              if(FreeLibrary(AniLib)){
                  printf("Succeed free library\n");
                      } else {
                  printf("Failed free library\n");
              };

          
          if (CopyFileA("light32.dll", "light32_copy.dll",
                       false))
          {
              printf("Succeed copy lib file\n");
              AniLib = LoadLibraryA("light32_copy.dll");

              if(AniLib != NULL){
                  UpdateCamera = (updateCa)GetProcAddress(AniLib, "updateCamera_");          
                  //setup_pointlight = (setup_pointlight__)GetProcAddress(AniLib, "setup_pointlight_");
                  //Set_environmental_light = (Set_Light_)GetProcAddress(AniLib, "Set_environmental_light_");
                  move_ = (move_object_)GetProcAddress(AniLib, "move_object");
                  InitCamera_ = (InitCamera__)GetProcAddress(AniLib, "InitCamera_");
                  AutoAdjustCameraPos = (AutoAdjustCameraPos__)GetProcAddress(AniLib, "AutoAdjustCameraPos_");
                  Load_Textures_for_OpenGL = (Load_Textures_for_OpenGL__)GetProcAddress(AniLib, "Load_Textures_for_OpenGL_");
                  //LoadFont = (LoadFont__)GetProcAddress(AniLib, "LoadFont_");
                  //DrawFont = (DrawFont__)GetProcAddress(AniLib, "DrawFont_");
              }else{
                  printf("Failed loading library from dll file\n");
              }
          }
      }
// Camera here
      
      glm::vec3 tempPos = glm::vec3(9.0f, 0.3, 1.0f);
      glm::vec3 size = glm::vec3(1.0f);
      space_box obj1 = space_box(&tempPos, &size);
      tempPos = glm::vec3(-9.0f, 0.3, 1.0f);
      space_box obj2 = space_box(&tempPos, &size);

      if (refreshRate > 1) {
        printf("Refresh rate is : %dHz\n", refreshRate);
      };

#ifdef INTERNAL
      LPVOID BaseAddress = (LPVOID)MEGABYTES(5);
#else
      LPVOID BaseAddress = 0;
#endif

            //=======================================================
            std::srand(std::time(NULL));

            glm::vec3 randomRotateAxis = glm::vec3(0.4f*(float)(std::rand()*2),0.4f*(float)(std::rand()*2), 0.4f*(float)(std::rand()*2));

            Game_Memory game_memory = {};
            game_memory.PermanentStorageSize = MEGABYTES(128);
            game_memory.TransientStorageSize = MEGABYTES((uint64)6);

            uint64 TotalSize = game_memory.PermanentStorageSize + game_memory.TransientStorageSize;
            
            game_memory.PermanentStorage = VirtualAlloc(BaseAddress , TotalSize,  MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            game_memory.TransientStorage = ((uint8*)game_memory.PermanentStorage + game_memory.PermanentStorageSize);
            //=====================================================

          if(game_memory.TransientStorage && game_memory.PermanentStorage){
              
              debug_read_file_result result2;
              debug_read_file_result result;
              BMPContent =  DEBUGReadBMP("Harry_and_Accomplices_rescaled.bmp",&result);
            //  =============================================
            //BMPContent = DEBUGReadBMP("adventure_.jpg", &result);
            // printf("About to read image\n");
            //  NOTE: ???? Why when I change to different bmp image it crashed
            // byte order: AA BB GG RR bottom up
            // JPGContent = DEBUGReadJPG("Harry and Accomplices.jpg", &result2);
            // OpenGL part

              Win32_Front_Buffer ScreenBuffer = Win32_Front_Buffer(
                Game_Platform.BitmapWidth, Game_Platform.BitmapHeight,
                &Game_Platform.glData, Game_Platform.BitmapMemory, Game_Platform.Window);
              
            // Randomize cube direction
            std::srand(std::time(0));
                
// Cause the ScreenData will be deleted out of the loop so
                // We have to assign address of memory and glData to
                //InitOpenGL(Window, &Game_Platform, &ScreenBuffer, JPGContent);
                //RenderSplendidGradient(&Game_Platform, &ScreenBuffer, BMPContent, 0, 0, 4);
            
            InitOpenGL(&Game_Platform, &BackBuffer, &ScreenBuffer, BMPContent);
                GLenum err = glGetError();
                if (err != GL_NO_ERROR) {
                    std::cerr << "OpenGL Error: " << err << std::endl;
                }
                std::string shader_name = "animating sketching brush";
// Set up Shader program here
                // Consider move them somewhere
                BackBuffer.shaders_list.reserve(5);

                BackBuffer.shaders_list.push_back(new B_shader_program("2.skeletal_animation.vs", "2.skeletal_animation.fs", "animating sketching brush"));
                ScreenBuffer.glData.ProgramIDs.push_back(BackBuffer.shaders_list[BackBuffer.shaders_list.size()-1]->GetProgramID());

// Basic shader
                BackBuffer.shaders_list.push_back(new B_shader_program("shader.vs", "shader.fs", "basic brush"));                
                ScreenBuffer.glData.ProgramIDs.push_back(BackBuffer.shaders_list[BackBuffer.shaders_list.size()-1]->GetProgramID());

                BackBuffer.shaders_list.push_back(new B_shader_program("quad.vs", "quad.fs", "quad brush"));
                ScreenBuffer.glData.ProgramIDs.push_back(BackBuffer.shaders_list[BackBuffer.shaders_list.size()-1]->GetProgramID());

                BackBuffer.shaders_list.push_back(new B_shader_program("1.model.vs", "1.model.fs", "model drawing brush"));
                ScreenBuffer.glData.ProgramIDs.push_back(BackBuffer.shaders_list[BackBuffer.shaders_list.size()-1]->GetProgramID());                

                BackBuffer.shaders_list.push_back(new B_shader_program("quad_glyph.vs", "quad_glyph.fs", "glyph drawing brush"));
                ScreenBuffer.glData.ProgramIDs.push_back(BackBuffer.shaders_list[BackBuffer.shaders_list.size()-1]->GetProgramID());                

                set_tile_vertex(BackBuffer.shaders_list[2], &BackBuffer.TileProperty);

                for(B_shader_program* const &shader: BackBuffer.shaders_list){
                        CheckShader(shader->GetProgramID(), programme_, shader->GetShaderName());
                    }

                copyBufferData(&Game_Platform, &BackBuffer, &ScreenBuffer);
                //LoadFont(&test_platform, &Game_Platform, &BackBuffer, &Glyphs_Map, "./media/FiraCode-VariableFont_wght.ttf");
                LoadFont(&Glyphs_Map, "./media/FiraCode-VariableFont_wght.ttf");
                //LoadFont(&Game_Platform, &Glyphs_Map, "./media/arial.ttf");
                
                //????
                //glm::mat4 View = glm::mat4(1.0f);
                real32 UpdatedAngle = 0.0f;
                real32 updateDegreeInPi = 0.0f;
                float threshHold = glm::radians((float)360.0f);

                glm::mat4 basic_cube_core = glm::mat4(1.0f);
                glm::mat4 backpack_core = glm::mat4(1.0f);
                
                glm::mat4 Plane = glm::mat4(1.0f);
                Plane = glm::translate(Plane, glm::vec3(0.0f));
                glm::mat4 WorldToCamera = glm::mat4(1.0f);

//Font drawing rect
                Rect_ rect = {0, 0, 700, 700};
                
                //WorldToCamera = Game_Platform.camera.view * dancing_vampire_core;
                WorldToCamera = BackBuffer.camera.view * test_vampire_motion.position;
                //--------------------------For_Debugging--------------------------
                Load_Textures_for_OpenGL(&Game_Platform, &texture_collection, "./media");
                //--------------------------For_Debugging--------------------------                
                Init_Entity_Specs(&test_vampire_motion);
                
                TRACKMOUSEEVENT mouseEventVar = {};
                mouseEventVar.cbSize = sizeof(TRACKMOUSEEVENT);
                mouseEventVar.dwFlags = TME_HOVER|TME_LEAVE;
                mouseEventVar.hwndTrack = Game_Platform.Window;
                mouseEventVar.dwHoverTime = 1000;

                BackBuffer.camera.mouse.mouseEvent = &mouseEventVar;

                //if(Game_Platform.camera.mouse.mouseEvent == NULL){
                    //printf("Can't initialize mouse Event, hoverTime\n");
                //} else {
                    //printf("Succeed initialize mouse Event\n");                    
                //}
                
                // This will be replaced by camera.view matrix
                //
                GetWindowDimension(&Game_Platform);
                //InitCamera(&Game_Platform.camera, Game_Platform.BitmapWidth, Game_Platform.BitmapHeight, glm::vec3(-3.0f, -10.0f, -12.0f));
                InitCamera(&Game_Platform, &BackBuffer);

                BackBuffer.camera_set.push_back(new Camera);
                InitCamera_(BackBuffer.camera_set[0], Game_Platform.BitmapWidth, Game_Platform.BitmapHeight, glm::vec3(10.0f, -10.0f, -5.0f));

                Chosen_Camera = Game_Platform.SwitchCamera?BackBuffer.camera_set[0]:&BackBuffer.camera;
                
                ViewCamera(&BackBuffer.camera);
                ViewCamera(BackBuffer.camera_set[0]);

                basic_cube_core = glm::translate(basic_cube_core, glm::vec3(2.0f, -4.0f, 0.0f));
                backpack_core = glm::translate(backpack_core, glm::vec3(-4.0f, 2.0f, 3.0f));
                // Set containing model for dancing vampire
                

                // Bug is here
                BackBuffer.camera.projection = glm::perspective(glm::radians(Chosen_Camera->fov), (float)Game_Platform.BitmapWidth / (float)Game_Platform.BitmapHeight, 0.1f, 100.0f);
                // By some unknown dark magic, The set projection matrix part didn't work in that function
                BackBuffer.camera_set[0]->projection = glm::perspective(glm::radians(Chosen_Camera->fov), (float)Game_Platform.BitmapWidth / (float)Game_Platform.BitmapHeight, 0.1f, 100.0f);

// MODEL PART === 
                std::string Mname = "backpack";                
                Model_* backpack = nullptr;
                backpack = new Model_(false, &Mname);
                std::string backpack_path = "./media/backpack.obj";
                loadModel_(&Game_Platform, backpack, backpack_path);
                
//NOW THE ANIMATING PART
                if(!first_normal_time)
                    first_normal_time = !first_normal_time;
//==========================================================
                Model_* dancing_vampire = nullptr;
                Mname = "vampire";
                dancing_vampire = new Model_(false, &Mname);
                std::string dancing_vampire_path = "./media/dancing_vampire.dae";
                loadModel_(&Game_Platform, dancing_vampire, dancing_vampire_path);

                danceAnimation = new Animation((char* )dancing_vampire_path.c_str(), dancing_vampire);
                animator = new Animator(danceAnimation);
// MODEL PART======================================================

                Game_Input Input[2] = {};
                Game_Input* OldInput = &Input[0];
                Game_Input* NewInput = &Input[1];


                //win32_Sound_OutPut SoundOutPut = {};
                //Game_Sound_OutPut SoundBuffer = {};
                //InitSoundBuffer(&SoundOutPut);
                //int16* SSamples = (int16* )VirtualAlloc(0 , SoundOutPut.SecondBufferSize ,MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

                ////Why InitOpenGl only work in the app loop
            
                int MaxControllerCount = XUSER_MAX_COUNT;
/*
  Init here

  NOTE: Why InitOpenGL only work while in window loop
  May be this is related to Window and DC that hasn't been
  initialized yet
*/            

                //Window = SetCapture(Window);

// Set light environment here

                BackBuffer.shaders_list[0]->use();
                BackBuffer.shaders_list[0]->setMat4( "projection", Chosen_Camera->projection);
                
                GLuint UBO;
                int k = 0;                

                GLuint id = BackBuffer.shaders_list[0]->GetProgramID();
                //showUniformVarValuePerVertex(&UBO, &id, &dancing_vampire->meshes[0], false, false, false, false, false, false);

                //std::string* map_content = new std::string ;
                //map_content = load_bin_map("level.map");
                //std::cout<<map_content->data()<<std::endl;

                //setup_pointlight(&Game_Platform, &BackBuffer, &envir_light);
                setup_pointlight(&envir_light);
                for(B_shader_program* const &shader: BackBuffer.shaders_list){
                    if(shader->GetProgramID()!=7){
                        shader->use();
                        shader->setMat4("view", Chosen_Camera->view);

                        if(Chosen_Camera->move_perspective_instead)
                            shader->setMat4("world_cube", Chosen_Camera->perspective_cube);

                        shader->setVec3( "ViewPos", Chosen_Camera->Position);
                        shader->setMat4("projection", Chosen_Camera->projection);
                    }
                }
                glUseProgram(0);

                set_environmental_light(BackBuffer.shaders_list[0], &envir_light, Chosen_Camera);
                //Set_environmental_light(&Game_Platform, &BackBuffer, BackBuffer.shaders_list[2], &envir_light, Chosen_Camera);
                //Set_environmental_light(&Game_Platform, &BackBuffer, BackBuffer.shaders_list[3], &envir_light, Chosen_Camera);
                set_environmental_light(BackBuffer.shaders_list[2], &envir_light, Chosen_Camera);
                set_environmental_light(BackBuffer.shaders_list[3], &envir_light, Chosen_Camera);
//=================================================
                // L, R, B, T
                glm::mat4 othorForGlyph = glm::ortho(0.0f, (float)100.0f, (float)100.0f, 0.0f);
                BackBuffer.shaders_list[4]->use();
                BackBuffer.shaders_list[4]->setMat4("projection", othorForGlyph);
                BackBuffer.shaders_list[4]->setInt("material.diffused_texture", Glyphs_Map.TextureID);
//==================================================
                glUseProgram(0);

                while (GlobalRunning) {
                  if(first_announce) {
                      QueryPerformanceCounter(&(TimeSet.LastCounter));
                      TimeSet.previous_collided = TimeSet.LastCounter;
                      TimeSet.LastCycleCounts = __rdtsc();
                  } else {
                      CalEarlyFrameTime(&TimeSet);
                  }

                  Chosen_Camera = Game_Platform.SwitchCamera?BackBuffer.camera_set[0]:&BackBuffer.camera;
                    //printf("Count from start of frame: %I64d\n",LastCycleCounts);
                  //MSG Message;
                    //NOTE: This is where receiving the message to change
                    // for any change in window
                    //INPUT
                  while(PeekMessageA(&(Game_Platform.Message), 0, 0, 0, PM_REMOVE)) {
                        if(Game_Platform.Message.message == WM_QUIT){
                            if(GlobalRunning){
                                GlobalRunning = false;
                                break;
                            }
                        }
                        DispatchMessage(&Game_Platform.Message);
                        TranslateMessage(&Game_Platform.Message);
                    }
                    if( MaxControllerCount > ArrayCount(Input->Controller)) {
                        MaxControllerCount = ArrayCount(Input->Controller);   
                    }

                    TrackMouseEvent(Chosen_Camera->mouse.mouseEvent);
//After polling event we update the positon of object
                    if(test_vampire_motion.object_speed.current_states != IDLE || test_vampire_motion.object_speed.current_states != IDLE){
                        move_(TimeSet.SPerFrame, &test_vampire_motion);
                    }

                    if (Load_Lib) {
                      if (AniLib != NULL) {
                         if (FreeLibrary(AniLib)) {
                         printf("Succeed free current lib\n");
                        } else {
                             printf("fail to free current lib\n");
                        }

                         if (CopyFileA("light32.dll",
                                     "light32_copy.dll", false)) {
                           printf("Succeed copy dll file\n");
                          AniLib = LoadLibraryA("light32_copy.dll");
                        }

//
                        // Animation
                        if (AniLib != NULL) {
                            printf("Succeed reload code and opengl function from dll\n");
                            
                            //bool success = gladLoadGLLoader((GLADloadproc)GetAllFunctionPointerFromLib);
//
                            //if(success){
                             //printf("Success load function pointer from AniLib\n");
                            //}else{
                             //printf("Falied loading function pointer from AniLib\n");
                            //}

                          UpdateCamera = (updateCa)GetProcAddress(AniLib, "updateCamera_");
                          //setup_pointlight = (setup_pointlight__)GetProcAddress(AniLib, "setup_pointlight_");
                          //Set_environmental_light =                      (Set_Light_)GetProcAddress(AniLib, "Set_environmental_light_");
                          move_ = (move_object_)GetProcAddress(AniLib, "move_object");
                          AutoAdjustCameraPos = (AutoAdjustCameraPos__)GetProcAddress(AniLib, "AutoAdjustCameraPos_");
                          Load_Textures_for_OpenGL = (Load_Textures_for_OpenGL__)GetProcAddress(AniLib, "Load_Textures_for_OpenGL_");                          
                          //LoadFont = (LoadFont__)GetProcAddress(AniLib, "LoadFont_");
                          //DrawFont = (DrawFont__)GetProcAddress(AniLib, "DrawFont_");
//setupMesh =
                          // reload and recompile shader code
                          for(B_shader_program* const &brush: BackBuffer.shaders_list)
                          {
                          brush->ReLoadShaderCode();
                          };
                          }
                        }
                      if(Load_Lib)
                          Load_Lib = false;
                      //Set_environmental_light(BackBuffer.shaders_list[0], &envir_light, Chosen_Camera);
                      //Set_environmental_light(BackBuffer.shaders_list[2], &envir_light, Chosen_Camera);
                    };
                    //UPDATE
                    // ================================================================
                    
                    // NOTE: Sounding Part
                    //WriteSoundToBuffer(&SoundBuffer, &SoundOutPut, SSamples);
                    //=====================================================================
                    //Update here                
                    //printf("Just before Game update and render\n");                
                    // Attach VAO
                    if(Game_Platform.transferNeed){
                        int count = 0;
                        while (count < 6){
                            count++;
                        };
                        if (count >= 5){
                            copyBufferData(&Game_Platform, &BackBuffer, &ScreenBuffer);
                            Game_Platform.transferNeed = false;                        
                            displayBufferData(&Game_Platform, &BackBuffer, &ScreenBuffer);
                            glViewport(Game_Platform.ClientRect.left, Game_Platform.ClientRect.top, ScreenBuffer.BitmapWidth, ScreenBuffer.BitmapHeight);
                        };
                    }

                    DeviceContext = GetDC(Game_Platform.Window);
                    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);             

                    //GameUpdateAndRender(&game_memory, BMPContent, NewInput, &State, &ScreenBuffer , &SoundBuffer, NULL);
 
                    //__rdtsc() is an intrinsict (the one which looked like a function call
                    //but it actually a hint to the compiler to a specific dissembly language intstruction)

/*                
                  S : Single 
                  I : Instruction
                  M : Multiple
                  D : Data
                
*/
#if DISPLAY_TIME                
                    //char Buffer[256];
                    // NOTE: The '%' is to decide the format of the next thing
                    // to print for example: %d is the 32 bit
                    //sprintf(Buffer, "%f Miliseconds/Frame, %f FPS, %f Ms/f \n ",
                            //MsPerFrame, FPS, MsPerFrame);
                    //OutputDebugStringA(Buffer);
#endif                
                    int ChosenAxis = 0;

                    //Ratio is based on miscalculated Msperframe
                    CalDelayedRatio(&DelayedRatio, &TimeSet, &BackBuffer);
                    ColorOffset += (float)0.1*DelayedRatio;
                    if(ColorOffset > 1.0f){
                        ColorOffset -= 1.0f;  
                    };
                    //if(Chosen_Camera->moved || Chosen_Camera->mouse.moved){
                   UpdateCamera(&BackBuffer.camera, &DelayedRatio);
                   UpdateCamera(BackBuffer.camera_set[0], &DelayedRatio);

                   for(B_shader_program* const &shader: BackBuffer.shaders_list){
                       if(shader->GetProgramID() != 7){
                           shader->use();
                           shader->setMat4("view", Chosen_Camera->view);
                           shader->setVec3( "ViewPos", Chosen_Camera->Position);
                           shader->setMat4("projection", Chosen_Camera->projection);
                       }
                    }

                    if(Chosen_Camera->mouse.Wheeled)
                    {
                        Chosen_Camera->projection = glm::perspective(glm::radians(Chosen_Camera->fov), (float)ScreenBuffer.BitmapWidth / (float)ScreenBuffer.BitmapHeight, 0.1f, 100.0f);

                        Chosen_Camera->mouse.Wheeled = false;
                    }

                    //for(const auto &shader: BackBuffer.shaders_list){

                    BackBuffer.shaders_list[0]->use();
                    //WorldToCamera = Chosen_Camera->view * dancing_vampire_core;
                    WorldToCamera = Chosen_Camera->view * test_vampire_motion.position;
                    BackBuffer.shaders_list[0]->setMat4("WorldToCamera", WorldToCamera);
                    glUseProgram(0);

                    //Set_Projection_View(&Game_Platform);

                    // Start to add some basic lighting to the model
                    //if (first_size) {
                        //printf("counter: %f\n", WaitTimeCounter);
                    //}
//
                    bool TimeToChangeAxis = false;
                    if(TimeSet.WaitTimeCounter >= 16.67f){                        
                       TimeSet.WaitTimeCounter = 0.0f;
                    } else {
                        TimeSet.WaitTimeCounter += (float)TimeSet.MsPerFrame;
                        //printf("WaitTimeCounter: %f\n", WaitTimeCounter);
                    }
                        // Update animation

                        updateDegreeInPi += 0.087f * DelayedRatio;                        
                        UpdatedAngle += 10.0f * DelayedRatio;

                        if(UpdatedAngle > 360.0f){
                            UpdatedAngle -= 360.0f;
                        };
                        
                        if(updateDegreeInPi > threshHold){
                            updateDegreeInPi -= threshHold;
                        };

                        if(TimeSet.ChangeAxisCounter >= 1000.0f){
                            int axisIndex = std::rand()%3;
                            randomRotateAxis = randomRotateAxis_(axisIndex);
                            TimeSet.ChangeAxisCounter = 0.0f;
                            //printf("ChangeAxisCounter: %f\n", WaitTimeCounter);
                            //
                            if(!TimeSet.TimeToChangeAxis){
                                TimeSet.TimeToChangeAxis = true;
                            }
                        } else {
                            TimeSet.ChangeAxisCounter += TimeSet.WaitTimeCounter;
                        }

                        
                        //BUGGY
                        basic_cube_core = glm::rotate(basic_cube_core, glm::radians(10.0f) * (float)DelayedRatio, randomRotateAxis);

                        // Move and check collision here
                        glm::vec3 move_right = glm::vec3(DelayedRatio>0.0f?(0.2f * DelayedRatio):0.02f, 0.0f, 0.0f);
                        glm::vec3 move_left = glm::vec3(DelayedRatio>0.0f?(-0.2f * DelayedRatio):-0.02f, 0.0f, 0.0f);

                        collided_ = check_collision(&obj1, &obj2);

                        if(obj1.position[3][0] < -15.0f){
                            if(along1)
                            along1 = !along1;
                        }

                        if(obj2.position[3][0] > 15.0f){
                            if(along2)
                            along2 = !along2;
                        }

                        if(collided_){
                            QueryPerformanceCounter(&(TimeSet.current_collided));

                            if(first_announce){
                                printf("Time from begin to first time collide is:%f\n", TimeSet.collided_time);
                            }

                            GetBackToPosBeforeHit(&obj1.position, &(*obj1.collide_list)[0].space);
                            obj1.collide_list->pop_back();

                            GetBackToPosBeforeHit(&obj2.position, &(*obj2.collide_list)[0].space);
                            obj1.collide_list->pop_back();

                            if(!along1)
                            along1 = !along1;
                            if(!along2)
                            along2 = !along2;
                        }

                        obj1.position = glm::translate(obj1.position, along1?move_left:move_right);//along?move_left:move_right
                        obj2.position = glm::translate(obj2.position, along2?move_right:move_left);//along?move_right:move_left
                        
                        
//=========================================================================

                        
                    //RENDER =====================================
                    if(showMsPF){
                        glm::mat4 test_mat;
                        glGetUniformfv(BackBuffer.shaders_list[4]->GetProgramID(), glGetUniformLocation(BackBuffer.shaders_list[4]->GetProgramID(), "projection"),&test_mat[0][0]);
                        printf("projection: %s\n",glm::to_string(test_mat).c_str());
                        if(Glyphs_Map.Glyph_list[0]->upside_down_bitmap){
                            printf("font data is existed\n");
                        }else{
                            printf("font data is NULL\n");
                        }
                        showMsPF = !showMsPF;
                    }

                    //DrawFont(&Game_Platform, &BackBuffer, BackBuffer.glData.PlaneVAOs, BackBuffer.shaders_list[4], &Glyphs_Map, test_string.c_str(), &rect);
                    DrawFont(&Game_Platform, &BackBuffer, BackBuffer.shaders_list[4], &Glyphs_Map, test_string.c_str(), &rect);


//========================================================================
                    drawTile(&BackBuffer.TileProperty, BackBuffer.shaders_list[2]);

                    //===============================================

                    bool32 dummyflag = false;
                    BackBuffer.shaders_list[1]->use();
                    glBindVertexArray(ScreenBuffer.glData.VAOs);
                    
                    BackBuffer.shaders_list[1]->setBool("short_color_change_", dummyflag);
                    BackBuffer.shaders_list[1]->setFloat("colorOffset", ColorOffset);
                    BackBuffer.shaders_list[1]->setMat4("model", basic_cube_core);

                    
                    //Draw colliding objects here
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                    
                    //drawTile(ScreenBuffer.glData.VAOs, basic_shader_->GetProgramID(), DelayedRatio, &UpdatedAngle, TimeToChangeAxis, &rollCubeMap);
                    if(collided_){

                        if(!short_color_change_)
                            short_color_change_=!short_color_change_;

                        collided_ = !collided_;
                    }

                    CalColliInterv(&TimeSet);
                    CalStaticColorDur(&TimeSet, short_color_change_);

                    if(TimeSet.color_switch_dur == 0.0f){
                        if(short_color_change_)
                            short_color_change_=!short_color_change_;
                    }

                    BackBuffer.shaders_list[1]->setBool("short_color_change_", short_color_change_);

                    BackBuffer.shaders_list[1]->setMat4("model", obj1.position);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

                    BackBuffer.shaders_list[1]->setMat4("model", obj2.position);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

                        //if(showMsPF){
                            //printf("along1 is %s, along2 is %s\n", along1?"true":"false", along2?"true":"false");
                            //printf("obj1 %f %f %f\n",
                                   //obj1.position[3][0],
                                   //obj1.position[3][1],
                                   //obj1.position[3][2]);
                            //printf("Collided time: %f, Collided switch: %s, Color switch: %s, duration: %f\n",TimeSet.collided_time, collided_?"true":"false", short_color_change_?"true":"false", TimeSet.color_switch_dur );
                        //}
//
                    glUseProgram(0);
                    glBindVertexArray(0);
                    GLuint brushID;

                    if(TimeToChangeAxis){
                        TimeToChangeAxis = false;
                    }

                    brushID = BackBuffer.shaders_list[3]->GetProgramID();
                    //Draw the backpack

                    BackBuffer.shaders_list[3]->use();
                    BackBuffer.shaders_list[3]->setMat4("model", backpack_core);
                    DDraw(backpack, &brushID);

                    glUseProgram(0);

                    //printf("Count by the end of frame: %I64d\n", EndCycleCounts);

// animation update and render ================================================
                    
                    BackBuffer.shaders_list[0]->use();
                    brushID = BackBuffer.shaders_list[0]->GetProgramID();


                    if(test_vampire_motion.object_speed.current_states != IDLE || test_vampire_motion.object_speed.current_states != IDLE || first_announce){
                        if(animator->GetCurrentTime() > danceAnimation->GetDuration() )
                        {
                            //PlayAnimation(animator, danceAnimation);
                            animator->playAnimation(danceAnimation);
                            if(showMsPF)
                                printf("animator current time: %f, animation duration: %f", animator->GetCurrentTime(), danceAnimation->GetDuration());

                        } else {
                            if(TimeSet.SPerFrame > 0.0f && TimeSet.SPerFrame < 0.004f ) {
                                //AniUpdate expect S per frame;
                                //AniUpdate(animator, &(TimeSet.SPerFrame));
                                animator->updateAnimationTime(&(TimeSet.SPerFrame));
                                //updateUBOData(animator);
                                //std::vector<glm::mat4>* Transform = animator->getFinalBoneMatrices();
                            }else{
                                TimeSet.SPerFrame = 0.0035f;
                                //AniUpdate(animator, &(TimeSet.SPerFrame));
                                animator->updateAnimationTime(&(TimeSet.SPerFrame));                                
                            }                        
                        }
                        AutoAdjustCameraPos(BackBuffer.camera_set[0], glm::vec3(test_vampire_motion.position[3]), TimeSet.SPerFrame);
                        BackBuffer.shaders_list[2]->use();
                        BackBuffer.shaders_list[2]->setVec3("lightPosition", glm::vec3(test_vampire_motion.position[3]));
                        glUseProgram(0);
                    }

                    //if(showMsPF){
                                //printf("animator current time: %f, animation duration: %f\n", animator->GetCurrentTime(), danceAnimation->GetDuration());
                                //printf("LastCounter: %lld, EndCounter: %Id, SPerFrame: %f\n", TimeSet.LastCounter.QuadPart, TimeSet.EndCounter.QuadPart, TimeSet.SPerFrame);
                                //printf("Delayed Ratio: %f\n", DelayedRatio);
                            //}

                   if(Transform_ == nullptr || Transform_ != animator->getFinalBoneMatrices())
                    Transform_ = animator->getFinalBoneMatrices();

                        int i = 0;
                        std::string matrixName_;

                            char index[1];
                            char indexx[2];

                            
#ifdef STD_140
                    //printf("Little beast flag on\n");
                    if(first_announce){
                        //setUpUBO(animator, &brushID);
//Setup UBO
                        glGenBuffers(1, &UBO);
                        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
                        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)* 52, NULL, GL_STREAM_DRAW);
                        glBindBuffer(GL_UNIFORM_BUFFER, 0);

                        // get Uniform location from ProgramID
                        GLint boneMatricIndex = glGetUniformBlockIndex(brushID, "finalBone");
                        // bind that location to global binding point (using glUniformBlock binding)
                        glUniformBlockBinding(brushID, boneMatricIndex, 1);
                        // then bind the binding point to UBO using glBindbufferrange
                        glBindBufferRange(GL_UNIFORM_BUFFER, 1, UBO, 0, sizeof(glm::mat4)* 52);
                    }

                    //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16 * 52, Transform);
                        //if(Transform_->size() > 0){
                            glBindBuffer(GL_UNIFORM_BUFFER, UBO);
                            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4)* 52, Transform_->data());
                            glBindBuffer(GL_UNIFORM_BUFFER, UBO);
                            glBindBuffer(GL_UNIFORM_BUFFER, 0);
                        //}

#else
                            if (Transform_ != nullptr){
                                //for(glm::mat4 const &matrix_ : Transform_) {
                                for(int i = 0; i < 52; i++) {

                                matrixName_ = "finalBoneMatrices[]";
                                    if(i<10){
                                        sprintf(index, "%d", i);
                                        matrixName_.insert(matrixName_.size()-1,
index); } else { sprintf(indexx, "%d", i);
                                        matrixName_.insert(matrixName_.size()-1,
indexx);
                                    }

                                    BackBuffer.shaders_list[0]->use();
                                    //BackBuffer.shaders_list[0]->setMat4(matrixName_.c_str(), matrix_);
                                    BackBuffer.shaders_list[0]->setMat4(matrixName_.c_str(), (*Transform_)[i]);

                                    if(!showMsPF){
                                        printf("uniform name %s :%s\n", matrixName_.c_str(), glm::to_string(matrix_).c_str());
                                        printf("uniform name %s :%s\n", matrixName_.c_str(), glm::to_string((*Transform_)[i]).c_str());
                                    }                                        
                                };
                            glUseProgram(0);
                                }else{
                                printf("Transform matrices is null\n");
                                };
                            

#endif                            
 
                            if(is_moving){
                            BackBuffer.shaders_list[0]->use();
                            WorldToCamera = Chosen_Camera->view * dancing_vampire_core;
                            WorldToCamera = Chosen_Camera->view * test_vampire_motion.position;
                            BackBuffer.shaders_list[0]->setMat4("WorldToCamera", WorldToCamera);                                
                            }

                    BackBuffer.shaders_list[0]->setBool("is_moving", is_moving);

//Why the later shader texture drawing work but not the one above
                    //Render
                    brushID = BackBuffer.shaders_list[0]->GetProgramID();
                    DDraw(dancing_vampire, &brushID);

                      //if(showMsPF){
                          //glm::vec2 TexCoordToShow;
                          //int TextureID;
                          //showUniformVarValuePerVertex(&UBO, &brushID, &dancing_vampire->meshes[0], false, false, false, false, false, true);
                          //showMsPF =!showMsPF;
                      //};
//
// animation update and render ================================================
 
                    //}

                    if (first_announce) {
                       first_announce = false;
                      }

                    Game_Input* Temp = NewInput;
                    NewInput = OldInput;  //???? still don't understand
                    OldInput = Temp;

                    bool test_swap_buffer;                    
                    test_swap_buffer = SwapBuffers(DeviceContext);

                    if (!test_swap_buffer) {
                       DWORD err = GetLastError();
                       char buffer[256];
                       sprintf_s(buffer, "SwapBuffers failed: %lu\n", err);
                       OutputDebugStringA(buffer);
                    }

                    ReleaseDC(Game_Platform.Window, DeviceContext);//maybe this one

                    //End count of frame time
                    if (!QueryPerformanceCounter(&(TimeSet.EndCounter))) {
                        printf("Failed to call performancecounter function\n");
                      };
                      TimeSet.EndCycleCounts = __rdtsc();
                }

          }


/*
MULPD -> real32 ==> 128 bits / 32 bits -> 4 real32 packs per registerMULPS -> real64 ==> 128 bits / 64 bits -> 2 real32 packs per register
*/
          //SlayAnimator(animator);
          //KillAninmation(danceAnimation);          
          //delete Transform_;
          Transform_ = nullptr;
          delete animator;
          animator = nullptr;
          delete danceAnimation;
          danceAnimation = nullptr;
          CleanUpandExit(&Game_Platform, &BackBuffer, &Glyphs_Map);
        }
        else{
            //TODO: Logging

            if(!IsWindow(Game_Platform.Window)){
                printf("Window is NULL\n");
                DWORD errorCode = GetLastError();
                char buffer[256] = {};
                FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof(buffer), NULL);
                printf("%s\n", buffer);
            }

            if (!GetProcessId(NULL)){
                ErrorExit();
            }
        }
    } else {
        //TODO: Logging
        DWORD errorCode = GetLastError();
        char buffer[256] = {};
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof(buffer), NULL);
        printf("%s\n", buffer);
    }
    //wglDeleteContext(BackBuffer.glData.openglRC);
    return (0);
}
