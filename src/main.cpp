#include <Args.hpp>
#include <Config.hpp>
#include <writing.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, const char** argv)
{
	SArgs args;
	if (!ParseArgs(argc, argv, args))
	{
		return EXIT_FAILURE;
	}

	SConfig conf;
	conf.FromArgs(args);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(args.PathIn, conf.Flags);
	if (!scene)
	{
		std::cout << "ERROR: Could not load model " << args.PathIn << "!" << std::endl;
		return EXIT_FAILURE;
	}

	if (scene->mNumMeshes == 0)
	{
		std::cout << "INFO: Model has no meshes, quitting..." << std::endl;
		return EXIT_SUCCESS;
	}

	if (!args.OverrideOutputFile
		&& std::filesystem::exists(std::filesystem::path(args.PathOut)))
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

	std::ofstream file(args.PathOut, std::ios::out | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "ERROR: Could not open file " << args.PathOut << " for writing!" << std::endl;
		return EXIT_FAILURE;
	}

	if (!WriteScene(file, *scene, conf))
	{
		return EXIT_FAILURE;
	}

	file.flush();
	file.close();

	std::cout << "SUCCESS: Wrote vertex buffer to " << args.PathOut << "!" << std::endl;
	return EXIT_SUCCESS;
}
