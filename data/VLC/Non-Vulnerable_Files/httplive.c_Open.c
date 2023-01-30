 ****************************************************************************/
static int Open(vlc_object_t *p_this)
{
    stream_t *s = (stream_t*)p_this;
    stream_sys_t *p_sys;
    if (!isHTTPLiveStreaming(s))
        return VLC_EGENERIC;
    msg_Info(p_this, "HTTP Live Streaming (%s)", s->psz_path);
    /* Initialize crypto bit */
    vlc_gcrypt_init();
    /* */
    s->p_sys = p_sys = calloc(1, sizeof(*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    char *psz_uri = NULL;
    if (asprintf(&psz_uri,"%s://%s", s->psz_access, s->psz_path) < 0)
    {
        free(p_sys);
        return VLC_ENOMEM;
    }
    p_sys->m3u8 = psz_uri;
    char *new_path;
    if (asprintf(&new_path, "%s.ts", s->psz_path) < 0)
    {
        free(p_sys->m3u8);
        free(p_sys);
        return VLC_ENOMEM;
    }
    free(s->psz_path);
    s->psz_path = new_path;
    p_sys->bandwidth = 0;
    p_sys->b_live = true;
    p_sys->b_meta = false;
    p_sys->b_error = false;
    p_sys->hls_stream = vlc_array_new();
    if (p_sys->hls_stream == NULL)
    {
        free(p_sys->m3u8);
        free(p_sys);
        return VLC_ENOMEM;
    }
    /* */
    s->pf_read = Read;
    s->pf_peek = Peek;
    s->pf_control = Control;
    p_sys->paused = false;
    atomic_init(&p_sys->closing, false);
    atomic_init(&p_sys->eof, false);
    vlc_cond_init(&p_sys->wait);
    vlc_mutex_init(&p_sys->lock);
    /* Parse HLS m3u8 content. */
    uint8_t *buffer = NULL;
    ssize_t len = read_M3U8_from_stream(s->p_source, &buffer);
    if (len < 0)
        goto fail;
    if (parse_M3U8(s, p_sys->hls_stream, buffer, len, false) != VLC_SUCCESS)
    {
        free(buffer);
        goto fail;
    }
    free(buffer);
    /* HLS standard doesn't provide any guaranty about streams
       being sorted by bandwidth, so we sort them */
    qsort( p_sys->hls_stream->pp_elems, p_sys->hls_stream->i_count,
           sizeof( hls_stream_t* ), &hls_CompareStreams );
    /* Choose first HLS stream to start with */
    int current = p_sys->playback.stream = p_sys->hls_stream->i_count-1;
    p_sys->playback.segment = p_sys->download.segment = ChooseSegment(s, current);
    /* manage encryption key if needed */
    hls_ManageSegmentKeys(s, hls_Get(p_sys->hls_stream, current));
    if (Prefetch(s, &current) != VLC_SUCCESS)
    {
        msg_Err(s, "fetching first segment failed.");
    }
    p_sys->download.stream = current;
    p_sys->playback.stream = current;
    p_sys->download.seek = -1;
    vlc_mutex_init(&p_sys->download.lock_wait);
    vlc_cond_init(&p_sys->download.wait);
    vlc_mutex_init(&p_sys->read.lock_wait);
    vlc_cond_init(&p_sys->read.wait);
    /* Initialize HLS live stream */
    if (p_sys->b_live)
    {
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, current);
        p_sys->playlist.last = mdate();
        p_sys->playlist.wakeup = p_sys->playlist.last +
                ((mtime_t)hls->duration * CLOCK_FREQ );
        if (vlc_clone(&p_sys->reload, hls_Reload, s, VLC_THREAD_PRIORITY_LOW))
        {
            goto fail_thread;
        }
    }
    if (vlc_clone(&p_sys->thread, hls_Thread, s, VLC_THREAD_PRIORITY_INPUT))
    {
        if (p_sys->b_live)
        {
            vlc_cancel(p_sys->reload);
            vlc_join(p_sys->reload, NULL);
        }
        goto fail_thread;
    }
    return VLC_SUCCESS;
fail_thread:
    vlc_mutex_destroy(&p_sys->download.lock_wait);
    vlc_cond_destroy(&p_sys->download.wait);
    vlc_mutex_destroy(&p_sys->read.lock_wait);
    vlc_cond_destroy(&p_sys->read.wait);
fail:
    /* Free hls streams */
    for (int i = 0; i < vlc_array_count(p_sys->hls_stream); i++)
    {
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, i);
        if (hls) hls_Free(hls);
    }
    vlc_array_destroy(p_sys->hls_stream);
    vlc_mutex_destroy(&p_sys->lock);
    vlc_cond_destroy(&p_sys->wait);
    /* */
    free(p_sys->m3u8);
    free(p_sys);
    return VLC_EGENERIC;
}
