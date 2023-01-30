 */
static int OpenCommon(vlc_object_t *p_this, bool b_sub)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* */
    if (!b_sub && !es_format_IsSimilar(&p_filter->fmt_in, &p_filter->fmt_out)) {
        msg_Err(p_filter, "Input and output format does not match");
        return VLC_EGENERIC;
    }
    /* */
    p_filter->p_sys = p_sys = malloc(sizeof(*p_sys));
    if (!p_sys)
        return VLC_ENOMEM;
    /* */
    p_sys->p_blend = NULL;
    if (!b_sub) {
        p_sys->p_blend = filter_NewBlend(VLC_OBJECT(p_filter),
                                          &p_filter->fmt_in.video);
        if (!p_sys->p_blend) {
            free(p_sys);
            return VLC_EGENERIC;
        }
    }
    /* */
    config_ChainParse(p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg);
    /* create and initialize variables */
    p_sys->i_pos = var_CreateGetInteger(p_filter, CFG_PREFIX "position");
    p_sys->i_pos_x = var_CreateGetInteger(p_filter, CFG_PREFIX "x");
    p_sys->i_pos_y = var_CreateGetInteger(p_filter, CFG_PREFIX "y");
    BarGraph_t *p_BarGraph = &p_sys->p_BarGraph;
    p_BarGraph->p_pic = NULL;
    p_BarGraph->i_alpha = var_CreateGetInteger(p_filter, CFG_PREFIX "transparency");
    p_BarGraph->i_alpha = VLC_CLIP(p_BarGraph->i_alpha, 0, 255);
    p_BarGraph->i_values = NULL;
    parse_i_values(p_BarGraph, &(char){ 0 });
    p_BarGraph->alarm = false;
    p_BarGraph->barWidth = var_CreateGetInteger(p_filter, CFG_PREFIX "barWidth");
    p_BarGraph->scale = 400;
    /* Ignore aligment if a position is given for video filter */
    if (!b_sub && p_sys->i_pos_x >= 0 && p_sys->i_pos_y >= 0)
        p_sys->i_pos = 0;
    vlc_mutex_init(&p_sys->lock);
    var_Create(p_filter->p_libvlc, CFG_PREFIX "alarm", VLC_VAR_BOOL);
    var_Create(p_filter->p_libvlc, CFG_PREFIX "i_values", VLC_VAR_STRING);
    var_AddCallback(p_filter->p_libvlc, CFG_PREFIX "alarm",
                    BarGraphCallback, p_sys);
    var_AddCallback(p_filter->p_libvlc, CFG_PREFIX "i_values",
                    BarGraphCallback, p_sys);
    var_TriggerCallback(p_filter->p_libvlc, CFG_PREFIX "alarm");
    var_TriggerCallback(p_filter->p_libvlc, CFG_PREFIX "i_values");
    for (int i = 0; ppsz_filter_callbacks[i]; i++)
        var_AddCallback(p_filter, ppsz_filter_callbacks[i],
                         BarGraphCallback, p_sys);
    if (b_sub)
        p_filter->pf_sub_source = FilterSub;
    else
        p_filter->pf_video_filter = FilterVideo;
    return VLC_SUCCESS;
}
