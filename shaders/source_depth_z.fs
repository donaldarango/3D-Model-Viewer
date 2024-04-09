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
        float depth = LinearizeDepth(gl_FragCoord.z);
        FragColor = vec4(vec3(depth), 1.0);
    }