static void show_cellauto_row(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    int i;
    uint8_t *row = cellauto->buf + cellauto->w * cellauto->buf_row_idx;
    char *line = av_malloc(cellauto->w + 1);
    if (!line)
        return;
    for (i = 0; i < cellauto->w; i++)
        line[i] = row[i] ? '@' : ' ';
    line[i] = 0;
    av_log(ctx, AV_LOG_DEBUG, "generation:%"PRId64" row:%s|\n", cellauto->generation, line);
    av_free(line);
}
