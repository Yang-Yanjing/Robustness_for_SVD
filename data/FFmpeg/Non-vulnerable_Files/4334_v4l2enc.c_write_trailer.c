static int write_trailer(AVFormatContext *s1)
{
    const V4L2Context *s = s1->priv_data;
    close(s->fd);
    return 0;
}
