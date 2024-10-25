<h1 align="center">TrianglesGL</h1>

## Description

 Implementation of the rendering triangles with OpenGl.

## How to run

1. Clone <br>
    write <code>git clone https://github.com/baitim/TrianglesGL.git</code> in terminal

2. Go to folder <br>
    write <code>cd TrianglesGL</code> in terminal

3. Prepare conan <br>
    write <code>conan profile detect --force</code> in terminal

4. Init dependencies <br>
    write <code>conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True -s compiler.cppstd=gnu20</code> in terminal
    
5. Init submodules <br>
    write <code>git submodule update --init --recursive</code> in terminal

6. Build <br>
    write <code>cmake . -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake; cmake --build build</code> in terminal

7. Run <br>
    write <code>./build/src/scene</code> in terminal <br>

## How to test

1. Prepare
    - Go to folder <br>
        write <code>cd Triangles3D/tests/end_to_end</code> in terminal

    - Generate tests <br>
        write <code>python3 generate.py</code> in terminal

2. Testing
    - End to end & Unit<br>
        in root dir write <code>ctest --test-dir build/Triangles3D</code> in terminal <br>
        maybe you will need these flags for the ctest <code>--rerun-failed --output-on-failure</code>

## How to use

1. Scroll mouse wheel to change view angle
2.  * <code>W</code> to move forward
    * <code>S</code> to move back
    * <code>D</code> to move right
    * <code>A</code> to move left
3.    <code>Q</code> to activate/deactivate control camera by mouse moving(1 click)
4.    <code>E</code> to change scene

## How to generate scenes

1. - Go to folder <br>
        write <code>cd scenes</code> in terminal

2. - Generate scenes <br>
        write <code>python3 generate.py</code> in terminal

## Features

1. Tested project building in ubuntu:latest
2. Changing the scene during execution

## Example
<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/triangles_example.png" width="80%"></p>

<p align="center"><img src="https://github.com/baitim/TrianglesGL/blob/main/images/cat.gif" width="60%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**