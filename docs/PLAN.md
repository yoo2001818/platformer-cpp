# Plan

The general objective (of the project) is to create a minimal working example
of something that can be classified as a game, preferably a 3D platformer.

Instead of putting efforts to make something "perfect" from the first place,
which usually results in the project screeching to halt, starting from a minimal
viable product and gradually improving it would be a better choice overall, from
the motivation, an opportunity to learn about C++, etc.

Given this direction, it'd be desirable to make the projects in few phases.
Though, it's not even meaningful to smooth out all the details and phases from
the beginning.

So, let's just start with a minimal product that I should focus right now.

## The First Phase

The primary objective of the first phase is, creating a 3D platformer, whatever
it might be. So, let's just be content with cubes, AABB physics, and minimal
shading (or even no shading is an option). The playability is what matters at
this point.

Here's a rough direction of what should be done (or rather, what should not be done):

- Cube for everything (no OBJ loaders, GLTF loaders)
- A hardcoded, or a very simple file-based map system
- A playable character, along with keyboard/mouse control
- A very crude forward rendering pipeline
- AABB collision detection only
- Very minimal GUI to show current stage, pause menu, etc.

Further details should be postponed after finishing this details.

So, in order to achieve the first goal, what should be done?

### General structure

Well, the main decisions have already been straighten out. I'll use SDL, OpenGL,
and entt ECS framework for basic foundation.

So, it's basically a matter to write down the components and systems I need to
achieve the goal:

- components
  - transform (already done)
  - mesh
  - physics
  - player
  - obstacle
  - goal
- systems
  - move player
  - update physics
  - check if player has reached goal
  - render mesh
- additional structures
  - renderer
    - resource handles for geometry, shader
  - gui
    - a very crude logic that directly taps to the renderer

It's way too simple when it's listed like that, because it is!
