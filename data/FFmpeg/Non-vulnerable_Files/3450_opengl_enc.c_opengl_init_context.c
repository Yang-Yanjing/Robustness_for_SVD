static av_cold int opengl_init_context(OpenGLContext *opengl)
{
    int i, ret;
    const AVPixFmtDescriptor *desc;
    if ((ret = opengl_compile_shaders(opengl, opengl->pix_fmt)) < 0)
        goto fail;
    desc = av_pix_fmt_desc_get(opengl->pix_fmt);
    av_assert0(desc->nb_components > 0 && desc->nb_components <= 4);
    glGenTextures(desc->nb_components, opengl->texture_name);
    opengl->glprocs.glGenBuffers(2, &opengl->index_buffer);
    if (!opengl->index_buffer || !opengl->vertex_buffer) {
        av_log(opengl, AV_LOG_ERROR, "Buffer generation failed.\n");
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    opengl_configure_texture(opengl, opengl->texture_name[0], opengl->width, opengl->height);
    if (desc->nb_components > 1) {
        int has_alpha = desc->flags & AV_PIX_FMT_FLAG_ALPHA;
        int num_planes = desc->nb_components - (has_alpha ? 1 : 0);
        if (opengl->non_pow_2_textures) {
            opengl->chroma_div_w = 1.0f;
            opengl->chroma_div_h = 1.0f;
        } else {
            opengl->chroma_div_w = 1 << desc->log2_chroma_w;
            opengl->chroma_div_h = 1 << desc->log2_chroma_h;
        }
        for (i = 1; i < num_planes; i++)
            if (opengl->non_pow_2_textures)
                opengl_configure_texture(opengl, opengl->texture_name[i],
                        FF_CEIL_RSHIFT(opengl->width, desc->log2_chroma_w),
                        FF_CEIL_RSHIFT(opengl->height, desc->log2_chroma_h));
            else
                opengl_configure_texture(opengl, opengl->texture_name[i], opengl->width, opengl->height);
        if (has_alpha)
            opengl_configure_texture(opengl, opengl->texture_name[3], opengl->width, opengl->height);
    }
    opengl->glprocs.glBindBuffer(FF_GL_ELEMENT_ARRAY_BUFFER, opengl->index_buffer);
    opengl->glprocs.glBufferData(FF_GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index), g_index, FF_GL_STATIC_DRAW);
    opengl->glprocs.glBindBuffer(FF_GL_ELEMENT_ARRAY_BUFFER, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor((float)opengl->background[0] / 255.0f, (float)opengl->background[1] / 255.0f,
                 (float)opengl->background[2] / 255.0f, 1.0f);
    ret = AVERROR_EXTERNAL;
    OPENGL_ERROR_CHECK(opengl);
    return 0;
  fail:
    return ret;
}
