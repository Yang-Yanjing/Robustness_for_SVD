static void show_life_grid(AVFilterContext *ctx)
{
    LifeContext *life = ctx->priv;
    int i, j;
    char *line = av_malloc(life->w + 1);
    if (!line)
        return;
    for (i = 0; i < life->h; i++) {
        for (j = 0; j < life->w; j++)
            line[j] = life->buf[life->buf_idx][i*life->w + j] == ALIVE_CELL ? '@' : ' ';
        line[j] = 0;
        av_log(ctx, AV_LOG_DEBUG, "%3d: %s\n", i, line);
    }
    av_free(line);
}
