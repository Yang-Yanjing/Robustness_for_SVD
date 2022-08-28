static int v4l2_read_close(AVFormatContext *ctx)
{
    struct video_data *s = ctx->priv_data;
    if (avpriv_atomic_int_get(&s->buffers_queued) != s->buffers)
        av_log(ctx, AV_LOG_WARNING, "Some buffers are still owned by the caller on "
               "close.\n");
    mmap_close(s);
    v4l2_close(s->fd);
    return 0;
}
