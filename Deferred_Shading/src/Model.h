//
//  Model.h
//  I_OGL
//
//  Created by apple on 2016/12/24.
//  Copyright (c) 2016年 nara. All rights reserved.
//

#ifndef I_OGL_Model_h
#define I_OGL_Model_h


#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <FreeImage.h>
#include "Shader.h"

using namespace std;


GLint imageW, imageH;

unsigned char* imgBuffer;

GLint TextureFromFile(const char* path, string directory);

static bool loadImage(const char* fname);


class Model
{
public:
    Model(const GLchar* path);
    void Draw(Shader shader);
    
private:
    // 模型数据
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;
    
    void loadModel(string path);
    
    void processNode(aiNode* node, const aiScene* scene);
    
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    
    // 加载生成纹理
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};



Model::Model(const GLchar* path)
{
    this->loadModel(path);
}


void Model::Draw(Shader shader)
{
    for (GLuint i = 0; i < this->meshes.size(); ++i)
        this->meshes[i].Draw(shader);
}


void Model::loadModel(string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "Error::Assimp::" << importer.GetErrorString() << endl;
        return;
    }
    
    this->directory = path.substr(0, path.find_last_of('/'));
    this->processNode(scene->mRootNode, scene);
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }
    
    for (GLuint i = 0; i < node->mNumChildren; ++i)
        this->processNode(node->mChildren[i], scene);
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    
    // 获取每个网格顶点
    for (GLuint i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex ivertex;
        
        glm::vec3 vTemp;
        vTemp.x = mesh->mVertices[i].x;
        vTemp.y = mesh->mVertices[i].y;
        vTemp.z = mesh->mVertices[i].z;
        ivertex.Position= vTemp;
        
        glm::vec3 nTemp;
        nTemp.x = mesh->mNormals[i].x;
        nTemp.y = mesh->mNormals[i].y;
        nTemp.z = mesh->mNormals[i].z;
        ivertex.Normal = nTemp;
        
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 tTemp;
            tTemp.x = mesh->mTextureCoords[0][i].x;
            tTemp.y = mesh->mTextureCoords[0][i].y;
            ivertex.TexCoords = tTemp;
        }
        else
            ivertex.TexCoords = glm::vec2(0.0f, 0.0f);
        
        vertices.push_back(ivertex);
    }
    
    // 获取面索引
    for (GLuint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }
    
    // 获取纹理
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return Mesh(vertices, indices, textures);
}


vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        GLboolean skip = false;
        for (GLuint j = 0; j < textures_loaded.size(); ++j)
        {
            if (textures_loaded[j].path == str)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture itexture;
            itexture.id = TextureFromFile(str.C_Str(), this->directory);
            itexture.type = typeName;
            itexture.path = str;
            textures.push_back(itexture);
            this->textures_loaded.push_back(itexture);
        }
    }
    return textures;
}


GLint TextureFromFile(const char* path, string directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    
    GLuint textureID;
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if (!loadImage(filename.c_str()))
        cout << "Load Image Failed!" << endl;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageW, imageH, 0, GL_RGB, GL_UNSIGNED_BYTE, imgBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return textureID;
}


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
    imageW = (GLint)FreeImage_GetWidth(bitmap);
    imageH = (GLint)FreeImage_GetHeight(bitmap);
    
    imgBuffer = new unsigned char[imageW * imageH * 4];
    
    // 获取图片数据 按RGBA保存
    for (y = 0; y < imageH; ++y)
    {
        for (x = 0; x < imageW; ++x)
        {
            // 获取像素值
            FreeImage_GetPixelColor(bitmap, x, y, &m_rgb);
            
            // 将RGB存入数组
            imgBuffer[y * imageW * 3 + x * 3 + 0] = m_rgb.rgbRed;
            imgBuffer[y * imageW * 3 + x * 3 + 1] = m_rgb.rgbGreen;
            imgBuffer[y * imageW * 3 + x * 3 + 2] = m_rgb.rgbBlue;
            /*
             // 判断alpha值
             if (FreeImage_IsTransparent(bitmap))
             imgBuffer[y * imageW * 4 + x * 4 + 3] = m_rgb.rgbReserved;
             else
             imgBuffer[y * imageW * 4 + x * 4 + 3] = 255;
             */
        }
    }
    
    // 释放
    FreeImage_Unload(bitmap);
    return true;
}


#endif
