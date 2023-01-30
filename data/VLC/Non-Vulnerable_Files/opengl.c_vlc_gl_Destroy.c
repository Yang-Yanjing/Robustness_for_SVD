}
void vlc_gl_Destroy(vlc_gl_t *gl)
{
    module_unneed(gl, gl->module);
    vlc_object_release(gl);
}
