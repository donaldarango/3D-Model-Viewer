#version 330 core
    in vec3 v_vertexColors;
    out vec4 FragColor;

    float near = 0.1f;
    float far = 100.0f;

    float linearizeDepth(float depth)
   {
      return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
    }

    void main()
    {
      //  FragColor = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0f);
      FragColor = vec4(vec3(linearizeDepth(gl_FragCoord.z) / far ), 1.0f);
    }