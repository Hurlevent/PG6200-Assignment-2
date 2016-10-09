#include "Model.h"

#include "GameException.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(std::string filename, bool invert) : min_dim(std::numeric_limits<float>::max()), max_dim(-std::numeric_limits<float>::max()) {
	std::vector<float> vertex_data, normal_data;
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Quality);// | aiProcess_FlipWindingOrder);
	if (!scene) {
		std::string log = "Unable to load mesh from ";
		log.append(filename);
		THROW_EXCEPTION(log);
	}

	//Load the model recursively into data
	loadRecursive(root, invert, vertex_data, normal_data, scene, scene->mRootNode);

	n_vertices = vertex_data.size();

	// Create the Axis-aligned bounding box
	MakeBoudingBox(vertex_data);
	

	root.transform = glm::scale(root.transform, FindScaleVector());
	root.transform = glm::translate(root.transform, FindTranslateVector());


	//Create the VBOs from the data.
	if (fmod(static_cast<float>(n_vertices), 3.0f) < 0.000001f) {

		std::vector<float> vertex_attrib_data = MakeInterleavedVBO(vertex_data, normal_data);

		vertices.reset(new GLUtils::VBO(vertex_attrib_data.data(), vertex_attrib_data.size()*sizeof(float)));
		
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

		//apply_material(scene->mMaterials[mesh->mMaterialIndex]); // I'll leave this line up, in case I want to continue working on this project in the future

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

// We want to scale our model to an appropriate size
glm::vec3 Model::FindScaleVector()
{
	glm::vec3 difference = glm::abs(min_dim - max_dim);

	float biggest_diff;

	if (difference.x > difference.y)
	{
		biggest_diff = difference.x;
	}
	else
	{
		biggest_diff = difference.y;
	}

	if (difference.z > biggest_diff)
	{
		biggest_diff = difference.z;
	}

	glm::vec3 scale_vec3(1.0f / biggest_diff);

	return scale_vec3;
}

// We want to translate the model to the center of our window
glm::vec3 Model::FindTranslateVector()
{
	float translate_x = max_dim.x + min_dim.x;
	float translate_y = max_dim.y + min_dim.y;
	float translate_z = max_dim.z + min_dim.z;

	glm::vec3 translate_vec3(translate_x, translate_y, translate_z);

	translate_vec3 /= 2.0f;
	translate_vec3 *= -1.0f;

	return translate_vec3;
}

// We want to create an interleaved VBO. To do that, we'll need a block of memory that contains the interleaved vertex attributes
std::vector<float> Model::MakeInterleavedVBO(std::vector<float> vertex_data, std::vector<float> normal_data)
{
	std::vector<float> vertex_attrib_data; // The final container where the interleaved VBO will be stored

	for (unsigned int offset = 0; offset < n_vertices; offset += 3)
	{
		vertex_attrib_data.push_back(vertex_data[offset]);
		vertex_attrib_data.push_back(vertex_data[offset + 1]);
		vertex_attrib_data.push_back(vertex_data[offset + 2]);
		vertex_attrib_data.push_back(normal_data[offset]);
		vertex_attrib_data.push_back(normal_data[offset + 1]);
		vertex_attrib_data.push_back(normal_data[offset + 2]);
	}

	return vertex_attrib_data;
}


void Model::MakeBoudingBox(std::vector<float> vertex_data)
{
	// Finding the Axis-aligned bounding box
	for (unsigned int offset = 0; offset < n_vertices; offset += 3)
	{
		if (vertex_data[offset] > max_dim.x)
		{
			max_dim.x = vertex_data[offset];
		}
		else if (vertex_data[offset + 1] > max_dim.y)
		{
			max_dim.y = vertex_data[offset + 1];
		}
		else if (vertex_data[offset + 2] > max_dim.z)
		{
			max_dim.z = vertex_data[offset + 2];
		}
		else if (vertex_data[offset] < min_dim.x)
		{
			min_dim.x = vertex_data[offset];
		}
		else if (vertex_data[offset + 1] < min_dim.y)
		{
			min_dim.y = vertex_data[offset + 1];
		}
		else if (vertex_data[offset + 2] < min_dim.z)
		{
			min_dim.z = vertex_data[offset + 2];
		}
	}
}