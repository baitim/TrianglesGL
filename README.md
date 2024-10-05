<h1 align="center">TrianglesGl</h1>

## Description

 Implementation of the rendering triangles with OpenGl.

## How to run

1. Clone <br>
    write <code>git clone https://github.com/baitim/TrianglesGl.git</code> in terminal

2. Go to folder <br>
    write <code>cd TrianglesGl</code> in terminal

3. Build <br>
    write <code>cmake . -B build ; cmake --build build</code> in terminal

4. Run <br>
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
3. Use <code>Q</code> to control view by mouse moving

## Example
<p align="center"><img src="https://github.com/baitim/TrianglesGl/blob/main/images/triangles_example.png" width="80%"></p>

<p align="center"><img src="https://github.com/baitim/TrianglesGl/blob/main/images/cat.gif" width="60%"></p>

## Support
**This project is created by [baitim](https://t.me/bai_tim)**