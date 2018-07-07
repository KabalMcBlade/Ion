# Ion

Ion is a Vulkan Render Engine, made just to learn something about this new graphic API.
I'm not a render or engine programmer and this work MUST NOT BE USED for any commercial purpose.

> Ion in the Greek Mythology is the illegitimate child of Creüsa, daughter of Erechtheus and wife of Xuthus

 
## Important

As I wrote before, this is made to learn something about this API.
Most of the code has been studied and inspired by:
- Dustin Land (https://www.fasterthan.life/) 
	- GitHub: https://github.com/DustinHLand
	- From Dustin (and id software) I used the buffers, caches and the initialization and setup of Vulkan
- Sascha Willems (https://www.saschawillems.de/)
	- GitHub: https://github.com/SaschaWillems
	- From Sascha some shaders



## Status

**[X64] WORKING**

I'm not planning to do a x86 version


## LICENSE

- Sources:
	- **ION**, the main project, is under GPL-3.0
	- **Eos**, **Nix** and **vkMemoryAllocator** are under Apache-2.0
	- **json.hpp** is under MIT License
	- **stb_image.h** is under MIT License OR Public Domain
	- **stb_image_write.h** is under MIT License OR Public Domain
	- **tiny_gltf.h** is under MIT License

- Assets:
	- **Ash Canyon** is under GPL-2.0, downloaded from here: http://www.custommapmakers.org/skyboxes.php
	- **Damaged Helmet** is under a Creative Commons Attribution-NonCommercial license: https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/DamagedHelmet
	- **frace_cross.hdr** found at http://www.pauldebevec.com/Probes/ with copyright to Images Copyright © 1998, 1999 Paul Debevec (I didn't mail them, but because of the opensource nature, I hope is not an issue! Otherwise I'll remove it)

## NOTE

- **Using NVIDIA Drive before the 397.31 has an issue to upload large texture in memory (2048 x 2048 or more). This want mean that the DEMO attached CAN'T WORK! Please update your driver!**
- **The Vulkan Version used is 1.0.68.0 but you could actually use any after this**


### Almost self inclusive

I tried to reduce external dependencies to the minimum.
All the dependencies are inside DEPENDENCIES folder.
The main dependencies at the moment are the memory allocators for CPU and GPU, made by me (Eos and vkMemoryAllocator)
Other dependencies are single file, like json and stb_image


### About dependencies

About how to use the dependencies please take a look to their own owner. (also for the mine!)


## Example

In the Demo folder you will find a demo project using ION Library


# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2015) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/ion?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/ion) |
