<h1 align="center">TrianglesGL</h1>

## Description

 Implementation of the rendering triangles with OpenGl.

## How to run

1. Clone <br>
    <code>git clone https://github.com/baitim/TrianglesGL.git</code>

2. Go to folder <br>
    <code>cd TrianglesGL</code>

3. Prepare conan <br>
    <code>conan profile detect --force</code>

4. Init dependencies <br>
    <code>conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True -s compiler.cppstd=gnu20</code> <br>
    maybe you will need these flags for the conan <code>-s build_type=Debug</code>
    
5. Init submodules <br>
    <code>git submodule update --init --recursive</code>

6. Build <br>
    <code>cmake --preset release</code><br>
    <code>cmake --build build</code>

7. Run <br>
    <code>./build/src/scene [scene.in]?</code> <br>

## How to test

1. Prepare
    - Go to folder <br>
        <code>cd Triangles3D/tests/end_to_end</code>

    - Generate tests <br>
        <code>python3 generate.py</code>

2. Testing
    - End to end & Unit<br>
        in root dir <code>ctest --test-dir build/Triangles3D</code> <br>
        maybe you will need these flags for the ctest <code>--rerun-failed --output-on-failure</code>

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

1. Tested project building in ubuntu:latest
2. Changing the scene during execution
2. Smart shadows

## Example
<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/triangles_example.png" width="80%"></p>

<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/cat.gif" width="60%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**