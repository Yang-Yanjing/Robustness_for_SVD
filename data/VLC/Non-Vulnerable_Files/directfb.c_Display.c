}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    IDirectFBSurface *primary = sys->primary;
    primary->Flip(primary, NULL, 0);
    if (++sys->idx >= 3)
        sys->idx = 0;
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
