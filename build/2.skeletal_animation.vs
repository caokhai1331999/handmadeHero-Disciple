#version 330 core

const int MAX_BONE_INFLUENCE = 4;
const int MAX_BONES = 52;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneids;
layout (location = 6) in vec4 weights;

// uniform mat4 finalBoneMatrices [MAX_BONES];
uniform mat4 model;
uniform mat4 world_cube;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 WorldToCamera;

// uniform vec3 lightPos;
uniform bool is_moving;

#define NR_POINT_LIGHTS 2

// uniform vec3 FragPos;
// out vec2 TexCoord;
uniform vec3 ViewPos;
uniform vec3 lightPos;
uniform vec3 pointLight_Pos[NR_POINT_LIGHTS];

struct VS_OUT{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 tangent_light_pos;
    vec3 tangentViewPos;
    vec3 tangentpointLight_Pos[NR_POINT_LIGHTS];
};

out VS_OUT fs_in;

// uniform mat4[MAX_BONES] finalBoneMatrices;
// out vec2 TexCoord;
// out vec3 Normal;
// out vec3 FragPos;

layout (std140) uniform finalBone{
 mat4 finalBoneMatrices [MAX_BONES];
};


void main()
{

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0f)));
// re-othorgonalize TBN
    T = (T - dot(T, N) * N);
    vec3 bitangent_ = cross(aNormal, tangent);
    vec3 B = normalize(vec3(model * vec4(bitangent_, 0.0f)));
    
    // inverse of orthogonal matrix is its transpos form
    mat3 TBN = transpose(mat3(T, B, N));
    
    fs_in.TexCoord = aTexCoord;
    fs_in.FragPos = TBN * (vec3(model * vec4(aPos, 1.0f)));
    fs_in.tangentViewPos = TBN * ViewPos;
    fs_in.tangent_light_pos = TBN * lightPos;
	//aPos is the vertex position so the Fragment position is
	//the dot product of model and vertex Position (plus two vector)
	// This is to create a world space coor of fragment

    for(int i = 0; i < NR_POINT_LIGHTS; i++){;
        fs_in.tangentpointLight_Pos[i] = TBN * pointLight_Pos[i];
    }

    //// Normal = vec3(0.0f);
    vec4 totalPosition = vec4(0.0f);

mat4 totalBoneMatrices = mat4(1.0f);
mat4 localBone = mat4(1.0f);

vec3 Normal_;

bool clone_flag = is_moving;
for (int i = 0; i < MAX_BONE_INFLUENCE; i++){

if(boneids[i] == -1){
continue;
}

if(boneids[i] >= MAX_BONES){
totalPosition = vec4(aPos, 1.0f);
continue;
}

vec4 localPosition = finalBone.finalBoneMatrices[boneids[i]] * vec4(aPos, 1.0f);
 // vec4 localPosition = finalBoneMatrices[boneids[i]] * vec4(aPos, 1.0f);
totalPosition += localPosition * weights[i];

// vec3 localNormal = mat3(transpose(inverse(finalBone.finalBoneMatrices[boneids[i]]))) * aNormal;
// Normal_ += localNormal;

}



       // if(totalPosition.w == 0.0f){
       // totalPosition = vec4(-aPos, 1.0f);
       // }

// Normal = vec3(transpose(inverse(WorldToCamera)))*Normal_;

totalPosition.x *= -1;
totalPosition.y *= -1;
totalPosition.z *= -1;

bool notNull = false;

for (int i = 0; i < 4 ; i++){
    if(totalPosition[i]!=0.0f){
	notNull = true;
	break;
    }
}

//aPos is the vertex position so the Fragment position is
	//the dot product of model and vertex Position (plus two vector)
	// This is to create a world space coor of fragment

     // FragPos = vec3(model *vec4(aPos, 1.0f));
     // Actually the model matrix is what carried world's feature

     // TexCoord = aTexCoord;
     // FragPos = vec3(model *vec4(aPos, 1.0f));
     // Normal = vec3(transpose(inverse(WorldToCamera))) * aNormal;
     // Normal = vec3(transpose(inverse(WorldToCamera))) * aNormal;

if (world_cube[0][0] == 0) {
    if(totalPosition != vec4(0.0f)){
        gl_Position = projection * WorldToCamera * totalPosition;
    }else{
        gl_Position = projection * WorldToCamera * vec4(-aPos, 1.0f);
    }
} else {
    if(totalPosition != vec4(0.0f)){        
        gl_Position = projection * WorldToCamera * world_cube * totalPosition;
    } else {
        gl_Position = projection * WorldToCamera * world_cube * vec4(-aPos, 1.0f);
    }
}
/*
if(totalPosition != vec4(0.0f)){
    gl_Position = projection * WorldToCamera * totalPosition;     
}else{
    gl_Position = projection * WorldToCamera * vec4(-aPos, 1.0f);     
}
*/

};

