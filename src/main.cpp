#include "glad.h" //manages function pointers for OpenGL
#include <GLFW/glfw3.h>

//Math libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

//text libs
#include "../stb/stb_image.h"
#include <iostream>
#include <vector>
#include <string.h>

//my libs
#include "./helpingFunctions/Shader.h"
#include "./helpingFunctions/Keyboard.h"
#include "./helpingFunctions/Camera.h"
#include "./helpingFunctions/Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);//when we re-size the window(so we dont get a black screen)
void processInput(GLFWwindow *window, double deltaTime);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
static bool paused = false;
Camera camera(glm::vec3(0.0f, 12.0f, 25.0f));
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame   

// shaders
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"

    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "out vec2 TexCoord;\n"

    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0f));\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   TexCoord = aTexCoord;\n"   
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"   
    "}\n\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"

    "//COLORS\n"
    "struct Material {\n"
    "   vec3 ambient;\n"
    "   sampler2D diffuse;\n"
    "   vec3 specular;\n"
    "   float shininess;\n"
    "};\n"

    "//LIGHT STRENGHT\n"
    "struct Light {\n"
    "   vec3 position;\n"
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "};\n"

    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "in vec2 TexCoord;\n"
    
    "uniform Material material;\n"
    "uniform Light light;\n"
    "uniform vec3 viewPos;\n"
    
    "void main()\n"
    "{\n"

    "   //ambient\n"
    "   vec3 ambient = light.ambient * material.ambient;\n"

    "   //diffuse\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(light.position - FragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0f);\n"
    "   vec3 diffuse = light.diffuse *(diff * vec3(texture(material.diffuse, TexCoord)));\n"

    "   //specular\n"
    "   vec3 viewDir = normalize(viewPos - FragPos);\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);  \n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess *128);\n"
    "   vec3 specular = light.specular * (spec * material.specular);\n"

    "   FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);\n"
    "}\n\0";

const char *lampShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D lampTexture;\n"
    "uniform vec3 lightColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(lampTexture, TexCoord) *vec4(lightColor, 1.0f);\n"
    "}\n\0";
    
const char *skyboxVertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"

    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\n\0";
const char *skyboxfragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"

    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n\0";
int main(){
    //------------------------------------INIT------------------------------------
    // glfw: initialize and configure
    glfwInit();
    //open gl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //use the core-profile:get access to a smaller subset of OpenGL features without backwards-compatible features

    //if you are running it in MAC
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ergasia", NULL, NULL);
    if (window == NULL){ //if we couldnt make the window
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // focus of glfw in this window

    // Load glad: load all OpenGL function pointers based on our OS.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){//if we couldn't load glad
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//for when we re-size the window
    glfwSetKeyCallback(window, Keyboard::keyCallback);//for keyboard input
    //----------------------------------------------------------------------------

   
    //------------------------------------SHADERS------------------------------------
    //build and compile our shader program
    Shader earthShader(vertexShaderSource, fragmentShaderSource);
    Shader moonShader(vertexShaderSource, fragmentShaderSource);
    Shader sunShader(vertexShaderSource, lampShaderSource);
    Shader SkyboxShader(skyboxVertexShaderSource, skyboxfragmentShaderSource);
    //----------------------------------------------------------------------------

    //------------------------------------MODELS------------------------------------
    //earth
    Model earth(glm::vec3(0.0f), glm::vec3(0.5f));
    earth.loadModel("./models/earth/Earth.obj");
    //moon
    Model moon(glm::vec3(0.0f), glm::vec3(0.3f));
    moon.loadModel("./models/moon/Moon.obj");
    //sun
    Model sun(glm::vec3(0.0f), glm::vec3(3.1f));
    sun.loadModel("./models/sun/sun.obj");
    //----------------------------------------------------------------------------
   
    //------------------------------------SKYBOX------------------------------------
    const int numStars = 1000;
    glm::vec2 stars2[numStars];
    for (int i = 0; i < numStars; ++i) {
        //calculate random position in a sphere
        float x = static_cast<float>(std::rand()*SCR_WIDTH % numStars);
        float y = static_cast<float>(std::rand()*SCR_HEIGHT% numStars);
        stars2[i] = glm::vec2(x, y);
    }
        
    GLuint VBO_skybox, VAO_skybox;
    glGenBuffers(1, &VBO_skybox);
    glGenVertexArrays(1, &VAO_skybox);

    glBindVertexArray(VAO_skybox);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_skybox);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numStars, (void*)stars2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glm::mat4 projectionStars = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, -1.0f, 1.0f);    //for skybox 
    //----------------------------------------------------------------------------

    float time = (float)glfwGetTime();
    //render loop
    while (!glfwWindowShouldClose(window)){
        
        // input
        processInput(window,deltaTime);
        if(paused){
            glfwSetTime(time);
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        } 

        // per-frame time logic
        time = (float)glfwGetTime();
        deltaTime = time - lastFrame;
        lastFrame = time;

        // Color background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //draw skybox
        glDisable(GL_DEPTH_TEST);//for sk
            SkyboxShader.activate();
            SkyboxShader.setMat4("projection", projectionStars);
            glBindVertexArray(VAO_skybox);
            glDrawArrays(GL_POINTS, 0, numStars); 
            glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        //transformation for screen
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        //draw earth
        earthShader.activate();
        earthShader.setMat4("view", view);
        earthShader.setMat4("projection", projection);
        earthShader.set3Float("viewPos", camera.cameraPosition);
        earthShader.set3Float("light.position", sun.pos);
        earthShader.set3Float("light.ambient", sun.ambient);
        earthShader.set3Float("light.diffuse", sun.diffuse);
        earthShader.set3Float("light.specular", sun.specular);
        glm::vec3 earthPoint = earth.render(earthShader, 3, glm::vec3(0.0f),time);

        // draw moon
        moonShader.activate();
        moonShader.setMat4("view", view);
        moonShader.setMat4("projection", projection);
        moonShader.set3Float("viewPos", camera.cameraPosition);
        moonShader.set3Float("light.position", sun.pos);
        moonShader.set3Float("light.ambient", sun.ambient);
        moonShader.set3Float("light.diffuse", sun.diffuse);
        moonShader.set3Float("light.specular", sun.specular);
        moon.render(moonShader, 2, earthPoint, time);

        //draw sun
        sunShader.activate();
        sunShader.setMat4("view", view);
        sunShader.setMat4("projection", projection);
        sunShader.set3Float("lightColor", glm::vec3(1.0f));
        sun.render(sunShader, 1, glm::vec3(0.0f), time);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    earth.cleanup();
    moon.cleanup();
    sun.cleanup();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, double deltaTime){
    if (Keyboard::key(GLFW_KEY_ESCAPE)){//check if the escapeKey is pressed, close the window
        glfwSetWindowShouldClose(window, true);//set property:'WindowShouldClose' to true
    }  
    if (Keyboard::keyDown(GLFW_KEY_SPACE)){//check if the escapeKey is pressed, close the window
        paused = !paused;
    }
    if(paused){
        return;
    } 
    if(Keyboard::key(GLFW_KEY_W)){
        //camera.updateCameraPosition(CameraDireaction::BACKWARD, deltaTime*5);
        camera.updateCameraDirection(0.0f, 0.1f);
    }
    if(Keyboard::key(GLFW_KEY_S)){
        //camera.updateCameraPosition(CameraDireaction::FORWARD, deltaTime*5);
        camera.updateCameraDirection(0.0f, -0.1f);
    }
    if(Keyboard::key(GLFW_KEY_D)){
        //camera.updateCameraPosition(CameraDireaction::RIGHT, deltaTime*5);
        camera.updateCameraDirection(0.1f, 0.0f);
    }
    if(Keyboard::key(GLFW_KEY_A)){
        //camera.updateCameraPosition(CameraDireaction::LEFT, deltaTime*5);
        camera.updateCameraDirection(-0.1f, 0.0f);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
}
