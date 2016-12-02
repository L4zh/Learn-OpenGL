//
//  main.cpp
//  OGL_Test
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <FreeImage.h>

#include "Shader.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


const GLuint winWidth = 800, winHeight = 600;

GLfloat mixValue = 0.1f;

// 纹理图片大小
GLint width, height;

// 载入图片
unsigned char* imgBuffer;

static bool loadImage(const char* fname)
{
    FreeImage_Initialise(TRUE);
    
    // 定义图片格式为未知
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    
    // 获取图片格式
    fif = FreeImage_GetFileType(fname, 0);
    
    // 根据获取格式读取图片数据
    FIBITMAP* bitmap = FreeImage_Load(fif, fname, 0);
    
    if (!bitmap)
    {
        std::cout << "Load Error!" << std::endl;
        return false;
    }
    
    int x, y;
    RGBQUAD m_rgb;
    
    // 获取图片长和宽
    width = (GLint)FreeImage_GetWidth(bitmap);
    height = (GLint)FreeImage_GetHeight(bitmap);
    
    imgBuffer = new unsigned char[width * height * 4];
    
    // 获取图片数据 按RGBA保存
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            // 获取像素值
            FreeImage_GetPixelColor(bitmap, x, y, &m_rgb);
            
            // 将RGB存入数组
            imgBuffer[y * width * 4 + x * 4 + 0] = m_rgb.rgbRed;
            imgBuffer[y * width * 4 + x * 4 + 1] = m_rgb.rgbGreen;
            imgBuffer[y * width * 4 + x * 4 + 2] = m_rgb.rgbBlue;
            
            // 判断alpha值
            if (FreeImage_IsTransparent(bitmap))
                imgBuffer[y * width * 4 + x * 4 + 3] = m_rgb.rgbReserved;
            else
                imgBuffer[y * width * 4 + x * 4 + 3] = 255;
        }
    }
    
    // 释放
    FreeImage_Unload(bitmap);
    return true;
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // MAC

    
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);
    
    glewExperimental = GL_TRUE;
    glewInit();
    
    glViewport(0, 0, winWidth, winHeight);
    
    
    Shader ourShader("/Users/apple/Documents/Xcode/OGL_Test/OGL_Test/object.vert",
                     "/Users/apple/Documents/Xcode/OGL_Test/OGL_Test/object.frag");
    
    
    GLfloat vertices[] =
    {
        // Positions          // Colors           // Texture Coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };
    
    GLuint indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // 纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    
    // 纹理
    GLuint tex1, tex2;
    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    loadImage("/Users/apple/Documents/Xcode/OGL_Test/OGL_Test/wall.png");
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);  // 多级渐远纹理
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    loadImage("/Users/apple/Documents/Xcode/OGL_Test/OGL_Test/awesomeface.png");
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    // Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        ourShader.Use();
        
        glActiveTexture(GL_TEXTURE0);  // 激活纹理单元
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
        glUniform1f(glGetUniformLocation(ourShader.Program, "viDig"), mixValue);
        
        
        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    // 向上键纹理显示增强
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        mixValue += 0.1f;
        mixValue = fmin(mixValue, 1.0f);
    }
    
    // 向下键纹理显示减弱
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        mixValue -= 0.1f;
        mixValue = fmax(mixValue, 0.0f);
    }
}

