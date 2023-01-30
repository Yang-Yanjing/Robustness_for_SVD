}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    /* swap the two Y offsets if the drivers supports panning */
    if (sys->has_pan) {
        sys->var_info.yoffset = 0;
        /*vd->sys->var_info.yoffset = vd->sys->var_info.yres; */
        /* the X offset should be 0, but who knows ...
         * some other app might have played with the framebuffer */
        sys->var_info.xoffset = 0;
        /* FIXME 'static' is damn wrong and it's dead code ... */
        static int panned = 0;
        if (panned < 0) {
            ioctl(sys->fd, FBIOPAN_DISPLAY, &sys->var_info);
            panned++;
        }
    }
    if (!sys->is_hw_accel)
        picture_Copy(sys->picture, picture);
    picture_Release(picture);
    VLC_UNUSED(subpicture);
}
