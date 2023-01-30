static int yae_set_tempo(AVFilterContext *ctx, const char *arg_tempo)
{
    const AudioFragment *prev;
    ATempoContext *atempo = ctx->priv;
    char   *tail = NULL;
    double tempo = av_strtod(arg_tempo, &tail);
    if (tail && *tail) {
        av_log(ctx, AV_LOG_ERROR, "Invalid tempo value '%s'\n", arg_tempo);
        return AVERROR(EINVAL);
    }
    if (tempo < 0.5 || tempo > 2.0) {
        av_log(ctx, AV_LOG_ERROR, "Tempo value %f exceeds [0.5, 2.0] range\n",
               tempo);
        return AVERROR(EINVAL);
    }
    prev = yae_prev_frag(atempo);
    atempo->origin[0] = prev->position[0] + atempo->window / 2;
    atempo->origin[1] = prev->position[1] + atempo->window / 2;
    atempo->tempo = tempo;
    return 0;
}
