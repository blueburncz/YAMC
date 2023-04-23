#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define PRIMITIVE_TYPE_NAME(_type) \
	(((_type & aiPrimitiveType_POINT) != 0) ? "pr_pointlist" \
	: (((_type & aiPrimitiveType_LINE) != 0) ? "pr_linelist" \
	: (((_type & aiPrimitiveType_TRIANGLE) != 0) ? "pr_trianglelist" \
	: "unknown")))

#define MESSAGE_HELP \
"Usage\n" \
"\n" \
"  yamc [-h] PATH_IN [PATH_OUT] [-c/-C] [-f] [-i] [-n/-N] [-p] [-t] [-u] [-2]\n" \
"       [-y] [-z]\n" \
"\n" \
"Arguments\n" \
"\n" \
"  -h       = Show this help message and exit.\n" \
"  PATH_IN  = The input file path.\n" \
"  PATH_OUT = The output file path. Defaults to the input file path with \".bin\"\n" \
"             file extension if not specified.\n" \
"  -c       = Export vertex colors. White is used if the model has none. Cannot\n" \
"             be combined with -C!\n" \
"  -C       = Bake material colors to vertex colors and export. Cannot be\n" \
"             combined with -c!\n" \
"  -f       = Flip texture coordinates on the Y axis.\n" \
"  -i       = Invert vertex winding order. Default is clockwise for backfaces!\n" \
"  -n       = Export normal vectors. If the model has none, generate flat\n" \
"             normals. Cannot be combined with -N!\n" \
"  -N       = Export normal vectors. If the model has none, generate smooth\n" \
"             normals. Cannot be combined with -n!\n" \
"  -p       = Export 3D vertex positions.\n" \
"  -t       = Export tangent vectors and bitangent signs.\n" \
"  -u       = Export texture coordinates. Zero is used if the model has none.\n" \
"  -2       = Export second texture coordinate layer. Zero is used if the model\n" \
"             has none.\n" \
"  -y       = Override output file if it already exists.\n" \
"  -z       = Convert model to Z-up coordinate system. Uses counter-clockwise\n" \
"             vertex winding order for backfaces!\n" \
"\n" \
"If you do not pass any arguments that affect vertex format, arguments pNufC are\n" \
"used. These make a model that is compatible with GM's built-in shaders."

#define MESSAGE_MULTIPLE_MATERIALS \
"WARNING: Model consists of multiple materials, but this tool collapses the\n" \
"entire model into a single vertex buffer! It is recommended to use a single\n" \
"material for the entire model instead!"

enum class EUpVector
{
	NegativeY,
	PositiveZ,
};

struct SConfig
{
	void Clear()
	{
		WriteNormals = false;
		WriteTextureCoords = false;
		WriteTextureCoords2 = false;
		WriteColors = false;
		WriteMaterialColors = false;
		WriteTangents = false;
		UpVector = EUpVector::NegativeY;
		FlipUVs = false;
		InvertWinding = false;
		Flags = 0;
	}

	void Default()
	{
		WriteNormals = true;
		WriteTextureCoords = true;
		WriteTextureCoords2 = false;
		WriteColors = false;
		WriteMaterialColors = true;
		WriteTangents = false;
		UpVector = EUpVector::NegativeY;
		FlipUVs = true;
		InvertWinding = false;
		Flags = 0;
	}

	bool WriteNormals;
	bool WriteTextureCoords;
	bool WriteTextureCoords2;
	bool WriteColors;
	bool WriteMaterialColors;
	bool WriteTangents;
	EUpVector UpVector;
	bool FlipUVs;
	bool InvertWinding;
	uint32_t Flags;
};

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

void WriteString(std::ofstream& _file, const char* _value)
{
	_file.write(_value, strlen(_value) + 1);
}

inline aiVector3D Vec3ConvertUp(const aiVector3D& _v, EUpVector _up)
{
	aiVector3D res;
	if (_up == EUpVector::NegativeY)
	{
		res.x = _v.x;
		res.y = -_v.y;
		res.z = -_v.z;
	}
	else if (_up == EUpVector::PositiveZ)
	{
		res.x = _v.x;
		res.y = _v.z;
		res.z = _v.y;
	}
	return res;
}

inline aiVector3D Vec3Cross(const aiVector3D& _v1, const aiVector3D& _v2)
{
	aiVector3D res;
	res.x = _v1.y * _v2.z - _v1.z * _v2.y;
	res.y = _v1.z * _v2.x - _v1.x * _v2.z;
	res.z = _v1.x * _v2.y - _v1.y * _v2.x;
	return res;
}

inline float Vec3Dot(const aiVector3D& _v1, const aiVector3D& _v2)
{
	return (_v1.x * _v2.x
		+ _v1.y * _v2.y
		+ _v1.z * _v2.z);
}

inline float GetBitangentSign(
	const aiVector3D& _normal,
	const aiVector3D& _tangent,
	const aiVector3D& _bitangent)
{
	aiVector3D cross = Vec3Cross(_normal, _tangent);
	float dot = Vec3Dot(cross, _bitangent);
	return (dot < 0.0f) ? -1.0f : 1.0f;
}

void WriteMesh(std::ofstream& _file, const aiScene& _scene, const aiMesh& _mesh, SConfig& _conf)
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
			aiVector3D position = Vec3ConvertUp(_mesh.mVertices[i], _conf.UpVector);
			WriteSingle<float>(_file, position.x);
			WriteSingle<float>(_file, position.y);
			WriteSingle<float>(_file, position.z);
			// std::cout << position.x << ", " << position.y << ", " << position.z << ", ";

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

int main(int argc, const char** argv)
{
	uint32_t flagsCommon = 0
		| aiProcess_Triangulate
		| aiProcess_SortByPType
		| aiProcess_PreTransformVertices
		| aiProcess_GenUVCoords
		| aiProcess_GlobalScale
		| aiProcess_CalcTangentSpace
		| aiProcess_FlipWindingOrder
		;

	SConfig confDefault;
	confDefault.Default();
	confDefault.Flags = flagsCommon
		| aiProcess_FindInvalidData
		| aiProcess_GenSmoothNormals
		;

	SConfig confCustom;
	confCustom.Clear();
	confCustom.Flags = flagsCommon;

	SConfig* confCurrent = &confDefault;

	const char* fin = nullptr;
	const char* fout = nullptr;
	bool foutOverride = false;
	bool showHelpAndExit = false;
	int generateNormals = 0;
	int writeColors = 0;

	for (int i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];

		if (arg[0] == '-')
		{
			for (size_t j = strlen(arg) - 1; j > 0; --j)
			{
				switch (arg[j])
				{
				case 'c':
					if (writeColors == 2)
					{
						std::cout << "ERROR: Cannot combine arguments c and C!" << std::endl;
						return EXIT_FAILURE;
					}
					confCurrent = &confCustom;
					confCurrent->WriteColors = true;
					writeColors = 1;
					break;

				case 'C':
					if (writeColors == 1)
					{
						std::cout << "ERROR: Cannot combine arguments c and C!" << std::endl;
						return EXIT_FAILURE;
					}
					confCurrent = &confCustom;
					confCurrent->WriteMaterialColors = true;
					writeColors = 2;
					break;

				case 'f':
					confCurrent = &confCustom;
					confCurrent->FlipUVs = true;
					break;

				case 'i':
					confCurrent = &confCustom;
					confCurrent->InvertWinding = true;
					break;

				case 'h':
					showHelpAndExit = true;
					break;

				case 'n':
					if (generateNormals == 2)
					{
						std::cout << "ERROR: Cannot combine arguments n and N!" << std::endl;
						return EXIT_FAILURE;
					}
					confCurrent = &confCustom;
					confCurrent->WriteNormals = true;
					confCurrent->Flags |= aiProcess_FindInvalidData | aiProcess_GenNormals;
					generateNormals = 1;
					break;

				case 'N':
					if (generateNormals == 1)
					{
						std::cout << "ERROR: Cannot combine arguments n and N!" << std::endl;
						return EXIT_FAILURE;
					}
					confCurrent = &confCustom;
					confCurrent->WriteNormals = true;
					confCurrent->Flags |= aiProcess_FindInvalidData | aiProcess_GenSmoothNormals;
					generateNormals = 2;
					break;

				case 'p':
					confCurrent = &confCustom;
					break;

				case 't':
					confCurrent = &confCustom;
					confCurrent->WriteTangents = true;
					break;

				case 'u':
					confCurrent = &confCustom;
					confCurrent->WriteTextureCoords = true;
					break;

				case '2':
					confCurrent = &confCustom;
					confCurrent->WriteTextureCoords2 = true;
					break;

				case 'y':
					foutOverride = true;
					break;

				case 'z':
					confDefault.UpVector = EUpVector::PositiveZ;
					confCustom.UpVector = EUpVector::PositiveZ;
					break;

				default:
					std::cout << "ERROR: Invalid argument -" << arg[j] << "!" << std::endl;
					return EXIT_FAILURE;
				}
			}

			continue;
		}
		else
		{
			if (fin == nullptr)
			{
				fin = arg;
				continue;
			}

			if (fout == nullptr)
			{
				fout = arg;
				continue;
			}
		}

		std::cout << "ERROR: Invalid argument " << arg << "!" << std::endl;
		return EXIT_FAILURE;
	}

	if (showHelpAndExit)
	{
		std::cout << MESSAGE_HELP << std::endl;
		return EXIT_SUCCESS;
	}

	if (fin == nullptr)
	{
		std::cout << "ERROR: Input file not specified!" << std::endl;
		return EXIT_FAILURE;
	}

	if (fout == nullptr)
	{
		fout = strdup(std::filesystem::path(fin).replace_extension(".bin").string().c_str());
	}

	if (!foutOverride && std::filesystem::exists(std::filesystem::path(fout)))
	{
		std::cout << "Output file already exists! Would you like to override it? (y/n): ";
		while (true)
		{
			int get = std::cin.get();
			if (get == 'y')
			{
				break;
			}
			else if (get == 'n')
			{
				return EXIT_SUCCESS;
			}
		}
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fin, confCurrent->Flags);
	if (!scene)
	{
		std::cout << "ERROR: Could not load model " << fin << "!" << std::endl;
		return EXIT_FAILURE;
	}

	if (scene->mNumMeshes == 0)
	{
		std::cout << "INFO: Model has no meshes, quitting..." << std::endl;
		return EXIT_SUCCESS;
	}

	std::ofstream file(fout, std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "ERROR: Could not open file " << fout << " for writing!" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Vertex format: position 3D, ";
	if (confCurrent->WriteNormals) std::cout << "normal, ";
	if (confCurrent->WriteTextureCoords) std::cout << "texcoord, ";
	if (confCurrent->WriteColors) std::cout << "color, ";
	if (confCurrent->WriteTangents) std::cout << "tangent and bitangent sign (float4), ";
	std::cout << std::endl;

	uint32_t primitiveType = scene->mMeshes[0]->mPrimitiveTypes;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		if (scene->mMeshes[i]->mPrimitiveTypes != primitiveType)
		{
			std::cout << "ERROR: Model must not consist of multiple primitive types!" << std::endl;
			return EXIT_FAILURE;
		}
	}

	std::cout
		<< "Primitive type: " << PRIMITIVE_TYPE_NAME(primitiveType) << std::endl
		<< "Up axis: " << ((confCurrent->UpVector == EUpVector::NegativeY) ? "-Y" : "Z") << std::endl
		<< "Invert vertex winding: " << (confCurrent->InvertWinding ? "Yes" : "No") << std::endl
		<< "Flip UV vertically: " << (confCurrent->FlipUVs ? "Yes" : "No") << std::endl
		<< "Bake materials to vertex colors: " << (confCurrent->WriteMaterialColors ? "Yes" : "No") << std::endl;

	uint32_t materialIndex = scene->mMeshes[0]->mMaterialIndex;
	bool mixedMaterials = false;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh& mesh = *scene->mMeshes[i];
		if (mesh.mMaterialIndex != materialIndex)
		{
			mixedMaterials = true;
		}
		WriteMesh(file, *scene, mesh, *confCurrent);
	}

	if (mixedMaterials && !confCurrent->WriteMaterialColors)
	{
		std::cout << MESSAGE_MULTIPLE_MATERIALS << std::endl;
	}

	file.flush();
	file.close();

	std::cout << "SUCCESS: Wrote vertex buffer to " << fout << "!" << std::endl;
	return EXIT_SUCCESS;
}
