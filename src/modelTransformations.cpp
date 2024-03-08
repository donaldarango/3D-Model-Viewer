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
#include <regex>

// GLOBAL VARIABLES
float X_OFFSET = 0.0f;
float Y_OFFSET = 0.0f;
float X_ROTATE = 0.0f;
float Y_ROTATE = 0.0f;
float Z_ROTATE = 0.0f;
float SCALE = 1.0f;
bool GPU_CALCULATIONS = true;

// functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<GLfloat> readObjFile(const std::string& filename, std::vector<glm::vec3>& vertices, size_t& numVertices);
void updateMinMax(float value, float &MIN, float &MAX);
std::string LoadShaderAsString(const std::string& filename);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

void updateMinMax(float value, float &min, float &max) {
    if (value > max)
        max = value;
    if (value < min)
        min = value;
}

int main()
{
    std::cout << "Enter filename: ";
    std::string filename = "";
    std::cin >> filename;

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

    // build and compile our shader program
    // ------------------------------------
    std::string vertexShaderString = LoadShaderAsString("shaders/source.vs");
    const char* vertexShaderSource = vertexShaderString.c_str();

    std::string fragmentShaderString = LoadShaderAsString("shaders/source.fs");
    const char* fragmentShaderSource = fragmentShaderString.c_str();

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
    

    std::string directory = "data/";
    filename += ".obj";
    std::cout << "Opening: " << filename << std::endl;
    std::string filepath = directory + filename;
    // std::string filepath = "/Users/donny/Documents/Model Transformations/data/dolphins.obj";
    std::vector<glm::vec3> verticesVector;
    size_t numVertices = 0;
    std::vector<GLfloat> vertices = readObjFile(filepath, verticesVector, numVertices);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() *  sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (void*)0);
    glEnableVertexAttribArray(0);

    // For colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(GL_FLOAT) * 6, (void*)(sizeof(GL_FLOAT) * 3));    

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);

    // glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(X_OFFSET, Y_OFFSET, 0.0f));
        model = glm::rotate(model, glm::radians(X_ROTATE), glm::vec3(1,0,0)); // x-axis rotation
        model = glm::rotate(model, glm::radians(Y_ROTATE), glm::vec3(0,1,0)); // y-axis rotation
        model = glm::rotate(model, glm::radians(Z_ROTATE), glm::vec3(0,0,1)); // z-axis rotation
        model = glm::scale(model, glm::vec3(SCALE, SCALE, SCALE)); // uniform scaling

        if (!GPU_CALCULATIONS) {
            std::vector<float> vertexData;
            for(int i = 0; i < numVertices; i++) {
                glm::vec4 v(vertices[(i*6)], vertices[(i*6)+1], vertices[(i*6)+2], 1.0f);
                glm::vec4 transform = model * v;

                // vertex data
                vertexData.push_back(transform.x);
                vertexData.push_back(transform.y);    
                vertexData.push_back(transform.z);

                // color data    
                vertexData.push_back(vertices[(i*6)]);
                vertexData.push_back(vertices[(i*6)+1]);    
                vertexData.push_back(vertices[(i*6)+2]);
            }

            model = glm::mat4(1.0f);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() *  sizeof(GL_FLOAT), vertexData.data(), GL_STATIC_DRAW);
        }


        GLint modelLocation = glGetUniformLocation(shaderProgram, "u_modelMatrix");
        if(modelLocation >= 0) {
            glUniformMatrix4fv(modelLocation, 1, false, glm::value_ptr(model));
        }   
        else {
            std::cout << "could not find u_modelMatrix" << std::endl;
            exit(EXIT_FAILURE);
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

    // ROTATION
    // X-AXIS
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        X_ROTATE += 1.0f;
        // std::cout << "x rotate: " << X_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         X_ROTATE -= 1.0f;
        // std::cout << "x rotate: " << X_ROTATE << std::endl;
    }
    // Y-AXIS
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Y_ROTATE -= 1.0f;
        // std::cout << "y rotate: " << Y_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Y_ROTATE += 1.0f;
        // std::cout << "y rotate: " << Y_ROTATE << std::endl;
    }
    // Z-AXIS
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        Z_ROTATE -= 1.0f;
        // std::cout << "z rotate: " << Z_ROTATE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        Z_ROTATE += 1.0f;
        // std::cout << "z rotate: " << Z_ROTATE << std::endl;
    }

    // SCALING
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        SCALE -= 0.01f;
        // std::cout << "scale: " << SCALE << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        SCALE += 0.01f;
        // std::cout << "scale: " << SCALE << std::endl;
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

std::vector<GLfloat> readObjFile(const std::string& filename, std::vector<glm::vec3>& vertices, size_t& numVertices) {    
    vertices.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
        return std::vector<GLfloat>();
    }

    std::vector<GLfloat> faceData;
    std::vector<glm::vec3> normalizedVertices;
    std::string line;
    float max = FLT_MIN;
    float min = FLT_MAX;
    bool normalized = false;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        float x,y,z;
        std::string sv1, sv2, sv3;
        int v1, v2, v3;
        
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> x >> y >> z;
            updateMinMax(x, min, max);
            updateMinMax(y, min, max);
            updateMinMax(z, min, max);
            vertex.x = x;
            vertex.y = y;
            vertex.z = z;

            vertices.push_back(vertex);
        } else if (prefix == "f") {
            if (!normalized) {
                 // normalize all of the vectors to be within -1 and 1
                for (glm::vec3 v: vertices) {
                    v.x = ((v.x - min) / (max - min)) * (1 - (-1)) - 1;
                    v.y = (v.y - min) / (max - min) * (1 - (-1)) - 1;
                    v.z = (v.z - min) / (max - min) * (1 - (-1)) - 1;
                    normalizedVertices.push_back(v);
                }
                normalized = true;
            }
            iss >> sv1 >> sv2 >> sv3;
            v1 = stoi(sv1);  v2 = stoi(sv2);  v3 = stoi(sv3);
            // Assuming vertices are 1-indexed in .obj files, convert to 0-indexed
            v1--; v2--; v3--;
            // vertex 1 location
            faceData.push_back(normalizedVertices[v1].x);
            faceData.push_back(normalizedVertices[v1].y);
            faceData.push_back(normalizedVertices[v1].z);
            // vertex 1 color
            // faceData.push_back(1.0f);
            // faceData.push_back(0.0f);
            // faceData.push_back(0.0f);
            faceData.push_back(normalizedVertices[v1].x);
            faceData.push_back(normalizedVertices[v1].y);
            faceData.push_back(normalizedVertices[v1].z);
            // vertex 2 location
            faceData.push_back(normalizedVertices[v2].x);
            faceData.push_back(normalizedVertices[v2].y);
            faceData.push_back(normalizedVertices[v2].z);
            // vertex 2 color
            // faceData.push_back(0.0f);
            // faceData.push_back(1.0f);
            // faceData.push_back(0.0f);
            faceData.push_back(normalizedVertices[v2].x);
            faceData.push_back(normalizedVertices[v2].y);
            faceData.push_back(normalizedVertices[v2].z);

            // vertex 3 location
            faceData.push_back(normalizedVertices[v3].x);
            faceData.push_back(normalizedVertices[v3].y);
            faceData.push_back(normalizedVertices[v3].z);
            // vertex 3 color
            // faceData.push_back(0.0f);
            // faceData.push_back(0.0f);
            // faceData.push_back(1.0f);
            faceData.push_back(normalizedVertices[v3].x);
            faceData.push_back(normalizedVertices[v3].y);
            faceData.push_back(normalizedVertices[v3].z);

            numVertices += 3;
        }
    }
    file.close();
    
    return faceData;
}