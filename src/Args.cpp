#include <Args.hpp>

#include <filesystem>
#include <iostream>

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

bool ParseArgs(int _argc, const char** _argv, SArgs& _argsOut)
{
	for (int i = 1; i < _argc; ++i)
	{
		const char* arg = _argv[i];

		if (arg[0] == '-')
		{
			for (size_t j = strlen(arg) - 1; j > 0; --j)
			{
				switch (arg[j])
				{
				case 'c':
					if (_argsOut.WriteMaterialColors)
					{
						std::cout << "ERROR: Cannot combine arguments c and C!" << std::endl;
						return false;
					}
					_argsOut.WriteColors = true;
					break;

				case 'C':
					if (_argsOut.WriteColors)
					{
						std::cout << "ERROR: Cannot combine arguments c and C!" << std::endl;
						return false;
					}
					_argsOut.WriteMaterialColors = true;
					break;

				case 'f':
					_argsOut.FlipUVs = true;
					break;

				case 'i':
					_argsOut.InvertWinding = true;
					break;

				case 'h':
					_argsOut.ShowHelpAndExit = true;
					break;

				case 'n':
					if (_argsOut.WriteSmoothNormals)
					{
						std::cout << "ERROR: Cannot combine arguments n and N!" << std::endl;
						return false;
					}
					_argsOut.WriteNormals = true;
					break;

				case 'N':
					if (_argsOut.WriteNormals)
					{
						std::cout << "ERROR: Cannot combine arguments n and N!" << std::endl;
						return false;
					}
					_argsOut.WriteSmoothNormals = true;
					break;

				case 'p':
					_argsOut.WritePositions = true;
					break;

				case 't':
					_argsOut.WriteTangents = true;
					break;

				case 'u':
					_argsOut.WriteTextureCoords = true;
					break;

				case '2':
					_argsOut.WriteTextureCoords2 = true;
					break;

				case 'y':
					_argsOut.OverrideOutputFile = true;
					break;

				case 'z':
					_argsOut.ConvertToZUp = true;
					break;

				default:
					std::cout << "ERROR: Invalid argument -" << arg[j] << "!" << std::endl;
					return false;
				}
			}

			continue;
		}
		else
		{
			if (_argsOut.PathIn == nullptr)
			{
				_argsOut.PathIn = arg;
				continue;
			}

			if (_argsOut.PathOut == nullptr)
			{
				_argsOut.PathOut = arg;
				continue;
			}
		}

		std::cout << "ERROR: Invalid argument " << arg << "!" << std::endl;
		return false;
	}

	if (_argsOut.ShowHelpAndExit)
	{
		std::cout << MESSAGE_HELP << std::endl;
		return true;
	}

	if (_argsOut.PathIn == nullptr)
	{
		std::cout << "ERROR: Input file not specified!" << std::endl;
		return false;
	}

	if (_argsOut.PathOut == nullptr)
	{
		_argsOut.PathOut = strdup(std::filesystem::path(_argsOut.PathIn)
			.replace_extension(".bin").string().c_str());
	}

	return true;
}
