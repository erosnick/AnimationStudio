#include <Loader/GLTFLoader.h>

#include <iostream>

namespace Loader
{
	cgltf_data* Loader::loadGLTFFile(const std::string& path)
	{
		cgltf_options options = {};
		cgltf_data* data = nullptr;
		
		cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
		
		if (result != cgltf_result_success)
		{
			std::cout << "Failed to parse glTF file: " << path << std::endl;
			cgltf_free(data);
			return nullptr;
		}

		result = cgltf_validate(data);

		if (result != cgltf_result_success)
		{
			std::cout << "Failed to validate glTF file: " << path << std::endl;
			cgltf_free(data);
			return nullptr;
		}
		
		return data;
	}
}

