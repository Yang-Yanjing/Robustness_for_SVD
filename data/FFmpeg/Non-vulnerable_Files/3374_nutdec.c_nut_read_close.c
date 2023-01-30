static int nut_read_close(AVFormatContext *s)
{
    NUTContext *nut = s->priv_data;
    int i;
    av_freep(&nut->time_base);
    av_freep(&nut->stream);
    ff_nut_free_sp(nut);
    for (i = 1; i < nut->header_count; i++)
        av_freep(&nut->header[i]);
    return 0;
}
