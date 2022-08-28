static int handle_invoke_result(URLContext *s, RTMPPacket *pkt)
{
    RTMPContext *rt = s->priv_data;
    char *tracked_method = NULL;
    int ret = 0;
    if ((ret = find_tracked_method(s, pkt, 10, &tracked_method)) < 0)
        return ret;
    if (!tracked_method) {
        
        return ret;
    }
    if (!strcmp(tracked_method, "connect")) {
        if (!rt->is_input) {
            if ((ret = gen_release_stream(s, rt)) < 0)
                goto fail;
            if ((ret = gen_fcpublish_stream(s, rt)) < 0)
                goto fail;
        } else {
            if ((ret = gen_server_bw(s, rt)) < 0)
                goto fail;
        }
        if ((ret = gen_create_stream(s, rt)) < 0)
            goto fail;
        if (rt->is_input) {
            

            if (rt->subscribe) {
                if ((ret = gen_fcsubscribe_stream(s, rt, rt->subscribe)) < 0)
                    goto fail;
            } else if (rt->live == -1) {
                if ((ret = gen_fcsubscribe_stream(s, rt, rt->playpath)) < 0)
                    goto fail;
            }
        }
    } else if (!strcmp(tracked_method, "createStream")) {
        double stream_id;
        if (read_number_result(pkt, &stream_id)) {
            av_log(s, AV_LOG_WARNING, "Unexpected reply on connect()\n");
        } else {
            rt->stream_id = stream_id;
        }
        if (!rt->is_input) {
            if ((ret = gen_publish(s, rt)) < 0)
                goto fail;
        } else {
            if (rt->live != -1) {
                if ((ret = gen_get_stream_length(s, rt)) < 0)
                    goto fail;
            }
            if ((ret = gen_play(s, rt)) < 0)
                goto fail;
            if ((ret = gen_buffer_time(s, rt)) < 0)
                goto fail;
        }
    } else if (!strcmp(tracked_method, "getStreamLength")) {
        if (read_number_result(pkt, &rt->duration)) {
            av_log(s, AV_LOG_WARNING, "Unexpected reply on getStreamLength()\n");
        }
    }
fail:
    av_free(tracked_method);
    return ret;
}
