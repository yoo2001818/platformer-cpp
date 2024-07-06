#include "render/render_state.hpp"

void platformer::apply_render_state(render_state &from,
                                    const render_state &to) {
  // Blend state
  if (to.blendEnabled) {
    if (!from.blendEnabled) {
      from.blendEnabled = true;
      glEnable(GL_BLEND);
    }
    if (from.blendColor != to.blendColor) {
      from.blendColor = to.blendColor;
      glBlendColor(from.blendColor[0], from.blendColor[1], from.blendColor[2],
                   from.blendColor[3]);
    }
    if (from.blendEquation != to.blendEquation) {
      from.blendEquation = to.blendEquation;
      glBlendEquationSeparate(from.blendEquation[0], to.blendEquation[1]);
    }
    if (from.blendFunc != to.blendFunc) {
      from.blendFunc = to.blendFunc;
      glBlendFuncSeparate(from.blendFunc[0], from.blendFunc[1],
                          from.blendFunc[2], from.blendFunc[3]);
    }
  } else {
    if (from.blendEnabled) {
      from.blendEnabled = false;
      glDisable(GL_BLEND);
    }
  }

  // Color mask
  if (from.colorMask != to.colorMask) {
    from.colorMask = to.colorMask;
    glColorMask(from.colorMask[0], from.colorMask[1], from.colorMask[2],
                from.colorMask[3]);
  }

  // Depth mask
  if (from.depthMask != to.depthMask) {
    from.depthMask = to.depthMask;
    glDepthMask(from.depthMask);
  }

  // Stencil mask
  if (from.stencilMask != to.stencilMask) {
    from.stencilMask = to.stencilMask;
    glStencilMaskSeparate(GL_FRONT, from.stencilMask[0]);
    glStencilMaskSeparate(GL_BACK, from.stencilMask[1]);
  }

  // Cull face
  if (to.cullEnabled) {
    if (!from.cullEnabled) {
      from.cullEnabled = true;
      glEnable(GL_CULL_FACE);
    }
    if (from.cullFaceMode != to.cullFaceMode) {
      from.cullFaceMode = to.cullFaceMode;
      glCullFace(from.cullFaceMode);
    }
    if (from.frontFace != to.frontFace) {
      from.frontFace = to.frontFace;
      glFrontFace(from.frontFace);
    }
  } else {
    if (from.cullEnabled) {
      from.cullEnabled = false;
      glDisable(GL_CULL_FACE);
    }
  }

  // Depth test
  if (to.depthEnabled) {
    if (!from.depthEnabled) {
      from.depthEnabled = true;
      glEnable(GL_DEPTH_TEST);
    }
    if (from.depthFunc != to.depthFunc) {
      from.depthFunc = to.depthFunc;
      glDepthFunc(from.depthFunc);
    }
    if (from.depthRange != to.depthRange) {
      from.depthRange = to.depthRange;
      glDepthRange(from.depthRange[0], from.depthRange[1]);
    }
  } else {
    if (from.depthEnabled) {
      from.depthEnabled = false;
      glDisable(GL_DEPTH_TEST);
    }
  }

  // Dithering
  if (from.ditherEnabled != to.ditherEnabled) {
    from.ditherEnabled = to.ditherEnabled;
    if (from.ditherEnabled) {
      glEnable(GL_DITHER);
    } else {
      glDisable(GL_DITHER);
    }
  }

  // Stencil test
  if (to.stencilEnabled) {
    if (!from.stencilEnabled) {
      from.stencilEnabled = true;
      glEnable(GL_STENCIL_TEST);
    }
    if (from.stencilFunc != to.stencilFunc ||
        from.stencilFuncRef != to.stencilFuncRef ||
        from.stencilFuncMask != to.stencilFuncMask) {
      from.stencilFunc = to.stencilFunc;
      from.stencilFuncRef = to.stencilFuncRef;
      from.stencilFuncMask = to.stencilFuncMask;
      glStencilFuncSeparate(GL_FRONT, from.stencilFunc[0],
                            from.stencilFuncRef[0], from.stencilFuncMask[0]);
      glStencilFuncSeparate(GL_BACK, from.stencilFunc[1],
                            from.stencilFuncRef[1], from.stencilFuncMask[1]);
    }
    if (from.stencilFuncOp != to.stencilFuncOp) {
      from.stencilFuncOp = to.stencilFuncOp;
      glStencilOpSeparate(GL_FRONT, from.stencilFuncOp[0],
                          from.stencilFuncOp[1], from.stencilFuncOp[2]);
      glStencilOpSeparate(GL_BACK, from.stencilFuncOp[3], from.stencilFuncOp[4],
                          from.stencilFuncOp[5]);
    }
  } else {
    if (from.stencilEnabled) {
      from.stencilEnabled = false;
      glDisable(GL_STENCIL_TEST);
    }
  }

  // Viewport
  if (from.viewport != to.viewport) {
    from.viewport = to.viewport;
    glViewport(from.viewport[0], from.viewport[1], from.viewport[2],
               from.viewport[3]);
  }

  // Scissor test
  if (to.scissorEnabled) {
    if (!from.scissorEnabled) {
      from.scissorEnabled = true;
      glEnable(GL_SCISSOR_TEST);
    }
    if (from.scissor != to.scissor) {
      from.scissor = to.scissor;
      glScissor(from.scissor[0], from.scissor[1], from.scissor[2],
                from.scissor[3]);
    }
  } else {
    if (from.scissorEnabled) {
      from.scissorEnabled = false;
      glDisable(GL_SCISSOR_TEST);
    }
  }

  // Polygon offset
  if (to.polygonOffsetEnabled) {
    if (!from.polygonOffsetEnabled) {
      from.polygonOffsetEnabled = true;
      glEnable(GL_POLYGON_OFFSET_FILL);
    }
    if (from.polygonOffset != to.polygonOffset) {
      from.polygonOffset = to.polygonOffset;
      glPolygonOffset(from.polygonOffset[0], from.polygonOffset[1]);
    }
  } else {
    if (from.polygonOffsetEnabled) {
      from.polygonOffsetEnabled = false;
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
  }
}
