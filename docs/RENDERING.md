# Rendering

Obviously, the renderer is a significant part of a game engine. While I do not
aim to implement a full-blown renderer right now, it would be a part of the plan
in the future. So, let's just map the features we might need and figure out the
pipeline.

- Armatures
- Instancing
- Textures (duh)
- Multiple materials
- Transparent materials
- Deferred rendering, and multiple lights
- Shadows
- PBR
- Culling

The renderer could support multiple rendering modes, from forward rendering to
deferred rendering, but I think it's not necessary at all to implement forward
rendering at this point - VRAM is so cheap, and even mobile devices nowadays can
comfortably use deferred rendering, along with multiple render targets.

However, it's also true that in order to support transparent materials, I need
to implement forward rendering specifically for them, so ditching forward
rendering is not possible anyway.

## Pipeline

The renderer pipeline should be responsible for making draw calls, preparements
and shaders. It should assemble various requests from materials, lights, etc and
orchestrate everything.

In a nutshell, it should do the following:

- Set up common VBOs (camera positions, etc)
- Set up framebuffers to render onto (for deferred rendering)
- Set up material shaders as needed
- Set up instancing if possible to do so
- Prepare skeletal animation data and upload as UBO
- Render meshes
- Set up lights
- Prepare shadow maps if needed
- Draw shadow maps, repeating rendering meshes
- Render lights onto final render target
- Render transparent objects onto final render target
- Apply post-process filters
- Present the final texture on the screen

This is quite a handful amount of code. The mesh rendering procedure should
account for various types of rendering modes - skeletal animations, forward,
deferred, instancing, shadow maps, LoDs, etc.
