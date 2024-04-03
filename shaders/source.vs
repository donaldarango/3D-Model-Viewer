#version 330 core
    layout (location = 0) in vec3 a_Pos;
    layout (location = 1) in vec3 a_Color;
    layout (location = 2) in vec3 a_Normal;

    uniform mat4 u_ModelMatrix;
    uniform mat4 u_Perspective;

    out vec3 v_vertexColors;
    void main()
    {
      v_vertexColors = a_Color;
      vec4 newPosition = u_Perspective * u_ModelMatrix * vec4(a_Pos, 1.0f);
      // gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
      gl_Position = newPosition;

    }