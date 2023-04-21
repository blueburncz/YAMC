# YAMC
> Yet Another Model Converter for GameMaker

[![License](https://img.shields.io/github/license/blueburncz/YAMC)](LICENSE)
[![Discord](https://img.shields.io/discord/298884075585011713?label=Discord)](https://discord.gg/ep2BGPm)

# About
Yet Another Model Converter (YAMC) is a tiny tool that loads a model from one of
[many supported file formats](https://github.com/assimp/assimp/blob/master/doc/Fileformats.md)
and exports it into a binary file that can be simply loaded in GameMaker using
functions [buffer_load](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Buffers/buffer_load.htm)
and [vertex_create_buffer_from_buffer](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/Primitives/vertex_create_buffer_from_buffer.htm).

See file [yamc.gml](utils/yamc.gml) for simple utility functions. The
[utils](utils) folder also includes a shader that you can use as a base for your
custom ones. If you are not familiar with shaders, you can also draw the models
using GameMaker's built-in shaders with [lighting](manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/Lighting/Lighting.htm),
[fog](https://manual.yoyogames.com/GameMaker_Language/GML_Reference/Drawing/GPU_Control/gpu_set_fog.htm)
etc. See [examples](examples) for little example projects.

# Usage
Run `yamc -h` to see help message with all arguments and their description.

# Building from source
Requires [CMake](https://cmake.org/) 3.23 at least!

```sh
git clone https://github.com/blueburncz/YAMC.git
cd YAMC
mkdir build
cd build
cmake ..
cmake --build . --config=Release
```

# Links
* [Assimp](https://github.com/assimp/assimp) - Used to load models.
* [BBMOD](https://github.com/blueburncz/BBMOD) - More advanced tool with support \
  for animated models and a huge library for advanced 3D rendering in GameMaker.
