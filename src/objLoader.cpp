#include "objLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <cstring>

// source: https://github.com/tinyobjloader/tinyobjloader
std::vector<float> load_model_from_file(std::string &filename, glm::mat4 preTransform, size_t &numVertices) {
	
	std::vector<float> vertices;

    tinyobj::ObjReaderConfig reader_config;
    // reader_config.mtl_search_path = "../data/"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filename, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    // auto& materials = reader.GetMaterials();

    // Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            // Loop over vertices in the face.
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
		    for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                glm::vec4 pos = {
                    attrib.vertices[3*size_t(idx.vertex_index)+0],
                    attrib.vertices[3*size_t(idx.vertex_index)+1],
                    attrib.vertices[3*size_t(idx.vertex_index)+2],
                    1
                };

                pos = preTransform * pos;

                glm::vec3 normal;
                if (idx.normal_index >= 0) { // if negative, no vertex normals
                    normal = {
                        attrib.normals[3 * idx.normal_index],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }
                else {
                    normal = {
                        0.0f,
                        0.0f,
                        0.0f
                    };
                }

                normal = glm::normalize(glm::mat3(preTransform) * normal);

                // set colors to be equal to pretransformed x,y,z of vertex
                glm::vec3 colors = {
                    // attrib.vertices[3 * idx.vertex_index],
                    // attrib.vertices[3 * idx.vertex_index + 1],
                    // attrib.vertices[3 * idx.vertex_index + 2],
                    1.0,
                    0.0,
                    0.0
                };

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
            index_offset += fv;

            shapes[s].mesh.material_ids[f];
        }
	}

    std::cout << "finished loading object" << std::endl;

	return vertices;
}