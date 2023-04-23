#include <writing.hpp>

#include <cstdint>
#include <iostream>

#define PRIMITIVE_TYPE_NAME(_type) \
	(((_type & aiPrimitiveType_POINT) != 0) ? "pr_pointlist" \
	: (((_type & aiPrimitiveType_LINE) != 0) ? "pr_linelist" \
	: (((_type & aiPrimitiveType_TRIANGLE) != 0) ? "pr_trianglelist" \
	: "unknown")))

#define MESSAGE_MULTIPLE_MATERIALS \
"WARNING: Model consists of multiple materials, but this tool collapses the\n" \
"entire model into a single vertex buffer! It is recommended to use a single\n" \
"material for the entire model instead!"

void WriteString(std::ofstream& _file, const char* _value)
{
	_file.write(_value, strlen(_value) + 1);
}

void WriteMesh(std::ofstream& _file, const aiScene& _scene, const aiMesh& _mesh, const SConfig& _conf)
{
	bool hasNormals = _mesh.HasNormals();
	bool hasTextureCoords = _mesh.HasTextureCoords(0);
	bool hasTextureCoords2 = _mesh.HasTextureCoords(1);
	bool hasVertexColors = _mesh.HasVertexColors(0);
	bool hasTangentsAndBitangents = _mesh.HasTangentsAndBitangents();
	aiVector3D up(0.0f, 1.0f, 0.0f);

	aiMaterial* material = _scene.mMaterials[_mesh.mMaterialIndex];
	aiColor3D materialColor(1.0f, 1.0f, 1.0f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, materialColor);
	float materialOpacity = 1.0f;
	material->Get(AI_MATKEY_OPACITY, materialOpacity);

	for (uint32_t f = 0; f < _mesh.mNumFaces; ++f)
	{
		const aiFace& face = _mesh.mFaces[f];

		for (uint32_t v = 0; v < face.mNumIndices; ++v)
		{
			uint32_t vReal = _conf.InvertWinding ? (face.mNumIndices - (v + 1)) : v;
			uint32_t i = face.mIndices[vReal];

			// Position
			if (_conf.WritePositions)
			{
				aiVector3D position = Vec3ConvertUp(_mesh.mVertices[i], _conf.UpVector);
				WriteSingle<float>(_file, position.x);
				WriteSingle<float>(_file, position.y);
				WriteSingle<float>(_file, position.z);
				// std::cout << position.x << ", " << position.y << ", " << position.z << ", ";
			}

			// Normal vectors
			aiVector3D normal = Vec3ConvertUp(hasNormals ? _mesh.mNormals[i] : up, _conf.UpVector);

			if (_conf.WriteNormals)
			{
				WriteSingle<float>(_file, normal.x);
				WriteSingle<float>(_file, normal.y);
				WriteSingle<float>(_file, normal.z);
				// std::cout << normal.x << ", " << normal.y << ", " << normal.z << ", ";
			}

			// Texture coords
			if (_conf.WriteTextureCoords)
			{
				if (hasTextureCoords)
				{
					aiVector3D uv = aiVector3D(_mesh.mTextureCoords[0][i]);
					if (_conf.FlipUVs)
					{
						uv.y = 1.0f - uv.y;
					}
					WriteSingle<float>(_file, uv.x);
					WriteSingle<float>(_file, uv.y);
					// std::cout << uv.x << ", " << uv.y << ", ";
				}
				else
				{
					WriteSingle<float>(_file, 0.0f);
					WriteSingle<float>(_file, 0.0f);
					// std::cout << 0.0f << ", " << 0.0f << ", ";
				}
			}

			// Texture coords 2
			if (_conf.WriteTextureCoords2)
			{
				if (hasTextureCoords2)
				{
					aiVector3D uv = aiVector3D(_mesh.mTextureCoords[1][i]);
					if (_conf.FlipUVs)
					{
						uv.y = 1.0f - uv.y;
					}
					WriteSingle<float>(_file, uv.x);
					WriteSingle<float>(_file, uv.y);
					// std::cout << uv.x << ", " << uv.y << ", ";
				}
				else
				{
					WriteSingle<float>(_file, 0.0f);
					WriteSingle<float>(_file, 0.0f);
					// std::cout << 0.0f << ", " << 0.0f << ", ";
				}
			}

			// Colors
			if (_conf.WriteColors)
			{
				if (hasVertexColors)
				{
					const aiColor4D& color = _mesh.mColors[0][i];
					uint32_t colorEncoded = 0
						| ((uint32_t)(color.a * 255.0f) << 24)
						| ((uint32_t)(color.b * 255.0f) << 16)
						| ((uint32_t)(color.g * 255.0f) << 8)
						| ((uint32_t)(color.r * 255.0f) << 0);
					WriteSingle<uint32_t>(_file, colorEncoded);
					// std::cout << colorEncoded << ", ";
				}
				else
				{
					WriteSingle<uint32_t>(_file, 0xFFFFFFFF);
					// std::cout << 0xFFFFFFFF << ", ";
				}
			}
			else if (_conf.WriteMaterialColors)
			{
				uint32_t colorEncoded = 0
					| ((uint32_t)(materialOpacity * 255.0f) << 24)
					| ((uint32_t)(materialColor.b * 255.0f) << 16)
					| ((uint32_t)(materialColor.g * 255.0f) << 8)
					| ((uint32_t)(materialColor.r * 255.0f) << 0);
				WriteSingle<uint32_t>(_file, colorEncoded);
				// std::cout << colorEncoded << ", ";
			}

			// Tangent vector and bitangent sign
			if (_conf.WriteTangents)
			{
				if (hasTangentsAndBitangents)
				{
					aiVector3D tangent = Vec3ConvertUp(_mesh.mTangents[i], _conf.UpVector);
					WriteSingle<float>(_file, tangent.x);
					WriteSingle<float>(_file, tangent.y);
					WriteSingle<float>(_file, tangent.z);
					// std::cout << tangent.x << ", " << tangent.y << ", " << tangent.z << ", ";

					aiVector3D bitangent = Vec3ConvertUp(_mesh.mBitangents[i], _conf.UpVector);
					float bitangentSign = GetBitangentSign(normal, tangent, bitangent);
					WriteSingle<float>(_file, bitangentSign);
					// std::cout << bitangentSign << ", ";
				}
				else
				{
					// Can ignore Vec3ToZUp here since both Y and Z are 0
					WriteSingle<float>(_file, 1.0f);
					WriteSingle<float>(_file, 0.0f);
					WriteSingle<float>(_file, 0.0f);
					WriteSingle<float>(_file, 1.0f);
					// std::cout << 1.0f << ", " << 0.0f << ", " << 0.0f << ", " << 1.0f << ", ";
				}
			}

			// std::cout << std::endl;
		}
	}
}

bool WriteScene(std::ofstream& _file, const aiScene& _scene, const SConfig& _conf)
{
	std::cout << "Vertex format: position 3D, ";
	if (_conf.WriteNormals) std::cout << "normal, ";
	if (_conf.WriteTextureCoords) std::cout << "texcoord, ";
	if (_conf.WriteColors || _conf.WriteMaterialColors) std::cout << "color, ";
	if (_conf.WriteTangents) std::cout << "tangent and bitangent sign (float4), ";
	std::cout << std::endl;

	uint32_t primitiveType = _scene.mMeshes[0]->mPrimitiveTypes;
	for (uint32_t i = 0; i < _scene.mNumMeshes; ++i)
	{
		if (_scene.mMeshes[i]->mPrimitiveTypes != primitiveType)
		{
			std::cout << "ERROR: Model must not consist of multiple primitive types!" << std::endl;
			return false;
		}
	}

	std::cout
		<< "Primitive type: " << PRIMITIVE_TYPE_NAME(primitiveType) << std::endl
		<< "Up axis: " << ((_conf.UpVector == EAxis::NegativeY) ? "-Y" : "Z") << std::endl
		<< "Invert vertex winding: " << (_conf.InvertWinding ? "Yes" : "No") << std::endl
		<< "Flip UV vertically: " << (_conf.FlipUVs ? "Yes" : "No") << std::endl
		<< "Bake materials to vertex colors: " << (_conf.WriteMaterialColors ? "Yes" : "No") << std::endl;

	uint32_t materialIndex = _scene.mMeshes[0]->mMaterialIndex;
	bool mixedMaterials = false;
	for (uint32_t i = 0; i < _scene.mNumMeshes; ++i)
	{
		const aiMesh& mesh = *_scene.mMeshes[i];
		if (mesh.mMaterialIndex != materialIndex)
		{
			mixedMaterials = true;
		}
		WriteMesh(_file, _scene, mesh, _conf);
	}

	if (mixedMaterials && !_conf.WriteMaterialColors)
	{
		std::cout << MESSAGE_MULTIPLE_MATERIALS << std::endl;
	}

	return true;
}
