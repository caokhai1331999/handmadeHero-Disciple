/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Camera.h"

//void InitCamera(Win32_OffScreen_Buffer* BackBuffer){
void InitCamera(Camera* camera, int Width, int Height, glm::vec3& Position_){

    //glm::vec3 Position = glm::vec3(-3.0f, -10.0f, -12.0f);
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 Right =  glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    //set camera view here
    //std::cout<<"View matrix from camera: "<<glm::to_string(BackBuffer.camera.view)<<std::endl;

    camera->fov = 45.0f;
    camera->projection = glm::perspective(glm::radians(camera->fov), (float)Dimens.Width / (float)Dimens.Height, 0.1f, 100.0f);
    *camera = Camera(Width, Height, Position_, Front, Right, Up);    

};

void UpdateCamera (Camera* camera, float* DelayRatio) {
    //Update the camera front based on yaw/bearing (around the y axe), pitch/elevation (around the x axe), roll (around the z axe)

    // First take the perspective from above we have the x is sin(yaw) and z is
    // cos(yaw)
    // Then do the similar look on pitch(y and z) we have y equal to sin(pitch)
    // and z is cos(pitch)
        if(camera->mouse.moved){
        
            //We'll use quaternion here;
            // by using slerp 
                // NOTE: Still don't know why we have to do oppositely for Y offset cause of
                // the y axe go down
         
            camera->mouse.MouseXOffset = camera->mouse.LastX - camera->mouse.xPos;
            camera->mouse.MouseYOffset = camera->mouse.LastY - camera->mouse.yPos;

            camera->mouse.LastX = camera->mouse.xPos;
            camera->mouse.LastY = camera->mouse.yPos;        

            if(!camera->move_perspective_instead){
                camera->Yaw += camera->mouse.MouseXOffset * camera->speed * SENSITIVITY;
                camera->Pitch += camera->mouse.MouseYOffset * camera->speed * SENSITIVITY;
            }else{
                camera->Yaw -= camera->mouse.MouseXOffset * camera->speed * SENSITIVITY;
                camera->Pitch -= camera->mouse.MouseYOffset * camera->speed * SENSITIVITY;
            }


            // Lock pitch at 120 degree
            if(camera->Pitch > 120.0f){
                camera->Pitch = 120.0f;
            }

            if(camera->Pitch < 0.0f){
                camera->Pitch = 1.0f;
            }

            //  But Yaw is all round degree
            // we can dynamically convert the degree by +- for 360
            if(camera->Yaw > 360.0f){
                camera->Yaw -= 360.0f;
            }
//
            if(camera->Yaw < 0.0f){
                camera->Yaw += 360.0f;
            }

            printf("Pitch is %f, Yaw is %f\n", camera->Pitch, camera->Yaw);
            // Direction based on mouse
            // yaw is formed by x and x1
            // similarly Pitch formed by y and y1

            //Got to understand this one.
            camera->Direction.x = glm::cos((glm::radians(camera->Yaw)));
            camera->Direction.y = glm::cos((glm::radians(camera->Pitch)));
            camera->Direction.z = glm::sin(glm::radians(camera->Yaw)) + glm::sin(glm::radians(camera->Pitch));
            
            // How to apply quaternion on this.
            // our purpose is to reduce accumulated orthormalizing error.
            //
            //
            //
            //std::cout<<"Direction is: "<<glm::to_string(camera->Direction)<<std::endl;
//Camera->LastFrameTime = FrameTime; ?? ??

        
            //if(camera->moved){
            // Cross product between the constant Up Vector which pointing upward and
            // cammera Direction in world space we have camera Right vector
        
            camera->mouse.moved = false;
    }

        camera->Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera->Direction));
        camera->Up = glm::normalize(glm::cross(camera->Right, camera->Direction));
        std::string UpContent = glm::to_string(camera->Up);
        //printf("%s \n", UpContent.c_str());
        glm::vec3 LookedPoint =  camera->Position + camera->Direction;


        camera->view = glm::lookAt(
//Camera Position
            camera->Position,
            //One above make camera rotate around center
            //!camera->focusCenter?LookedPoint:glm::vec3(-4.0f, 4.0f, 0.0f),
            //glm::vec3(-4.0f, 4.0f, 0.0f),
            LookedPoint,
//Position Where the camera is looking at (formular is AB-> = B - A(we can apply basic number subtraction rule))
            camera->Up
//How camera is oriented (Normalized up vector of camera NOT WORLD)
                                   );


    
    //}
         //camera->view = glm::mat4_cast(&Camera->orientation);
//===================================================
        ////ViewRotateCount++;
        //float CamX = sin(ViewRotateCount)*10.0f;
        //float CamZ = cos(ViewRotateCount)*10.0f;
        //BackBuffer.camera.view = glm::lookAt(glm::vec3(CamX, 0.0f, CamZ), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //setMat4(ScreenBuffer.glData.ProgramID, "view", BackBuffer.camera.view);
        

};

void updateCamera_ (Camera* camera, float* DelayRatio){
    UpdateCamera(camera, DelayRatio);
};

void InitCamera_ (Camera* camera, int width, int height, glm::vec3& Position_){
    InitCamera(camera, width, height, Position_);
}

void Zoom(Camera* camera, float offset){
    camera->fov -= offset;

    if(camera->fov < 1.0f){
        camera->fov = 1.0f;
    };

    if(camera->fov > 45.0f){
        camera->fov = 45.0f;
    };
}

void AutoAdjustCameraPos(Camera* camera, const glm::vec3& Object_Pos, float frameTime){
// Delve more about this.
    camera->Position = glm::mix(camera->Position, Object_Pos + glm::vec3(5.0f, -7.0f, 0.0f), 2.5f* frameTime);
    camera->Direction = Object_Pos - camera->Position;
    camera->fov = 70.0f;
};

void AutoAdjustCameraPos_(Camera* camera, const glm::vec3& Object_Pos, const float frameTime){
    AutoAdjustCameraPos(camera, Object_Pos, frameTime);
};

void Set_Mouse_Event_Tracker(TRACKMOUSEEVENT* mouseEventVar, HWND Window){
    mouseEventVar->cbSize = sizeof(TRACKMOUSEEVENT);
    mouseEventVar->dwFlags = TME_HOVER|TME_LEAVE;
    mouseEventVar->hwndTrack = Window;
    mouseEventVar->dwHoverTime = 1000;
};

void Set_Mouse_Event_Tracker_(TRACKMOUSEEVENT* mouseEventVar){
    Set_Mouse_Event_Tracker(mouseEventVar);
};
