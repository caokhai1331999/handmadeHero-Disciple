#if !defined(CAMERA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include "handmade.h"
#include "glm/glm.hpp" 
#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>/*

// Don't know shit about this
glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

// camera/view transformation
glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

*/

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.2f;
const float ZOOM        =  45.0f;

struct MouseInfo{
    int xPos;
    int yPos;

    int LastX;
    int LastY;

    int MouseXOffset;
    int MouseYOffset;

    float lastWheel;
    
    bool moved;
    bool Wheeled;
    LPTRACKMOUSEEVENT mouseEvent = NULL;

    MouseInfo(int xPoss = 0, int yPoss = 0, int ScreentWidth = 0, int ScreentHeight = 0): xPos(xPoss), yPos(yPoss){

        mouseEvent = new TRACKMOUSEEVENT();

        LastX = ScreentWidth/2;
        LastY = ScreentHeight/2;

        MouseYOffset = 0;
        MouseXOffset = 0;

        lastWheel = 0.0f;
        moved = false;
        Wheeled = false;
    }
    
};

struct Camera{
    bool32 moved;
    bool32 focusCenter;
    bool32 move_perspective_instead;

    float LastFrameTime;    
    // Euler/Tait-Bryan angles
    float Pitch;
    float Yaw;
    glm::quat orientation;
    //float Roll;// May be this one is unecessary

    // Camera attributes
    glm::vec3 Position;
    glm::vec3 WorldUp;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Direction;
    glm::vec3 Right;

    glm::mat4 projection;
    glm::mat4 view;
    
    // Options
    float fov;
    float sentivity;
    real64 speed;

    MouseInfo mouse;

    Camera(int ScreenWidth = 0, int ScreenHeight = 0, glm::vec3 position = glm::vec3(0.0f), glm::vec3 front = glm::vec3(0.0f), glm::vec3 right = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f)): Front(front), Position(position), Right(right), Up(up)
    {
        // We just could take Front as default
        // The Up have to be calculated from Right(Which is calculated from Direction)
        Direction = glm::vec3(-2.0f, 0.0f, 0.0f) - Position;
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        
        view = glm::lookAt(
            Position, //Camera Position
            Position + Direction, // Pointed object Position
            Up        // How camera is oriented (Normalized up vector)
                           );

        orientation = glm::quatLookAt(Direction, Up);

        fov = 45.0f;
        Pitch = PITCH;
        Yaw = YAW;

        focusCenter = false;
        moved = false;
        sentivity = 0.1f;
        LastFrameTime = 0.0f;

        speed = 2.5f;        
        mouse = MouseInfo(0, 0, ScreenWidth, ScreenHeight);
    }
};

void InitCamera(Camera* camera = nullptr, int width = 0, int height = 0, glm::vec3& Position_ = glm::vec3(0.0f));
void UpdateCamera(Camera* camera = nullptr, float* DelayRatio = nullptr);
void Zoom (Camera* camera, float offset);

// On working
void AutoAdjustCameraPos(Camera* camera = nullptr, const glm::vec3& Object_Pos = glm::vec3(0), float frameTime = 0);
void Set_Mouse_Event_Tracker(TRACKMOUSEEVENT* mouseEventVar = nullptr, HWND window = NULL);

extern "C" __declspec(dllexport) void Set_Mouse_Event_Tracker_(TRACKMOUSEEVENT* mouseEventVar = nullptr, HWND window = NULL);
typedef void (*Set_Mouse_Event_Tracker__)(TRACKMOUSEEVENT*, HWND);

extern "C" __declspec(dllexport) void AutoAdjustCameraPos_ (Camera* camera = nullptr, const glm::vec3& Object_Pos = glm::vec3(0), const float frameTime = 0);
typedef void (*AutoAdjustCameraPos__) (Camera*, const glm::vec3&, const float);

extern "C" __declspec(dllexport) void InitCamera_ (Camera* camera = nullptr, int width = 0, int height = 0, glm::vec3& Position_ = glm::vec3(0.0f));
typedef void (*InitCamera__) (Camera*, int, int, glm::vec3&);
//camera update wrapper
extern "C" __declspec(dllexport) void updateCamera_ (Camera* camera = nullptr, float* DelayRatio = nullptr);
typedef void (*updateCa) (Camera*, float*);

void ViewCamera(Camera* camera = nullptr){
    std::cout<<"View matrix: "<<glm::to_string(camera->view)<<std::endl;
    std::cout<<"Front vec: "<<glm::to_string(camera->Front)<<std::endl;
    std::cout<<"Right vec: "<<glm::to_string(camera->Right)<<std::endl;
    std::cout<<"Up vec: "<<glm::to_string(camera->Up)<<std::endl;
};
#define CAMERA_H
#endif
