static int read_close(AVFormatContext *s)
{
    WtvContext *wtv = s->priv_data;
    av_freep(&wtv->index_entries);
    wtvfile_close(wtv->pb);
    return 0;
}
