#if !defined(PSUEDO_H)
/* ========================================================================
 $File: $
   $Date: $
   $Revision: $
   $Creator: Cao Khai(Casey's disciple) $
   $Notice: (C) Copyright 2024 by Cao Khai, Inc. All Rights Reserved. $
   ======================================================================== 
*/

//=====================SIMPLE_FILE_HANDLE========================
struct File_Manager{
    WIN32_FIND_DATA find_data;
    HANDLE handle;
};
//=====================SIMPLE_FILE_HANDLE========================

std::vector<Entities*>*World_Entities_Trackers;
class basic_entity{
private:
    space_box collided_box;
    // This matrix will contain anything about
    //the entity in the made-up world like:
    //position, rotation, scale
    glm::mat4 world_present_model;    
};

struct GamePlay_Entity{

    int storage; // ??

    bool32 is_moving;
    bool32 is_dead;

    uint16 current_level;
    // actually skill is just some set of animation that deal specific damage;
    skills skill; // This one is fictious
    uint16 stregth;

    uint16 health;
    steerer driver;
    // this one haven't been realistic yet
    // This is supposed to calculate the next position of entities based on its
    // area surrounding.
}

//======================LIGHT_PART==========================

struct basic_light_specs{
    glm::vec3 ambient;
    glm::vec3 specular;
    glm::vec3 diffuse;
};

struct dir_light_specs{
    general_light basic_specs;
    glm::vec3 direction;
};

struct attenuation_specs{
    // For attenuation (Point Light)
    float constant;
    float linearTerm;
    float quadraticTerm;
};

struct spot_light_specs{
    // For spotlight effect
    // spotlight area defining angle(Phi) maybe with the different name such as cutoff
    float CutOff;
    // Now the smooth/soft edge effect
    float OuterCutOff;
};

struct general_light{
    // Basic specs
    basic_light_specs basic_specs;
    glm::vec3 direction;
    // For attenuation (Point Light)
    attenuation_specs attenuation;
    // For spotlight effect
    // spotlight area defining angle(Phi) maybe with the different name such as cutoff
    spot_light_specs spot_specs;
};
//======================LIGHT_PART==========================

//
//======================MESH_PART==========================
// So this can replace class function member effectively
// inside redering_platform.h/or graphic_api.h
typedef mesh_name uint8

enum primitive_shape{
    //TRIANGLE = (mesh_name)0,
    PLANE = (mesh_name)0,
    POLYGON = (mesh_name)1,
    CUBE_SHAPE = (mesh_name)2
};

//set primitives for mass drawing here
struct vertex_{
    float Positions[3];
    float Normals[3];
    float Texcoords[3];
};

struct triangle{
    vertex vertices[3];
    face_type_rhs face;
    float size;
};

struct plane{
    face_type_rhs face;
    float size;
    vertex vertices[6];
    index indices[6];
    // Should put it in here
    unsigned int texture;
};
// or we can store volumetric's data here
struct cube{
    vertex vertices[24];    
    index indices[24];
    float size;
    //unsigned int texture;
};

//======================MESH_PART==========================
// May be this one is no longer needed and be replaced by graphic type and vectors of shaders, lights_specs and textures.
// use this whenever I done the dynamic array
// how about light options
// should I put light here
//NOTE: A mesh is a groups of vertex's data(all possible) in order to draw shape
// union enable its member have the same memory location
struct{
    union v3{
        struct v3{
            float x, y;
        }
        float E[2];
        inline &operator *(float scale){
            if(scale != 0.0f){
                x *= scale;
                y *= scale;                
            }
        };
    }    
}
// function
inline v3
v3 v3(float x_val, float y_val, float z_val){
    v3 result = {};
    result.x = x_val ;
    result.y = y_val ;
    result.z = z_val ;
    return result;
}

struct M_Mesh{
    // how to arrange indices data for dynamically drawing complex shape??...
    // May be the Polygon or triangle to start
    // NOTE: Can we spawn other vertices from these first triangle/polygon one
    // There are two basic use cases: one is load vertices data from model(fbx, dae...), two is spawn one out
    std::vector<vertex> vertices_data;
    //vertex* vertices;
    std::vector<unsigned int> indices_data;
    //unsigned int* indices;
};

//===========================================================
// PURPOSE: Create a mechanism that draw multiple object of scene using
// instancing method and reuse available asset.

typedef uint8 be_drawn_type;

// The passable unit have to be aligned with each other
// This group of ID mark element for the engine to drawn accordingly
// This type will be drawn alot...

#define Still_or_Movable(x) \
    x ## _Static, \
    x ## _Moving

// NOTE: Let alone this later
// there still something we haven't figure out completely yet

enum light_type__:be_drawn_type{
    //How about normal mapping 
    basic_light = 0, //  diffuse + ambient + specular /or any simple approximated alternative
    emission = 2, // pass optional's light specs **
    normal_map_light = 3, // pass optional's light specs **
    shade = 4, // still haven't decide yet
};

enum shader_type__:be_drawn_type{
    //How about normal mapping 
    basic_light = 0, //  diffuse + ambient + specular /or any simple approximated alternative
    animating = 1, // pass optional's light specs **
};

// NOTE: where to put animation in graphic type
enum effect_type:be_drawn_type{
    //How about normal mapping 
    animating = 1 
};

//==========================================================
struct shape_vertices_data{
    float* data_; 
    unsigned int* VAO;
    unsigned int* VBO;
    unsigned int* EBO;
};

struct shape_vertices_store{
    shape_vertices_data triangle_data;
    shape_vertices_data plane_data;
    shape_vertices_data cube_data;
};

// we put the texture inside the mesh -> how to load the textures and store them inside the name matched M_mesh 

local_persist void Load_Textures_for_OpenGL(Graphic_Properties* Graphic_Obj, const char* media_folder_path);
extern "C" __declspec(dllexport) void Load_Textures_for_OpenGL_(Platform_Properties* Game_Platform, Graphic_Properties* Graphic_Obj, const char* media_folder_path = nullptr);
typedef void (*Load_Textures_for_OpenGL__) (Platform_Properties*, Graphic_Properties* , const char*);

// How can I make sure that all the shaders draw the same object

// light based on normal map
//struct environment_map{
    //bitmap* LOD[4];
//}
//
//
// 
//
//
// GROUP_EVERYTHING_UP
//
// How we do layer of effect on the same object
// How to first prototype the graphic property
// NOTE: How to come up with simple enough but effective name system for grouping light and shader
// together

struct graphic_object_type{
    light_type__ light_type_;
    shader_type__ shader_type_;
};

#define basic_draw_type (graphic_object_type){light_type__::basic_light, shader_type__::basic_light}
// more optional types if needed
#define basic_draw_type (graphic_object_type){light_type__::basic_light, shader_type___::basic_lighting_shader}

class graphic_property{
private:
    // This will be geometry collection
    // NOTE: instead of vector of shader.
    // we sort them out as set(each has particular purpose)
    std::vector<B_shader_program*>* shader_group;
    //TODO: need to arrange the mesh order based on drawn type for the sake of calling later
    std::vector<M_Mesh>* Mesh_Group;
    shape_vertices_store* current_shape_store;
public:
    update_(clock_set* clock);
    B_shader_program* get_shader(return shader);
    // model path is optional
    object(char *media_folder_path = nullptr, char *light_shader_name_ = nullptr)
        : name{name} {
        std::string shader_name = light_shader_name_;
        //search inside the folder for matched source for shader
        basic_light_shader = new B_shader_program(shader_name+".vs", shader_name+".fs", shader_path);       
    }
};

// ====================== Map constructing ===================================

typedef uint8 entity_type;
// We then bind single texture/simple model
// to specific object id
// This is for game play

// Be pragmatic, think about what its real use in shader and game play(collision)
// we need a tracker to watch all of entities in room
// NOTE: what I'm gonna do with map unit and entity
struct map_unit{ // when to use map_unit and when to use entity
// This is just should be an entity
    // store multiple vec3 is not cheap,we need to find the
    // alternatives
    // This is for choosing suitable mesh to draw
// NOTE: These will be used for fectching data from graphic object
    uint8* vertices_data_id;
    uint8* texture_id;
    uint8* mesh_id;
    // This graphic_style will be supposed to replace all other ID if we did well .
    graphic_object_type graphic_type;// include light and shader type
    // model space shape vertices data(pos, textcoord, normal)
//=======================================================
    // This is for position reconstruction
    uint16 space_id;
    bool32 tangible;
    bool32 movable;

    map_unit(){};
};

// NOTE: This one is for object that have rigid body

struct simple_volume_map{    
    // one is mesh type, the other is the position;
    std::vector<map_unit>downground_content;
    std::vector<map_unit>moving_obj_group;
    // the lowest layer of room is alway where the background object is
    // so from 0 -> length*breath contain the static object id
    uint8* map_content;// we haven't decide what this value hold yet???
    // mesh, texture, or light ID
    size_t map_size;
    // Volumme/Room 3D size in world space
    uint16 height;
    uint16 breadth;
    uint16 length;
};

// We spawn/randomize new map everytime we change room
// Think about this is the volume/room not the mere flat ground
//
// How to relatively define map size based one world
// first we have to know how big is the unit cube size to compare to the world
// current ground size is 1(x0.375)x30(x0.5)x30(x0.5 )

#define ROOM_HEIGHT (uint8)50
#define ROOM_BREADTH (uint8)50
#define ROOM_LENGTH (uint8)50

#define PSUEDO_H
#endif
