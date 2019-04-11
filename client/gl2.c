
#include <dlfcn.h>
#include <stddef.h>
#include <errno.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

#include <hybris/common/binding.h>

#pragma GCC visibility push(hidden)
HYBRIS_LIBRARY_INITIALIZE(glesv2, "libGLESv2.so");
#if 1
HYBRIS_LIRBARY_CHECK_SYMBOL(glesv2);
#endif
#pragma GCC visibility pop

/*-------------------------------------------------------------------------
 * GL core functions.
 *-----------------------------------------------------------------------*/
HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glActiveTexture, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glAttachShader, GLuint, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glBindAttribLocation, GLuint, GLuint, const GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBindBuffer, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBindFramebuffer, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBindRenderbuffer, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBindTexture, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glBlendColor, GLclampf, GLclampf, GLclampf, GLclampf);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glBlendEquation, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBlendEquationSeparate, GLenum, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glBlendFunc, GLenum, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glBlendFuncSeparate, GLenum, GLenum, GLenum, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glBufferData, GLenum, GLsizeiptr, const GLvoid*, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glBufferSubData, GLenum, GLintptr, GLsizeiptr, const GLvoid*);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLenum, glCheckFramebufferStatus, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glClear, GLbitfield);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glClearColor, GLclampf, GLclampf, GLclampf, GLclampf);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glClearDepthf, GLclampf);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glClearStencil, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glColorMask, GLboolean, GLboolean, GLboolean, GLboolean);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glCompileShader, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION8(glesv2, glCompressedTexImage2D, GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION9(glesv2, glCompressedTexSubImage2D, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION8(glesv2, glCopyTexImage2D, GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION8(glesv2, glCopyTexSubImage2D, GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);

HYBRIS_IMPLEMENT_FUNCTION0(glesv2, GLuint, glCreateProgram);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLuint, glCreateShader, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glCullFace, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteBuffers, GLsizei, const GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteFramebuffers, GLsizei, const GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDeleteProgram, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteRenderbuffers, GLsizei, const GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDeleteShader, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteTextures, GLsizei, const GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDepthFunc, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDepthMask, GLboolean);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDepthRangef, GLclampf, GLclampf);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDetachShader, GLuint, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDisable, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDisableVertexAttribArray, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glDrawArrays, GLenum, GLint, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glDrawElements, GLenum, GLsizei, GLenum, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glEnable, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glEnableVertexAttribArray, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION0(glesv2, glFinish);

HYBRIS_IMPLEMENT_VOID_FUNCTION0(glesv2, glFlush);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glFramebufferRenderbuffer, GLenum, GLenum, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glFrontFace, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenBuffers, GLsizei, GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glGenerateMipmap, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenFramebuffers, GLsizei, GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenRenderbuffers, GLsizei, GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenTextures, GLsizei, GLuint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION7(glesv2, glGetActiveAttrib, GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION7(glesv2, glGetActiveUniform, GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetAttachedShaders, GLuint, GLsizei, GLsizei*, GLuint*);

HYBRIS_IMPLEMENT_FUNCTION2(glesv2, GLint, glGetAttribLocation, GLuint, const GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGetBooleanv, GLenum, GLboolean*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetBufferParameteriv, GLenum, GLenum, GLint*);

HYBRIS_IMPLEMENT_FUNCTION0(glesv2, GLenum, glGetError);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGetFloatv, GLenum, GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetFramebufferAttachmentParameteriv, GLenum, GLenum, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGetIntegerv, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetProgramiv, GLuint, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetRenderbufferParameteriv, GLenum, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetShaderiv, GLuint, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetShaderPrecisionFormat, GLenum, GLenum, GLint*, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetShaderSource, GLuint, GLsizei, GLsizei*, GLchar*);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, const GLubyte*, glGetString, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetTexParameterfv, GLenum, GLenum, GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetTexParameteriv, GLenum, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetUniformfv, GLuint, GLint, GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetUniformiv, GLuint, GLint, GLint*);

HYBRIS_IMPLEMENT_FUNCTION2(glesv2, GLint, glGetUniformLocation, GLuint, const GLchar*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetVertexAttribfv, GLuint, GLenum, GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetVertexAttribiv, GLuint, GLenum, GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetVertexAttribPointerv, GLuint, GLenum, GLvoid**);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glHint, GLenum, GLenum);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsBuffer, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsEnabled, GLenum);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsFramebuffer, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsProgram, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsRenderbuffer, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsShader, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsTexture, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glLineWidth, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glLinkProgram, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glPixelStorei, GLenum, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glPolygonOffset, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION7(glesv2, glReadPixels, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION0(glesv2, glReleaseShaderCompiler);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glRenderbufferStorage, GLenum, GLenum, GLsizei, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glSampleCoverage, GLclampf, GLboolean);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glScissor, GLint, GLint, GLsizei, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glShaderBinary, GLsizei, const GLuint*, GLenum, const GLvoid*, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glShaderSource, GLuint, GLsizei, const GLchar*const*, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glStencilFunc, GLenum, GLint, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glStencilFuncSeparate, GLenum, GLenum, GLint, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glStencilMask, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glStencilMaskSeparate, GLenum, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glStencilOp, GLenum, GLenum, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glStencilOpSeparate, GLenum, GLenum, GLenum, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION9(glesv2, glTexImage2D, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glTexParameterf, GLenum, GLenum, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glTexParameterfv, GLenum, GLenum, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glTexParameteri, GLenum, GLenum, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glTexParameteriv, GLenum, GLenum, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION9(glesv2, glTexSubImage2D, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glUniform1f, GLint, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform1fv, GLint, GLsizei, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glUniform1i, GLint, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform1iv, GLint, GLsizei, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform2f, GLint, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform2fv, GLint, GLsizei, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform2i, GLint, GLint, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform2iv, GLint, GLsizei, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glUniform3f, GLint, GLfloat, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform3fv, GLint, GLsizei, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glUniform3i, GLint, GLint, GLint, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform3iv, GLint, GLsizei, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glUniform4f, GLint, GLfloat, GLfloat, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform4fv, GLint, GLsizei, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glUniform4i, GLint, GLint, GLint, GLint, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glUniform4iv, GLint, GLsizei, const GLint*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glUniformMatrix2fv, GLint, GLsizei, GLboolean, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glUniformMatrix3fv, GLint, GLsizei, GLboolean, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glUniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glUseProgram, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glValidateProgram, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glVertexAttrib1f, GLuint, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glVertexAttrib1fv, GLuint, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glVertexAttrib2f, GLuint, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glVertexAttrib2fv, GLuint, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glVertexAttrib3f, GLuint, GLfloat, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glVertexAttrib3fv, GLuint, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glVertexAttrib4f, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glVertexAttrib4fv, GLuint, const GLfloat*);

HYBRIS_IMPLEMENT_VOID_FUNCTION6(glesv2, glVertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glViewport, GLint, GLint, GLsizei, GLsizei);

/*------------------------------------------------------------------------*
 * OES extension functions
 *------------------------------------------------------------------------*/
#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glEGLImageTargetTexture2DOES, GLenum, GLeglImageOES);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glEGLImageTargetRenderbufferStorageOES, GLenum, GLeglImageOES);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glGetProgramBinaryOES, GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glProgramBinaryOES, GLuint, GLenum, const GLvoid *, GLint);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_FUNCTION2(glesv2, void*, glMapBufferOES, GLenum, GLenum);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glUnmapBufferOES, GLenum);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetBufferPointervOES, GLenum, GLenum, GLvoid**);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION10(glesv2, glTexImage3DOES, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION11(glesv2, glTexSubImage3DOES, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION9(glesv2, glCopyTexSubImage3DOES, GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION9(glesv2, glCompressedTexImage3DOES, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION11(glesv2, glCompressedTexSubImage3DOES, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*);

HYBRIS_IMPLEMENT_VOID_FUNCTION6(glesv2, glFramebufferTexture3DOES, GLenum, GLenum, GLenum, GLuint, GLint, GLint);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glBindVertexArrayOES, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteVertexArraysOES, GLsizei, const GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenVertexArraysOES, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsVertexArrayOES, GLuint);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetPerfMonitorGroupsAMD, GLint *, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glGetPerfMonitorCountersAMD, GLuint, GLint *, GLint *, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetPerfMonitorGroupStringAMD, GLuint, GLsizei, GLsizei *, GLchar *);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glGetPerfMonitorCounterStringAMD, GLuint, GLuint, GLsizei, GLsizei *, GLchar *);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetPerfMonitorCounterInfoAMD, GLuint, GLuint, GLenum, GLvoid *);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenPerfMonitorsAMD, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeletePerfMonitorsAMD, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glSelectPerfMonitorCountersAMD, GLuint, GLboolean, GLuint, GLint, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glBeginPerfMonitorAMD, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glEndPerfMonitorAMD, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glGetPerfMonitorCounterDataAMD, GLuint, GLenum, GLsizei, GLuint *, GLint *);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glDiscardFramebufferEXT, GLenum, GLsizei, const GLenum *);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glMultiDrawArraysEXT, GLenum, const GLint *, const GLsizei *, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glMultiDrawElementsEXT, GLenum, const GLsizei *, GLenum, const GLvoid*const*, GLsizei);

#endif /* GL_GLEXT_PROTOTYPES */

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glRenderbufferStorageMultisampleIMG, GLenum, GLsizei, GLenum, GLsizei, GLsizei);

HYBRIS_IMPLEMENT_VOID_FUNCTION6(glesv2, glFramebufferTexture2DMultisampleIMG, GLenum, GLenum, GLenum, GLuint, GLint, GLsizei);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glDeleteFencesNV, GLsizei, const GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glGenFencesNV, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glIsFenceNV, GLuint);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glTestFenceNV, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetFenceivNV, GLuint, GLenum, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glFinishFenceNV, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glSetFenceNV, GLuint, GLenum);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glCoverageMaskNV, GLboolean);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glCoverageOperationNV, GLenum);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glGetDriverControlsQCOM, GLint *, GLsizei, GLuint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glGetDriverControlStringQCOM, GLuint, GLsizei, GLsizei *, GLchar *);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glEnableDriverControlQCOM, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glDisableDriverControlQCOM, GLuint);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetTexturesQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetBuffersQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetRenderbuffersQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetFramebuffersQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glExtGetTexLevelParameterivQCOM, GLuint, GLenum, GLint, GLenum, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtTexObjectStateOverrideiQCOM, GLenum, GLenum, GLint);

HYBRIS_IMPLEMENT_VOID_FUNCTION11(glesv2, glExtGetTexSubImageQCOM, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLvoid *);

HYBRIS_IMPLEMENT_VOID_FUNCTION2(glesv2, glExtGetBufferPointervQCOM, GLenum, GLvoid **);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetShadersQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_VOID_FUNCTION3(glesv2, glExtGetProgramsQCOM, GLuint *, GLint, GLint *);

HYBRIS_IMPLEMENT_FUNCTION1(glesv2, GLboolean, glExtIsProgramBinaryQCOM, GLuint);

HYBRIS_IMPLEMENT_VOID_FUNCTION4(glesv2, glExtGetProgramBinarySourceQCOM, GLuint, GLenum, GLchar *, GLint *);

#endif

#ifdef GL_GLEXT_PROTOTYPES
HYBRIS_IMPLEMENT_VOID_FUNCTION5(glesv2, glStartTilingQCOM, GLuint, GLuint, GLuint, GLuint, GLbitfield);

HYBRIS_IMPLEMENT_VOID_FUNCTION1(glesv2, glEndTilingQCOM, GLbitfield);

#endif


