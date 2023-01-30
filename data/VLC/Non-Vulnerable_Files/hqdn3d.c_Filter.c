 *****************************************************************************/
static picture_t *Filter(filter_t *filter, picture_t *src)
{
    picture_t *dst;
    filter_sys_t *sys = filter->p_sys;
    struct vf_priv_s *cfg = &sys->cfg;
    bool recalc = false;
    if (!src) return NULL;
    dst = filter_NewPicture(filter);
    if (!dst) {
        picture_Release(src);
        return NULL;
    }
    vlc_mutex_lock( &sys->coefs_mutex );
    recalc = sys->b_recalc_coefs;
    sys->b_recalc_coefs = false;
    if( unlikely( recalc ) )
    {
        msg_Dbg( filter, "Changing coefs to %.2f %.2f %.2f %.2f",
                            sys->luma_spat, sys->luma_temp, sys->chroma_spat, sys->chroma_temp );
        PrecalcCoefs(cfg->Coefs[0], sys->luma_spat);
        PrecalcCoefs(cfg->Coefs[1], sys->luma_temp);
        PrecalcCoefs(cfg->Coefs[2], sys->chroma_spat);
        PrecalcCoefs(cfg->Coefs[3], sys->chroma_temp);
    }
    vlc_mutex_unlock( &sys->coefs_mutex );
    deNoise(src->p[0].p_pixels, dst->p[0].p_pixels,
            cfg->Line, &cfg->Frame[0], sys->w[0], sys->h[0],
            src->p[0].i_pitch, dst->p[0].i_pitch,
            cfg->Coefs[0],
            cfg->Coefs[0],
            cfg->Coefs[1]);
    deNoise(src->p[1].p_pixels, dst->p[1].p_pixels,
            cfg->Line, &cfg->Frame[1], sys->w[1], sys->h[1],
            src->p[1].i_pitch, dst->p[1].i_pitch,
            cfg->Coefs[2],
            cfg->Coefs[2],
            cfg->Coefs[3]);
    deNoise(src->p[2].p_pixels, dst->p[2].p_pixels,
            cfg->Line, &cfg->Frame[2], sys->w[2], sys->h[2],
            src->p[2].i_pitch, dst->p[2].i_pitch,
            cfg->Coefs[2],
            cfg->Coefs[2],
            cfg->Coefs[3]);
    return CopyInfoAndRelease(dst, src);
}
