# opengl-cpp-quest-room
OpenGL C++ QuestRoom

Requirements for this template to work
1.  To run this code you should have GCC C++ compiler and the C/C++ extension for VS Code.
    To setup both, follow this https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites

How to run this project

1.  Clone this repo
2.  Open this repo as a folder in Visual Studio Code
3.  In VSCode menu under `Run` option select `run without debugging`

Input sample file
```
800 600
400 400
5
10,10 10,20 20,20 20,10
200,200 200,300 300,250
10,250 10,270 20,250
600,500 650,550 700,500
770,570 750,570 760,580
```

Output based on input file

```
Room: 800x600
Observer: 400,400
Number of elements: 5

Cartesian coordinates:
10,10 10,20 20,20 20,10
200,200 200,300 300,250
10,250 10,270 20,250
600,500 650,550 700,500
770,570 750,570 760,580

Polar coordinates:
0.785398,551.543 0.772412,544.518 0.785398,537.401 0.798384,544.518
0.785398,282.843 0.463648,223.607 0.982794,180.278
0.367174,417.852 0.321751,411.096 0.375961,408.534
3.60524,223.607 3.68201,291.548 3.46334,316.228
3.57229,407.185 3.59375,389.102 3.60524,402.492

Lines calculated based on points:
0.785398,551.543 .. 0.798384,544.518; 0.785398,537.401 .. 0.798384,544.518; 0.772412,544.518 .. 0.785398,537.401; 0.772412,544.518 .. 0.785398,551.543;
0.785398,282.843 .. 0.982794,180.278; 0.463648,223.607 .. 0.982794,180.278; 0.463648,223.607 .. 0.785398,282.843;
0.367174,417.852 .. 0.375961,408.534; 0.321751,411.096 .. 0.375961,408.534; 0.321751,411.096 .. 0.367174,417.852;
3.46334,316.228 .. 3.60524,223.607; 3.46334,316.228 .. 3.68201,291.548; 3.60524,223.607 .. 3.68201,291.548;
3.57229,407.185 .. 3.60524,402.492; 3.59375,389.102 .. 3.60524,402.492; 3.57229,407.185 .. 3.59375,389.102;

Visible phigures: 1 2 3
Total number of visible phigures: 3
Renderer: GeForce GTX 1050/PCIe/SSE2
OpenGL version supported 4.6.0 NVIDIA 452.06
```