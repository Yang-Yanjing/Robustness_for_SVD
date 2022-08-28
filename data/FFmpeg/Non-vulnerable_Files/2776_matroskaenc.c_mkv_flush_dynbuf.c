static void mkv_flush_dynbuf(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    int bufsize;
    uint8_t *dyn_buf;
    if (!mkv->dyn_bc)
        return;
    bufsize = avio_close_dyn_buf(mkv->dyn_bc, &dyn_buf);
    avio_write(s->pb, dyn_buf, bufsize);
    av_free(dyn_buf);
    mkv->dyn_bc = NULL;
}
