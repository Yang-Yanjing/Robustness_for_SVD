static void av_cold uninit(AVFilterContext *ctx)
{
    ZMQContext *zmq = ctx->priv;
    zmq_close(zmq->responder);
    zmq_ctx_destroy(zmq->zmq);
}
