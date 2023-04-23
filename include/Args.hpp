#pragma once

struct SArgs
{
	bool ShowHelpAndExit = false;
	const char* PathIn = nullptr;
	const char* PathOut = nullptr;
	bool WriteColors = false;
	bool WriteMaterialColors = false;
	bool FlipUVs = false;
	bool InvertWinding = false;
	bool WriteNormals = false;
	bool WriteSmoothNormals = false;
	bool WritePositions = false;
	bool WriteTangents = false;
	bool WriteTextureCoords = false;
	bool WriteTextureCoords2 = false;
	bool OverrideOutputFile = false;
	bool ConvertToZUp = false;
};

bool ParseArgs(int _argc, const char** _argv, SArgs& _argsOut);
