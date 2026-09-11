/* ========================================================================
   $file: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey Muratori's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== */
#include "psuedo.h"

// Handshake cross check for collision between entities and entities with background

// TileMap contain array of positions in xz plane
// wherein x is analogous to y and z is similar to x in 2D coordinates
//

//===========================================================================
//NOTE: THIS BLOCK IS IN THE ATTEMPT OF CREATING ROOM/WORLD LIGHT
// We light up billboard/flat card using the environment/global light set up
// So We apply one big light source we call sun. And everyobjects have to apply
// its feature and change its look based on its position or intensity.

// Light
// May be loop over all of shader to apply this evironment 
   /*
unsigned int LoadCubeMap(const char* path){
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    // Create and assign the state for cube map texture here
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    // Then bind each image data for each texture faces
    unsigned char* data;
    unsigned int width, height, nrchannel;
    for(unsigned int i = 0; i < textures_faces.size(); i++){
        data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrchannel, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_X + i, 0, nrchannel, width, height, 0, nrchannel, GL_UNSIGNED_BYTE, data);
    }
    return cubemap;
}
*/
//========================PROFILER================================
// we got clock, we need to show them on opengl
// Casey store all pixel's data of one frame in large buffer and once pass them
// all to and it still worked.
//
//========================PROFILER================================
// TODO: How to apply material property to shader for drawing.
//===========================================================================

// Build world including group of mesh chunks.

// update :
// render : loop through entitys and used matched shader to draw;
//;

// Mesh
// coimponent:
//    . still objects: + rock 
//                     + tree
//    . moving entities : + river
//                        + animals
//    . weather elements : + wind
//                         + snow
//                         + mist
      //  components's properties: . collided volume
      //                           . moving information: T, S, R.
      //                           . primitive for drawing:(mesh)
      //                                       .. position, texture
//
//===> Render: feed shader : . fixed primitve data |  . VAO (contains shape's vertices's position, textcoord and normal)
      //                     . uniform offset data |  . shader
      //                     . camera pos for light|  . -//-

// chunk of meshes
//
//======================LIGHT_PART==========================

//============================================================
// Think about set this light group shrewly
//
void distribute_light(std::vector<general_light*>light_group){
    // Normal map global light here;
    (*light_group)[spot_light]->pos;
}

void set_light(glm::vec3* position, std::vector<general_light*>* light_group){
    light_group->basic_specs = ;
    light_group->direction = ;
    light_group->attenuation = ;
    light_group->spot_specs = ;
}

// Manually set light here.
//
void turn_on_light(std::vector<general_light*>* light_group){
    // so we have to manually set it here
    // Is there anyway to automatic this one;
}
//======================LIGHT_PART==========================

// We can replay game by thanks to this struct
struct game_state{
    void* BitmapMemory;
    int BitmapWidth;
    int BitmapHeight;
    int Pitch;
    int BitmapMemorySize;
// One pit of performance to use this one
    // be careful
    std::vector<C_Model*>Model_Collection;    
};

// ====================== Map constructing ===================================

// ZII(Zero Initializtion) is good
void init_volume_map(simple_map* map, std::vector<Mesh*>*Mesh_Group){
    map->height = ROOM_HEIGHT;
    map->breadth = ROOM_BREADTH;
    map->length = ROOM_LENGTH;
    map->map_size = (size_t)(2 * map->height * map->breadth * map->length)+1;

#if TEST_DYNAMICALLY_ALLOCATION
    map->map_content = (unsigned int*)VirtualAlloc(map->map_content, map_size);
#else
    map->map_content.reserve(map->map_size);
}

// We need a pre-created Texture group
// Sketch map and spawn entities
// layer of background and layer of moving entities

void sketch_room_map(simple_volume_map* map, Mesh* mesh_group){
    srand(time(NULL));
    // rational map sketcher here.
    // 1. first thing first we need to decide where can the character where is not
    int x = 0;
    int y = 0;
    int h = 0;
    //for(int y = 0; y < map->height; y++){
        //for(int x = 0; x < map->breadth; x++){
            //for(int z = 0; z < map->length; z++)
    // We distribute the drawn type here
    // These objects graphical differences are the shader and its position in room
    //
    // Total object will be drawn in displaying range in room
    // Understand data on the level of interger/float is an advantage
    uint8 Block_Object_Count = (uint8)((float)map->map_size * 0.2f);
    // all of the will be drawn obj is the under lit one
    uint8 total_objects = 10 + rand()%15;
    uint8 plane_size = map->breadth * map->length;
    bool32 plane_ids [plane_size] = {};

    // spawn moving objects here
    map->moving_obj_group.reserve((size_t)total_objects);
    uint8 object_count_down = total_objects;
    uint8 rand_id = 0;
    map_unit_specs temp_unit = {};

    while(object_count_down > 0){
        rand_id = rand()%plane_size;
        if(!plane_ids[rand_id]){
            plane_ids[rand_id] = 1;
            temp_unit.space_id = rand_id;
            map->moving_obj_group.push_back(map_unit_specs);
            object_count_down--;
        }else{
            continue;
        }
    }

    for(size_t int i = 0; i < map->size; i++)
    {
        // we do every single cube here which have the size of 1,1,1
        // have to be more specificly rational about this one
        // ground first then up
        // what to store OMG we just need to store the mesh ID
        // need to be more rational, can not let it randome like this
        x++;
        if(x == map->w -1 ){
            x -= map->w - 1;
            z++;
        };

        if(z == map->l-1){
            h++;
            z -= map->l-1;
        }

        // All right. First simple approach is we make the majority of room
        // passable then sprinkle few block objects on the way.
/*
#if TEST_DYNAMICALLY_ALLOCATION
        {
        *map->map_content++ = rand()%(mesh_group->size()-1);
        if(block_object_count > 0)
        *map->map_content++ = rand()%1;
        if(*map->map_content == 0)
            block_object_count--;
        }
#else
*/
        //{
        //static one
         map->map_content[i] = rand()%(mesh_group->size()-1);
         map->map_content[i] = rand()%(graphic_obj->VAOssize()-1);
         //moving one
         if(block_object_count > 0 && h == 0)
         map->map_content[i] = rand()%1;
         if(!(*map->map_content))
            block_object_count--;
        // how shrewly decide drawn type for these entities of map
         map->map_content[i+1] = (be_drawn_type)rand()%((uint8)2);        
        //}
    }
}
// So the map is the place to store entities position
// ====================== Map constructing ===========================

//===============LOOP_RUNNING_THEM=====================
// In Big inititalization : Init OpenGL
//                          Turn On Light
//                          Init Object: mesh, rigid body


void init_graphic (Win32_OffScreen_Buffer* BackBuffer, graphic_property* object_group, std::vector<general_light*> light_group, const basic_shape_vertices_data* vertices_groups = nullptr){
    // set light
    // where is the proper place for this light group: backbuffer or object group
    InitOpenGl(BackBuffer);
    // set VAOs array for data
// NOTE: How to parse data to vertex level???
    load_primitive_vertices_data(&vertice_store);
    // Light here
    turn_on_light(light_group);
    set_light_for_shader(light_group, object_group->shaders_list);
    // we need to set this one for every shader we have.
    // consider loading textures group separatedly here.

/*
    . Ambient Light - Kind of constance to store it
    . Point Light -
                   |
                   | take camera position as input
                   |
    . Spot Light  -
*/
    // set mesh;
    // rand map
    // set rigid body
    //. Init Position
    glm::vec3 pos = glm::vec3(0.0f);
    // Time to init a 2D map here for every single entity in the current volumm
}

void set_rigid_body(glm::vec3* init_pos){
    Init_Entity_Specs(body);
}

//How to make this compact and automatically??
//
// Hard code vertices data here
// ========WORKING===========
// NOTE: what the map give and what I can extrapolate from it??
// Do we need to construct the vertex data from space id?
// other from just mere position we still have normal and textcoord
// maybe this reason is solid enough

vertex_ spawn_polygon_vertex_data(plane* plane_info, uint8 vertexID, simple_volume_map* world_map/*something like plane dim*/){
    // mark the id of the vertex to intrapolate the normal and textcoord
    float origin_pos[3] = {0.0f, 0.0f, 0.0f};// we need to have
// pos from spaceID
    vertex.position = {(float)i%world_map->w, ((world_map->w*world_map->l))?(float)(plane_info->spaceID/(world_map->w*world_map->h)):0.0f, plane_info->spaceID>world_map->w?(plane_info->spaceID/world_map->w)%world_map->l:0.0f};
// normal and textcoords from pos /??/
    // NOTE: This one is tricky how do we know the vertex is above or below the visible drawn plane
    // the normal has to be related with the point light source
    uint8 original_vector[3] = {0, 1, 0};
    vertex.normal = glm::normalize(glm::cos(plane_info->face_angle_) * original_vector);
    // x(0 or 1), y(0 or -1)
    vertex.textcoord = {vertexID<2?0:1, vertexID%2!=0?0:-1};

    return vertex;
}
// NOTE: assign graphic id to the map unit to use it later for in mass instancing draw
// IMPORTANT!!: how can I draw out a sketch of object 
map_unit spawn_map_unit(uint16 spaceID, ){
//NOTE: one thing we are stupid about is that we have to know the plane or polygon we need to draw
    map_unit temp_unit;
    spawn_vertex();
}

void OutputLightingTexture(Voxel* table){
    uint16* lookupAt = table[][][];
}

// NOTE: On Working here
//=====================WORKING==============================
void construct_plane_vertices_data(vertex* vertices, const simple_volume_map* map){
    plane current_plane = {};
    
    while(marker < map->size - 1){
        map.push_back(map->spawn_map_unit(map->map_content[marker]));
    };
}
//=====================WORKING==============================
//
// Handshake cross check for collision between entities and entities with background
// TileMap contain array of positions in xz plane
// wherein x is analogous to y and z is similar to x in 2D coordinates
//

//===========================================================================
//NOTE: THIS BLOCK IS IN THE ATTEMPT OF CREATING ROOM/WORLD LIGHT
// We light up billboard/flat card using the environment/global light set up
// So We apply one big light source we call sun. And everyobjects have to apply
// its feature and change its look based on its position or intensity.

// Light
// May be loop over all of shader to apply this evironment 
   /*
unsigned int LoadCubeMap(const char* path){
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    // Create and assign the state for cube map texture here
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    // Then bind each image data for each texture faces
    unsigned char* data;
    unsigned int width, height, nrchannel;
    for(unsigned int i = 0; i < textures_faces.size(); i++){
        data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrchannel, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_X + i, 0, nrchannel, width, height, 0, nrchannel, GL_UNSIGNED_BYTE, data);
    }
    return cubemap;
}
*/
//========================PROFILER================================
// we got clock, we need to show them on opengl
// Casey store all pixel's data of one frame in large buffer and once pass them
// all to and it still worked.
//
//========================PROFILER================================
// TODO: How to apply material property to shader for drawing.
//===========================================================================

// Build world including group of mesh chunks.

// update :
// render : loop through entitys and used matched shader to draw;
//;

// Mesh
// coimponent:
//    . still objects: + rock 
//                     + tree
//    . moving entities : + river
//                        + animals
//    . weather elements : + wind  |
//                         + snow  |==> These are screen space posted-game effect
//                         + mist  |
// NOTE: We mainly use texture and (and may be light probing) due to the limit of
// indie game and hardward management 
      //  components's properties: . collided volume
      //                           . moving information: T, S, R.
      //                           . primitive for drawing:(mesh)
      //                                       .. position, texture
//
//===> Render: feed shader : . fixed primitve data |  . VAO
      //                     . uniform offset data |  . shader
      //                     . camera pos for light|  . -//-

// chunk of meshes
//======================LIGHT_PART==========================

//============================================================
// Think about set this light group shrewly

void set_light(glm::vec3* position, std::vector<general_light*>* light_group){
    light_group->basic_specs =;
    light_group->direction =;
    light_group->attenuation =;
    light_group->spot_specs =;
}

// Manually set light here.
void turn_on_light(std::vector<general_light*>* light_group){
    // so we have to manually set it here
    // Is there anyway to automatic this one;
}
//======================LIGHT_PART==========================

// We can replay game by thanks to this struct
struct game_state{
    void* BitmapMemory;
    int BitmapWidth;
    int BitmapHeight;
    int Pitch;
    int BitmapMemorySize;
// One pit of performance to use this one
    // be careful
    std::vector<C_Model*>Model_Collection;    
};

//===============LOOP_RUNNING_THEM=====================
// In Big inititalization : Init OpenGL
//                          Turn On Light
//                          Init Object: mesh, rigid body
//

void init_graphic (Win32_OffScreen_Buffer* BackBuffer, graphic_property* object_group, std::vector<general_light*> light_group, const basic_shape_vertices_data* vertices_groups = nullptr){
    // TODO: Arrange mesh based on the drawn type attached to it.;
    // HOW??? 
    InitOpenGl(BackBuffer);
    set_whole_mesh_data(vertices_groups, object_group);
    // Light here
    turn_on_light(light_group);
    set_light_for_shader(light_group, object_group->shaders_list);
/*
    . Ambient Light - Kind of constance to store it
    . Point Light -
                   |
                   | take camera position as inpt
                   |
    . Spot Light  -
*/
    glm::vec3 pos = glm::vec3(0.0f);
    // Time to init a 2D map here for every single entity in the current volumm
}

void set_rigid_body(glm::vec3* init_pos){
    Init_Entity_Specs(body);
}

//How to make this compact and automatically??
//
// calculate vertex's data(position, normal, texture coords) here
void spawn_vertex(uint8 size, /*what to pass here*/ ){

    float position[3];
    float normal[3];
    float texcoords[2];
    float stride[8];

    vertex.position = glm::vec3();
    // This is based on map and plane
    vertex.position.x;
    vertex.position.y;
    vertex.position.z;
}

void spawn_triangle(plane_type, map){
    ;
};
// ===============================Primitives_Data_Construction============================================
//
// First we hardcode it to see how it work, and then think about the proper approach of this one.

local_persist void Load_Textures_for_OpenGL(Graphic_Properties* Graphic_obj, const char* media_folder_path){
// Recursively loop over the folder and load group of textures here
    //graphic_obj->texture_collection.reserve(10); 
    texture_collection->reserve(10); 

    File_Manager folder_looker;
    std::string folder_path;

    // we just need to feed the folder path for stb_image to load texture's data
// Loop through all of the media folder for textures.
    PVOID OldValue = NULL;
    //Wow64DisableWow64FsRedirection(&OldValue);
    folder_path = media_folder_path;
    folder_path += "/*";
    folder_looker.handle = FindFirstFileA(folder_path.c_str(), &folder_looker.find_data);
    // which function I have to use to mark whether the maker reach the end of folder
    bool32 load_succeeded = true;
    if(folder_looker.handle != INVALID_HANDLE_VALUE){
        while(load_succeeded/*reach the end of folder*/ ){
            //stb_image(temp_image_content, path)
            // i
            // Win32 way        
                // TODO: Put a null check flag hee
                folder_path.clear();
            folder_path = media_folder_path;
            folder_path = folder_path + "/" + folder_looker.find_data.cFileName;

            if((folder_looker.find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (!string_contain(new string(folder_looker.find_data.cFileName),"."))){
             Graphic_Obj->M_mesh[].textures_group = load_textures_in_folder(&folder_looker, folder_path.c_str());
// TODO:This one
             Graphic_Obj->M_mesh[].
             Graphic_Obj->M_mesh[].name = folder_looker.find_data.cFileName;
            }

            load_succeeded = FindNextFileA(folder_looker.handle, &folder_looker.find_data);
//This cause thread thread violation NOTE: Be careful
            //if(!load_succeeded)
                //printf("Error: %s\n", loadCurrentErr());
        }
        //Wow64RevertWow64FsRedirection(OldValue);
    }
    FindClose(folder_looker.handle);
}

// change graphic_property* graphic_obj to vector of texture_group
extern "C" __declspec(dllexport) void Load_Textures_for_OpenGL_(Platform_Properties* Game_Platform, Graphic_Properties* Graphic_Obj, const char* media_folder_path){
    ReloadGLFunction(Game_Platform);
    Load_Textures_for_OpenGL(Graphic_Obj, media_folder_path);
};

// Still think about this one.=================================
void load_primitive_vertices_data(graphic_property* graphic_obj, shape_vertices_store* vertices_store){
    // data can be loaded from text file!!;
    // we need to copy whole data not just the address
float plane_vertices[] = {
    // positions
    // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    // x,    y,    z    //Normal    //TexCoord
    1.0f, -1.0f, 1.0f,  1.0f, 0.0f,
   -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 
   -1.0f,  1.0f, 1.0f,  0.0f, 1.0f,
                           
   -1.0f,  1.0f, 1.0f,  0.0f, 1.0f,                   
    1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,  1.0f, 0.0f
};

external unsigned int plane_indices[] =  {
//Even though the vertex 1, 0 will be reused but we have to feed them name for opengl just like this
    0, 1, 2, 2, 4, 0
};

vertices_store->plane.vertices = plane_vertices;
vertices_store->plane.indices = plane_indices;
set_each_Mesh_up(&vertices_store->plane);

//cube
float cube_vertices[] = {
      //Position           //Normal           //TexCoords
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                                                           
     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
`     
     -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                                                           
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                                                           
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
                                                           
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f   0.0f,  1.0f,  0.0f,  0.0f, 1.0f 
    };

int cube_indices[] = {
    0, 1, 2, 2, 4, 0,//0
    6, 7, 8, 7, 6, 11,
    12, 13, 14, 13, 12, 17,//2
    18, 19, 20, 20, 22, 18,
    24, 25, 26, 25, 24, 29,//4
    30, 31, 32, 32, 34, 30
};

vertices_store->cube.vertices = cube_vertices;
vertices_store->cube.indices = cube_indices;
set_each_Mesh_up(&vertices_store->cube);
// This one is for later
    //tile-liked shapes
float tile_vertices[] = {// vertex                    TextCoords
      //BACK FACE
      -0.5f,  0.375f, -0.5f,     0.0f, 0.0f,
       0.5f,  0.375f, -0.5f,     1.0f, 0.0f,
       0.5f,  0.5f,   -0.5f,     1.0f, 1.0f,
       0.5f,  0.5f,   -0.5f,     1.0f, 1.0f,
      -0.5f,  0.5f,   -0.5f,     0.0f, 1.0f,
      -0.5f,  0.375f, -0.5f,     0.0f, 0.0f,
       //FRONT FACE                         
      -0.5f,  0.375f,  0.5f,     0.0f, 0.0f,
       0.5f,  0.5f,    0.5f,     1.0f, 1.0f,
       0.5f,  0.375f,  0.5f,     1.0f, 0.0f,
       0.5f,  0.5f,    0.5f,     1.0f, 1.0f,
      -0.5f,  0.375f,  0.5f,     0.0f, 0.0f,
      -0.5f,  0.5f,    0.5f,     0.0f, 1.0f,
       // LEFT FACE                       
      -0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
      -0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
      -0.5f,  0.5f,   -0.5f,     1.0f, 1.0f,
      -0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
      -0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
      -0.5f,  0.375f,  0.5f,     0.0f, 0.0f,
       // RIGH0.375ACE                      
       0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
       0.5f,  0.5f,   -0.5f,     1.0f, 1.0f,
       0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
       0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
       0.5f,  0.375f,  0.5f,     0.0f, 0.0f,
       0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
       // BOTT0.375FACE                     
      -0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
       0.5f,  0.375f,  0.5f,     1.0f, 0.0f,
       0.5f,  0.375f, -0.5f,     1.0f, 1.0f,
       0.5f,  0.375f,  0.5f,     1.0f, 0.0f,
      -0.5f,  0.375f, -0.5f,     0.0f, 1.0f,
      -0.5f,  0.375f,  0.5f,     0.0f, 0.0f,
      //TOP                 ,               
      -0.5f,  0.5f,   -0.5f,     0.0f, 1.0f,
       0.5f,  0.5f,   -0.5f,     1.0f, 1.0f,
       0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
       0.5f,  0.5f,    0.5f,     1.0f, 0.0f,
      -0.5f,  0.5f,    0.5f,     0.0f, 0.0f,
      -0.5f,  0.5f,   -0.5f,     0.0f, 1.0f
};

unsigned int tile_indices[] = {
       0, 1, 2, 2, 4, 0,//0
       6, 7, 8, 7, 6, 11,
       12, 13, 14, 13, 12, 17,//2
       18, 19, 20, 20, 22, 18,
       24, 25, 26, 25, 24, 29,//4
       30, 31, 32, 32, 34, 30
};    
}

void set_each_Mesh_up(shape_vertices_data* shape_data){

        glGenVertexArrays(1, shape_data->VAO);
        glGenBuffers(1, shape_data->VBO);
        glGenBuffers(1, shape_data->EBO);
        glBindVertexArray(mesh->VAO);
        // Good thing about struct is that their memory is sequential for all its time
        //glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned int), (const void*)0);
        
        // Load data into vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, shape_data->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(*shape_data->vertices)*sizeof(float), VBOdata, GL_STATIC_DRAW);        
        // Time to set vertex attribute pointers
        // POSITION
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (const void*)0);

        // NORMAL
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (const void*)offsetof(struct vertex_, Normal));

        // TEXCOORDS
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_), (const void*)offsetof(struct vertex_, TexCoords));

        // TANGENT
        //glEnableVertexAttribArray(3);
        //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(struct vertex_, Tangent));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape_data->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*sizeof(*shape_data->indices), EBOdata, GL_STATIC_DRAW);        

        glBindVertexArray(0);        
}


// NOTE: Then we have something to do with the pre-lighted texels
// we will write a function that compute normal from texture's texel data
// (if we don't have any normal map) and compute light based on that one of the cheapest
//
// Apply textures group here
// How can this one being pickily among grahic types and still decent in term of
// performance??
void feed_shader_data(B_shader_program* shader){
    shader->set;
    shader->set;
    shader->set;
    shader->set;
};
// NOTE: So we load all but draw object pickily based on its be_drawn_type

switch(unit.graphic_type){
    //set textures
// Then feed the shader based on graphic type it got
    case map_drawn_element::In_World_Static :        
        break;

    case map_drawn_element::In_World_Moving :
        break;

    case map_drawn_element::Light_Effect :
        break;

    case map_drawn_element::Shading :
        break;

    case map_drawn_element::Pos_Game_Effect :
        break;

    default :
        break;
};

//TODO: Focus on this one. We draw objs dynamically based on its be_drawn_type
// NOTE: We got to divide more clearly among the shader(its job and type of input it will get)

void render_room_scene (Graphic_Properties* Graphic, simple_volume_map* world_map, Camera* chosen_camera, bool32 post_effect_on){    
    // first feed shader with mesh data (Store with VAO)
    // Then update relative position of the obj(with world, or just mere screen space with text)
    // Then use each brush(shader) in brush_set to draw object
    //NOTE: if(/*in view range*/)
    for(B_shader_program* const &shader: Graphic->shader){
        glBindVertexArray(Graphics->VAOS[/*mesh id*/]);
        shader->setMat4("projection", chosen_camera->projection); 
        shader->shader[i]->setMat4("view", chosen_camera->view);            
        // also bind matched texture here.        
    };
    glUseProgram(0);
    size_t i = 0;
    
    //So with the catatonic object we just pass its position as an unchanged
    //
    //layout data/ InstanceID
    // How to specify entities position just above the down-ground without making looked like they're floating
// NOTE: we got two loop here: O(n) + O(m)
// 
    for(moving_entity_specs* const &unit:world_map->moving_obj_group){
        // VAOs - shape vertice's data (at least: Position and TexCoord);
        if(unit->pos > /*specific number*/)
            continue;
        // I don't think the meshID is viable right now.
        glBindVertexArray(Graphics->mesh_group[unit->basic_unit_specs.MeshID].VAO);
        // Remember we already set all lights's basic specs before in the init graphics
        // add switch case for type here
        // Texture
        Graphic_Obj->shader[unit->basic_unit_specs.MeshID]->use();
       // Set Light
        // Choose among lights; here
        // View and Projection mat4 here
         Graphic_Obj->shader[unit->basic_unit_specs.MeshID]->setMat4("Postion["+to_string(unit->basic_unit_specs.space_id)+"]", unit->position);        Graphic_Obj->shader;        
        // Lastly, Entity world's position
         Graphic_Obj->shader[unit->basic_unit_specs.MeshID]->setVec3("Postion["+to_string(unit->basic_unit_specs.space_id)+"]", unit->position);        Graphic_Obj->shader;
        glDrawElements();
    }
//NOTE: Now the background unit.
    while(i < world_map->size){
// Each shader represent for one layer of effect at least.
        // Same process here for map-background-components here
        if(!world_map->map_content[i]){
            // By modding for that dimension we always have a number in its range
            // I think I done interpret the index to the entities postion in world space
            // Postion may be we use i * w * l * h
            glm::vec3 position = {(float)i%world_map->w, (i>(world_map->w*world_map->l))?(float)(i/(world_map->w*world_map->h)):0.0f, i>world_map->w?(i/world_map->w)%world_map->l:0.0f};       
            // now we decide how to add matched id in Graphic object

            if(map->map_content[i+1]){
                glBindVertexArray(Graphics->mesh_group[map->map_content[i]].VAO);
                // Remember we already set all lights's basic specs before in the init graphics
                // NOTE: How about textures and normal map lighting
                Graphic->shader[map->map_content[i+1]]->use();
                Graphic->shader[i]->setVec3("Postion["+to_string(i)+"]", position);
                // This line will draw object based on its be_drawn_type
                // Set Light
                // Choose among lights here
                glDrawElements();
                glBindVertexArray(0);
                glUseProgram(0);
            }
        }
    i+=2;
    };

    if(post_effect_on){
        glUseProgram(Graphic->shader[effect]);
        glBindVertexArray(Graphic->VAOs[effect]);
        glSetVec2(glm::vec2(ScreenH, ScreenW), "screen_dimesion");
        glDrawElements();
    }

    // Then draw post effect here.
    glUseProgram(0);
}
//===================================================================
// In group:
// How to manage these vertex's data efficiently
// when ever we load small mesh
// we can sample out the function
// Init: Set environment light, entity's pos, load texture, mesh: indices, vertices
//(pos, texcoord, normal, tangent, bitangent, boneid[4], weight[4])
// how do we RENDER: bind VAO -> use shader -> set uniform(camera's pos, entity's
//pos, light) -> draw element
// Think about this in group
// these are in one struct call game_state
 //
// These kind objects will reside inside something call window_game_state.
// render(&BackBuffer.Game_State)

//  ========================= Rigid Body =============================
class object{
private:
    std::string name;
    rigid_body* body;
public:
    void set_rigid_body();
    void move();
}
