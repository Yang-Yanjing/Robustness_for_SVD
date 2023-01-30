 *****************************************************************************/
static int Open(vlc_object_t *this)
{
    filter_t *filter = (filter_t *)this;
    filter_sys_t *sys;
    struct vf_priv_s *cfg;
    const video_format_t *fmt_in  = &filter->fmt_in.video;
    const video_format_t *fmt_out = &filter->fmt_out.video;
    const vlc_fourcc_t fourcc_in  = fmt_in->i_chroma;
    const vlc_fourcc_t fourcc_out = fmt_out->i_chroma;
    int wmax = 0;
    const vlc_chroma_description_t *chroma =
            vlc_fourcc_GetChromaDescription(fourcc_in);
    if (!chroma || chroma->plane_count != 3 || chroma->pixel_size != 1) {
        msg_Err(filter, "Unsupported chroma (%4.4s)", (char*)&fourcc_in);
        return VLC_EGENERIC;
    }
    if (fourcc_in != fourcc_out) {
        msg_Err(filter, "Input and output chromas don't match");
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    sys = calloc(1, sizeof(filter_sys_t));
    if (!sys) {
        return VLC_ENOMEM;
    }
    cfg = &sys->cfg;
    sys->chroma = chroma;
    for (int i = 0; i < 3; ++i) {
        sys->w[i] = fmt_in->i_width  * chroma->p[i].w.num / chroma->p[i].w.den;
        if (sys->w[i] > wmax) wmax = sys->w[i];
        sys->h[i] = fmt_out->i_height * chroma->p[i].h.num / chroma->p[i].h.den;
    }
    cfg->Line = malloc(wmax*sizeof(unsigned int));
    if (!cfg->Line) {
        free(sys);
        return VLC_ENOMEM;
    }
    config_ChainParse(filter, FILTER_PREFIX, filter_options,
                      filter->p_cfg);
    vlc_mutex_init( &sys->coefs_mutex );
    sys->b_recalc_coefs = true;
    sys->luma_spat = var_CreateGetFloatCommand(filter, FILTER_PREFIX "luma-spat");
    sys->chroma_spat = var_CreateGetFloatCommand(filter, FILTER_PREFIX "chroma-spat");
    sys->luma_temp = var_CreateGetFloatCommand(filter, FILTER_PREFIX "luma-temp");
    sys->chroma_temp = var_CreateGetFloatCommand(filter, FILTER_PREFIX "chroma-temp");
    filter->p_sys = sys;
    filter->pf_video_filter = Filter;
    var_AddCallback( filter, FILTER_PREFIX "luma-spat", DenoiseCallback, sys );
    var_AddCallback( filter, FILTER_PREFIX "chroma-spat", DenoiseCallback, sys );
    var_AddCallback( filter, FILTER_PREFIX "luma-temp", DenoiseCallback, sys );
    var_AddCallback( filter, FILTER_PREFIX "chroma-temp", DenoiseCallback, sys );
    return VLC_SUCCESS;
}
