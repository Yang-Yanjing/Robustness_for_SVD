static av_cold void opengl_get_texture_params(OpenGLContext *opengl)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(opengl_format_desc); i++) {
        if (opengl_format_desc[i].fixel_format == opengl->pix_fmt) {
            opengl->format = opengl_format_desc[i].format;
            opengl->type = opengl_format_desc[i].type;
            break;
        }
    }
}
