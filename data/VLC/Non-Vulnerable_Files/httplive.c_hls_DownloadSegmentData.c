}
static int hls_DownloadSegmentData(stream_t *s, hls_stream_t *hls, segment_t *segment, int *cur_stream)
{
    stream_sys_t *p_sys = s->p_sys;
    assert(hls);
    assert(segment);
    vlc_mutex_lock(&segment->lock);
    if (segment->data != NULL)
    {
        /* Segment already downloaded */
        vlc_mutex_unlock(&segment->lock);
        return VLC_SUCCESS;
    }
    /* sanity check - can we download this segment on time? */
    if ((p_sys->bandwidth > 0) && (hls->bandwidth > 0))
    {
        uint64_t size = (segment->duration * hls->bandwidth); /* bits */
        int estimated = (int)(size / p_sys->bandwidth);
        if (estimated > segment->duration)
        {
            msg_Warn(s,"downloading segment %d predicted to take %ds, which exceeds its length (%ds)",
                        segment->sequence, estimated, segment->duration);
        }
    }
    mtime_t start = mdate();
    int i_ret = hls_Download(s, segment);
    if (i_ret != VLC_SUCCESS)
    {
        msg_Err(s, "downloading segment %d from stream %d failed",
                    segment->sequence, *cur_stream);
        vlc_mutex_unlock(&segment->lock);
        return i_ret;
    }
    mtime_t duration = mdate() - start;
    if (hls->bandwidth == 0 && segment->duration > 0)
    {
        /* Try to estimate the bandwidth for this stream */
        hls->bandwidth = (uint64_t)(((double)segment->size * 8) / ((double)segment->duration));
    }
    /* If the segment is encrypted, decode it */
    i_ret = hls_DecodeSegmentData(s, hls, segment);
    vlc_mutex_unlock(&segment->lock);
    if(i_ret != VLC_SUCCESS)
        return i_ret;
    msg_Dbg(s, "downloaded segment %d from stream %d",
                segment->sequence, *cur_stream);
    uint64_t bw = segment->size * 8 * 1000000 / __MAX(1, duration); /* bits / s */
    p_sys->bandwidth = bw;
    if (p_sys->b_meta && (hls->bandwidth != bw))
    {
        int newstream = BandwidthAdaptation(s, hls->id, &bw);
        /* FIXME: we need an average here */
        if ((newstream >= 0) && (newstream != *cur_stream))
        {
            msg_Dbg(s, "detected %s bandwidth (%"PRIu64") stream",
                     (bw >= hls->bandwidth) ? "faster" : "lower", bw);
            *cur_stream = newstream;
        }
    }
    return VLC_SUCCESS;
}
