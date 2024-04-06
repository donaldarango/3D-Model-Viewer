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
      vec3 lightVector = normalize(LightPos - Pos);
  
      // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
      // pointing in the same direction then it will get max illumination.
      float diffuse = max(dot(Normal, lightVector), 0.1);
  
      // Add attenuation.
      diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
  
      // Multiply the color by the diffuse illumination level to get final output color.
      FragColor = vec4(Color * diffuse, 1.0f);
    }