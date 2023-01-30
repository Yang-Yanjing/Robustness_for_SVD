FF_DISABLE_DEPRECATION_WARNINGS
static void compat_free_buffer(AVFilterBuffer *buf)
{
    AVFrame *frame = buf->priv;
    av_frame_free(&frame);
    av_free(buf);
}
