#version 330 core
    precision mediump float;
    
    in vec3 Normal;
    in vec3 Color;
    in vec3 Pos;
    in vec3 LightPos;

    out vec4 FragColor;

    void main()
    {
      // Will be used for attenuation.
      float distance = length(LightPos - Pos);
  
      // Get a lighting direction vector from the light to the vertex.
      vec3 lightDir = normalize(LightPos - Pos);

      float ambient = 0.2;
  
      // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
      // pointing in the same direction then it will get max illumination.
      float diffuse = max(dot(Normal, lightDir), 0.1);
      // Add attenuation.
      diffuse = diffuse * (1.0 / (1.0 + (.75 * distance * distance)));

      vec3 viewDir = normalize(vec3(0) - Pos);
      vec3 reflectDir = reflect(-lightDir, Normal);

      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      float specular = 0.4 * spec;  
  
      // Multiply the color by the diffuse illumination level to get final output color.
      FragColor = vec4(Color * (ambient + diffuse + specular), 1.0f);
    }