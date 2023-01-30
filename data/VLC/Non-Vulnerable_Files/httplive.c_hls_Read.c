/* p_read might be NULL if caller wants to skip data */
static ssize_t hls_Read(stream_t *s, uint8_t *p_read, unsigned int i_read)
{
    stream_sys_t *p_sys = s->p_sys;
    ssize_t used = 0;
    do
    {
        /* Determine next segment to read. If this is a meta playlist and
         * bandwidth conditions changed, then the stream might have switched
         * to another bandwidth. */
        segment_t *segment = GetSegment(s);
        if (segment == NULL)
            break;
        vlc_mutex_lock(&segment->lock);
        if (segment->data->i_buffer == 0)
        {
            if (!p_sys->b_cache || p_sys->b_live)
            {
                block_Release(segment->data);
                segment->data = NULL;
            }
            else
                segment_RestorePos(segment);
            vlc_mutex_unlock(&segment->lock);
            /* signal download thread */
            vlc_mutex_lock(&p_sys->download.lock_wait);
            p_sys->playback.segment++;
            vlc_cond_signal(&p_sys->download.wait);
            vlc_mutex_unlock(&p_sys->download.lock_wait);
            continue;
        }
        if (segment->size == segment->data->i_buffer)
            msg_Dbg(s, "playing segment %d from stream %d",
                     segment->sequence, p_sys->playback.stream);
        ssize_t len = -1;
        if (i_read <= segment->data->i_buffer)
            len = i_read;
        else if (i_read > segment->data->i_buffer)
            len = segment->data->i_buffer;
        if (len > 0)
        {
            if (p_read) /* if NULL, then caller skips data */
                memcpy(p_read + used, segment->data->p_buffer, len);
            segment->data->i_buffer -= len;
            segment->data->p_buffer += len;
            used += len;
            i_read -= len;
        }
        vlc_mutex_unlock(&segment->lock);
    } while (i_read > 0);
    return used;
}
