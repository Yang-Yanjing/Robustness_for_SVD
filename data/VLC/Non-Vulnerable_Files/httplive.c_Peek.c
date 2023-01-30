}
static int Peek(stream_t *s, const uint8_t **pp_peek, unsigned int i_peek)
{
    stream_sys_t *p_sys = s->p_sys;
    segment_t *segment;
    unsigned int len = i_peek;
    segment = GetSegment(s);
    if (segment == NULL)
    {
        msg_Err(s, "segment %d should have been available (stream %d)",
                p_sys->playback.segment, p_sys->playback.stream);
        return 0; /* eof? */
    }
    vlc_mutex_lock(&segment->lock);
    size_t i_buff = segment->data->i_buffer;
    uint8_t *p_buff = segment->data->p_buffer;
    if ( likely(i_peek < i_buff))
    {
        *pp_peek = p_buff;
        vlc_mutex_unlock(&segment->lock);
        return i_peek;
    }
    else /* This will seldom be run */
    {
        /* remember segment to read */
        int peek_segment = p_sys->playback.segment;
        size_t curlen = 0;
        segment_t *nsegment;
        p_sys->playback.segment++;
        block_t *peeked = p_sys->peeked;
        if (peeked == NULL)
            peeked = block_Alloc (i_peek);
        else if (peeked->i_buffer < i_peek)
            peeked = block_Realloc (peeked, 0, i_peek);
        if (peeked == NULL)
        {
            vlc_mutex_unlock(&segment->lock);
            return 0;
        }
        p_sys->peeked = peeked;
        memcpy(peeked->p_buffer, p_buff, i_buff);
        curlen = i_buff;
        len -= i_buff;
        vlc_mutex_unlock(&segment->lock);
        i_buff = peeked->i_buffer;
        p_buff = peeked->p_buffer;
        *pp_peek = p_buff;
        while (curlen < i_peek)
        {
            nsegment = GetSegment(s);
            if (nsegment == NULL)
            {
                msg_Err(s, "segment %d should have been available (stream %d)",
                        p_sys->playback.segment, p_sys->playback.stream);
                /* restore segment to read */
                p_sys->playback.segment = peek_segment;
                return curlen; /* eof? */
            }
            vlc_mutex_lock(&nsegment->lock);
            if (len < nsegment->data->i_buffer)
            {
                memcpy(p_buff + curlen, nsegment->data->p_buffer, len);
                curlen += len;
            }
            else
            {
                size_t i_nbuff = nsegment->data->i_buffer;
                memcpy(p_buff + curlen, nsegment->data->p_buffer, i_nbuff);
                curlen += i_nbuff;
                len -= i_nbuff;
                p_sys->playback.segment++;
            }
            vlc_mutex_unlock(&nsegment->lock);
        }
        /* restore segment to read */
        p_sys->playback.segment = peek_segment;
        return curlen;
    }
}
