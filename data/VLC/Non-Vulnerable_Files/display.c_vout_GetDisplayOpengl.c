}
struct vlc_gl_t *vout_GetDisplayOpengl(vout_display_t *vd)
{
    struct vlc_gl_t *gl;
    if (vout_display_Control(vd, VOUT_DISPLAY_GET_OPENGL, &gl))
        return NULL;
    return gl;
}
