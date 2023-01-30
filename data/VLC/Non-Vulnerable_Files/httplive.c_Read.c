}
static int Read(stream_t *s, void *buffer, unsigned int i_read)
{
    stream_sys_t *p_sys = s->p_sys;
    ssize_t length = 0;
    assert(p_sys->hls_stream);
    while (length == 0)
    {
        // In case an error occurred or the stream was closed return 0
        if (p_sys->b_error || atomic_load(&p_sys->closing))
            return 0;
        // Lock the mutex before trying to read to avoid a race condition with the download thread
        vlc_mutex_lock(&p_sys->read.lock_wait);
        /* NOTE: buffer might be NULL if caller wants to skip data */
        length = hls_Read(s, (uint8_t*) buffer, i_read);
        // An error has occurred in hls_Read
        if (length < 0)
        {
            vlc_mutex_unlock(&p_sys->read.lock_wait);
            return 0;
        }
        // There is no data available yet for the demuxer so we need to wait until reload and
        // download operation are over.
        // Download thread will signal once download is finished.
        // A timed wait is used to avoid deadlock in case data never arrives since the thread
        // running this read operation is also responsible for closing the stream
        if (length == 0)
        {
            if(atomic_load(&p_sys->eof)) /* finished reading last segment */
            {
                vlc_mutex_unlock(&p_sys->read.lock_wait);
                return 0;
            }
            mtime_t start = mdate();
            // Wait for 10 seconds
            mtime_t timeout_limit = start + (10 * CLOCK_FREQ);
            int res = vlc_cond_timedwait(&p_sys->read.wait, &p_sys->read.lock_wait, timeout_limit);
            // Error - reached a timeout of 10 seconds without data arriving - kill the stream
            if (res == ETIMEDOUT)
            {
                msg_Warn(s, "timeout limit reached!");
                vlc_mutex_unlock(&p_sys->read.lock_wait);
                return 0;
            }
            else if (res == EINVAL)
                return 0; // Error - lock is not locked so we can just return
        }
        vlc_mutex_unlock(&p_sys->read.lock_wait);
    }
    p_sys->playback.offset += length;
    return length;
}
