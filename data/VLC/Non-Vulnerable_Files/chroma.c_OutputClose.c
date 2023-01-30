}
static void OutputClose(vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    filter_sys_t *sys = filter->p_sys;
    Flush(filter);
    if (sys->mixer != VDP_INVALID_HANDLE)
    {
        vdp_video_mixer_destroy(sys->vdp, sys->mixer);
        vdp_release_x11(sys->vdp);
    }
    free(sys);
}
