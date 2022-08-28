static int rewind_file(AVFilterContext *ctx)
{
    MovieContext *movie = ctx->priv;
    int64_t timestamp = movie->seek_point;
    int ret, i;
    if (movie->format_ctx->start_time != AV_NOPTS_VALUE)
        timestamp += movie->format_ctx->start_time;
    ret = av_seek_frame(movie->format_ctx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to loop: %s\n", av_err2str(ret));
        movie->loop_count = 1; 
        return ret;
    }
    for (i = 0; i < ctx->nb_outputs; i++) {
        avcodec_flush_buffers(movie->st[i].st->codec);
        movie->st[i].done = 0;
    }
    movie->eof = 0;
    return 0;
}
