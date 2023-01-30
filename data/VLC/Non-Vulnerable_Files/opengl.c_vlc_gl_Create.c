 */
vlc_gl_t *vlc_gl_Create(struct vout_window_t *wnd, unsigned flags,
                        const char *name)
{
    vlc_object_t *parent = (vlc_object_t *)wnd;
    vlc_gl_t *gl;
    const char *type;
    switch (flags /*& VLC_OPENGL_API_MASK*/)
    {
        case VLC_OPENGL:
            type = "opengl";
            break;
        case VLC_OPENGL_ES:
            type = "opengl es";
            break;
        case VLC_OPENGL_ES2:
            type = "opengl es2";
            break;
        default:
            return NULL;
    }
    gl = vlc_custom_create(parent, sizeof (*gl), "gl");
    if (unlikely(gl == NULL))
        return NULL;
    gl->surface = wnd;
    gl->module = module_need(gl, type, name, true);
    if (gl->module == NULL)
    {
        vlc_object_release(gl);
        return NULL;
    }
    return gl;
}
