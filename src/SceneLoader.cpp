#include "SceneLoader.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <assimp/Importer.hpp>
#include <cassert>
#include <filesystem>
#include <string>

#include "Material.h"
#include "ResourceManager.h"
#include "TextureManager.h"
#include "glm/geometric.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/material.h>
#include <assimp/matrix4x4.h>
#include <assimp/types.h>
#include <assimp/vector3.h>
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <unordered_map>
#include <vector>

#include "Node.h"
#include "Primitive.h"
#include "Resources.h"
#include "Scene.h"
#include "VertexArray.h"

GLenum getIndexFormat(uint32_t format);

glm::mat4 getBaseTransform(aiNode& node, const aiScene& scene) {
	glm::mat4 transform;
	aiMatrix4x4 base = node.mTransformation;

	transform[0][0] = base.a1;
	transform[1][0] = base.a2;
	transform[2][0] = base.a3;
	transform[3][0] = base.a4;
	transform[0][1] = base.b1;
	transform[1][1] = base.b2;
	transform[2][1] = base.b3;
	transform[3][1] = base.b4;
	transform[0][2] = base.c1;
	transform[1][2] = base.c2;
	transform[2][2] = base.c3;
	transform[3][2] = base.c4;
	transform[0][3] = base.d1;
	transform[1][3] = base.d2;
	transform[2][3] = base.d3;
	transform[3][3] = base.d4;

	if (node.mParent == nullptr) return transform;

	return transform * getBaseTransform(*node.mParent, scene);
}

Primitive loadMesh(
	aiMesh& mesh, std::unordered_map<uint32_t, MaterialHandle>& materialCache
) {
	std::vector<unsigned char> vertices;
	float colliderSize = 0;
	for (int i = 0; i < mesh.mNumVertices; i++) {
		aiVector3t<float> position = mesh.mVertices[i];

		colliderSize = std::max(position.Length(), colliderSize);
		vertices.insert(
			vertices.end(),
			(unsigned char*)&position,
			(unsigned char*)&position + 12
		);

		aiVector3t<float> normal = mesh.mNormals[i];
		vertices.insert(
			vertices.end(),
			(unsigned char*)&normal,
			(unsigned char*)&normal + 12
		);

		aiVector3D textCoord = mesh.mTextureCoords[0][i];
		vertices.insert(
			vertices.end(),
			(unsigned char*)&textCoord,
			(unsigned char*)&textCoord + 8
		);
	}

	std::vector<unsigned char> indices;
	for (int i = 0; i < mesh.mNumFaces; i++) {
		aiFace face = mesh.mFaces[i];

		assert(face.mNumIndices == 3);

		indices.insert(
			indices.end(),
			(unsigned char*)face.mIndices,
			(unsigned char*)face.mIndices + 12
		);
	}
	VertexArray::Specifications specs { .type = VertexArray::Type::STATIC_MESH,
		                                .vertices = vertices,
		                                .indices = indices,
		                                .indicesFormat = GL_UNSIGNED_INT };
	VertexArray vao(specs);

	return Primitive(vao, materialCache.at(mesh.mMaterialIndex), colliderSize);
}
struct SceneLoadSpecs {
	std::unordered_map<uint32_t, MaterialHandle> materialCache;
	Scene& baseScene;
	const aiScene& importedScene;
};

void loadScene(aiNode& root, SceneLoadSpecs& specs) {
	if (root.mNumMeshes > 0) {
		glm::mat4 transform = getBaseTransform(root, specs.importedScene);

		glm::vec3 scale;
		glm::quat orientation;
		glm::vec3 position;
		glm::vec4 perspective;
		glm::vec3 skew;

		glm::decompose(
			transform, scale, orientation, position, skew, perspective
		);

		for (uint32_t i = 0; i < root.mNumMeshes; i++) {
			specs.baseScene.m_primitives.push_back(loadMesh(
				*specs.importedScene.mMeshes[root.mMeshes[i]],
				specs.materialCache
			));
		}
	}

	for (int i = 0; i < root.mNumChildren; i++) {
		loadScene(*root.mChildren[i], specs);
	}
}

struct TextureLoadSpecs {
	aiMaterial* material;
	aiTextureType type;

	TextureManager& textureManager;
	std::unordered_map<aiString*, TextureHandle>& cache;
	std::filesystem::path& folderPath;
};
TextureHandle loadTexture(TextureLoadSpecs specs) {
	aiString path;
	specs.material->GetTexture(specs.type, 0, &path);

	if (path.length == 0) return TextureHandle::UNASSIGNED;

	TextureHandle handle = specs.textureManager.loadTexture(
		specs.folderPath / std::filesystem::path(path.C_Str())
	);
	return handle;
}

std::unordered_map<uint32_t, MaterialHandle> loadMaterials(
	const aiScene& scene,
	std::filesystem::path& texturePath,
	ResourceManager& resourceManager
) {
	std::unordered_map<uint32_t, MaterialHandle> materialCache;
	std::unordered_map<aiString*, TextureHandle> textureCache;

	for (uint32_t i = 0; i < scene.mNumMaterials; i++) {
		aiMaterial* materialInstance = scene.mMaterials[i];

		PBRMaterialValues values;

		TextureLoadSpecs specs { .material = materialInstance,
			                     .type = aiTextureType_DIFFUSE,

			                     .textureManager =
			                         resourceManager.getTextureManager(),
			                     .cache = textureCache,
			                     .folderPath = texturePath };

		specs.type = aiTextureType_DIFFUSE;

		values.albedo = loadTexture(specs);
		aiColor4D diffuseColor;

		if (materialInstance->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) ==
		    AI_SUCCESS)
			values.albedoColor = glm::vec4(
				diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a
			);

		specs.type = aiTextureType_NORMALS;
		values.normal = loadTexture(specs);

		specs.type = aiTextureType_EMISSION_COLOR;
		values.emission = loadTexture(specs);
		float emissiveValue;
		if (materialInstance->Get(
				AI_MATKEY_EMISSIVE_INTENSITY, emissiveValue
			) == AI_SUCCESS)
			values.emissionValue = emissiveValue;

		specs.type = aiTextureType_UNKNOWN;
		values.roughnessMetallic = loadTexture(specs);
		float metallicValue;
		if (materialInstance->Get(AI_MATKEY_METALLIC_FACTOR, metallicValue) ==
		    AI_SUCCESS)
			values.metallicValue = metallicValue;

		float roughnessValue;
		if (materialInstance->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessValue) ==
		    AI_SUCCESS)
			values.roughnessValue = roughnessValue;

		Material material = Material::StandardPBRMaterial(values);
		MaterialHandle handle = resourceManager.registerMaterial(material);

		materialCache.insert({ i, handle });
	}

	return materialCache;
}

Scene SceneLoader::Load(
	std::filesystem::path& path, ResourceManager& resourceManager
) {
	Assimp::Importer importer;
	auto import = importer.ReadFile(
		path.string().c_str(), aiProcessPreset_TargetRealtime_Quality
	);
	auto folderPath = path.parent_path();
	auto materialCache = loadMaterials(*import, folderPath, resourceManager);

	Scene scene;
	SceneLoadSpecs specs {
		.materialCache = materialCache,
		.baseScene = scene,
		.importedScene = *import,

	};
	loadScene(*import->mRootNode, specs);

	float lowbound = 0, highbound = 0;
	for (Primitive& primitive : scene.m_primitives) {
		lowbound = std::min(
			glm::length(primitive.getPosition()) - primitive.getSize(), lowbound
		);
		highbound = std::max(
			glm::length(primitive.getPosition()) + primitive.getSize(),
			highbound
		);
	}
	scene.m_lowBoundHeight = lowbound;
	scene.m_highBoundHeight = highbound;

	return scene;
}