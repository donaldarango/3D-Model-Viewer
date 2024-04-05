#include <vector>
#include <string>
#include "glm/glm.hpp"

#pragma once

std::vector<float> load_model_from_file(std::string &filename, glm::mat4 preTransform, size_t &numVertices);