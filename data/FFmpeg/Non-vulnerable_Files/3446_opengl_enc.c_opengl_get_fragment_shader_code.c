static const char* opengl_get_fragment_shader_code(enum AVPixelFormat format)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(opengl_format_desc); i++) {
        if (opengl_format_desc[i].fixel_format == format)
            return *opengl_format_desc[i].fragment_shader;
    }
    return NULL;
}
