#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else
                number = std::to_string(specularNr++); // transfer unsigned int to string

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    }
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. maps
        vector<Texture> map = loadMaterialTextures(material, aiTextureType_DIFFUSE, "map");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};



unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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


float rotationSpeed = 0.1;
float r = 0.8;
float initialTime = glfwGetTime();
float xLast, yLast;

GLuint pyramidVAO, pyramidVBO;
GLuint floorVAO, floorVBO;
unsigned pyramidTexture, sandTexture;

GLuint waterVAO, waterVBO;
unsigned waterTexture;

glm::vec3 pyramidPositions[] = {
    glm::vec3(-6.0f,  0.0f,  -6.0f),
    glm::vec3(6.0f,  0.0f, 7.0f),
    glm::vec3(-4.0f,  0.0f, 6.0f),
};

glm::vec3 pyramidScaling[] = {
    glm::vec3(2.0f,  1.2f,  2.0f),
    glm::vec3(2.3f,  1.2f, 2.3f),
    glm::vec3(3.5f,  2.2f, 3.5f),
};

glm::vec3 floorPositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(0.0f,  0.0f, 10.0f),
    glm::vec3(10.0f,  0.0f, 10.0f),
};


void updateVariables(float paused, float restared) {
    float ydelta = r * (sin((glfwGetTime() - initialTime) * rotationSpeed));
    float xdelta = r * (cos((glfwGetTime() - initialTime) * rotationSpeed));
    if (!paused) {
        xLast = xdelta;
        yLast = ydelta;
    }
    if (restared) {
        initialTime = glfwGetTime();
    }
}

double mapRange(double value, double inMin, double inMax, double outMin, double outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

void setLight(Shader& lightingShader, glm::vec3 cameraTranslation, glm::vec3 pyramidPositions[], bool paused, bool restarted) {
    updateVariables(paused, restarted);
    glClearColor(0.243 + yLast / 2, 0.435 + yLast / 2, 0.529 + yLast / 2, 1.0);

    float lightValue = mapRange(yLast, -0.8, 0.8, 0.0, 0.6);
    float colorDelta;
    if (lightValue > 0.0f && lightValue < 0.45f) {
        colorDelta = mapRange(lightValue, 0.2, 0.45, 0.6, 1.0);;
    }
    else {
        colorDelta = 1.0;
    }

    // directional light
    lightingShader.setVec3("dirLight.direction", xLast * 3, yLast * 5, xLast * 2);
    lightingShader.setVec3("dirLight.ambient", lightValue, lightValue * colorDelta, lightValue);
    lightingShader.setVec3("dirLight.diffuse", lightValue, lightValue * colorDelta, lightValue);
    lightingShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pyramidPositions[0].x, pyramidPositions[0].y + 1.0f, pyramidPositions[0].z);
    lightingShader.setVec3("pointLights[0].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[0].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.7f);
    lightingShader.setFloat("pointLights[0].quadratic", 1.8f);
    // point light 2                 
    lightingShader.setVec3("pointLights[1].position", pyramidPositions[1].x, pyramidPositions[1].y + 1.0f, pyramidPositions[1].z);
    lightingShader.setVec3("pointLights[1].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[1].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.7f);
    lightingShader.setFloat("pointLights[1].quadratic", 1.8f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pyramidPositions[2].x, pyramidPositions[2].y + 1.0f, pyramidPositions[2].z);
    lightingShader.setVec3("pointLights[2].ambient", 0.0f, 0.7f, 0.7f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.6f, 0.6f, 0.6f);
    lightingShader.setVec3("pointLights[2].specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.7f);
    lightingShader.setFloat("pointLights[2].quadratic", 1.8f);

    lightingShader.setVec3("viewPos", cameraTranslation.x, cameraTranslation.y, cameraTranslation.z); //todo adjust with view

    // spotLight
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.position"), 0.0f, 5.0f, 5.0f); //todo adjust with view
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.direction"), 0.0f, 0.0f, 0.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
    //glUniform3f(glGetUniformLocation(lightingShader, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.constant"), 1.0f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.linear"), 0.09f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.quadratic"), 0.032f);
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
    //glUniform1f(glGetUniformLocation(lightingShader, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));
}

void renderSphere(Shader shaderProgram, glm::mat4 view, glm::mat4 projection, Model sphere, glm::vec3 pyramidPositions[]) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i] + glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        shaderProgram.setMat4("uM", model);

        sphere.Draw(shaderProgram);
    }

    glUseProgram(0);
}


void createWater() {
    // Vertices for the floor
    GLfloat vertices[] = {
        0.0f, 0.0f, -10.0f,  0.0f, -1.0f, 0.0f,  -1.0, -1.0,
        0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f,  -1.0, 1.0,
        10.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  1.0, -1.0,
        10.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0, 1.0,
    };

    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);

    glBindVertexArray(waterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    waterTexture = loadImageToTexture("res/water.jpg");
    glBindTexture(GL_TEXTURE_2D, waterTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void createPyramids() {

    GLfloat vertices[] = {

        // Back face
        -1.0f, 0.0f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        0.0f, 3.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.5f, 1.0f,
         1.0f, 0.0f, -1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

         // Front face
        0.0f, 3.0f, 0.0f,  0.0f, 0.0f, -1.0f,    0.5f, 1.0f,
       -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,   1.0f, 0.0f,

        // Left face
         0.0f, 3.0f, 0.0f,  1.0f, 0.0f, 0.0f,    0.5f, 1.0f,
        -1.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
         -1.0f, 0.0f,  1.0f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

         // Right face
          0.0f, 3.0f, 0.0f,  -1.0f, 0.0f, 0.0f,     0.5f, 1.0f,
          1.0f, 0.0f,  1.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
          1.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    };

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindVertexArray(pyramidVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    pyramidTexture = loadImageToTexture("res/pyramid-texture.jpg");
    glBindTexture(GL_TEXTURE_2D, pyramidTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void createFloor() {
    // Vertices for the floor
    GLfloat vertices[] = {
        -10.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  -1.0, -1.0,
        -10.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  -1.0, 1.0,
         0.0f, 0.0f, -10.0f, 0.0f, -1.0f, 0.0f,  1.0, -1.0,
         0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0, 1.0,
    };

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);

    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    sandTexture = loadImageToTexture("res/sand.jpg");
    glBindTexture(GL_TEXTURE_2D, sandTexture); //to set up the texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderPyramids(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pyramidTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pyramidTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pyramidPositions[i]);
        model = glm::scale(model, pyramidScaling[i]);

        shaderProgram.setMat4("uM", model);

        glBindVertexArray(pyramidVAO);

        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void renderFloor(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sandTexture);

    for (unsigned int i = 0; i < 3; i++)
    {
        // define the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, floorPositions[i]);

        shaderProgram.setMat4("uM", model);

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void renderWater(Shader shaderProgram, glm::mat4 view, glm::mat4 projection) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 0.3f);

    glm::mat4 model = glm::mat4(1.0f);
    shaderProgram.setMat4("uM", model);
    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    glBindVertexArray(waterVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void renderFish(Shader shaderProgram, glm::mat4 view, glm::mat4 projection, Model fish) {
    shaderProgram.use();

    //material
    shaderProgram.setInt("material.diffuse", 0);
    shaderProgram.setInt("material.specular", 0);
    shaderProgram.setFloat("map", 0);
    shaderProgram.setFloat("material.shininess", 32.0f);
    shaderProgram.setFloat("alpha", 1.0f);

    shaderProgram.setMat4("uV", view);
    shaderProgram.setMat4("uP", projection);

    // define the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));

    shaderProgram.setMat4("uM", model);

    fish.Draw(shaderProgram);

    glUseProgram(0);
}


/*  TODO
* 9. dodati uniformu za odredjivanje providnosti ove boje tj teksture u fragment shader
* 10. dodati ime i br indeksa
*/

bool paused = false;
bool restared = false;
float cameraSpeed = 0.03f;

glm::vec3 pyramidPeakPositions[] = {
glm::vec3(-6.0f,  3.6f,  -6.0f),
glm::vec3(6.0f,  3.6f, 7.0f),
glm::vec3(-4.0f, 6.6f, 6.0f),
};

GLFWwindow* initWindow() {
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width, mode->height, "Desert", monitor, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        exit(2);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        exit(3);
    }

    return window;
}

int main() {
    GLFWwindow* window = initWindow();
    if (!window) return -1;

    // Create objects
    createWater();
    createPyramids();
    createFloor();
    Model sphere(std::string("res/sphere.obj"));
    Model fish(std::string("res/Fish/12265_Fish_v1_L2.obj"));

    // Compile shaders
    Shader phongShader("phong.vert", "phong.frag");
    Shader gouraudShader("gouraud.vert", "gouraud.frag");
    Shader activeShader = phongShader;

    // Create MVP matrices
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)mode->width / (float)mode->height, 0.1f, 100.0f);
    glm::mat4 projectionO = glm::ortho(-30.0f, 30.0f, -20.0f, 20.0f, 0.1f, 100.0f);
    glm::mat4 projection = projectionP;
    // Create the camera view matrix based on the biggest pyramid position
    glm::vec3 pyramidPosition(-4.0f, 0.0f, 6.0f);
    glm::vec3 cameraPosition = pyramidPosition + glm::vec3(0.0f, 30.0f, 0.0f);  
    glm::vec3 cameraFront = pyramidPosition - cameraPosition;
    glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.337, 0.451, 0.51, 1.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input actions
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        // choose projection type
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            projection = projectionP;
        }
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            projection = projectionO;
        }
        // choose shader type
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            activeShader = phongShader;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            activeShader = gouraudShader;
        }
        // pause or restart the scene
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            paused = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            restared = true;
            paused = false;
        }
        // move up-down
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { 
            cameraPosition += cameraSpeed * cameraUp;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraUp;
        }
        // move left-right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        // rotate
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  
            float rotationAngle = glm::radians(cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            float rotationAngle = glm::radians(-cameraSpeed);
            cameraUp = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.0f), rotationAngle, cameraFront) * glm::vec4(cameraUp, 0.0f)));
        }
        // zoom
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
            cameraPosition += cameraSpeed * cameraFront / 20.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraFront / 20.0f;
        }

        view = glm::lookAt(cameraPosition, pyramidPosition + glm::vec3(0.0f, 6.6f, 0.0f), cameraUp);
        activeShader.use();
        setLight(activeShader, cameraPosition, pyramidPeakPositions, paused, restared);
        glUseProgram(0);

        // render created objects
        renderWater(activeShader, view, projection);
        renderPyramids(activeShader, view, projection);
        renderFloor(activeShader, view, projection);
        renderSphere(activeShader, view, projection, sphere, pyramidPeakPositions);
        renderFish(activeShader, view, projection, fish);

        glfwSwapBuffers(window);
        glfwPollEvents();

        restared = false;
    }

    //glDeleteProgram(phongShader);
    //glDeleteProgram(gouraudShader);
    //DeleteDesertVariables();

    glfwTerminate();
    return 0;
}
