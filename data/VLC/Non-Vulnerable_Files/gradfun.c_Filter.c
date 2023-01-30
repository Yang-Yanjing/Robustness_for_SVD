}
static picture_t *Filter(filter_t *filter, picture_t *src)
{
    filter_sys_t *sys = filter->p_sys;
    picture_t *dst = filter_NewPicture(filter);
    if (!dst) {
        picture_Release(src);
        return NULL;
    }
    vlc_mutex_lock(&sys->lock);
    float strength = VLC_CLIP(sys->strength, STRENGTH_MIN, STRENGTH_MAX);
    int   radius   = VLC_CLIP((sys->radius + 1) & ~1, RADIUS_MIN, RADIUS_MAX);
    vlc_mutex_unlock(&sys->lock);
    const video_format_t *fmt = &filter->fmt_in.video;
    struct vf_priv_s *cfg = &sys->cfg;
    cfg->thresh = (1 << 15) / strength;
    if (cfg->radius != radius) {
        cfg->radius = radius;
        cfg->buf    = vlc_memalign(16,
                                   (((fmt->i_width + 15) & ~15) * (cfg->radius + 1) / 2 + 32) * sizeof(*cfg->buf));
    }
    for (int i = 0; i < dst->i_planes; i++) {
        const plane_t *srcp = &src->p[i];
        plane_t       *dstp = &dst->p[i];
        const vlc_chroma_description_t *chroma = sys->chroma;
        int w = fmt->i_width  * chroma->p[i].w.num / chroma->p[i].w.den;
        int h = fmt->i_height * chroma->p[i].h.num / chroma->p[i].h.den;
        int r = (cfg->radius  * chroma->p[i].w.num / chroma->p[i].w.den +
                 cfg->radius  * chroma->p[i].h.num / chroma->p[i].h.den) / 2;
        r = VLC_CLIP((r + 1) & ~1, RADIUS_MIN, RADIUS_MAX);
        if (__MIN(w, h) > 2 * r && cfg->buf) {
            filter_plane(cfg, dstp->p_pixels, srcp->p_pixels,
                         w, h, dstp->i_pitch, srcp->i_pitch, r);
        } else {
            plane_CopyPixels(dstp, srcp);
        }
    }
    picture_CopyProperties(dst, src);
    picture_Release(src);
    return dst;
}
