static av_cold void uninit(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    av_file_unmap(cellauto->file_buf, cellauto->file_bufsize);
    av_freep(&cellauto->buf);
    av_freep(&cellauto->pattern);
}
