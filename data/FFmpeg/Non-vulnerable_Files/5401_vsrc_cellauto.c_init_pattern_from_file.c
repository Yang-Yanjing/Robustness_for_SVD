static int init_pattern_from_file(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    int ret;
    ret = av_file_map(cellauto->filename,
                      &cellauto->file_buf, &cellauto->file_bufsize, 0, ctx);
    if (ret < 0)
        return ret;
    
    cellauto->pattern = av_malloc(cellauto->file_bufsize + 1);
    if (!cellauto->pattern)
        return AVERROR(ENOMEM);
    memcpy(cellauto->pattern, cellauto->file_buf, cellauto->file_bufsize);
    cellauto->pattern[cellauto->file_bufsize] = 0;
    return init_pattern_from_string(ctx);
}
