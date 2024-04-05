#include "objLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <cstring>

std::vector<float> load_model_from_file(std::string &filename, glm::mat4 preTransform, size_t &numVertices) {
	
	std::vector<float> vertices;

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;

    const char* c_filename = filename.c_str();

	if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, c_filename)) {
		std::cout << warning << error << '\n';
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {

			glm::vec4 pos = {
				attributes.vertices[3 * index.vertex_index],
				attributes.vertices[3 * index.vertex_index + 1],
				attributes.vertices[3 * index.vertex_index + 2],
				1
			};

			pos = preTransform * pos;

			glm::vec3 normal = {
				attributes.normals[3 * index.normal_index],
				attributes.normals[3 * index.normal_index + 1],
				attributes.normals[3 * index.normal_index + 2]
			};

			normal = glm::normalize(glm::mat3(preTransform) * normal);

            // set colors to be equal to pretransformed x,y,z of vertex
			glm::vec3 colors = {
                attributes.vertices[3 * index.vertex_index],
				attributes.vertices[3 * index.vertex_index + 1],
				attributes.vertices[3 * index.vertex_index + 2],
            };

            // TO ADD: GENERATE VERTEX NORMALS BY TAKING CROSS PRODUCT, IF NO NORMALS IN .OBJ FILE

			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);
            vertices.push_back(colors.r);
            vertices.push_back(colors.g);
            vertices.push_back(colors.b);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);

            numVertices += 3;
		}
	}

    std::cout << "finished loading object" << std::endl;

	return vertices;
}