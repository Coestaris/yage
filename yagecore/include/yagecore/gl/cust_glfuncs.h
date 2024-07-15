#include "GL/gl.h"

/* This type is intentionally in a global namespace */
using PFNGLDRAWELEMENTSPROC
      = void (*)(GLenum mode, GLsizei count, GLenum type, const void * indices);
