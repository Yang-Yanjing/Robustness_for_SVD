}
static void Prepare(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    vout_display_opengl_Prepare(sys->vgl, picture, subpicture);
}
