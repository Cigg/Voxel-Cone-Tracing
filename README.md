# Voxel Cone Tracing
Interactive Indirect Illumination Using Voxel Cone Tracing based on [this article](https://research.nvidia.com/sites/default/files/publications/GIVoxels-pg2011-authors.pdf)

Project in TSBK03 - Advanced Game Programming, at Linköping University.

## Project members
[Oscar Westberg](http://oscarwestberg.github.io)
[Mikael Zackrisson](http://mikaelzackrisson.se)

## Links
[Course webpage](http://www.computer-graphics.se/TSBK03.html)
http://simonstechblog.blogspot.com/2013/01/implementing-voxel-cone-tracing.html
http://cis565-fall-2012.github.io/lectures/11-01-GigaVoxels-And-Sparse-Textures.pdf
http://maverick.inria.fr/Publications/2011/CNSGE11a/GIVoxels_Siggraph_Talk.pdf
http://leifnode.com/2015/05/voxel-cone-traced-global-illumination/
http://on-demand.gputechconf.com/gtc/2012/presentations/SB134-Voxel-Cone-Tracing-Octree-Real-Time-Illumination.pdf
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
  
