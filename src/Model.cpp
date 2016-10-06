#include "Model.h"

#include "GameException.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(std::string filename, bool invert) {
	std::vector<float> vertex_data, normal_data;
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Quality);// | aiProcess_FlipWindingOrder);
	if (!scene) {
		std::string log = "Unable to load mesh from ";
		log.append(filename);
		THROW_EXCEPTION(log);
	}

	/**
	  * FIXME: Alter loadRecursive, so that it also loads normal data
	  */
	//Load the model recursively into data
	loadRecursive(root, invert, vertex_data, normal_data, scene, scene->mRootNode);
	
	//Set the transformation matrix for the root node
	//These are hard-coded constants for the stanford bunny model.
	root.transform = glm::scale(root.transform, glm::vec3(6.44));
	root.transform = glm::translate(root.transform, glm::vec3(0.016800813, -0.11015295, 0.0014822669));

	n_vertices = vertex_data.size();

	std::vector<float> vertex_attrib_data;

	//Create the VBOs from the data.
	if (fmod(static_cast<float>(n_vertices), 3.0f) < 0.000001f) {
		// We want to create an interleaved VBO. To do that, we'll need a block of memory that contains the interleaved vertex attributes
		// FOR EVERY VERTEX IN THE NEW BUFFER THAT WILL CONTAIN BOTH POSITIONS AND NORMALS
		for (int offset = 0; offset < n_vertices; offset += 3)
		{
			vertex_attrib_data.push_back(vertex_data[offset]);
			vertex_attrib_data.push_back(vertex_data[offset + 1]);
			vertex_attrib_data.push_back(vertex_data[offset + 2]);
			vertex_attrib_data.push_back(normal_data[offset]);
			vertex_attrib_data.push_back(normal_data[offset + 1]);
			vertex_attrib_data.push_back(normal_data[offset + 2]);
		}
		vertices.reset(new GLUtils::VBO(vertex_attrib_data.data(), vertex_attrib_data.size()*sizeof(float))); // INTERLEAVED
		//vertices.reset(new GLUtils::VBO(vertex_data.data(), n_vertices*sizeof(float))); MARTIN MADE THIS LINE
		//normals.reset(new GLUtils::VBO(normal_data.data(), n_vertices*sizeof(float))); I MADE THIS LINE, AND WILL THEREFORE REMOVE IT LATER
	}
	else
		THROW_EXCEPTION("The number of vertices in the mesh is wrong");
}

Model::~Model() {

}

void Model::loadRecursive(MeshPart& part, bool invert,
			std::vector<float>& vertex_data, std::vector<float>& normal_data, const aiScene* scene, const aiNode* node) {
	//update transform matrix. notice that we also transpose it
	aiMatrix4x4 m = node->mTransformation;
	for (int j=0; j<4; ++j)
		for (int i=0; i<4; ++i)
			part.transform[j][i] = m[i][j];

	// draw all meshes assigned to this node
	for (unsigned int n=0; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];

		//apply_material(scene->mMaterials[mesh->mMaterialIndex]);

		part.first = vertex_data.size()/3; // I DON'T THINK THIS IS NEEDED, WHY DID MARTIN PUT THIS HERE?
		part.count = mesh->mNumFaces*3; // Since we are only dealing with triangles, number_of_faces * 3 = number_of_vertices

		//Allocate data
		vertex_data.reserve(vertex_data.size() + part.count*3);
		normal_data.reserve(normal_data.size() + part.count * 3);

		//Add the vertices from file   (FOR EVERY PRIMITIVE, THAT IS A TRIANGLE)
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];

			if(face->mNumIndices != 3)
				THROW_EXCEPTION("Only triangle meshes are supported");

			// FOR EVERY VERTEX THAT DEFINES THE PRIMITIVE TRIANGLE
			for(unsigned int i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				vertex_data.push_back(mesh->mVertices[index].x);
				vertex_data.push_back(mesh->mVertices[index].y);
				vertex_data.push_back(mesh->mVertices[index].z);
				normal_data.push_back(mesh->mNormals[index].x);
				normal_data.push_back(mesh->mNormals[index].y);
				normal_data.push_back(mesh->mNormals[index].z);
			}
		}
	}

	// load all children
	for (unsigned int n = 0; n < node->mNumChildren; ++n) {
		part.children.push_back(MeshPart());
		loadRecursive(part.children.back(), invert, vertex_data, normal_data, scene, node->mChildren[n]);
	}
}