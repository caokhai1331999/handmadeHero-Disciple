#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneids;
layout (location = 6) in vec4 weights;

#define NR_POINT_LIGHTS 2
uniform vec3 ViewPos;
uniform vec3 lightPos;
uniform vec3 pointLight_Pos[NR_POINT_LIGHTS];

uniform mat4 model;
uniform mat4 world_cube;
uniform mat4 view;
uniform mat4 projection;

// out vec3 Normal;
// out vec3 FragPos;
out vec2 TexCoord;

out VS_OUT{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 tangent_light_pos;
    vec3 tangentViewPos;
    vec3 tangentpointLight_Pos[NR_POINT_LIGHTS];
}vs_out;


void main()
{

    if (world_cube[0][0] == 0)
        gl_Position = projection * view * model * vec4(-aPos, 1.0f);
    else
        gl_Position = projection * view * world_cube * model * vec4(-aPos, 1.0f);
    
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0f)));

    T = (T - dot(T, N) * N);
    
    vec3 bitangent_ = cross(aNormal, tangent);
    vec3 B = normalize(vec3(model * vec4(bitangent_, 0.0f)));
    
    // inverse of orthogonal matrix is its transpos form
    mat3 TBN = transpose(mat3(T, B, N));
    
    vs_out.TexCoord = aTexCoord;
    vs_out.FragPos = TBN * (vec3(model * vec4(aPos, 1.0f)));
    vs_out.tangentViewPos = TBN * ViewPos;
    vs_out.tangent_light_pos = TBN * lightPos;
	//aPos is the vertex position so the Fragment position is
	//the dot product of model and vertex Position (plus two vector)
	// This is to create a world space coor of fragment

    for(int i = 0; i < NR_POINT_LIGHTS; i++){;
        vs_out.tangentpointLight_Pos[i] = TBN * pointLight_Pos[i];
    }
    
     // FragPos = vec3(model *vec4(aPos, 1.0f));
     // Normal = vec3(transpose(inverse(view * model))) * aNormal;
     TexCoord = aTexCoord;
}
