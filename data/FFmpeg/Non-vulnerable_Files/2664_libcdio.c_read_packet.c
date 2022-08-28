static int read_packet(AVFormatContext *ctx, AVPacket *pkt)
{
    CDIOContext *s = ctx->priv_data;
    int ret;
    uint16_t *buf;
    char *err = NULL;
    if (ctx->streams[0]->cur_dts > s->last_sector)
        return AVERROR_EOF;
    buf = cdio_paranoia_read(s->paranoia, NULL);
    if (!buf)
        return AVERROR_EOF;
    if (err = cdio_cddap_errors(s->drive)) {
        av_log(ctx, AV_LOG_ERROR, "%s\n", err);
        free(err);
        err = NULL;
    }
    if (err = cdio_cddap_messages(s->drive)) {
        av_log(ctx, AV_LOG_VERBOSE, "%s\n", err);
        free(err);
        err = NULL;
    }
    if ((ret = av_new_packet(pkt, CDIO_CD_FRAMESIZE_RAW)) < 0)
        return ret;
    memcpy(pkt->data, buf, CDIO_CD_FRAMESIZE_RAW);
    return 0;
}
