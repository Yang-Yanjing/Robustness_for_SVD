static av_cold int opengl_compile_shaders(OpenGLContext *opengl, enum AVPixelFormat pix_fmt)
{
    GLint result;
    const char *fragment_shader_code = opengl_get_fragment_shader_code(pix_fmt);
    if (!fragment_shader_code) {
        av_log(opengl, AV_LOG_ERROR, "Provided pixel format '%s' is not supported\n",
               av_get_pix_fmt_name(pix_fmt));
        return AVERROR(EINVAL);
    }
    opengl->vertex_shader = opengl_load_shader(opengl, FF_GL_VERTEX_SHADER,
                                               FF_OPENGL_VERTEX_SHADER);
    if (!opengl->vertex_shader) {
        av_log(opengl, AV_LOG_ERROR, "Vertex shader loading failed.\n");
        goto fail;
    }
    opengl->fragment_shader = opengl_load_shader(opengl, FF_GL_FRAGMENT_SHADER,
                                                 fragment_shader_code);
    if (!opengl->fragment_shader) {
        av_log(opengl, AV_LOG_ERROR, "Fragment shader loading failed.\n");
        goto fail;
    }
    opengl->program = opengl->glprocs.glCreateProgram();
    if (!opengl->program)
        goto fail;
    opengl->glprocs.glAttachShader(opengl->program, opengl->vertex_shader);
    opengl->glprocs.glAttachShader(opengl->program, opengl->fragment_shader);
    opengl->glprocs.glLinkProgram(opengl->program);
    opengl->glprocs.glGetProgramiv(opengl->program, FF_GL_LINK_STATUS, &result);
    if (!result) {
        char *log;
        opengl->glprocs.glGetProgramiv(opengl->program, FF_GL_INFO_LOG_LENGTH, &result);
        if (result) {
            log = av_malloc(result);
            if (!log)
                goto fail;
            opengl->glprocs.glGetProgramInfoLog(opengl->program, result, NULL, log);
            av_log(opengl, AV_LOG_ERROR, "Link error: %s\n", log);
            av_free(log);
        }
        goto fail;
    }
    opengl->position_attrib = opengl->glprocs.glGetAttribLocation(opengl->program, "a_position");
    opengl->texture_coords_attrib = opengl->glprocs.glGetAttribLocation(opengl->program, "a_textureCoords");
    opengl->projection_matrix_location = opengl->glprocs.glGetUniformLocation(opengl->program, "u_projectionMatrix");
    opengl->model_view_matrix_location = opengl->glprocs.glGetUniformLocation(opengl->program, "u_modelViewMatrix");
    opengl->color_map_location = opengl->glprocs.glGetUniformLocation(opengl->program, "u_colorMap");
    opengl->texture_location[0] = opengl->glprocs.glGetUniformLocation(opengl->program, "u_texture0");
    opengl->texture_location[1] = opengl->glprocs.glGetUniformLocation(opengl->program, "u_texture1");
    opengl->texture_location[2] = opengl->glprocs.glGetUniformLocation(opengl->program, "u_texture2");
    opengl->texture_location[3] = opengl->glprocs.glGetUniformLocation(opengl->program, "u_texture3");
    opengl->chroma_div_w_location = opengl->glprocs.glGetUniformLocation(opengl->program, "u_chroma_div_w");
    opengl->chroma_div_h_location = opengl->glprocs.glGetUniformLocation(opengl->program, "u_chroma_div_h");
    OPENGL_ERROR_CHECK(opengl);
    return 0;
  fail:
    opengl->glprocs.glDeleteShader(opengl->vertex_shader);
    opengl->glprocs.glDeleteShader(opengl->fragment_shader);
    opengl->glprocs.glDeleteProgram(opengl->program);
    opengl->fragment_shader = opengl->vertex_shader = opengl->program = 0;
    return AVERROR_EXTERNAL;
}
