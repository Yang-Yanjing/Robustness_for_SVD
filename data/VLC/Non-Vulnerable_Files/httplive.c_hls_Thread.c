}
static void* hls_Thread(void *p_this)
{
    stream_t *s = (stream_t *)p_this;
    stream_sys_t *p_sys = s->p_sys;
    for( ;; )
    {
        hls_stream_t *hls = hls_Get(p_sys->hls_stream, p_sys->download.stream);
        assert(hls);
        /* Sliding window (~60 seconds worth of movie) */
        vlc_mutex_lock(&hls->lock);
        int count = vlc_array_count(hls->segments);
        vlc_mutex_unlock(&hls->lock);
        /* Is there a new segment to process? */
        if ((!p_sys->b_live && (p_sys->playback.segment < (count - 6))) ||
            (p_sys->download.segment >= count))
        {
            /* wait */
            vlc_mutex_lock(&p_sys->download.lock_wait);
            mutex_cleanup_push(&p_sys->download.lock_wait); //CO
            while (((p_sys->download.segment - p_sys->playback.segment > 6) ||
                    (p_sys->download.segment >= count)) &&
                   (p_sys->download.seek == -1))
            {
                if(!p_sys->b_live && p_sys->download.segment >= count)
                {
                    /* this was last segment to read */
                    atomic_store(&p_sys->eof, true);
                }
                vlc_cond_wait(&p_sys->download.wait, &p_sys->download.lock_wait);
                if (p_sys->b_live /*&& (mdate() >= p_sys->playlist.wakeup)*/)
                    break;
            }
            vlc_cleanup_pop( ); //CO
            /* */
            if (p_sys->download.seek >= 0)
            {
                p_sys->download.segment = p_sys->download.seek;
                p_sys->download.seek = -1;
                atomic_store(&p_sys->eof, false);
            }
            vlc_mutex_unlock(&p_sys->download.lock_wait);
        }
        vlc_mutex_lock(&p_sys->lock);
        mutex_cleanup_push(&p_sys->lock); //C1
        while (p_sys->paused)
            vlc_cond_wait(&p_sys->wait, &p_sys->lock);
        vlc_cleanup_run( ); //C1 vlc_mutex_unlock(&p_sys->lock);
        vlc_mutex_lock(&hls->lock);
        segment_t *segment = segment_GetSegment(hls, p_sys->download.segment);
        vlc_mutex_unlock(&hls->lock);
        int i_canc = vlc_savecancel();
        if ((segment != NULL) &&
            (hls_DownloadSegmentData(s, hls, segment, &p_sys->download.stream) != VLC_SUCCESS))
        {
            if (!p_sys->b_live)
            {
                p_sys->b_error = true;
                break;
            }
        }
        vlc_restorecancel(i_canc);
        /* download succeeded */
        /* determine next segment to download */
        vlc_mutex_lock(&p_sys->download.lock_wait);
        if (p_sys->download.seek >= 0)
        {
            p_sys->download.segment = p_sys->download.seek;
            p_sys->download.seek = -1;
        }
        else if (p_sys->download.segment < count)
            p_sys->download.segment++;
        vlc_cond_signal(&p_sys->download.wait);
        vlc_mutex_unlock(&p_sys->download.lock_wait);
        // In case of a successful download signal the read thread that data is available
        vlc_mutex_lock(&p_sys->read.lock_wait);
        vlc_cond_signal(&p_sys->read.wait);
        vlc_mutex_unlock(&p_sys->read.lock_wait);
        vlc_testcancel();
    }
    return NULL;
}
