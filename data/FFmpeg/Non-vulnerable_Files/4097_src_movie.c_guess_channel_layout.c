static int guess_channel_layout(MovieStream *st, int st_index, void *log_ctx)
{
    AVCodecContext *dec_ctx = st->st->codec;
    char buf[256];
    int64_t chl = av_get_default_channel_layout(dec_ctx->channels);
    if (!chl) {
        av_log(log_ctx, AV_LOG_ERROR,
               "Channel layout is not set in stream %d, and could not "
               "be guessed from the number of channels (%d)\n",
               st_index, dec_ctx->channels);
        return AVERROR(EINVAL);
    }
    av_get_channel_layout_string(buf, sizeof(buf), dec_ctx->channels, chl);
    av_log(log_ctx, AV_LOG_WARNING,
           "Channel layout is not set in output stream %d, "
           "guessed channel layout is '%s'\n",
           st_index, buf);
    dec_ctx->channel_layout = chl;
    return 0;
}
