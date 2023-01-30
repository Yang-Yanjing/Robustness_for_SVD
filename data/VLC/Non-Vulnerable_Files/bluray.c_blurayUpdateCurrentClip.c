}
static void blurayUpdateCurrentClip(demux_t *p_demux, uint32_t clip)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    vlc_mutex_lock(&p_sys->pl_info_lock);
    p_sys->p_clip_info = NULL;
    p_sys->i_video_stream = -1;
    if (p_sys->p_pl_info && clip < p_sys->p_pl_info->clip_count) {
        p_sys->p_clip_info = &p_sys->p_pl_info->clips[clip];
    /* Let's assume a single video track for now.
     * This may brake later, but it's enough for now.
     */
        assert(p_sys->p_clip_info->video_stream_count >= 1);
        p_sys->i_video_stream = p_sys->p_clip_info->video_streams[0].pid;
    }
    vlc_mutex_unlock(&p_sys->pl_info_lock);
    blurayResetStillImage(p_demux);
}
