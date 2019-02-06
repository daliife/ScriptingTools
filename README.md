# Scripting Tools

## Description
Final project for the subject *Tool Scripting* of the La Salle [MVD](https://www.salleurl.edu/es/estudios/master-en-desarrollo-avanzado-de-videojuegos). The project includes the customization of the debug system and gui interface, the proper exportation from 3Ds Max file to json and other improvements.

## Exercises
1. **Map:** You can find the 3ds project (with the materials folder as well) in the *level_design* folder.

2. **Scene export:** Due to unknown reasons, the colliders do not seem to be working properly when exporting the scene (probably because the objects are not primitives). If needed, the user can navigate the scene with the freecam mode (aka pressing 'O')

3. **MovingPlatform component:** This new component will add a repetitive translation between two points on any object of the scene. Speed, direction and distance will be editable parameters for this specific component.
Instructions for the MovingPlatform addition:
  - Creation of *MovingPlatform.cpp* and *MovingPlatform.h* inside the components folder.
  - Addition of struct in *Components.h*, repeating the same steps as the Rotator.
  - Update and add similar code in *EntityComponentStore.h*.
  - Include the *MovingPlatform.h* in the required files.


4. **Renderer debug menu:** Looking at the material struct (from *GraphicsSystem.h*) we can extract more information about the material (i.e. ambient, diffuse, specular, specular gloss) and show it with *imgui* on the render inspector to be editable parameters as well.

  ![Material Inspector](https://i.imgur.com/Pvoz5G6.png)


5. **Console Scripts:** Added 3 more new basic console scripts:

  - **colorbackground** - modifies the color of the glClearColor on the GraphicsSystem. The input parameters are 3 float from 0 to 1 (if not it is clamped to 1).

  - **debug** - enables the Debug Renderer Graphics for the colliders, rays, grid, etc. Turn it on and off with a 0 or a 1.

  - **changecamera** - changes the current main camera and type of movement (free or fps).


6. **Loading json for scene:** Did not have time nor skills to finish this point.

7. **Adding delete button:** Because the delete functionalityimplies to erase the index reference in the Entities vector (and thus, breaking the link/index reference with all components), we decided to implement a workaround. In our case, we've added a boolean in the *Component* struct to detect when an object is active. With that we can easily modify the GraphicsSystem to render or not the specific mesh.

  ![Show/Hide Button](https://i.imgur.com/vIbwOBM.png)

## Authors
- [Ferran Ruiz](https://github.com/Raindrinker)
- [David Gimeno](https://github.com/daliife)

## Screenshots
  ![Perspective View](https://i.imgur.com/E0NJJFS.jpg)

  ![Top View](https://i.imgur.com/4F5Fgyq.jpg)

  ![ingame](https://i.imgur.com/6OOYehn.png)
