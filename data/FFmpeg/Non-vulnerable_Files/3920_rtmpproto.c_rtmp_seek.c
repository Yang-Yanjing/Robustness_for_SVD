static int64_t rtmp_seek(URLContext *s, int stream_index, int64_t timestamp,
                         int flags)
{
    RTMPContext *rt = s->priv_data;
    int ret;
    av_log(s, AV_LOG_DEBUG,
           "Seek on stream index %d at timestamp %"PRId64" with flags %08x\n",
           stream_index, timestamp, flags);
    if ((ret = gen_seek(s, rt, timestamp)) < 0) {
        av_log(s, AV_LOG_ERROR,
               "Unable to send seek command on stream index %d at timestamp "
               "%"PRId64" with flags %08x\n",
               stream_index, timestamp, flags);
        return ret;
    }
    rt->flv_off = rt->flv_size;
    rt->state = STATE_SEEKING;
    return timestamp;
}
