 *****************************************************************************/
static int OpenDisplay(vout_display_t *vd, bool force_resolution)
{
    vout_display_sys_t *sys = vd->sys;
    char *psz_device;                             /* framebuffer device path */
    /* Open framebuffer device */
    if (!(psz_device = var_InheritString(vd, FB_DEV_VAR))) {
        msg_Err(vd, "don't know which fb device to open");
        return VLC_EGENERIC;
    }
    sys->fd = vlc_open(psz_device, O_RDWR);
    if (sys->fd == -1) {
        msg_Err(vd, "cannot open %s (%s)", psz_device, vlc_strerror_c(errno));
        free(psz_device);
        return VLC_EGENERIC;
    }
    free(psz_device);
    /* Get framebuffer device information */
    if (ioctl(sys->fd, FBIOGET_VSCREENINFO, &sys->var_info)) {
        msg_Err(vd, "cannot get fb info (%s)", vlc_strerror_c(errno));
        close(sys->fd);
        return VLC_EGENERIC;
    }
    sys->old_info = sys->var_info;
    /* Get some info on the framebuffer itself */
    if (force_resolution) {
        sys->var_info.xres = sys->var_info.xres_virtual = sys->width;
        sys->var_info.yres = sys->var_info.yres_virtual = sys->height;
    }
    /* Set some attributes */
    sys->var_info.activate = sys->is_tty ? FB_ACTIVATE_NXTOPEN :
                                           FB_ACTIVATE_NOW;
    sys->var_info.xoffset  =  0;
    sys->var_info.yoffset  =  0;
    if (ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->var_info)) {
        msg_Err(vd, "cannot set fb info (%s)", vlc_strerror_c(errno));
        close(sys->fd);
        return VLC_EGENERIC;
    }
    struct fb_fix_screeninfo fix_info;
    /* Get some information again, in the definitive configuration */
    if (ioctl(sys->fd, FBIOGET_FSCREENINFO, &fix_info) ||
        ioctl(sys->fd, FBIOGET_VSCREENINFO, &sys->var_info)) {
        msg_Err(vd, "cannot get additional fb info (%s)",
                vlc_strerror_c(errno));
        /* Restore fb config */
        ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->old_info);
        close(sys->fd);
        return VLC_EGENERIC;
    }
    /* If the fb has limitations on mode change,
     * then keep the resolution of the fb */
    if ((sys->height != sys->var_info.yres) ||
        (sys->width != sys->var_info.xres)) {
        msg_Warn(vd,
                 "using framebuffer native resolution instead of requested (%ix%i)",
                 sys->width, sys->height);
    }
    sys->height = sys->var_info.yres;
    sys->width  = sys->var_info.xres_virtual ? sys->var_info.xres_virtual :
                                               sys->var_info.xres;
    sys->line_length = fix_info.line_length;
    /* FIXME: if the image is full-size, it gets cropped on the left
     * because of the xres / xres_virtual slight difference */
    msg_Dbg(vd, "%ix%i (virtual %ix%i) (request %ix%i)",
            sys->var_info.xres, sys->var_info.yres,
            sys->var_info.xres_virtual,
            sys->var_info.yres_virtual,
            sys->width, sys->height);
    sys->palette = NULL;
    sys->has_pan = (fix_info.ypanstep || fix_info.ywrapstep);
    switch (sys->var_info.bits_per_pixel) {
    case 8:
        sys->palette = malloc(4 * 256 * sizeof(uint16_t));
        if (!sys->palette) {
            /* Restore fb config */
            ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->old_info);
            close(sys->fd);
            return VLC_ENOMEM;
        }
        sys->fb_cmap.start = 0;
        sys->fb_cmap.len = 256;
        sys->fb_cmap.red = sys->palette;
        sys->fb_cmap.green = sys->palette + 256;
        sys->fb_cmap.blue = sys->palette + 2 * 256;
        sys->fb_cmap.transp = sys->palette + 3 * 256;
        /* Save the colormap */
        ioctl(sys->fd, FBIOGETCMAP, &sys->fb_cmap);
        sys->bytes_per_pixel = 1;
        break;
    case 15:
    case 16:
        sys->bytes_per_pixel = 2;
        break;
    case 24:
        sys->bytes_per_pixel = 3;
        break;
    case 32:
        sys->bytes_per_pixel = 4;
        break;
    default:
        msg_Err(vd, "screen depth %d is not supported",
                sys->var_info.bits_per_pixel);
        /* Restore fb config */
        ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->old_info);
        close(sys->fd);
        return VLC_EGENERIC;
    }
    sys->video_size = sys->line_length * sys->var_info.yres_virtual;
    /* Map a framebuffer at the beginning */
    sys->video_ptr = mmap(NULL, sys->video_size,
                          PROT_READ | PROT_WRITE, MAP_SHARED, sys->fd, 0);
    if (sys->video_ptr == MAP_FAILED) {
        msg_Err(vd, "cannot map video memory (%s)", vlc_strerror_c(errno));
        if (sys->var_info.bits_per_pixel == 8) {
            free(sys->palette);
            sys->palette = NULL;
        }
        /* Restore fb config */
        ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->old_info);
        close(sys->fd);
        return VLC_EGENERIC;
    }
    ClearScreen(sys);
    msg_Dbg(vd,
            "framebuffer type=%d, visual=%d, ypanstep=%d, ywrap=%d, accel=%d",
            fix_info.type, fix_info.visual,
            fix_info.ypanstep, fix_info.ywrapstep, fix_info.accel);
    return VLC_SUCCESS;
}
