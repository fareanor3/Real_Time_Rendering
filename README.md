# Realtime Rendering

> Welcome to the Real-Time 3D Rendering project, a second-year C programming project that utilizes the concepts of linear algebra and geometric transformations to create stunning 3D visuals. This project is the result of a course on matrices and builds upon the basic concepts of coordinate systems, endomorphism reduction, diagonalization, and projective spaces.

The 3D pipeline has been developed in C, with a focus on Application, Geometry, and Rasterization. The project also includes the implementation of various shaders such as Diffuse light, Specular light, and the Blinn-Phong shading model, and finally, the implementation of a Normal Mapping shader.

All, whithout the use of any external libraries like OpenGL or DirectX.

## Images

- ![Image]()

## Buttons

- N: Displays mode without shaders
- , : Displays mode with Normal Mapping on Blinn-Phong
- B: Displays Bling-Phong mode
- T: Cell Shading
- Space: Auto camera rotation mode
- Left click: manual camera rotation mode
- Right click: light rotation
- up: Increases light intensity
- down: Decrease light intensity

- w: wireframe mode
- echap: exits the program

## Other improvements ( mathematics )

- Animation of object appearance when program starts

## Optimisation (IT)

- Caching of normals, tangents and bitangents to avoid recalculating them each time ( x2 fps )
- Optimisation of a * b + c calculations by float using the fmaf function ( + 4fps without shaders) in the math.h library

## Authors

- [@fareanor3](https://github.com/fareanor3)
- Special thanks to my friends for their assistance in optimizing the calculations ^^
