#version 330 core
    in vec4 vertexPosition;
    out vec4 FragColor;

    float near = 0.1f;
    float far = 100.0f;

    
    float LinearizeDepth(float depth) 
    {
        float z = depth * 2.0 - 1.0; // back to NDC 
        return (2.0 * near) / (far + near - z * (far - near));	  
    }

    void main()
    {             
        // float depth = LinearizeDepth(vertexPosition.z / vertexPosition.w); // divide by far for demonstration
        // float depth = LinearizeDepth(gl_FragCoord.z);

        // Z value on left side of screen
        float depth;
        if (vertexPosition.x > 0.5) {
          depth = gl_FragCoord.z * gl_FragCoord.w;
        }
        // Z' value on right side of screen
        else if (vertexPosition.x >= 0.5) {
          depth = gl_FragCoord.z;
        }

        // float depth = vertexPosition.z / vertexPosition.w;
        // float depth = vertexPosition.z;

        FragColor = vec4(vec3(depth), 1.0);
    }