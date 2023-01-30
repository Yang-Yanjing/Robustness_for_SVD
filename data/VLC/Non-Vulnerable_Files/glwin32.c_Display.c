}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    vout_display_opengl_Display(sys->vgl, &vd->source);
    picture_Release(picture);
    if (subpicture)
        subpicture_Delete(subpicture);
    CommonDisplay(vd);
}
