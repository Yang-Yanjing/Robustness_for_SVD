static int rm_read_close(AVFormatContext *s)
{
    int i;
    for (i=0;i<s->nb_streams;i++)
        ff_rm_free_rmstream(s->streams[i]->priv_data);
    return 0;
}
