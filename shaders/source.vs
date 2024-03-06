#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 vertexColors;
    uniform mat4 u_modelMatrix;
    out vec3 v_vertexColors;
    void main()
    {
       v_vertexColors = vertexColors;
       vec4 newPosition = u_modelMatrix * vec4(aPos, 1.0f);
       gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
    }