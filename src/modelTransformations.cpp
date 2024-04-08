// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtx/string_cast.hpp>
#include "objLoader.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

// GLOBAL VARIABLES
float X_OFFSET = 0.0f;
float Y_OFFSET = 0.0f;
float Z_OFFSET = 0.0f;
float X_ROTATE = 0.0f;
float Y_ROTATE = 0.0f;
float Z_ROTATE = 0.0f;
float SCALE = 1.0f;
bool GPU_CALCULATIONS = true;

// functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void processShaderInput(GLFWwindow *window, GLuint* program);
// std::vector<GLfloat> readObjFile(const std::string& filename, std::vector<glm::vec3>& vertices, size_t& numVertices, std::vector<glm::vec3>& normals);
// void updateMinMax(float value, float &MIN, float &MAX);
std::string LoadShaderAsString(const std::string& filename);
GLuint createShaderProgramFromStrings(
    const char* vertex_shader_str,
    const char* fragment_shader_str );
void reloadShaderProgramFromFiles(
    GLuint* program,
    const char* vertex_shader_filename,
    const char* fragment_shader_filename );

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// loads shaders from filepath and converts to string
std::string LoadShaderAsString(const std::string& filepath) {
    std::string result = "";

    std::string line;
    std::ifstream myFile(filepath.c_str());

    if (myFile.is_open()){
        while (std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();
    }

    return result;
}

// function creates a shader program from a vertex and fragment shader
// vertex_shader_str - a null-terminated string of text containing a vertex shader
// fragment_shader_str - a null-terminated string of text containing a fragment shader
// returns a new, valid shader program handle, or 0 if there was a problem
// asserts on NULL parameters
GLuint createShaderProgramFromStrings(
  const char* vertex_shader_str,
  const char* fragment_shader_str ) {

  assert( vertex_shader_str && fragment_shader_str );

  GLuint shader_program         = glCreateProgram();
  GLuint vertex_shader_handle   = glCreateShader( GL_VERTEX_SHADER );
  GLuint fragment_shader_handle = glCreateShader( GL_FRAGMENT_SHADER );

  { // compile vertex shader and check for errors
    glShaderSource( vertex_shader_handle, 1, &vertex_shader_str, NULL );
    glCompileShader( vertex_shader_handle );
    int lparams = -1;
    glGetShaderiv( vertex_shader_handle, GL_COMPILE_STATUS, &lparams );

    if ( GL_TRUE != lparams ) {
      fprintf( stderr, "ERROR: vertex shader index %u did not compile\n",
        vertex_shader_handle );

      const int max_length = 2048;
      int actual_length    = 0;
      char slog[2048];
      glGetShaderInfoLog( vertex_shader_handle, max_length, &actual_length, slog );
      fprintf( stderr, "shader info log for GL index %u:\n%s\n", vertex_shader_handle,
        slog );

      glDeleteShader( vertex_shader_handle );
      glDeleteShader( fragment_shader_handle );
      glDeleteProgram( shader_program );
      return 0;
    }

  }

  { // compile fragment shader and check for errors
    glShaderSource( fragment_shader_handle, 1, &fragment_shader_str, NULL );
    glCompileShader( fragment_shader_handle );
    int lparams = -1;
    glGetShaderiv( fragment_shader_handle, GL_COMPILE_STATUS, &lparams );

    if ( GL_TRUE != lparams ) {
      fprintf( stderr, "ERROR: fragment shader index %u did not compile\n",
        fragment_shader_handle );

      const int max_length = 2048;
      int actual_length    = 0;
      char slog[2048];
      glGetShaderInfoLog( fragment_shader_handle, max_length, &actual_length, slog );
      fprintf( stderr, "shader info log for GL index %u:\n%s\n", fragment_shader_handle,
        slog );

      glDeleteShader( vertex_shader_handle );
      glDeleteShader( fragment_shader_handle );
      glDeleteProgram( shader_program );
      return 0;
    }

  }

  glAttachShader( shader_program, fragment_shader_handle );
  glAttachShader( shader_program, vertex_shader_handle );

  { // link program and check for errors
    glLinkProgram( shader_program );
    glDeleteShader( vertex_shader_handle );
    glDeleteShader( fragment_shader_handle );
    int lparams = -1;
    glGetProgramiv( shader_program, GL_LINK_STATUS, &lparams );

    if ( GL_TRUE != lparams ) {
      fprintf( stderr, "ERROR: could not link shader program GL index %u\n",
        shader_program );

      const int max_length = 2048;
      int actual_length    = 0;
      char plog[2048];
      glGetProgramInfoLog( shader_program, max_length, &actual_length, plog );
      fprintf( stderr, "program info log for GL index %u:\n%s", shader_program, plog );

      glDeleteProgram( shader_program );
      return 0;
    }

  }

  return shader_program;
}

void reloadShaderProgramFromFiles(
  GLuint* program,
  const char* vertex_shader_filename,
  const char* fragment_shader_filename ) {

  assert( program && vertex_shader_filename && fragment_shader_filename );

  std::string vertexShader = LoadShaderAsString(vertex_shader_filename);
  const char* vertexShaderSource = vertexShader.c_str();
  std::string fragmentShader = LoadShaderAsString(fragment_shader_filename);
  const char* fragmentShaderSource = fragmentShader.c_str();

  GLuint reloaded_program = createShaderProgramFromStrings(
    vertexShaderSource, fragmentShaderSource );

  if ( reloaded_program ) {
    glDeleteProgram( *program );
    *program = reloaded_program;
  }
}
  

int main()
{
    std::cout << "Enter filename: ";
    std::string filename = "";
    std::cin >> filename;
    filename += ".obj";

    std::string response = "";
    std::cout << "transform the geometry on GPU? (y/n): ";
    std::cin >> response;
    GPU_CALCULATIONS = strcmp(response.c_str(), "y") == 0 ? true : false;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();


    // load our shaders from filepath and convert to string
    // ------------------------------------
    std::string gouraudVertexShaderString = LoadShaderAsString("shaders/source_gouraud.vs");
    const char* gouraudVertexShaderSource = gouraudVertexShaderString.c_str();
    std::string gouraudFragmentShaderString = LoadShaderAsString("shaders/source_gouraud.fs");
    const char* gouraudFragmentShaderSource = gouraudFragmentShaderString.c_str();


    std::string depthVertexShaderString = LoadShaderAsString("shaders/source_depth.vs");
    const char* depthVertexShaderSource = depthVertexShaderString.c_str();
    std::string depthFragmentShaderString = LoadShaderAsString("shaders/source_depth.fs");
    const char* depthFragmentShaderSource = depthFragmentShaderString.c_str();

    std::string phongVertexShaderString = LoadShaderAsString("shaders/source_phong.vs");
    const char* phongVertexShaderSource = phongVertexShaderString.c_str();
    std::string phongFragmentShaderString = LoadShaderAsString("shaders/source_phong.fs");
    const char* phongFragmentShaderSource = phongFragmentShaderString.c_str();
    

    // build and compile our shader program
    // ------------------------------------
    GLuint shaderProgram = createShaderProgramFromStrings(gouraudVertexShaderSource, gouraudFragmentShaderSource);

    // // vertex shader
    // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // glCompileShader(vertexShader);
    // // check for shader compile errors
    // int success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if (!success)
    // {
    //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }
    // // fragment shader
    // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);
    // // check for shader compile errors
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if (!success)
    // {
    //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // // link shaders
    // unsigned int shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);
    // // check for linking errors
    // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // if (!success) {
    //     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    // }
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);
    

    std::string directory = "data/";
    std::cout << "Opening: " << filename << std::endl;
    std::string filepath = directory + filename;
    std::vector<glm::vec3> verticesVector;
    std::vector<glm::vec3> normals;
    size_t numVertices = 0;

    glm::mat4 preTransform(1.0f);

    //std::vector<GLfloat> vertices = readObjFile(filepath, verticesVector, numVertices, normals);
    std::vector<float> vertices = load_model_from_file(filepath, preTransform, numVertices);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() *  sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);

    // For vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (void*)0);
    glEnableVertexAttribArray(0);

    // For colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (void*)(sizeof(GL_FLOAT) * 3));    

    // For normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (void*)(sizeof(GL_FLOAT) * 6));    

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // create uniform light position variable
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

    GLint lightPosLocation = glGetUniformLocation(shaderProgram, "u_LightPos");
    if(lightPosLocation >= 0) {
        glUniformMatrix4fv(lightPosLocation, 1, false, glm::value_ptr(lightPos));
    }   
    else {
        std::cout << "could not find u_LightPos" << std::endl;
        exit(EXIT_FAILURE);
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        processShaderInput(window, &shaderProgram);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);

        float near = 0.1f;
        float far = 100.0f;

        // perpspective model matrix
        glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
                                                 (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                 near,
                                                 far);

        GLint perpectiveLocation = glGetUniformLocation(shaderProgram, "u_Projection");
        if(perpectiveLocation >= 0) {
            glUniformMatrix4fv(perpectiveLocation, 1, false, glm::value_ptr(perspective));
        }   
        else {
            std::cout << "could not find u_Projection" << std::endl;
            exit(EXIT_FAILURE);
        }

        // creating model transformation matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(X_OFFSET, Y_OFFSET, Z_OFFSET));
        model = glm::rotate(model, glm::radians(X_ROTATE), glm::vec3(1,0,0)); // x-axis rotation
        model = glm::rotate(model, glm::radians(Y_ROTATE), glm::vec3(0,1,0)); // y-axis rotation
        model = glm::rotate(model, glm::radians(Z_ROTATE), glm::vec3(0,0,1)); // z-axis rotation
        model = glm::scale(model, glm::vec3(SCALE, SCALE, SCALE)); // uniform scaling


        // update vertex data for CPU calculations if requested
        std::vector<float> vertexData;
        if (!GPU_CALCULATIONS) {
            for(int i = 0; i < numVertices; i++) {
                glm::vec4 v(vertices[(i*9)], vertices[(i*9)+1], vertices[(i*9)+2], 1.0f);
                glm::vec4 transform = model * v;

                // vertex data
                vertexData.push_back(transform.x);
                vertexData.push_back(transform.y);
                vertexData.push_back(transform.z);

                // color data    
                vertexData.push_back(vertices[(i*9)+3]);
                vertexData.push_back(vertices[(i*9)+4]);    
                vertexData.push_back(vertices[(i*9)+5]);

                // normal data
                vertexData.push_back(vertices[(i*9)+6]);
                vertexData.push_back(vertices[(i*9)+7]);    
                vertexData.push_back(vertices[(i*9)+8]);
            }

            model = glm::mat4(1.0f);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() *  sizeof(GL_FLOAT), vertexData.data(), GL_STATIC_DRAW);
        }


        // pass model matrix data to the GPU
        GLint modelLocation = glGetUniformLocation(shaderProgram, "u_Model");
        if(modelLocation >= 0) {
            glUniformMatrix4fv(modelLocation, 1, false, glm::value_ptr(model));
        }   
        else {
            std::cout << "could not find u_Model" << std::endl;
            exit(EXIT_FAILURE);
        }


        // Z' and Z CALCULATIONS
        if (!GPU_CALCULATIONS) {
            float z = vertexData[2];
            float z_prime = (-(near + far) * z + (near * far)) / -z;

            std::cout << "Z: " << z << std::endl;
            std::cout << "Z': " << z_prime << std::endl;
        }
        



        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()); 
        // glBindVertexArray(0); // unbind our VA no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // TRANSLATION
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        X_OFFSET += 0.01f;
        // std::cout << "x offset: " << X_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        X_OFFSET -= 0.01f;
        // std::cout << "x offset: " << X_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Y_OFFSET += 0.01f;
        // std::cout << "y offset: " << Y_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Y_OFFSET -= 0.01f;
        // std::cout << "y offset: " << Y_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
        Z_OFFSET += 0.25f;
        std::cout << "z offset: " << Z_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
        Z_OFFSET -= 0.25f;
        std::cout << "z offset: " << Z_OFFSET << std::endl;
    }

    // ROTATION
    // X-AXIS
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        X_ROTATE += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         X_ROTATE -= 1.0f;
    }
    // Y-AXIS
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Y_ROTATE -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Y_ROTATE += 1.0f;
    }
    // Z-AXIS
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        Z_ROTATE -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        Z_ROTATE += 1.0f;
    }

    // SCALING
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        SCALE -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        SCALE += 0.01f;
    }
        
}

void processShaderInput(GLFWwindow *window, GLuint *program) {
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        reloadShaderProgramFromFiles(program, "shaders/source_gouraud.vs", "shaders/source_gouraud.fs");
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        reloadShaderProgramFromFiles(program, "shaders/source_phong.vs", "shaders/source_phong.fs");
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        reloadShaderProgramFromFiles(program, "shaders/source_depth_z.vs", "shaders/source_depth_z.fs");
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        reloadShaderProgramFromFiles(program, "shaders/source_depth_z_prime.vs", "shaders/source_depth_z_prime.fs");
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}