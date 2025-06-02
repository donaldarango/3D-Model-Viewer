## Disclaimer
Due to some technical issues during the semester, Gavin's laptop and IDE was not properly linking to the project's set up however he stilled contributed as the project was mostly done via pair programming either over screen-shared calls or in person meet ups. While the GitHub may not be reflected of these contributions, they were recognized in the class and within the team itself. 

## Description
3D Model Viewer project created for Graphics Programming class written in C++.
The program can load in .obj files from the data directory and you can apply transformations to them using the keyboard.
There are a few shaders to choose from, which each have different effects:
- Gouraud Shading
- Phong Shading
- Depth Testing with Z value
- Depth Testing with Z' value

## Running Locally
Created by Gavin Baltar and Donald Arango

- Run the makefile in the root directory
    - This was run on a mac, so if there are any issues let us know, because we can attempt to help for running on windows.
- The program then asks for input
    - Enter a file name (without a .obj extension)
        - Ex: cube, castle, galleon, monke, couch, etc.
    - Enter if you want the model transformation calculated on the CPU or GPU (y for GPU, n for CPU)
- Model Transformation Input:
    - Translation (x,y) : UP, DOWN, LEFT, RIGHT arrow keys (moving in the direction of the arrow key)
    - Translation (z) : COMMA and PERIOD keys  (move the model backwards and forwards)
    - Rotation: W, A, S, D, Q, E (rotating around the x, y, and z axes, respectively)
    - Scale: 1,2  (scaling down and up)
- Shader Program Hot Swapping:
Press the following keys to select a different shader to display:
    - [8]: Gouraud Shading (starting shader)
    - [9]: Phong Shading
    - [0]: Depth Testing with Z value
    - [-]: Depth Testing with Z' value
- Enjoy!
