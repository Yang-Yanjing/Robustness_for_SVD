static av_cold int read_close(AVFormatContext *ctx)
{
    CDIOContext *s = ctx->priv_data;
    cdio_paranoia_free(s->paranoia);
    cdio_cddap_close(s->drive);
    return 0;
}
