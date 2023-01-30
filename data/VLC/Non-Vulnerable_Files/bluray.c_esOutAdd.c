}
static es_out_id_t *esOutAdd(es_out_t *p_out, const es_format_t *p_fmt)
{
    demux_sys_t *p_sys = p_out->p_sys->p_demux->p_sys;
    es_format_t fmt;
    es_format_Copy(&fmt, p_fmt);
    vlc_mutex_lock(&p_sys->pl_info_lock);
    switch (fmt.i_cat) {
    case VIDEO_ES:
        if (p_sys->i_video_stream != -1 && p_sys->i_video_stream != p_fmt->i_id)
            fmt.i_priority = ES_PRIORITY_NOT_SELECTABLE;
        break ;
    case AUDIO_ES:
        if (p_sys->i_audio_stream != -1 && p_sys->i_audio_stream != p_fmt->i_id)
            fmt.i_priority = ES_PRIORITY_NOT_SELECTABLE;
        if (p_sys->p_clip_info)
            setStreamLang(&fmt, p_sys->p_clip_info->audio_streams, p_sys->p_clip_info->audio_stream_count);
        break ;
    case SPU_ES:
        if (p_sys->i_spu_stream != -1 && p_sys->i_spu_stream != p_fmt->i_id)
            fmt.i_priority = ES_PRIORITY_NOT_SELECTABLE;
        if (p_sys->p_clip_info)
            setStreamLang(&fmt, p_sys->p_clip_info->pg_streams, p_sys->p_clip_info->pg_stream_count);
        break ;
    }
    vlc_mutex_unlock(&p_sys->pl_info_lock);
    es_out_id_t *p_es = es_out_Add(p_out->p_sys->p_demux->out, &fmt);
    if (p_fmt->i_id >= 0) {
        /* Ensure we are not overriding anything */
        int idx = findEsPairIndex(p_sys, p_fmt->i_id);
        if (idx == -1) {
            fmt_es_pair_t *p_pair = malloc(sizeof(*p_pair));
            if (likely(p_pair != NULL)) {
                p_pair->i_id = p_fmt->i_id;
                p_pair->p_es = p_es;
                msg_Info(p_out->p_sys->p_demux, "Adding ES %d", p_fmt->i_id);
                vlc_array_append(&p_sys->es, p_pair);
            }
        }
    }
    es_format_Clean(&fmt);
    return p_es;
}
