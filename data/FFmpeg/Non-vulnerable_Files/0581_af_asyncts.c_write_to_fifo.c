static int write_to_fifo(ASyncContext *s, AVFrame *buf)
{
    int ret = avresample_convert(s->avr, NULL, 0, 0, buf->extended_data,
                                 buf->linesize[0], buf->nb_samples);
    av_frame_free(&buf);
    return ret;
}
