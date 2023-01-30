static av_cold int init(AVFilterContext *ctx)
{
    ZMQContext *zmq = ctx->priv;
    zmq->zmq = zmq_ctx_new();
    if (!zmq->zmq) {
        av_log(ctx, AV_LOG_ERROR,
               "Could not create ZMQ context: %s\n", zmq_strerror(errno));
        return AVERROR_EXTERNAL;
    }
    zmq->responder = zmq_socket(zmq->zmq, ZMQ_REP);
    if (!zmq->responder) {
        av_log(ctx, AV_LOG_ERROR,
               "Could not create ZMQ socket: %s\n", zmq_strerror(errno));
        return AVERROR_EXTERNAL;
    }
    if (zmq_bind(zmq->responder, zmq->bind_address) == -1) {
        av_log(ctx, AV_LOG_ERROR,
               "Could not bind ZMQ socket to address '%s': %s\n",
               zmq->bind_address, zmq_strerror(errno));
        return AVERROR_EXTERNAL;
    }
    zmq->command_count = -1;
    return 0;
}
