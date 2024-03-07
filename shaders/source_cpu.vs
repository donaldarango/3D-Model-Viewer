#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vertexColors;
    uniform vec4 u_vertexData[]; 
    out vec3 v_vertexColors;
    void main()
    {
       v_vertexColors = vertexColors;
       vec4 newPosition = u_vertexData;
       gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
    }