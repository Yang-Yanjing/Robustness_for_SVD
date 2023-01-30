static int rtmp_close(URLContext *h)
{
    RTMPContext *rt = h->priv_data;
    int ret = 0, i, j;
    if (!rt->is_input) {
        rt->flv_data = NULL;
        if (rt->out_pkt.size)
            ff_rtmp_packet_destroy(&rt->out_pkt);
        if (rt->state > STATE_FCPUBLISH)
            ret = gen_fcunpublish_stream(h, rt);
    }
    if (rt->state > STATE_HANDSHAKED)
        ret = gen_delete_stream(h, rt);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < rt->nb_prev_pkt[i]; j++)
            ff_rtmp_packet_destroy(&rt->prev_pkt[i][j]);
        av_freep(&rt->prev_pkt[i]);
    }
    free_tracked_methods(rt);
    av_freep(&rt->flv_data);
    ffurl_close(rt->stream);
    return ret;
}
