#version 330 core

layout(location = 0) in vec3 aPos;
// layout(location = 1) in vec3 aNormal;
layout(location = 1) in vec2 aTextCoord;
//color per vetex (one point)
layout(location = 3) in vec3 VertexColor;

out vec2 TextCoord;
out vec4 FragColorr;
//out vec3 ModelPos;
out vec3 ViewPos_;
out vec3 FragPos;

uniform vec3 insertColor;
uniform vec3 ViewPos;

uniform mat4 view;
uniform mat4 world_cube;
uniform mat4 model;
uniform mat4 projection;

uniform vec2 offsets[900];

bool notNUll(mat4 matrix){
 for(int i = 0; i < 4; i++){
 for(int j = 0 ; j < 4; j++){
      if(matrix[i][j] != 0.0){
        return true;
     };     
   }
 }
 return false;
}

void main(){

// Place Model First -> calculate the local space to World/Perspective space
//                   -> Then Calculate latest space to clip space
    // So all we need is to feed the position array data based on ID
    // Then the vertex shader automatically know when to call each one
    // in order
    vec2 offset = offsets[gl_InstanceID];

//       ModelPos = vec3(aPos.x + offset.x, aPos.y, aPos.z + offset.y);
       vec3 modelPos = vec3(aPos.x + offset.x, aPos.y, aPos.z + offset.y);
       // We transform textCoord to world space
       // FragPos = vec3(aTextCoord.x * modelPos.x, aPos.y, aTextCoord.y * modelPos.z); 
       FragPos = modelPos; 


    if (world_cube[0][0] == 0)
       gl_Position = projection * view * vec4(vec3(aPos.x + offset.x, aPos.y, aPos.z + offset.y), 1.0f);
    else
       gl_Position = projection * view * world_cube * vec4(vec3(aPos.x + offset.x, aPos.y, aPos.z + offset.y), 1.0f);

    ViewPos_ = ViewPos;
       TextCoord = aTextCoord;
       FragColorr = vec4(VertexColor,1.0f);
}
