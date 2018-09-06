# Ion

Ion is a Vulkan Render Engine, made just to learn something about this new graphic API.
I'm not a render or engine programmer and this work MUST NOT BE USED for any commercial purpose.

> Ion in the Greek Mythology is the illegitimate child of Creüsa, daughter of Erechtheus and wife of Xuthus


## Status

**[X64] WORKING**

I'm not planning to do a x86 version

This is a screenshot about a first test on PBR render.

In the top left you can see an "arrow" that indicate (more or less) the orientation of the light, that could be useful when you go in the rotating light directional light mode pressing **L** key.
I did in this way to show you the capabilities to have another camera which render on top of another with another canvas, in fact is not a object rendered at the location but it is another render camera.

Using the DamagedHelmet supplied with this demo (for the license raed the end of this file)
<img src="./Screenshots/Screenshot1.jpg">


## Demo Controls

The demo uses command line (or from VS command line argument under the property of the demo project) in order to render.
The demo in fact is a GLTF viewer at the moment, because I'm using it for render purpose only.


#### Command lines (ION General)

* -width
	* set the width size of the window
* -height
	* set the height size of the window
* -fullscreen
	* set fullscreen (true or false)
* -showcursor
	* show mouse cursor (true or false)

For instance:	-width 1024 -height 768 -fullscreen true


#### Command lines (Demo)

* -model
	* load a model, if -usepath IS NOT DEFINED, the model MUST be only the filename and MUST be inside the Assets/Models demo folder
* -primitive
	* load primitives (quad, triangle, cube, sphere)
* -usepath
	* if -usepath is set, you need to specified the fullpath of the model file and can be everywhere 

For instance:	-model DamagedHelmet.gltf
Or: -usepath -model C:\Users\MyUser\Downloads\DamagedHelmet\DamagedHelmet.gltf

#### Controls

The controls in this demo test are:
* If you move the mouse without press any button, your rotate the camera, otherwise the action is on the object
* Hold **left mouse button** in order to rotate the object
* Hold **right mouse button** in order to pan right or left and up or down the object
* Use the **mouse wheel** to go toward or far away the object
* Press **R** to auto rotate the model
* Hold key **G** increase x 10 the wheel speed of the mouse and **H** to increase x 100
* Press **U** will decrease the scale by 10, press **I** will increase the scale by 10

Pressing the key **L**, you can change the control from the model to the light
In this case, toggling the **L** key you will have:
* Hold **left mouse button** in order to rotate the light

To Debug PBR
* Press **1**, **2** or **3** to change the active value between esposure, gamma or prefiltered cube mip levels
* Press **Q** or **Z** to increment or decrement the selected values


## Important

As I wrote before, this is made to learn something about this API.
I got my inspiration from the following persons and their code
- Dustin Land (https://www.fasterthan.life/) 
	- GitHub: https://github.com/DustinHLand
- Sascha Willems (https://www.saschawillems.de/)
	- GitHub: https://github.com/SaschaWillems


## LICENSE

- Sources:
	- **ION**, the main project, is under GPL-3.0
	- **Eos**, **Nix** and **vkMemoryAllocator** are under Apache-2.0
	- **json.hpp** is under MIT License
	- **stb_image.h** is under MIT License OR Public Domain
	- **stb_image_write.h** is under MIT License OR Public Domain
	- **tiny_gltf.h** is under MIT License

- Assets:
	- **misty_pines_4k.hdr** is under CC0 license (https://creativecommons.org/publicdomain/zero/1.0/) and was downloaded from here https://hdrihaven.com/hdri/?c=morning-afternoon&h=misty_pines
	- **Yokohama3** is under Creative Commons Attribution 3.0 Unported License http://creativecommons.org/licenses/by/3.0/
	- **Damaged Helmet** is under a Creative Commons Attribution-NonCommercial license: https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/DamagedHelmet
	- **Arrow** is under CC-BY-4.0 license (http://creativecommons.org/licenses/by/4.0/) and was downloaded from here https://sketchfab.com/models/56629a8186854a6aa9a94cb8896974d6


### Almost self inclusive

I tried to reduce external dependencies to the minimum.
All the dependencies are inside DEPENDENCIES folder.
The main dependencies at the moment are the memory allocators for CPU and GPU, made by me (Eos and vkMemoryAllocator)
Other dependencies are single file, like json and stb_image


### About dependencies

About how to use the dependencies please take a look to their own owner. (also for the mine!)



# Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Windows (Visual Studio 2015) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabalmcblade/ion?branch=master&svg=true)](https://ci.appveyor.com/project/kabalmcblade/ion) |
