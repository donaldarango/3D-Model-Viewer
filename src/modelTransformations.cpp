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

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

// GLOBAL VARIABLES
float X_OFFSET = 0.0f;
float Y_OFFSET = 0.0f;
float X_ROTATE = 0.0f;
float Y_ROTATE = 0.0f;
float Z_ROTATE = 0.0f;
float SCALE = 1.0f;

struct Vertex {
    float x, y, z;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<GLfloat> readObjFile(const std::string& filename, std::vector<Vertex>& vertices, size_t& numVertices);
float normalize(float value, float minVal, float maxVal);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 vertexColors;\n"
    "uniform mat4 u_modelMatrix;\n"
    "out vec3 v_vertexColors;\n"
    "void main()\n"
    "{\n"
    "   v_vertexColors = vertexColors;\n"
    "   vec4 newPosition = u_modelMatrix * vec4(aPos, 1.0f);\n"
    "   gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, 1.0f);\n"
    "}\0";
// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//     "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 v_vertexColors;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0f);\n"
    "}\n\0";

float normalize(float value, float minVal, float maxVal) {
    return (value - minVal / (maxVal - minVal));
}

int main()
{
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


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    

    // std::string directory = "data/";
    // Implement user input to read filename
    // std::string filename = "dolphins.obj";
    // std::string filepath = directory + filename;
    std::string filepath = "/Users/donny/Documents/Model Transformations/data/head.obj";
    std::vector<Vertex> verticesVector;
    size_t numVertices = 0;
    std::vector<GLfloat> vertices = readObjFile(filepath, verticesVector, numVertices);


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() *  sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // For colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(GL_FLOAT) * 6, (void*)(sizeof(GL_FLOAT) * 3));    

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(X_OFFSET, Y_OFFSET, 0.0f));
        model = glm::rotate(model, glm::radians(X_ROTATE), glm::vec3(1,0,0)); // x-axis rotation
        model = glm::rotate(model, glm::radians(Y_ROTATE), glm::vec3(0,1,0)); // y-axis rotation
        model = glm::rotate(model, glm::radians(Z_ROTATE), glm::vec3(0,0,1)); // z-axis rotation
        model = glm::scale(model, glm::vec3(SCALE, SCALE, SCALE)); // uniform scaling

        GLint modelLocation = glGetUniformLocation(shaderProgram, "u_modelMatrix");
    
        if(modelLocation >= 0) {
            glUniformMatrix4fv(modelLocation, 1, false, glm::value_ptr(model));
        }
        else {
            exit(EXIT_FAILURE);
            std::cout << "could not find u_modelMatrix" << std::endl;
        }

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, numVertices); 
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
        std::cout << "x offset: " << X_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        X_OFFSET -= 0.01f;
        std::cout << "x offset: " << X_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Y_OFFSET += 0.01f;
        std::cout << "y offset: " << Y_OFFSET << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Y_OFFSET -= 0.01f;
        std::cout << "y offset: " << Y_OFFSET << std::endl;
    }

    // ROTATION
    // X-AXIS
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        X_ROTATE += 1.0f;
        std::cout << "x rotate: " << X_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         X_ROTATE -= 1.0f;
        std::cout << "x rotate: " << X_ROTATE << std::endl;
    }
    // Y-AXIS
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Y_ROTATE -= 1.0f;
        std::cout << "y rotate: " << Y_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Y_ROTATE += 1.0f;
        std::cout << "y rotate: " << Y_ROTATE << std::endl;
    }
    // Z-AXIS
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        Z_ROTATE -= 1.0f;
        std::cout << "z rotate: " << Z_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        Z_ROTATE += 1.0f;
        std::cout << "z rotate: " << Z_ROTATE << std::endl;
    }

    // SCALING
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        SCALE -= 0.0001f;
        std::cout << "scale: " << SCALE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        SCALE += 0.0001f;
        std::cout << "scale: " << SCALE << std::endl;
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

std::vector<GLfloat> readObjFile(const std::string& filename, std::vector<Vertex>& vertices, size_t& numVertices) {    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return std::vector<GLfloat>();
    }

    std::vector<GLfloat> faceData;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex vertex;
            float x,y,z;
            iss >> x >> y >> z;

            // normalize the values to fit within the viewport
            vertex.x = normalize(x, -1.0f, 1.0f);
            vertex.y = normalize(y, -1.0f, 1.0f);
            vertex.z = normalize(z, -1.0f, 1.0f);
            
            vertices.push_back(vertex);
            numVertices++;
        } else if (prefix == "f") {
            int v1, v2, v3;
            char slash, trash;
            // iss >> v1 >> slash >> trash >> slash >> trash;
            // iss >> v2 >> slash >> trash >> slash >> trash;
            // iss >> v3 >> slash >> trash >> slash >> trash;
            iss >> v1;
            iss >> v2;
            iss >> v3;
            // Assuming vertices are 1-indexed in .obj files, convert to 0-indexed
            v1--; v2--; v3--;
            // vertex 1 location
            faceData.push_back(vertices[v1].x);
            faceData.push_back(vertices[v1].y);
            faceData.push_back(vertices[v1].z);
            // vertex 1 color
            // faceData.push_back(1.0f);
            // faceData.push_back(0.0f);
            // faceData.push_back(0.0f);
            faceData.push_back(vertices[v1].x);
            faceData.push_back(vertices[v1].y);
            faceData.push_back(vertices[v1].z);
            // vertex 2 location
            faceData.push_back(vertices[v2].x);
            faceData.push_back(vertices[v2].y);
            faceData.push_back(vertices[v2].z);
            // vertex 2 color
            // faceData.push_back(0.0f);
            // faceData.push_back(1.0f);
            // faceData.push_back(0.0f);
            faceData.push_back(vertices[v2].x);
            faceData.push_back(vertices[v2].y);
            faceData.push_back(vertices[v2].z);

            // vertex 3 location
            faceData.push_back(vertices[v3].x);
            faceData.push_back(vertices[v3].y);
            faceData.push_back(vertices[v3].z);
            // vertex 3 color
            // faceData.push_back(0.0f);
            // faceData.push_back(0.0f);
            // faceData.push_back(1.0f);
            faceData.push_back(vertices[v3].x);
            faceData.push_back(vertices[v3].y);
            faceData.push_back(vertices[v3].z);
        }
    }
    file.close();
    
    return faceData;
}