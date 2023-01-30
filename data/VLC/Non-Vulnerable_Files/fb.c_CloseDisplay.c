 *****************************************************************************/
static void CloseDisplay(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->video_ptr != MAP_FAILED) {
        ClearScreen(sys);
        munmap(sys->video_ptr, sys->video_size);
    }
    if (sys->fd >= 0) {
        /* Restore palette */
        if (sys->var_info.bits_per_pixel == 8) {
            ioctl(sys->fd, FBIOPUTCMAP, &sys->fb_cmap);
            free(sys->palette);
            sys->palette = NULL;
        }
        /* Restore fb config */
        ioctl(sys->fd, FBIOPUT_VSCREENINFO, &sys->old_info);
        /* Close fb */
        close(sys->fd);
    }
}
