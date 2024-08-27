#include <glad/glad.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// �ӿڵĻص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// �������
void processInput(GLFWwindow* window);

// ����
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// phong����ģ��
// ���嶥����ɫ��
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

// ����Ƭ����ɫ��
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
// ��������
"   float ambientStrength = 0.1;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
// ���������
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - FragPos);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"
// �������
"   float specularStrength = 0.5;\n"
"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"   vec3 specular = specularStrength * spec * lightColor;\n"
// ���
"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"   FragColor = vec4(result, 1.0);\n"
"}\0";

// ����
glm::vec3 lightPos(0.6f, 0.5f, 1.0f);

int main()
{
    // ʵ����GLFW����
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // �������ڶ���
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lexiaoyuan", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ������������
    int nSlices = 50; // ����
    int nStacks = 50; // γ��
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
    // ����������������
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

    // ���嶥�㻺�����
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // ���嶥���������
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // ���������������
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // �󶨶��㻺�����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // ���ƶ������ݵ������ڴ�
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    // �󶨶����������
    glBindVertexArray(VAO);

    // �������������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);


    // ����������ɫ������
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // ���Ӷ�����ɫ�����뵽������ɫ������
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // ���붥����ɫ��
    glCompileShader(vertexShader);

    // ����Ƭ����ɫ������
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // ����Ƭ����ɫ�����뵽Ƭ����ɫ������
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // ����Ƭ����ɫ��
    glCompileShader(fragmentShader);

    // ����һ����ɫ���������
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // ����ɫ�����󸽼ӵ�������
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // ������ɫ������
    glLinkProgram(shaderProgram);

    // ��ɫ���������ӳɹ������ɾ����
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // �����ӿ�
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // ע��ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������ɫ���������
        glUseProgram(shaderProgram);

        // �󶨶�������
        glBindVertexArray(VAO);

        // �������������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // ʹ���߿�ģʽ
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // ʹ�����ģʽ��Ĭ�ϣ�
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.5f, 0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &glm::vec3(0.0f, 0.0f, 3.0f)[0]);

        // ����ģ�;���
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // ����۲����
        glm::mat4 view = glm::mat4(1.0f);
        // ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // ����ͶӰ����
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // ���������ͳһλ��
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        // �����Ǵ��ݸ���ɫ��
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


        // ����
        glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, elements);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // �ͷ�֮ǰ�������Դ
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

