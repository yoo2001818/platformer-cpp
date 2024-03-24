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
