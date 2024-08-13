#include "model.h"

Model::Model(const char* path)
{
    this->path = path;
    size_t dotPosition = this->path.find_last_of('.');
    extension = this->path.substr(dotPosition);
    std::cout << "----------------" << path << "\n";
    loadModel(path);
}

void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
    }
}

void Model::Draw(Shader& shader, unsigned int numInstances)
{
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader);
    }
}

int Model::getNumVertices()
{
    int numVertices = 0;
    for (auto& mesh : meshes) {
        numVertices += mesh.getNumVertices();
    }
    return numVertices;
}

void Model::loadModel(std::string path)
{
    auto start = std::chrono::high_resolution_clock::now();
    Assimp::Importer import;
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GlobalScale
        | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_SplitByBoneCount;
    const aiScene *scene = import.ReadFile(path, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::string error = import.GetErrorString();
        std::string message = "Model Loading failed: ERROR::ASSIMP::" + error;
        throw std::runtime_error(message);
        std::cerr << message << std::endl;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Model loading success: " << path << ", time taken: " << duration.count() << '\n';
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        SetVertexBoneDataToDefault(vertex);

        // process vertex positions, normals and texture coordinates
        vertex.positions = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
        vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> albedoMaps;
        std::vector<Texture> normalMaps;
        std::vector<Texture> metalnessMaps;
        std::vector<Texture> roughnessMaps;
        std::vector<Texture> aoMaps;
        std::vector<Texture> emissiveMaps;

        // support gltf for pbr materials
        if (this->extension == ".gltf") {
            albedoMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "albedoMap"); //aiTextureType_BASE_COLOR
            textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());
            
            normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalMap");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            
            metalnessMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "metallicMap");
            textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());
            
            roughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "roughnessMap");
            textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

            aoMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "aoMap");
            textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
            
            emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "emissiveMap");
            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
        }

        // try to set up as much materials as possible (might looks wrong in PBR shading)
        else {
            albedoMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "albedoMap");
            textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

            normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normalMap");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "metallicMap");
            textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

            roughnessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "roughnessMap");
            textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

            aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "aoMap");
            textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

            emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "emissiveMap");
            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
        }

    }

    ExtractBoneWeightForVertices(vertices, mesh, scene);

    return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures = {};
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::string path = directory + '/' + std::string(str.C_Str());

        if (loaded_textures.find(path.data()) != loaded_textures.end()) {
            textures.push_back(loaded_textures[path.data()]);
            if (typeName == "roughnessMap") // roughnessMap uses the same file as metallicMap which will cause wrong type as map only store one path
                textures[textures.size() - 1].type = "roughnesMap";
            break;
        }

        Texture texture(path.c_str(), typeName.c_str());
        textures.push_back(texture);
        loaded_textures[path.data()] = texture;
    }

    if (textures.empty()) {
        if (typeName == "albedoMap") {
            textures.push_back(Texture("Textures/default/albedo.png", "albedoMap"));
        }
        else if (typeName == "normalMap") {
            textures.push_back(Texture("Textures/default/normal.png", "normalMap"));
        }
        else if (typeName == "metallicMap") {
            textures.push_back(Texture("Textures/default/metallic.png", "metallicMap"));
        }
        else if (typeName == "roughnessMap") {
            textures.push_back(Texture("Textures/default/roughness.png", "roughnessMap"));
            textures[textures.size() - 1].type = "roughnesMap";
        }
        else if (typeName == "aoMap") {
            textures.push_back(Texture("Textures/default/ao.png", "aoMap"));
        }
        else if (typeName == "emissiveMap") {
            textures.push_back(Texture("Textures/default/emissive.png", "emissiveMap"));
        }
        else if (typeName == "heightMap") {    // for future height map support
            //Texture("Textures/default/height.png", "heightMap");
        }
    }
    return textures;
}


std::map<std::string, BoneInfo> Model::GetBoneInfoMap() {
    return m_BoneInfoMap;
}

int& Model::GetBoneCount() {
    return m_BoneCounter;
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.m_BoneIDs[i] < 0)
        {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}


void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    auto& boneInfoMap = m_BoneInfoMap;
    int& boneCount = m_BoneCounter;

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {

        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}