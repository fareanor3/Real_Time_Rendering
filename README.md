# Realtime Rendering

## buttons

* N: Displays mode without shaders
* , : Displays mode with Normal Mapping on Blinn-Phong
* B: Displays Bling-Phong mode
* T: Cell Shading
* Space: Auto camera rotation mode
* Left click: manual camera rotation mode
* Right click: light rotation
* up: Increases light intensity
* down: Decrease light intensity

* w: wireframe mode
* echap: exits the program

## Other improvements ( mathematics )

* Animation of object appearance when program starts

## Optimisation (IT)

* Caching of normals, tangents and bitangents to avoid recalculating them each time ( x2 fps )
* Optimisation of a * b + c calculations by float using the fmaf function ( + 4fps without shaders) in the math.h library
