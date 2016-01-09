# Voxel Cone Tracing
Interactive Indirect Illumination Using Voxel Cone Tracing based on [this article](https://research.nvidia.com/sites/default/files/publications/GIVoxels-pg2011-authors.pdf)

Project in TSBK03 - Advanced Game Programming, at Linköping University.

## Project members
[Oscar Westberg](http://oscarwestberg.github.io)  
[Mikael Zackrisson](http://mikaelzackrisson.se)

## Links
[Course webpage](http://www.computer-graphics.se/TSBK03.html)  
https://research.nvidia.com/sites/default/files/publications/GIVoxels-pg2011-authors.pdf
http://www.seas.upenn.edu/~pcozzi/OpenGLInsights/OpenGLInsights-SparseVoxelization.pdf  

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
* Better mipmapping (faster + anisotropic)
* Light injection like in the article
* Dynamic objects
* Maybe sparse voxel octree someday