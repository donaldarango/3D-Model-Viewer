#version 330 core
    precision mediump float;
    
    in vec3 Normal;
    in vec3 Color;
    in vec3 Pos;
    in vec3 LightPos;

    out vec4 FragColor;

    void main()
    {
  
      // Get a lighting direction vector from the light to the vertex.
      vec3 lightDir = normalize(LightPos - Pos);

      float ambient = 0.3;
  
      float diffuse = max(dot(Normal, lightDir), 0.1);
      diffuse = diffuse * 0.3;

      vec3 viewDir = normalize(vec3(0) - Pos);
      vec3 reflectDir = reflect(-lightDir, Normal);

      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
      float specular = 0.4 * spec;  
  
      // Multiply the color by the diffuse illumination level to get final output color.
      FragColor = vec4(Color * (ambient + diffuse + specular), 1.0f);
    }