#pragma once

#include <Args.hpp>
#include <math.hpp>

struct SConfig
{
	void Clear();
	void Default();
	void FromArgs(const SArgs& _args);

	bool WritePositions;
	bool WriteNormals;
	bool WriteTextureCoords;
	bool WriteTextureCoords2;
	bool WriteColors;
	bool WriteMaterialColors;
	bool WriteTangents;
	EAxis UpVector;
	bool FlipUVs;
	bool InvertWinding;
	uint32_t Flags;
};
