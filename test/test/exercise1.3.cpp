#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);




// ������ɫ��
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"   gl_PointSize = 10.0f;\n"
"}\0";

//Ƭ����ɫ��
const char* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

//Ƭ����ɫ��
const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";
int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// �汾Ϊ3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "learnOpengl", NULL, NULL);
    if (window == NULL) {
        std::cout << "����GLFW����ʧ��" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "��ʼ��GLADʧ��" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    //����������ɫ������
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // ������ɫ������(��ɫ���������ַ���������������ɫ�����룬NULL)
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // ����Ƿ����ɹ�(ûɶ��Ҫ)
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //Ƭ����ɫ��
    unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);

    // �����������
    unsigned int shaderProgram1 = glCreateProgram();
    // ����ɫ�����ӵ����������
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    // ����
    glLinkProgram(shaderProgram1);
    //������ɫ������
    glUseProgram(shaderProgram1);
    //ɾ����ɫ������
    glDeleteShader(fragmentShader1);

    //Ƭ����ɫ��
    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);

    // �����������
    unsigned int shaderProgram2 = glCreateProgram();
    // ����ɫ�����ӵ����������
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    // ����
    glLinkProgram(shaderProgram2);
    //������ɫ������
    glUseProgram(shaderProgram2);
    //ɾ����ɫ������
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader2);


    float firstTriangle[] = {
           -0.9f, -0.5f, 0.0f,  // left 
           -0.0f, -0.5f, 0.0f,  // right
           -0.45f, 0.5f, 0.0f,  // top 
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };

    //����VBO���㻺������VAO�����������
    unsigned int VBOs[2], VAOs[2];
    glGenBuffers(2, VBOs);
    glGenVertexArrays(2, VAOs);

    //��VAO
    glBindVertexArray(VAOs[0]);
    //�󶨻���
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    //���VAO����(Ŀ�껺�����ͣ��������ݴ�С��ʵ�ʵ����ݣ��Կ���ι����������)
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    //��ν�������ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //��VAO
    glBindVertexArray(VAOs[1]);
    //�󶨻���
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    //���VAO����(Ŀ�껺�����ͣ��������ݴ�С��ʵ�ʵ����ݣ��Կ���ι����������)
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    //��ν�������ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);





    //�����߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram1);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // ���
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}