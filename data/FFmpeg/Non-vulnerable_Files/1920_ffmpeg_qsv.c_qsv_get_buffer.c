char *qsv_device = NULL;
static int qsv_get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
    InputStream *ist = s->opaque;
    return av_hwframe_get_buffer(ist->hw_frames_ctx, frame, 0);
}
