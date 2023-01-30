}
static void blurayStreamSelect(demux_t *p_demux, uint32_t i_type, uint32_t i_id)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int i_pid = -1;
    vlc_mutex_lock(&p_sys->pl_info_lock);
    if (p_sys->p_clip_info) {
        /* The param we get is the real stream id, not an index, ie. it starts from 1 */
        i_id--;
        if (i_type == BD_EVENT_AUDIO_STREAM) {
            if (i_id < p_sys->p_clip_info->audio_stream_count) {
                i_pid = p_sys->p_clip_info->audio_streams[i_id].pid;
                p_sys->i_audio_stream = i_pid;
            }
        } else if (i_type == BD_EVENT_PG_TEXTST_STREAM) {
            if (i_id < p_sys->p_clip_info->pg_stream_count) {
                i_pid = p_sys->p_clip_info->pg_streams[i_id].pid;
                p_sys->i_spu_stream = i_pid;
            }
        }
    }
    vlc_mutex_unlock(&p_sys->pl_info_lock);
    if (i_pid > 0) {
        int i_idx = findEsPairIndex(p_sys, i_pid);
        if (i_idx >= 0) {
            es_out_id_t *p_es = vlc_array_item_at_index(&p_sys->es, i_idx);
            es_out_Control(p_demux->out, ES_OUT_SET_ES, p_es);
        }
    }
}
