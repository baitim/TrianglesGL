<h1 align="center">TrianglesGL</h1>

## Description

 Implementation of the rendering triangles with OpenGl.

## How to integrate
 
 use [storage](https://github.com/baitim/ConanPackages), project = "trianglesgl", version = "1.0", user = "baitim"

## Requirements

* Cmake >= 3.30
* Conan >= 2.12

## How to run

1. Clone <br>
    <code>git clone https://github.com/baitim/TrianglesGL.git</code>

2. Go to folder <br>
    <code>cd TrianglesGL</code>

3. Prepare conan <br>
    conan installation: <code>uv sync --group dev; source .venv/bin/activate</code><br>
    <code>conan profile detect --force</code>

4. Add conan required packages from [storage](https://github.com/baitim/ConanPackages)<br>
    <code>conan remote add conan_packages http://188.225.84.75:9300</code>

5. Init dependencies <br>
    <code>conan install . --build=missing -s compiler.cppstd=gnu20</code><br>
    maybe you will need these flags for the conan <code>-s build_type=Debug</code>

6. Build <br>
    <code>cmake --preset release; cmake --build build/Release</code>

7. Run <br>
    <code>./build/Release/src/scene [scene.in]?</code> <br>

## How to use

1. Scroll mouse wheel to change view angle
2.  * <code>W</code> to move forward
    * <code>S</code> to move back
    * <code>D</code> to move right
    * <code>A</code> to move left
3.    <code>Q</code> to activate/deactivate control camera by mouse moving(1 click)
4.    <code>E</code> to change scene
4.    <code>Escape</code> to exit

## How to generate scenes

1. - Go to folder <br>
        <code>cd scenes</code>

2. - Generate scenes <br>
        <code>python3 generate.py</code>

## Features

1. Tested project building in ubuntu-24.04
2. Changing the scene during execution
2. Smart shadows

## Example
<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/triangles_example.png" width="80%"></p>

<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/cat.gif" width="60%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**