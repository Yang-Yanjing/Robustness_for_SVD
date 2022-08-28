static void close_input(AVFilterContext *ctx, unsigned in_no)
{
    ConcatContext *cat = ctx->priv;
    cat->in[in_no].eof = 1;
    cat->nb_in_active--;
    av_log(ctx, AV_LOG_VERBOSE, "EOF on %s, %d streams left in segment.\n",
           ctx->input_pads[in_no].name, cat->nb_in_active);
}
