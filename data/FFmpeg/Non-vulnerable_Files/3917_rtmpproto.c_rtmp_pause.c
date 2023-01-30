static int rtmp_pause(URLContext *s, int pause)
{
    RTMPContext *rt = s->priv_data;
    int ret;
    av_log(s, AV_LOG_DEBUG, "Pause at timestamp %d\n",
           rt->last_timestamp);
    if ((ret = gen_pause(s, rt, pause, rt->last_timestamp)) < 0) {
        av_log(s, AV_LOG_ERROR, "Unable to send pause command at timestamp %d\n",
               rt->last_timestamp);
        return ret;
    }
    return 0;
}
