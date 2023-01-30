}
static int hls_ReloadPlaylist(stream_t *s)
{
    stream_sys_t *p_sys = s->p_sys;
    // Flag to indicate if we should signal download thread
    bool stream_appended = false;
    vlc_array_t *hls_streams = vlc_array_new();
    if (hls_streams == NULL)
        return VLC_ENOMEM;
    msg_Dbg(s, "Reloading HLS live meta playlist");
    if (get_HTTPLiveMetaPlaylist(s, &hls_streams) != VLC_SUCCESS)
    {
        /* Free hls streams */
        for (int i = 0; i < vlc_array_count(hls_streams); i++)
        {
            hls_stream_t *hls;
            hls = hls_Get(hls_streams, i);
            if (hls) hls_Free(hls);
        }
        vlc_array_destroy(hls_streams);
        msg_Err(s, "reloading playlist failed");
        return VLC_EGENERIC;
    }
    /* merge playlists */
    int count = vlc_array_count(hls_streams);
    for (int n = 0; n < count; n++)
    {
        hls_stream_t *hls_new = hls_Get(hls_streams, n);
        if (hls_new == NULL)
            continue;
        hls_stream_t *hls_old = hls_Find(p_sys->hls_stream, hls_new);
        if (hls_old == NULL)
        {   /* new hls stream - append */
            vlc_array_append(p_sys->hls_stream, hls_new);
            msg_Dbg(s, "new HLS stream appended (id=%d, bandwidth=%"PRIu64")",
                     hls_new->id, hls_new->bandwidth);
            // New segment available -  signal download thread
            stream_appended = true;
            continue;
        }
        else if (hls_UpdatePlaylist(s, hls_new, hls_old, &stream_appended) != VLC_SUCCESS)
            msg_Warn(s, "failed updating HLS stream (id=%d, bandwidth=%"PRIu64")",
                     hls_new->id, hls_new->bandwidth);
        hls_Free(hls_new);
    }
    vlc_array_destroy(hls_streams);
    // Must signal the download thread otherwise new segments will not be downloaded at all!
    if (stream_appended == true)
    {
        vlc_mutex_lock(&p_sys->download.lock_wait);
        vlc_cond_signal(&p_sys->download.wait);
        vlc_mutex_unlock(&p_sys->download.lock_wait);
    }
    return VLC_SUCCESS;
}
