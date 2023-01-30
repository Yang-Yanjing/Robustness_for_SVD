 * the first packets for NPT=0 is returned instead. */
int64_t rtp_get_ts( const sout_stream_t *p_stream, const sout_stream_id_sys_t *id,
                    const vod_media_t *p_media, const char *psz_vod_session,
                    int64_t *p_npt )
{
    if (p_npt != NULL)
        *p_npt = 0;
    if (id != NULL)
        p_stream = id->p_stream;
    if (p_stream == NULL)
        return rtp_init_ts(p_media, psz_vod_session);
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    mtime_t i_npt_zero;
    vlc_mutex_lock( &p_sys->lock_ts );
    i_npt_zero = p_sys->i_npt_zero;
    vlc_mutex_unlock( &p_sys->lock_ts );
    if( i_npt_zero == VLC_TS_INVALID )
        return p_sys->i_pts_zero;
    mtime_t now = mdate();
    if( now < i_npt_zero )
        return p_sys->i_pts_zero;
    int64_t npt = now - i_npt_zero;
    if (p_npt != NULL)
        *p_npt = npt;
    return p_sys->i_pts_zero + npt; 
}
