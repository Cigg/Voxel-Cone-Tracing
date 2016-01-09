# Voxel Cone Tracing
Project in TSBK03 - Advanced Game Programming, at Linköping University.

The scene is voxelized and stored in a 3D texture. The voxelized scene representation is "cone traced" in the final rendering step to get indirect light in the scene.

### Project members
[Oscar Westberg](http://oscarwestberg.github.io)  
[Mikael Zackrisson](http://mikaelzackrisson.se)

### Links
* [Course webpage](http://www.computer-graphics.se/TSBK03.html)  
* [Original implementation technical paper](https://research.nvidia.com/sites/default/files/publications/GIVoxels-pg2011-authors.pdf)
* [OpenGLInsights chapter on voxelization](http://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-SparseVoxelization.pdf)

## Requirements
* CMake
* OpenGL 4.1 support

Tested on Linux and Windows with Nvidia GPUs.

## Getting started
Clone the repository using the `--recursive` flag to get all dependecies.

```bash
git clone --recursive https://github.com/Cigg/Voxel-Cone-Tracing.git
cd Voxel-Cone-Tracing
```
  
If you already cloned the repo:

```bash
git clone https://github.com/Cigg/Voxel-Cone-Tracing.git
cd Voxel-Cone-Tracing
git submodule update --init --recursive
```
  
Now generate a project or makefile for your platform

```bash
  mkdir bin
  cd bin
  cmake ..
```

## TODO
* Conservative voxelization
* Atomic operations for image writing to get an averaged voxel value
* Better mipmapping (faster + anisotropic)
* Light injection like in the article
* Dynamic objects
* Maybe sparse voxel octree someday

![](https://github.com/Cigg/Voxel-Cone-Tracing/raw/master/vct.jpg "vct")