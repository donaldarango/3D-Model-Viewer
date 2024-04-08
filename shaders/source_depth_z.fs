#version 330 core
    in vec4 vertexPosition;
    out vec4 FragColor;

    float near = 0.1f;
    float far = 100.0f;

    
    float LinearizeDepth(float depth) 
    {
        // float z = depth * 2.0 - 1.0; // back to NDC 
        return (2.0 * near) / (far + near - depth * (far - near));	  
    }

    void main()
    {             

        // Z
        float depth = (near * far) / (-(vertexPosition.z) + (near + far));

        FragColor = vec4(vec3(depth), 1.0);
    }