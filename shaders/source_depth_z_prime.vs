#version 330 core
    layout (location = 0) in vec3 a_Pos;
    layout (location = 1) in vec3 a_Color;
    layout (location = 2) in vec3 a_Normal;

    uniform mat4 u_Model;
    uniform mat4 u_Projection; // perspective matrix
    uniform vec3 u_LightPos; 

    out vec4 vertexPosition;

    void main()
    {
       vec3 garbage = u_LightPos;
      
      // there is no view matrix, so just multiply model * position
      vec3 modelViewVertex = vec3(u_Model * vec4(a_Pos, 1.0)); // vertex pos in eye space

      // projection * model * position
      vertexPosition = vec4(modelViewVertex, 1.0);
      gl_Position = u_Projection * vec4(modelViewVertex, 1.0);
    } 