#ifndef MODEL_HPP
#define MODEL_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
    Model(const std::string& path);

    void Draw(unsigned int shader);

private:
    struct Mesh {
        unsigned int VAO, VBO, EBO;
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
    };

    std::vector<Mesh> meshes;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif // MODEL_HPP
