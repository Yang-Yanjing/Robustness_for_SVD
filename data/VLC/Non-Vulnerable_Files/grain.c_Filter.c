}
static picture_t *Filter(filter_t *filter, picture_t *src)
{
    filter_sys_t *sys = filter->p_sys;
    picture_t *dst = filter_NewPicture(filter);
    if (!dst) {
        picture_Release(src);
        return NULL;
    }
    vlc_mutex_lock(&sys->cfg.lock);
    const double variance = VLC_CLIP(sys->cfg.variance, VARIANCE_MIN, VARIANCE_MAX);
    vlc_mutex_unlock(&sys->cfg.lock);
    const int scale = 256 * sqrt(variance);
    if (scale != sys->scale) {
        sys->scale = scale;
        Scale(sys->bank_y,  sys->bank, sys->scale);
        Scale(sys->bank_uv, sys->bank, sys->scale / 2);
    }
    for (int i = 0; i < dst->i_planes; i++) {
        const plane_t *srcp = &src->p[i];
        plane_t       *dstp = &dst->p[i];
        if (i == 0 || sys->is_uv_filtered) {
            int16_t *bank = i == 0 ? sys->bank_y :
                                     sys->bank_uv;
            PlaneFilter(filter, dstp, srcp, bank, &sys->seed);
        }
        else {
            plane_CopyPixels(dstp, srcp);
        }
    }
    picture_CopyProperties(dst, src);
    picture_Release(src);
    return dst;
}
