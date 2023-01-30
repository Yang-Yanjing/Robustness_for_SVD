}
static void blurayDrawArgbOverlay(demux_t *p_demux, const BD_ARGB_OVERLAY* const ov)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    vlc_mutex_lock(&p_sys->p_overlays[ov->plane]->lock);
    /* Find a region to update */
    subpicture_region_t *p_reg = p_sys->p_overlays[ov->plane]->p_regions;
    if (!p_reg) {
        vlc_mutex_unlock(&p_sys->p_overlays[ov->plane]->lock);
        return;
    }
    /* Now we can update the region */
    const uint32_t *src0 = ov->argb;
    uint8_t        *dst0 = p_reg->p_picture->p[0].p_pixels +
                           p_reg->p_picture->p[0].i_pitch * ov->y +
                           ov->x * 4;
    for (int y = 0; y < ov->h; y++) {
        // XXX: add support for this format ? Should be possible with OPENGL/VDPAU/...
        // - or add libbluray option to select the format ?
        for (int x = 0; x < ov->w; x++) {
            dst0[x*4  ] = src0[x]>>16; /* R */
            dst0[x*4+1] = src0[x]>>8;  /* G */
            dst0[x*4+2] = src0[x];     /* B */
            dst0[x*4+3] = src0[x]>>24; /* A */
        }
        src0 += ov->stride;
        dst0 += p_reg->p_picture->p[0].i_pitch;
    }
    vlc_mutex_unlock(&p_sys->p_overlays[ov->plane]->lock);
    /*
     * /!\ The region is now stored in our internal list, but not in the subpicture /!\
     */
}
