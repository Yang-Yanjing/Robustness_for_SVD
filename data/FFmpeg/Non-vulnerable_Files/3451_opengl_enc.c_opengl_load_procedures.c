static int av_cold opengl_load_procedures(OpenGLContext *opengl)
{
    FFOpenGLFunctions *procs = &opengl->glprocs;
#if HAVE_SDL
    if (!opengl->no_window)
        return opengl_sdl_load_procedures(opengl);
#endif
    procs->glActiveTexture = glActiveTexture;
    procs->glGenBuffers = glGenBuffers;
    procs->glDeleteBuffers = glDeleteBuffers;
    procs->glBufferData = glBufferData;
    procs->glBindBuffer = glBindBuffer;
    procs->glGetAttribLocation = glGetAttribLocation;
    procs->glGetUniformLocation = glGetUniformLocation;
    procs->glUniform1f = glUniform1f;
    procs->glUniform1i = glUniform1i;
    procs->glUniformMatrix4fv = glUniformMatrix4fv;
    procs->glCreateProgram = glCreateProgram;
    procs->glDeleteProgram = glDeleteProgram;
    procs->glUseProgram = glUseProgram;
    procs->glLinkProgram = glLinkProgram;
    procs->glGetProgramiv = glGetProgramiv;
    procs->glGetProgramInfoLog = glGetProgramInfoLog;
    procs->glAttachShader = glAttachShader;
    procs->glCreateShader = glCreateShader;
    procs->glDeleteShader = glDeleteShader;
    procs->glCompileShader = glCompileShader;
    procs->glShaderSource = glShaderSource;
    procs->glGetShaderiv = glGetShaderiv;
    procs->glGetShaderInfoLog = glGetShaderInfoLog;
    procs->glEnableVertexAttribArray = glEnableVertexAttribArray;
    procs->glVertexAttribPointer = (FF_PFNGLVERTEXATTRIBPOINTERPROC) glVertexAttribPointer;
    return 0;
}
#else
static int av_cold opengl_load_procedures(OpenGLContext *opengl)
{
    FFOpenGLFunctions *procs = &opengl->glprocs;
#if HAVE_GLXGETPROCADDRESS
#define SelectedGetProcAddress glXGetProcAddress
#elif HAVE_WGLGETPROCADDRESS
#define SelectedGetProcAddress wglGetProcAddress
#endif
#define LOAD_OPENGL_FUN(name, type) \
    procs->name = (type)SelectedGetProcAddress(#name); \
    if (!procs->name) { \
        av_log(opengl, AV_LOG_ERROR, "Cannot load OpenGL function: '%s'\n", #name); \
        return AVERROR(ENOSYS); \
    }
#if HAVE_SDL
    if (!opengl->no_window)
        return opengl_sdl_load_procedures(opengl);
#endif
    LOAD_OPENGL_FUN(glActiveTexture, FF_PFNGLACTIVETEXTUREPROC)
    LOAD_OPENGL_FUN(glGenBuffers, FF_PFNGLGENBUFFERSPROC)
    LOAD_OPENGL_FUN(glDeleteBuffers, FF_PFNGLDELETEBUFFERSPROC)
    LOAD_OPENGL_FUN(glBufferData, FF_PFNGLBUFFERDATAPROC)
    LOAD_OPENGL_FUN(glBindBuffer, FF_PFNGLBINDBUFFERPROC)
    LOAD_OPENGL_FUN(glGetAttribLocation, FF_PFNGLGETATTRIBLOCATIONPROC)
    LOAD_OPENGL_FUN(glGetUniformLocation, FF_PFNGLGETUNIFORMLOCATIONPROC)
    LOAD_OPENGL_FUN(glUniform1f, FF_PFNGLUNIFORM1FPROC)
    LOAD_OPENGL_FUN(glUniform1i, FF_PFNGLUNIFORM1IPROC)
    LOAD_OPENGL_FUN(glUniformMatrix4fv, FF_PFNGLUNIFORMMATRIX4FVPROC)
    LOAD_OPENGL_FUN(glCreateProgram, FF_PFNGLCREATEPROGRAMPROC)
    LOAD_OPENGL_FUN(glDeleteProgram, FF_PFNGLDELETEPROGRAMPROC)
    LOAD_OPENGL_FUN(glUseProgram, FF_PFNGLUSEPROGRAMPROC)
    LOAD_OPENGL_FUN(glLinkProgram, FF_PFNGLLINKPROGRAMPROC)
    LOAD_OPENGL_FUN(glGetProgramiv, FF_PFNGLGETPROGRAMIVPROC)
    LOAD_OPENGL_FUN(glGetProgramInfoLog, FF_PFNGLGETPROGRAMINFOLOGPROC)
    LOAD_OPENGL_FUN(glAttachShader, FF_PFNGLATTACHSHADERPROC)
    LOAD_OPENGL_FUN(glCreateShader, FF_PFNGLCREATESHADERPROC)
    LOAD_OPENGL_FUN(glDeleteShader, FF_PFNGLDELETESHADERPROC)
    LOAD_OPENGL_FUN(glCompileShader, FF_PFNGLCOMPILESHADERPROC)
    LOAD_OPENGL_FUN(glShaderSource, FF_PFNGLSHADERSOURCEPROC)
    LOAD_OPENGL_FUN(glGetShaderiv, FF_PFNGLGETSHADERIVPROC)
    LOAD_OPENGL_FUN(glGetShaderInfoLog, FF_PFNGLGETSHADERINFOLOGPROC)
    LOAD_OPENGL_FUN(glEnableVertexAttribArray, FF_PFNGLENABLEVERTEXATTRIBARRAYPROC)
    LOAD_OPENGL_FUN(glVertexAttribPointer, FF_PFNGLVERTEXATTRIBPOINTERPROC)
    return 0;
#undef SelectedGetProcAddress
#undef LOAD_OPENGL_FUN
}
