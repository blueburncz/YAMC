#pragma once

#include <Config.hpp>

#include <assimp/scene.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

template<typename OUT, typename IN>
void WriteSingle(std::ofstream& _file, IN _value)
{
	OUT _out = (OUT)_value;
	_file.write(reinterpret_cast<const char*>(&_out), sizeof(_out));
}

template<typename OUT, typename IN>
void WriteArray(std::ofstream& _file, IN* _values, uint32_t _length)
{
	for (uint32_t i = 0; i < _length; ++i)
	{
		WriteSingle<OUT>(_file, _values[i]);
	}
}

void WriteString(std::ofstream& _file, const char* _value);

void WriteMesh(std::ofstream& _file, const aiScene& _scene, const aiMesh& _mesh, const SConfig& _conf);

bool WriteScene(std::ofstream& _file, const aiScene& _scene, const SConfig& _conf);
