}
static void* hls_Reload(void *p_this)
{
    stream_t *s = (stream_t *)p_this;
    stream_sys_t *p_sys = s->p_sys;
    assert(p_sys->b_live);
    double wait = 1.0;
    for ( ;; )
    {
        mtime_t now = mdate();
        if (now >= p_sys->playlist.wakeup)
        {
            int canc = vlc_savecancel();
            /* reload the m3u8 if there are less than 3 segments what aren't downloaded */
            if ( ( p_sys->download.segment - p_sys->playback.segment < 3 ) &&
                 ( hls_ReloadPlaylist(s) != VLC_SUCCESS) )
            {
                /* No change in playlist, then backoff */
                p_sys->playlist.tries++;
                if (p_sys->playlist.tries == 1) wait = 0.5;
                else if (p_sys->playlist.tries == 2) wait = 1;
                else if (p_sys->playlist.tries >= 3) wait = 1.5;
                /* Can we afford to backoff? */
                if (p_sys->download.segment - p_sys->playback.segment < 3)
                {
                    p_sys->playlist.tries = 0;
                    wait = 0.5;
                }
            }
            else
            {
                p_sys->playlist.tries = 0;
                wait = 1.0;
            }
            vlc_restorecancel(canc);
            hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->download.stream);
            assert(hls);
            /* determine next time to update playlist */
            p_sys->playlist.last = now;
            p_sys->playlist.wakeup = now;
            /* If there is no new segments,use playlist duration as sleep period base */
            if( likely( hls->max_segment_length > 0 ) )
                p_sys->playlist.wakeup += (mtime_t)((hls->max_segment_length * wait) * CLOCK_FREQ);
            else
                p_sys->playlist.wakeup += (mtime_t)((hls->duration * wait) * CLOCK_FREQ);
        }
        mwait(p_sys->playlist.wakeup);
    }
    return NULL;
}
