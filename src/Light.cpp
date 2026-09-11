/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */

#include "Light.h"


static void IncreaseFontAlpha(const unsigned char* source, void* dest, const Glyph_Property* glyp){
    uint8 *Source = (uint8*)source;
    // move the destination pointer to the head of the last row
    uint8* DestRow = (uint8*)dest + (glyp->w * (glyp->h - 1));
    //Turn the bitmap upside down
    for(uint8 y = 0; y < glyp->h; y++){
        //uint32* Dest = (uint32*)DestRow;
        uint8* Dest = DestRow;
        for(uint8 x = 0; x < glyp->w; x++){
            uint8 alpha = *Source++;
            *Dest++ = ((alpha << 24)| // r
                       (alpha << 16)| // g
                       (alpha <<  8)| // b
                       (alpha <<  0));// a
            //*Dest++ = *Source++;
        }
            DestRow -= glyp->w;
    };
}
// Init
// Load
// Render}

void LoadFont(Glyph_Map* map, const char* path){
// Load File + Init
        debug_read_file_result* TTFfile = DEBUGReadFileWhole(path);
        stbtt_InitFont(&map->FontInfo, (unsigned char*)TTFfile->Content, stbtt_GetFontOffsetForIndex((unsigned char*)TTFfile->Content, 0));

        Glyph_Property* glyph;
        unsigned char* bitmap;

        for(char c  = 'A'; c < 'Z'; c++){
            glyph = (Glyph_Property*)malloc(sizeof(Glyph_Property));

            glyph-> i = 0;
            glyph-> j = 0;

            glyph-> c = c;

            // glyph->h and w haven't been defined yet.
            //bitmap = (unsigned char* )malloc(sizeof(unsigned char) * glyph->h * glyph->w);
            // Still have no clue about the right height scale of font
            bitmap = stbtt_GetCodepointBitmap(&map->FontInfo, 0, stbtt_ScaleForPixelHeight(&map->FontInfo, 230.0f), c, &glyph->w, &glyph->h, &glyph->Xoffset, &glyph->Yoffset);

            assert(bitmap);

            //glyph->upside_down_bitmap = VirtualAlloc(0, sizeof(uint32) * glyph->h * glyph->w, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
            glyph->upside_down_bitmap = (uint32* )malloc(sizeof(uint32) * glyph->h * glyph->w);
            IncreaseFontAlpha(bitmap, glyph->upside_down_bitmap, glyph);
            
            assert(glyph->upside_down_bitmap);
            map->Glyph_list.push_back(glyph);
            stbtt_FreeBitmap(bitmap, nullptr);
            //free(glyph);
        }

        for(char c  = 'a'; c < 'z'; c++){
            glyph = (Glyph_Property*)malloc(sizeof(Glyph_Property));

            glyph-> i = 0;
            glyph-> j = 0;

            glyph-> c = c;

            
            //bitmap = (unsigned char* )malloc(sizeof(unsigned char) * glyph->h * glyph->w);
            //float hscale = 128.0f * glyph->h/20;
            bitmap = stbtt_GetCodepointBitmap(&map->FontInfo, 0, stbtt_ScaleForPixelHeight(&map->FontInfo, 140.0f), c, &glyph->w, &glyph->h, &glyph->Xoffset, &glyph->Yoffset);

            assert(bitmap);

            //glyph->upside_down_bitmap = VirtualAlloc(0, sizeof(uint32) * glyph->h * glyph->w, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
            glyph->upside_down_bitmap = (uint32* )malloc(sizeof(uint32) * glyph->h * glyph->w);
            IncreaseFontAlpha(bitmap, glyph->upside_down_bitmap, glyph);
            
            assert(glyph->upside_down_bitmap);
            map->Glyph_list.push_back(glyph);
            //free(glyph);
            stbtt_FreeBitmap(bitmap, nullptr);
        }

        if(TTFfile->Content)
            DEBUGFreeFileMemory(TTFfile->Content);
// Set texture config
            glGenTextures(1, &(map->TextureID));
            glActiveTexture(GL_TEXTURE0+map->TextureID);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0+map->TextureID);

            glGenerateMipmap(GL_TEXTURE_2D);
            // can free temp_bitmap at this point
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//==========================================

        delete TTFfile;
        TTFfile = nullptr;        
}

glm::vec4 CalcGlypProperty(const glm::vec4* glyp_specs, const Rect_* rect){
    glm::vec4 font_specs;
    if(glyp_specs->x < rect->w){
        font_specs.x = (float)glyp_specs->x  + (glyp_specs->w * 0.3f);
        font_specs.y = (float)glyp_specs->y;
    }else{
        font_specs.x = 0;
        // specs.z is h
        font_specs.y = (float)glyp_specs->y - (glyp_specs->z + (glyp_specs->z * 0.3f)) ;
    }
    //font_specs.z = 1.0f;
    //font_specs.w = 1.0f;
    font_specs.z = glyp_specs->z;
    font_specs.w = glyp_specs->w;
    return font_specs;
}

//glm::mat4 CalcGlypProperty_(const glm::vec4* previous_glyp_specs, const Rect_* rect, const Glyph_Property* glyph){
    //glm::mat4 font_specs = glm::mat4(1.0f);
    //font_specs[3][2] = 0.0f;
//
    //if(previous_glyp_specs->x - previous_glyp_specs->w > rect->w){
        //font_specs[3][0] = (float)(((float)previous_glyp_specs->x + (float )previous_glyp_specs->w)/rect->w);
    //}else{
        //font_specs[3][0] = 0;
         //specs.z is h
        //font_specs[3][1] = (float)(((float)previous_glyp_specs->y - (float)previous_glyp_specs->z)/rect->h);
    //}
    //return font_specs;
//}

// How to create pos based on each of glyph was draw before
void DrawFont(const Platform_Properties* Game_Platform, const Win32_OffScreen_Buffer* BackBuffer, B_shader_program* shader, const Glyph_Map* map, const char* string, const Rect_* rect){
    // L, R, B, T
    glm::mat4 othorForGlyph = glm::ortho(0.0f, (float)30.0f, 0.0f, (float)20.0f);
    shader->use();
    shader->setMat4("projection", othorForGlyph);

    glActiveTexture(GL_TEXTURE0+map->TextureID);
    glBindTexture(GL_TEXTURE_2D, map->TextureID);

    int i = 0;
    int increased_width = 0;
    int decreased_height = rect->h - 100;

    Glyph_Property* glyp_p;
     glm::vec4 current_glyp_specs;
    //glm::mat4 current_glyp_specs_ = glm::mat4(1.0f);
    glm::mat4 test_mat;
    std::string name = "GlyphPos";
    const char* test_text = "Q u erty To See\0";
    int drawtime = 0;
    ////while(string[i] != '\0'){
    while(test_text[i] != '\0'){
        // underlying argument is the i (index)
        if((test_text[i] >= 'A' && test_text[i] <= 'Z') || (test_text[i] >= 'a' && test_text[i] <= 'z')){
            for(Glyph_Property* const &iter: map->Glyph_list){
                //if(iter->c == string[i]){
                if(iter->c == test_text[i]){
                    glyp_p = iter;
                    break;
                }
            }
        }else{
            increased_width += 40;
            if(increased_width >= rect->w){
                decreased_height -= 30;
                increased_width = 0;
            }
            i++;
            continue;
        }
        // wrong here
        // w -> 0
        //current_glyp_specs_ = CalcGlypProperty_(&glm::vec4((float)increased_width, (float)decreased_height, (float)glyp_p->h, (float)glyp_p->w), rect, glyp_p);

        //if(test_text[i] != ' '){
        current_glyp_specs = CalcGlypProperty(&glm::vec4((float)increased_width, (float)decreased_height, (float)glyp_p->w, (float)glyp_p->h), rect);
        //}

        //name = "GlyphPoses["+to_string(i)+"]";
        //printf("glypos %d is %s\n", i, glm::to_string(current_glyp_specs).c_str());

        //glGetUniformfv(shader->GetProgramID(), glGetUniformLocation(shader->GetProgramID(), "GlyphPos"), &test_mat[0][0]);
        //printf("fed mat:%s\n", glm::to_string(test_mat).c_str());

        if(glyp_p->upside_down_bitmap){
            current_glyp_specs = glm::vec4(current_glyp_specs.x * 0.025f, current_glyp_specs.y * 0.025f, current_glyp_specs.w * 0.025f, current_glyp_specs.z * 0.025f);
            shader->use();
            glBindVertexArray(Game_Platform->glData.PlaneVAOs);
            glActiveTexture(GL_TEXTURE0+map->TextureID);
            glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0+map->TextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, glyp_p->w, glyp_p->h, 0, GL_RG, GL_UNSIGNED_BYTE, glyp_p->upside_down_bitmap);
            shader->setVec4(name.c_str(), current_glyp_specs);
            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
            //drawtime++;
            //printf("Draw %d times\n", drawtime);
        }

        // currently
        increased_width += glyp_p->w;

        if(increased_width >= rect->w){
            decreased_height -= glyp_p->h;
            increased_width = 0;
        }

        i++;
    }

    glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
    glUseProgram(0);
}

void setup_pointlight(global_light* envir_light){
    glm::vec3 pointlight_Pos[] =
        {
            glm::vec3(-0.2f, -1.0f, -0.3f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            glm::vec3(0.5f, 0.5f, 0.5f)
        };

    envir_light->Point_Lights[0].position = glm::vec3(7.0f,  5.0f,  3.0f);
    envir_light->Point_Lights[0].ambient = glm::vec3( 0.05f, 0.05f, 0.05f);
    envir_light->Point_Lights[0].diffuse = glm::vec3( 0.8f, 0.8f, 0.8f);
    envir_light->Point_Lights[0].specular = glm::vec3( 1.0f, 1.0f, 1.0f);
    envir_light->Point_Lights[0].constant = 1.0f;
    envir_light->Point_Lights[0].linearTerm = 0.09f;
    envir_light->Point_Lights[0].quadraticTerm = 0.032f;
          
    envir_light->Point_Lights[1].position = glm::vec3(-5.0f, 5.0f, 3.0f);    
    envir_light->Point_Lights[1].ambient = glm::vec3(0.05f, 0.05f, 0.05f);        
    envir_light->Point_Lights[1].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);           
    envir_light->Point_Lights[1].specular = glm::vec3(1.0f, 1.0f, 1.0f);          
    envir_light->Point_Lights[1].constant = 1.0f;                     
    envir_light->Point_Lights[1].linearTerm = 0.09f;                  
    envir_light->Point_Lights[1].quadraticTerm = 0.032f;

    envir_light->dirLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    envir_light->dirLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    envir_light->dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);
}

void set_environmental_light(B_shader_program* shader, const global_light* envir_light, const Camera* camera){

    shader->use();

    std::string test_name {shader->GetProgramName()};
    shader->setFloat("material.shininess", 25.0f);

    glm::vec3 lightPos (-4.0f, 10.0f, -15.0f);
        shader->setVec3("lightPos", lightPos);

        shader->setVec3("dirLight.ambient", envir_light->dirLight.ambient);
        shader->setVec3("dirLight.diffuse", envir_light->dirLight.diffuse);
        shader->setVec3("dirLight.specular", envir_light->dirLight.specular);

    
    std::string light_name;
    for(int i = 0; i < 2; i++){

        light_name = "pointLight_Pos[]";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);

        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[i].position);
        light_name = "pointLights[].diffuse";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);

        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[i].diffuse);

        light_name = "pointLights[].ambient";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[i].ambient);

        light_name = "pointLights[].specular";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[i].specular);

        light_name = "pointLights[].constant";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 1.0f);

        light_name = "pointLights[].linearTerm";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.09f);

        light_name = "pointLights[].quadraticTerm";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.032f);

        light_name = "spotlight[0].direction";
        //light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setVec3(light_name.c_str(), glm::vec3(0.0f, 1.0f, 0.0f));

        light_name = "spotlight[].ambient";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        // Pointlight[0] data  is wrong somewhere
        //shader->setVec3(light_name.c_str(), envir_light->Point_Lights[0].ambient);
        shader->setVec3(light_name.c_str(), glm::vec3(0.5f));

        light_name = "spotlight[].diffuse";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[0].diffuse);

        light_name = "spotlight[].specular";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setVec3(light_name.c_str(), envir_light->Point_Lights[0].specular);

        light_name = "spotlight[].constant";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 1.0f);

        light_name = "spotlight[].linearTerm";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.09f);

        light_name = "spotlight[].quadraticTerm";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.032f);

        light_name = "spotlight[].CutOff";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.3f);

        light_name = "spotlight[].OuterCutOff";
        light_name.insert(light_name.cbegin() + light_name.find_first_of('[') + 1, '0'+i);
        shader->setFloat(light_name.c_str(), 0.4f);
    }
    glUseProgram(0);
};
// for dll export
/*
void LoadFont_(platform_api* platform, Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, Glyph_Map* map , const char* path){
    //begin_ticket_mutex(&BackBuffer->ticket);
            //platform->reloadGLFuncPointer(BackBuffer);
    ReloadGLFunction(Game_Platform, BackBuffer);
    //end_ticket_mutex(&BackBuffer->ticket);

    LoadFont(BackBuffer, map, path);
};


void setup_pointlight_(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, global_light* envir_light){
    //platform->reloadGLFuncPointer(&BackBuffer);
    ReloadGLFunction(Game_Platform, BackBuffer);
    setup_pointlight(envir_light);
}

void Set_environmental_light_(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, B_shader_program* shader, const global_light* envir_light, const Camera* camera){
    ReloadGLFunction(Game_Platform, BackBuffer);
    set_environmental_light(shader, envir_light, camera);
};

 
void DrawFont_(Platform_Properties* Game_Platform, Win32_OffScreen_Buffer* BackBuffer, const GLuint VAO, B_shader_program* shader, const Glyph_Map* map, const char* string, const Rect_* rect){
    //platform->reloadGLFuncPointer(BackBuffer);
    ReloadGLFunction(Game_Platform, BackBuffer);
    DrawFont(BackBuffer, shader, map, string, rect);
}

*/
