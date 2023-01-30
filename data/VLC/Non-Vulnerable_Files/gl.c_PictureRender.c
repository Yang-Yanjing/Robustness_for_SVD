}
static void PictureRender (vout_display_t *vd, picture_t *pic, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    vlc_gl_MakeCurrent (sys->gl);
    vout_display_opengl_Prepare (sys->vgl, pic, subpicture);
    vlc_gl_ReleaseCurrent (sys->gl);
}
