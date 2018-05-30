# Ion

Ion is a Vulkan Render Engine, made just to learn something about this new graphic API.
I'm not a render or engine programmer and this work MUST NOT BE USED for any commercial purpose.

> Ion in the Greek Mythology is the illegitimate child of Creüsa, daughter of Erechtheus and wife of Xuthus


## Important

As I wrote before, this is made to learn something about this API.
Most of the code has been studied and inspired by:
- Dustin Land (https://www.fasterthan.life/) 
	- GitHub: https://github.com/DustinHLand

For many part of the initialization and setup of the Vulkan code I just merely copied and pasted, adding or removing what I had needed or not.



## Status

For now it compile only and run without showing anything!
When I'll finish the interface I'll add some things to render!

- **Using NVIDIA Drive before the 397.31 has an issue to upload large texture in memory (2048 x 2048 or more). This want mean that the DEMO attached CAN'T WORK! Please update your driver!**
- **The Vulkan Version used is 1.0.68.0**


### Almost self inclusive

I tried to reduce external dependencies to the minimum.
All the dependencies are inside DEPENDENCIES folder.
The main dependencies at the moment are the memory allocators for CPU and GPU, made by me (Eos and vkMemoryAllocator)
Other dependencies are single file, like json and stb_image


### About dependencies

About how to use the dependencies please take a look to their own owner. (also for the mine!)



## Example

In the Demo folder you will find a demo project using ION Library
Remember to change the path of the local model inside the Demo.cpp: E:/Projects/Ion/Demo/Assets/DamagedHelmet.gltf !!


# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2015) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/ion?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/ion) |
