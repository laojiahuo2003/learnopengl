#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include"shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include"gui.h"
#include"camera.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <map>
#include "BoxGeometry.h"
#include "PlaneGeometry.h"
#include "SphereGeometry.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 创建imgui上下文
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_DEPTH_TEST);


    Shader lightingShader("shaders/大作业测试.vs", "shaders/大作业测试.fs");
    Shader lightCubeShader("shaders/多光源.vs", "shaders/多光源.fs");
    Shader skyboxShader("./shaders/天空盒子.vs", "./shaders/天空盒子.fs");


    Shader reflectShader("./shaders/反射物体.vs", "./shaders/反射物体.fs");
    Shader refractShader("./shaders/折射物体.vs", "./shaders/折射物体.fs");


    PlaneGeometry planeGeometry(1.0, 1.0, 1.0, 1.0);
    SphereGeometry sphereGeometry2(0.5, 20.0, 20.0);
    BoxGeometry boxGeometry(1.0, 1.0, 1.0, 6, 6, 6);
    SphereGeometry sphereGeometry(0.1, 10.0, 10.0);
    BoxGeometry skyboxGeometry(1.0, 1.0, 1.0);           // 天空盒
    BoxGeometry reflectObjectBox(1.0, 1.0, 1.0, 6, 6, 6); //反射、折射物体

    GLuint VAO, VBO[2], EBO[2];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO); 

    glBindVertexArray(VAO);

    // 绑定并设置立方体 VBO 和 EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, boxGeometry.vertices.size() * sizeof(Vertex), &boxGeometry.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boxGeometry.indices.size() * sizeof(unsigned int), &boxGeometry.indices[0], GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // 绑定并设置球体 VBO 和 EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sphereGeometry2.vertices.size() * sizeof(Vertex), &sphereGeometry2.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereGeometry2.indices.size() * sizeof(unsigned int), &sphereGeometry2.indices[0], GL_STATIC_DRAW);

    // Position2
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(3);
    // Normal2
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(4);
    // TexCoords2
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);


    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
   
    //光源颜色
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f) };
    unsigned int diffuseMap = loadTexture("./textures/container2.png");
    unsigned int specularMap = loadTexture("./textures/container2_specular.png");
    unsigned int awesomeMap = loadTexture("./textures/awesomeface.png");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.haha", 2);

    // 天空盒贴图
    vector<string> faces{
        "textures/skybox/right.jpg",   // +X 方向（右面）
        "textures/skybox/left.jpg",    // -X 方向（左面）
        "textures/skybox/top.jpg",     // +Y 方向（顶部）
        "textures/skybox/bottom.jpg",  // -Y 方向（底部）
        "textures/skybox/front.jpg",   // +Z 方向（正面）
        "textures/skybox/back.jpg"     // -Z 方向（反面）
    };


    unsigned int cubemapTexture = loadCubemap(faces);
    //环境光初始
    float directionLightAmbient = 0.1f;
    float pointLightsAmbient = 0.01f;
    //漫反射
    float directionLightDiffuse = 0.2f;
    float pointLightsDiffuse = 0.0f;
    //镜面光照
    float directionLightSpecular = 0.5f;
    float pointLightsSpecular = 1.0f;


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());


        pointLightPositions[0].x = 0.7f + sin(currentFrame) * 2.0f;  // X 轴做周期性运动
        pointLightPositions[1].y = -3.3f + cos(currentFrame) * 2.0f; // Y 轴做周期性运动
        pointLightPositions[2].z = -12.0f + sin(currentFrame) * 5.0f; // Z 轴做周期性运动
        pointLightPositions[3].x = cos(currentFrame) * 3.0f; // X 轴做周期性运动
        pointLightPositions[3].y = sin(currentFrame) * 3.0f; // Y 轴做周期性运动

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float morphFactor = (sin(currentFrame) + 1.02f) / 2.0f; // 插值因子在 0 到 1 之间变化

        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("controls");
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::SliderFloat("morphFactor", &morphFactor, 0.0f, 1.0f);
        ImGui::SliderFloat("directionLightAmbient", &directionLightAmbient, 0.0f, 1.0f);
        ImGui::SliderFloat("directionLightDiffuse", &directionLightDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("directionLightSpecular", &directionLightSpecular, 0.0f, 1.0f);
        ImGui::SliderFloat("pointLightsAmbient", &pointLightsAmbient, 0.0f, 1.0f);
        ImGui::SliderFloat("pointLightsDiffuse", &pointLightsDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("pointLightsSpecular", &pointLightsSpecular, 0.0f, 1.0f);
        ImGui::End();


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // 绘制天空盒
        drawSkyBox(skyboxShader, skyboxGeometry, cubemapTexture);


        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);


        lightingShader.setVec3("directionLight.direction", -0.2f, -1.0f, -0.3f);// 平行光方向
        lightingShader.setVec3("directionLight.ambient", directionLightAmbient, directionLightAmbient, directionLightAmbient);
        lightingShader.setVec3("directionLight.diffuse", directionLightDiffuse, directionLightDiffuse, directionLightDiffuse);
        lightingShader.setVec3("directionLight.specular", directionLightSpecular, directionLightSpecular, directionLightSpecular);

       
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        lightingShader.setFloat("morphFactor", morphFactor);
        lightingShader.setFloat("morphFactor2", morphFactor);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, awesomeMap);

        // render containers
        //光源
        for (unsigned int i = 0; i < 4; i++)
        {
            // 设置点光源属性
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightsAmbient, pointLightsAmbient, pointLightsAmbient);
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", (pointLightsDiffuse == 0) ? glm::vec3(0.0f) : pointLightsDiffuse * (glm::vec3(1.0f) - pointLightColors[i]) + pointLightColors[i]);
            lightingShader.setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightsSpecular, pointLightsSpecular, pointLightsSpecular);

            // // 设置衰减
            lightingShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            lightingShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            lightingShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, sphereGeometry2.indices.size(), GL_UNSIGNED_INT, 0);
        }
        // 绘制灯光物体
        lightCubeShader.use();
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);

        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));


        glBindVertexArray(sphereGeometry.VAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);

            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("lightColor", pointLightColors[i]);

            glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(reflectObjectBox.VAO);



        reflectShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.1, 1.1, 1.0));
        reflectShader.setMat4("model", model);
        reflectShader.setMat4("view", view);
        reflectShader.setMat4("projection", projection);
        reflectShader.setVec3("cameraPos", camera.Position);
        // cubes
        glBindVertexArray(reflectObjectBox.VAO);
        glDrawElements(GL_TRIANGLES, reflectObjectBox.indices.size(), GL_UNSIGNED_INT, 0);


        refractShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.1, 1.1, 1.0));
        reflectShader.setMat4("model", model);
        reflectShader.setMat4("view", view);
        reflectShader.setMat4("projection", projection);
        reflectShader.setVec3("cameraPos", camera.Position);
        // cubes
        glBindVertexArray(reflectObjectBox.VAO);
        glDrawElements(GL_TRIANGLES, reflectObjectBox.indices.size(), GL_UNSIGNED_INT, 0);


        // 渲染gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, VBO);


    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (!ImGui::IsAnyItemActive() && !ImGui::IsAnyItemHovered()) {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



// 加载立方体贴图

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // 此处需要将y轴旋转关闭，若之前调用过loadTexture
    stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap)
{

    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 移除平移分量

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glBindVertexArray(geometry.VAO);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    view = camera.GetViewMatrix();
}