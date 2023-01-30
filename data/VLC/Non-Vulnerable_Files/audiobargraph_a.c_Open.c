 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t     *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys = malloc(sizeof(*p_sys));
    if (!p_sys)
        return VLC_ENOMEM;
    static const char *const options[] = {
        "bargraph", "bargraph_repetition", "silence", "time_window",
        "alarm_threshold", "repetition_time", NULL
    };
    config_ChainParse(p_filter, CFG_PREFIX, options, p_filter->p_cfg);
    p_sys->bargraph = !!var_CreateGetInteger(p_filter, CFG_PREFIX "bargraph");
    p_sys->bargraph_repetition = var_CreateGetInteger(p_filter, CFG_PREFIX "bargraph_repetition");
    p_sys->silence = !!var_CreateGetInteger(p_filter, CFG_PREFIX "silence");
    p_sys->time_window = var_CreateGetInteger(p_filter, CFG_PREFIX "time_window") * 1000;
    p_sys->alarm_threshold = var_CreateGetFloat(p_filter, CFG_PREFIX "alarm_threshold");
    p_sys->repetition_time = var_CreateGetInteger(p_filter, CFG_PREFIX "repetition_time") * 1000;
    p_sys->counter = 0;
    p_sys->first = NULL;
    p_sys->last = NULL;
    p_sys->started = 0;
    p_sys->lastAlarm = 0;
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    var_Create(p_filter->p_libvlc, "audiobargraph_v-alarm", VLC_VAR_BOOL);
    var_Create(p_filter->p_libvlc, "audiobargraph_v-i_values", VLC_VAR_STRING);
    return VLC_SUCCESS;
}
