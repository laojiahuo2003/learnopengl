#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
int main() {

	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// �汾Ϊ3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    // �������ڶ���
    GLFWwindow* window = glfwCreateWindow(800, 600, "learnOpengl", NULL, NULL);
    if (window == NULL) {
        std::cout << "����GLFW����ʧ��" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //GLAD����������OpenGL�ĺ���ָ��ģ������ڵ����κ�OpenGL�ĺ���֮ǰ������Ҫ��ʼ��GLAD��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "��ʼ��GLADʧ��" << std::endl;
        return -1;
    }
    
    //�����ӿ�(���½ǵ����꣬��ȣ��߶�)
    glViewport(0, 0, 800, 600);

    //ע�ᴰ�ڸı�Ļص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    //��Ⱦѭ��(Render Loop)��������������GLFW�˳�ǰһֱ��������
    while (!glfwWindowShouldClose(window))
    {
        //�������
        processInput(window);


        //��Ⱦָ��
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        ǰ���屣�������������ͼ����������Ļ����ʾ�������еĵ���Ⱦָ����ں󻺳��ϻ��ơ������е���Ⱦָ��ִ����Ϻ����ǽ���(Swap)ǰ����ͺ󻺳�
        */
        glfwSwapBuffers(window);// ������ɫ����
        glfwPollEvents();// �����¼�
    }
    //�ͷ���Դ
    glfwTerminate();
    return 0;
}
//�Դ���ע��һ���ص�����(Callback Function)��������ÿ�δ��ڴ�С��������ʱ�򱻵��á�
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

//GLFW��ʵ��һЩ�������
void processInput(GLFWwindow* window)
{
    //����û��Ƿ����˷��ؼ�(Esc)
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}