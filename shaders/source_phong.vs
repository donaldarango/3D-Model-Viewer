#version 330 core
    layout (location = 0) in vec3 a_Pos;
    layout (location = 1) in vec3 a_Color;
    layout (location = 2) in vec3 a_Normal;

    uniform mat4 u_Model;
    uniform mat4 u_Projection; // perspective matrix
    uniform vec3 u_LightPos;

    out vec3 Normal;
    out vec3 Color;
    out vec3 Pos;
    out vec3 LightPos;

    void main()
    {
     
      vec3 modelViewVertex =  vec3(u_Model * vec4(a_Pos, 1.0)); // vertex pos in eye space
      vec3 modelViewNormal = vec3(u_Model * vec4(a_Normal, 0.0)); // normal in eye space

      // Passing variables to fragment shader
      Normal = modelViewNormal;
      Color = a_Color;
      Pos = modelViewVertex;
      LightPos = u_LightPos;
      
      gl_Position = u_Projection * vec4(modelViewVertex, 1.0);
    }