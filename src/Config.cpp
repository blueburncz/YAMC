#include <Config.hpp>

#include <assimp/postprocess.h>

void SConfig::Clear()
{
	WritePositions = false;
	WriteNormals = false;
	WriteTextureCoords = false;
	WriteTextureCoords2 = false;
	WriteColors = false;
	WriteMaterialColors = false;
	WriteTangents = false;
	UpVector = EAxis::NegativeY;
	FlipUVs = false;
	InvertWinding = false;
	Flags = 0;
}

void SConfig::Default()
{
	WritePositions = true;
	WriteNormals = true;
	WriteTextureCoords = true;
	WriteTextureCoords2 = false;
	WriteColors = false;
	WriteMaterialColors = true;
	WriteTangents = false;
	UpVector = EAxis::NegativeY;
	FlipUVs = true;
	InvertWinding = false;
	Flags = 0;
}

void SConfig::FromArgs(const SArgs& _args)
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

	if (false
		|| _args.WritePositions
		|| _args.WriteNormals
		|| _args.WriteSmoothNormals
		|| _args.WriteTextureCoords
		|| _args.WriteTextureCoords2
		|| _args.WriteColors
		|| _args.WriteMaterialColors
		|| _args.WriteTangents)
	{
		Clear();
		Flags = flagsCommon;

		WritePositions = _args.WritePositions;

		if (_args.WriteNormals)
		{
			WriteNormals = true;
			Flags |= aiProcess_FindInvalidData | aiProcess_GenNormals;
		}
		else if (_args.WriteSmoothNormals)
		{
			WriteNormals = true;
			Flags |= aiProcess_FindInvalidData | aiProcess_GenSmoothNormals;
		}

		WriteTextureCoords = _args.WriteTextureCoords;
		WriteTextureCoords2 = _args.WriteTextureCoords2;

		WriteColors = _args.WriteColors;
		WriteMaterialColors = _args.WriteMaterialColors;

		WriteTangents = _args.WriteTangents;
	}
	else
	{
		Default();
		Flags = flagsCommon
			| aiProcess_FindInvalidData
			| aiProcess_GenSmoothNormals
			;
	}

	if (_args.ConvertToZUp)
	{
		UpVector = EAxis::PositiveZ;
	}

	FlipUVs = _args.FlipUVs;
	InvertWinding = _args.InvertWinding;
}
