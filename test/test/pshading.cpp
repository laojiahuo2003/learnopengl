#include <glad/glad.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 视口的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 输入控制
void processInput(GLFWwindow* window);

// 设置
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// phong光照模型
// 定义顶点着色器
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\0";

// 定义片段着色器
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
// 环境光照
"   float ambientStrength = 0.1;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
// 漫反射光照
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - FragPos);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"
// 镜面光照
"   float specularStrength = 0.5;\n"
"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"   vec3 specular = specularStrength * spec * lightColor;\n"
// 结果
"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"   FragColor = vec4(result, 1.0);\n"
"}\0";

// 光照
glm::vec3 lightPos(0.6f, 0.5f, 1.0f);

int main()
{
    // 实例化GLFW窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lexiaoyuan", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 定义球体坐标
    int nSlices = 50; // 经线
    int nStacks = 50; // 纬线
    int nVerts = (nStacks + 1) * (nSlices + 1);
    int elements = nSlices * nStacks * 6;
    float theta, phi;
    float thetaFac = 3.14f * 2.0f / nSlices;
    float phiFac = 3.14f * 1.0f / nStacks;
    float nx, ny, nz;
    int idx = 0;
    float sphere_vertices[51 * 51 * 3];
    int sphere_indices[50 * 50 * 6];
    for (int i = 0; i <= nSlices; i++)
    {
        theta = i * thetaFac;
        for (int j = 0; j <= nStacks; j++)
        {
            phi = j * phiFac;
            nx = sinf(phi) * cosf(theta);
            ny = sinf(phi) * sinf(theta);
            nz = cosf(phi);
            sphere_vertices[idx * 3] = 0.5f * nx;
            sphere_vertices[idx * 3 + 1] = 0.5f * ny;
            sphere_vertices[idx * 3 + 2] = 0.5f * nz;
            idx++;
        }
    }
    // 定义球体坐标索引
    int indx = 0;
    for (int i = 0; i < nStacks; i++)
    {
        for (int j = 0; j < nSlices; j++)
        {
            int i0 = i * (nSlices + 1) + j;
            int i1 = i0 + 1;
            int i2 = i0 + (nSlices + 1);
            int i3 = i2 + 1;
            if ((j + i) % 2 == 1) {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i1;
                sphere_indices[indx + 3] = i1;
                sphere_indices[indx + 4] = i2;
                sphere_indices[indx + 5] = i3;
                indx += 6;
            }
            else
            {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i3;
                sphere_indices[indx + 3] = i0;
                sphere_indices[indx + 4] = i3;
                sphere_indices[indx + 5] = i1;
                indx += 6;
            }
        }

    }

    // 定义顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 定义顶点数组对象
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 定义索引缓冲对象
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // 绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 复制顶点数据到缓冲内存
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    // 绑定顶点数组对象
    glBindVertexArray(VAO);

    // 绑定索引缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);


    // 创建顶点着色器对象
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 附加顶点着色器代码到顶点着色器对象
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // 编译顶点着色器
    glCompileShader(vertexShader);

    // 创建片段着色器对象
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 附加片段着色器代码到片段着色器对象
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // 编译片段着色器
    glCompileShader(fragmentShader);

    // 创建一个着色器程序对象
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 把着色器对象附加到程序上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // 链接着色器程序
    glLinkProgram(shaderProgram);

    // 着色器对象链接成功后可以删掉了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // 定义视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活着色器程序对象
        glUseProgram(shaderProgram);

        // 绑定顶点数组
        glBindVertexArray(VAO);

        // 绑定索引缓冲对象
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // 使用线框模式
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // 使用填充模式（默认）
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.5f, 0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &glm::vec3(0.0f, 0.0f, 3.0f)[0]);

        // 定义模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // 定义观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // 定义投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 检索矩阵的统一位置
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        // 将它们传递给着色器
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


        // 绘制
        glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, elements);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 释放之前分配的资源
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

