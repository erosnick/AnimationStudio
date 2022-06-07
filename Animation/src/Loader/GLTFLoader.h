#pragma once

#include <Loader/cgltf.h>
#include <string>

namespace Loader
{
	cgltf_data* loadGLTFFile(const std::string& path);
}