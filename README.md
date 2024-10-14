<p align="center">
  <img src="Logo.svg" height="256px" alt="Logo"/>
</p>

> Yet Another Model Converter for GameMaker

[![License](https://img.shields.io/github/license/blueburncz/YAMC)](LICENSE)
[![Discord](https://img.shields.io/discord/298884075585011713?label=Discord)](https://discord.gg/ep2BGPm)

## Table of Contents
* [About](#about)
* [Features](#features)
* [Limitations](#limitations)
* [Usage](#usage)
* [Building from source](#building-from-source)
* [Logo terms of use](#logo-terms-of-use)
* [Links](#links)

## About
Yet Another Model Converter (YAMC, pronounced yam-sea) is a tiny tool that loads
a model from one of [many supported file formats](https://github.com/assimp/assimp/blob/master/doc/Fileformats.md)
and exports it into a binary file that can be simply loaded in GameMaker using
functions [buffer_load](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Buffers/buffer_load.htm)
and [vertex_create_buffer_from_buffer](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/Primitives/vertex_create_buffer_from_buffer.htm).

See file [yamc.gml](utils/yamc.gml) for simple utility functions. The
[utils](utils) folder also includes a shader that you can use as a base for your
custom ones. If you are not familiar with shaders, you can also draw the models
using GameMaker's built-in shaders with [lighting](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/Lighting/Lighting.htm),
[fog](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/GPU_Control/gpu_set_fog.htm)
etc. See [examples](examples) folder for little example projects.

## Features
* Convert static model into a binary file, easily loadable using functions `buffer_load` and `vertex_create_buffer_from_buffer`.
* Customizable vertex format. Supports 3D position, normal, texcoord (up to two layers), color and tangent vector with bitangent sign (float4).
* Invert vertex winding order.
* Convert to -Y-up (default) or +Z-up space.
* Generate flat or smooth normals if the model has none.
* Flip UV coordinates on the Y axis.
* Bake materials' diffuse colors into vertex colors.

## Limitations
* The entire model is collapsed into a single vertex buffer, therefore it cannot have sub-meshes with different textures/materials/shaders, different vertex formats and different primitive types (the entire model needs to be either point list, line list or a triangle list).
* Animations are not supported.

## Usage
Run `yamc -h` to see help message with all arguments and their description.

## Building from source
Following commands build yamc binary into folder [dist](dist).
*Requires [CMake](https://cmake.org/) 3.23 at least and a C++17 compiler!*

```sh
git clone https://github.com/blueburncz/YAMC.git
cd YAMC
cmake -B build -S .
cmake --build ./build/ --config=Release
```

## Logo terms of use
YAMC logo is property of [BlueBurn](https://blueburn.cz) and you're not allowed to do any modifications to it! **Only uniform scaling is allowed, to change the logo size as required.**

## Links
* [Assimp](https://github.com/assimp/assimp) - Used to load models.
* [BBMOD](https://github.com/blueburncz/BBMOD) - More advanced tool with support for animated models and a huge library for advanced 3D rendering in GameMaker.
