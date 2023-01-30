/** Get a VLC picture for a VDPAU output surface */
static picture_t *OutputAllocate(filter_t *filter)
{
    filter_sys_t *sys = filter->p_sys;
    picture_t *pic = filter_NewPicture(filter);
    if (pic == NULL)
        return NULL;
    picture_sys_t *psys = pic->p_sys;
    assert(psys->vdp != NULL);
    if (likely(sys->mixer != VDP_INVALID_HANDLE))
    {   /* Not the first output picture */
        assert(psys->vdp == sys->vdp);
        return pic;
    }
    /* First picture: get the context and allocate the mixer */
    sys->vdp = vdp_hold_x11(psys->vdp, NULL);
    sys->device = psys->device;
    sys->mixer = MixerCreate(filter, filter->pf_video_filter == YCbCrRender);
    if (sys->mixer != VDP_INVALID_HANDLE)
        return pic;
    vdp_release_x11(psys->vdp);
    psys->vdp = NULL;
    picture_Release(pic);
    return NULL;
}
