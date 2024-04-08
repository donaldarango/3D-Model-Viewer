#version 330 core
    layout (location = 0) in vec3 a_Pos;
    layout (location = 1) in vec3 a_Color;
    layout (location = 2) in vec3 a_Normal;

    uniform mat4 u_Model;
    uniform mat4 u_Projection; // perspective matrix
    uniform vec3 u_LightPos; 

    out vec3 v_vertexColors;

    void main()
    {
      
      vec3 modelViewVertex =  vec3(u_Model * vec4(a_Pos, 1.0)); // vertex pos in eye space
      vec3 modelViewNormal = vec3(u_Model * vec4(a_Normal, 0.0)); // normal in eye space

      vec3 lightDir = normalize(u_LightPos - modelViewVertex);

      float ambient = 0.3;

      float diffuse = max(dot(modelViewNormal, lightDir), 0.1);
      diffuse = diffuse * 0.3;

      vec3 viewDir = normalize(vec3(0) - modelViewVertex);
      vec3 reflectDir = reflect(-lightDir, modelViewNormal);

      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      float specular = 0.4 * spec; 

      v_vertexColors = a_Color * (diffuse + ambient + specular);

      gl_Position = u_Projection * vec4(modelViewVertex, 1.0);  
      

    }