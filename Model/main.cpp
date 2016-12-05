
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <FreeImage.h>

#include "Shader.h"
#include "Mesh.h"
#include "Model.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// Todo
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callbaack(GLFWwindow* window, double xpos, double ypos);


const GLuint windowW = 800, windowH = 600;
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;


glm::vec3 lightPos(0.0f, 0.0f, 1.0f);


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // MAC
	
	
	GLFWwindow* window = glfwCreateWindow(windowW, windowH, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	glfwSetKeyCallback(window, key_callback);
	
	glewExperimental = GL_TRUE;
	glewInit();
	

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	glEnable(GL_DEPTH_TEST);  // 开启深度测试
	

    Shader ourShader("/Users/apple/Documents/Xcode/GLDemo/Modle/model.vert", "/Users/apple/Documents/Xcode/GLDemo/model.frag");
    Model ourModel("/Users/apple/Documents/Xcode/GLDemo/Model/nanosuit/nanosuit.obj");

    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        ourShader.Use();
        
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        //LookUp矩阵: view = glm::lookAt(glm::vec3(0.0, 0.0, 5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.55f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        ourModel.Draw(ourShader);
        
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)&& action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}
