};
static int Open(vlc_object_t *object)
{
    filter_t *filter = (filter_t *)object;
    const vlc_fourcc_t fourcc = filter->fmt_in.video.i_chroma;
    const vlc_chroma_description_t *chroma = vlc_fourcc_GetChromaDescription(fourcc);
    if (!chroma || chroma->plane_count < 3 || chroma->pixel_size != 1) {
        msg_Err(filter, "Unsupported chroma (%4.4s)", (char*)&fourcc);
        return VLC_EGENERIC;
    }
    filter_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    vlc_mutex_init(&sys->lock);
    sys->chroma   = chroma;
    sys->strength = var_CreateGetFloatCommand(filter,   CFG_PREFIX "strength");
    sys->radius   = var_CreateGetIntegerCommand(filter, CFG_PREFIX "radius");
    var_AddCallback(filter, CFG_PREFIX "strength", Callback, NULL);
    var_AddCallback(filter, CFG_PREFIX "radius",   Callback, NULL);
    sys->cfg.buf = NULL;
    struct vf_priv_s *cfg = &sys->cfg;
    cfg->thresh      = 0.0;
    cfg->radius      = 0;
    cfg->buf         = NULL;
#if HAVE_SSE2 && HAVE_6REGS
    if (vlc_CPU_SSE2())
        cfg->blur_line = blur_line_sse2;
    else
#endif
        cfg->blur_line   = blur_line_c;
#if HAVE_SSSE3
    if (vlc_CPU_SSSE3())
        cfg->filter_line = filter_line_ssse3;
    else
#endif
#if HAVE_MMX2
    if (vlc_CPU_MMXEXT())
        cfg->filter_line = filter_line_mmx2;
    else
#endif
        cfg->filter_line = filter_line_c;
    filter->p_sys           = sys;
    filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
