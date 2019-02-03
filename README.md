# Scripting Tools

## Screenshots
1. **Level design**

  ![level screenshot](https://i.imgur.com/kO9gQys.png)

2. **In-engine preview**

  ![ingame](https://i.imgur.com/6OOYehn.png)

## Description
Final project for the subject *Tool Scripting* of the La Salle MVD. The project includes the customization of the debug system and gui interface, the proper exportation from 3Ds Max file to json and other improvements.

## Exercises
1. **Map:** (WIP)

2. **Scene export:** (WIP)

3. **MovingPlatform component:** This new component will add a repetitive translation between two points on any object of the scene. Speed, direction and distance will be editable parameters for this specific component.
Instructions for the MovingPlatform addition:
  - Creation of *MovingPlatform.cpp* and *MovingPlatform.h* inside the components folder.
  - Addition of...

4. **Renderer debug menu:** Looking at the Material struct (from *GraphicsSystem.h*) we can extract more information about the material (i.e. ambient, diffuse, specular, specular gloss) and show it with imgui on the render inspector to be editable parameters as well.

  ![Material Inspector](https://i.imgur.com/Pvoz5G6.png)

5. **Console Scripts:** Added 3 more new basic console scripts:

  - **colorbackground** - modifies the color of the glClearColor on the GraphicsSystem. The input parameters are 3 float from 0 to 1 (if not it is clamped to 1).
  - **debug** - enables the Debug Renderer Graphics for the colliders, rays, grid, etc. Turn it on and off with a 0 or a 1.
  - **changecamera** - changes the current main camera and type of movement (free or fps).


6. **Loading json for scene:** (WIP)

7. **Adding delete button:** Because the delete function implies to erase the index reference in the Entities vector (and thus, breaking the link/index reference with all components), we decided to do a workaround. In our case, we've added a boolean in the component struct to detect when an object is active. With that we can easily modify the GraphicsSystem to render or not the specific mesh.

  ![Show/Hide Button](https://i.imgur.com/vIbwOBM.png)

## Authors
- [Ferran Ruiz](https://github.com/Raindrinker)
- [David Gimeno](https://github.com/daliife)
