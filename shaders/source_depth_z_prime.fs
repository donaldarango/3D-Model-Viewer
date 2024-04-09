#version 330 core
    in vec4 vertexPosition;
    out vec4 FragColor;

    float near = 0.1f;
    float far = 100.0f;

    float LinearizeDepth(float depth) 
    {
        return (2.0 * near) / (far + near - (depth * 2 - 1.0) * (far - near));	  
    }

    void main()
    {

        // Z'
        float depth = ((gl_FragCoord.z) + 1) / 2;  // mapping [-1,1] -> [0,1]
        depth = LinearizeDepth(depth);

        FragColor = vec4(vec3(depth), 1.0);
    }