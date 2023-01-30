static int recv_msg(AVFilterContext *ctx, char **buf, int *buf_size)
{
    ZMQContext *zmq = ctx->priv;
    zmq_msg_t msg;
    int ret = 0;
    if (zmq_msg_init(&msg) == -1) {
        av_log(ctx, AV_LOG_WARNING,
               "Could not initialize receive message: %s\n", zmq_strerror(errno));
        return AVERROR_EXTERNAL;
    }
    if (zmq_msg_recv(&msg, zmq->responder, ZMQ_DONTWAIT) == -1) {
        if (errno != EAGAIN)
            av_log(ctx, AV_LOG_WARNING,
                   "Could not receive message: %s\n", zmq_strerror(errno));
        ret = AVERROR_EXTERNAL;
        goto end;
    }
    *buf_size = zmq_msg_size(&msg) + 1;
    *buf = av_malloc(*buf_size);
    if (!*buf) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    memcpy(*buf, zmq_msg_data(&msg), *buf_size);
    (*buf)[*buf_size-1] = 0;
end:
    zmq_msg_close(&msg);
    return ret;
}
