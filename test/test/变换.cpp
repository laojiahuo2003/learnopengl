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
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


float vertices[] = {
    //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // ����
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // ����
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // ����
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

int main()
{
    // ��ʼ�� GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("shaders/�任.vs", "shaders/�任.fs");

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //�ڼ������ԣ����Գ��ȣ�ÿһ���Ĳ�����ƫ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    //��EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //���
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0); // �ڰ�����֮ǰ�ȼ�������Ԫ,Ĭ�ϼ�����Բ�д
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //��������С��ʱ��ʹ���ڽ����ˣ����Ŵ�ʱʹ�����Թ��ˡ�
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    //��������
    int width, height, nrChannels;
    unsigned char* image = stbi_load("./textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "�޷���������" << std::endl;
    }
    stbi_image_free(image);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1); // �ڰ�����֮ǰ�ȼ�������Ԫ,Ĭ�ϼ�����Բ�д
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //��������С��ʱ��ʹ���ڽ����ˣ����Ŵ�ʱʹ�����Թ��ˡ�
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //��������
    image = stbi_load("./textures/container.jpg", &width, &height, &nrChannels, 0);
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "�޷���������" << std::endl;
    }
    stbi_image_free(image);


    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);



    ////��ʼ��һ������
    //glm::vec4 position = glm::vec4(1.0, 1.0, 1.0, 1.0);
    //std::cout << position.x << ";" << position.y << ";" << position.z << std::endl;
    ////��ʼ����λ����
    //glm::mat4 trans = glm::mat4(1.0f);
    ////x����ƽ��һ����λ
    //trans = glm::translate(trans, glm::vec3(1.0f, 0.0f, 0.0f));
    //position = trans * position;
    //std::cout << position.x << ";" << position.y << ";" << position.z << std::endl;
    ////����
    //trans = glm::scale(trans, glm::vec3(2.0f,3.0f,4.0f));
    //position = trans * position;
    //std::cout << position.x << ";" << position.y << ";" << position.z << std::endl;
    ////ѡ��
    //trans = glm::rotate(trans,glm::radians(45.0f) ,glm::vec3(0.0,0.0,1.0));
    //position = trans * position;
    //std::cout << position.x << ";" << position.y << ";" << position.z << std::endl;


    //glm::vec4 position = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
   // glm::mat4 trans;
    //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    //unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        
        // ���Ƶ�һ������
        glm::mat4 trans1 = glm::mat4(1.0f);
        trans1 = glm::translate(trans1, glm::vec3(0.5f, -0.5f, 0.0f));
        trans1 = glm::rotate(trans1, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans1));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // ���Ƶڶ�������
        glm::mat4 trans2 = glm::mat4(1.0f);
        trans2 = glm::translate(trans2, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scaleFactor = sin(glfwGetTime());
        trans2 = glm::scale(trans2, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(ourShader.ID);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
