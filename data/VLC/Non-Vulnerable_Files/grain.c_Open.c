}
static int Open(vlc_object_t *object)
{
    filter_t *filter = (filter_t *)object;
    const vlc_chroma_description_t *chroma =
        vlc_fourcc_GetChromaDescription(filter->fmt_in.video.i_chroma);
    if (!chroma || chroma->plane_count < 3 || chroma->pixel_size != 1) {
        msg_Err(filter, "Unsupported chroma (%4.4s)",
                (char*)&(filter->fmt_in.video.i_chroma));
        return VLC_EGENERIC;
    }
    filter_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->is_uv_filtered = true;
    sys->scale          = -1;
    sys->seed           = URAND_SEED;
    int cutoff_low = BANK_SIZE - var_InheritInteger(filter, CFG_PREFIX "period-max");
    int cutoff_high= BANK_SIZE - var_InheritInteger(filter, CFG_PREFIX "period-min");
    cutoff_low  = VLC_CLIP(cutoff_low, 1, BANK_SIZE - 1);
    cutoff_high = VLC_CLIP(cutoff_high, 1, BANK_SIZE - 1);
    if (Generate(sys->bank, cutoff_low, cutoff_high, cutoff_low, cutoff_high)) {
        free(sys);
        return VLC_EGENERIC;
    }
    sys->blend = BlockBlendC;
    sys->emms  = NULL;
#if defined(CAN_COMPILE_SSE2) && 1
    if (vlc_CPU_SSE2()) {
        sys->blend = BlockBlendSse2;
        sys->emms  = Emms;
    }
#endif
    vlc_mutex_init(&sys->cfg.lock);
    sys->cfg.variance = var_CreateGetFloatCommand(filter, CFG_PREFIX "variance");
    var_AddCallback(filter, CFG_PREFIX "variance", Callback, NULL);
    filter->p_sys           = sys;
    filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
