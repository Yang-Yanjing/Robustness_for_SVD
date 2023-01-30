static int get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
    InputStream *ist = s->opaque;
    if (ist->hwaccel_get_buffer && frame->format == ist->hwaccel_pix_fmt)
        return ist->hwaccel_get_buffer(s, frame, flags);
    return avcodec_default_get_buffer2(s, frame, flags);
}
