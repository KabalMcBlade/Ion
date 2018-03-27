# Ion

Ion is a Vulkan Render Engine, made just to learn something about this new graphic API.
I'm not a render or engine programmer and this work MUST NOT BE USED for any commercial purpose.

> Ion in the Greek Mythology is the illegitimate child of Creüsa, daughter of Erechtheus and wife of Xuthus


## Important

As I wrote before, this is made to learn something about this API.
Most of the code has been studied and inspired by:
. Dustin Land (https://www.fasterthan.life/) and his GitHub: https://github.com/DustinHLand/vkDOOM3
. Sascha Willems and his GitHub: https://github.com/SaschaWillems

For some base initialization (like the whole Vulkan base pipeline initialization I just copied and past, adding or removing what I need or not.


## Status

For now it compile only and run without showing anything!
When I'll finish the interface I'll add some things to render!


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
| Windows (Visual Studio 2015) | [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/kabal2013/ion?branch=master&svg=true)](https://ci.appveyor.com/project/Kabal2013/ion) |


# License

```
 Copyright 2017 Michele Condo'

                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU General Public License is a free, copyleft license for
software and other kinds of works.
```
