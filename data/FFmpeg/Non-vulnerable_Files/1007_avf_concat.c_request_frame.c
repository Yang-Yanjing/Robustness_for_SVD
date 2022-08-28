static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ConcatContext *cat   = ctx->priv;
    unsigned out_no = FF_OUTLINK_IDX(outlink);
    unsigned in_no  = out_no + cat->cur_idx;
    unsigned str, str_max;
    int ret;
    while (1) {
        if (in_no >= ctx->nb_inputs)
            return AVERROR_EOF;
        if (!cat->in[in_no].eof) {
            ret = ff_request_frame(ctx->inputs[in_no]);
            if (ret != AVERROR_EOF)
                return ret;
            close_input(ctx, in_no);
        }
        
        
        str_max = cat->cur_idx + ctx->nb_outputs - 1;
        for (str = cat->cur_idx; cat->nb_in_active;
             str = str == str_max ? cat->cur_idx : str + 1) {
            if (cat->in[str].eof)
                continue;
            ret = ff_request_frame(ctx->inputs[str]);
            if (ret == AVERROR_EOF)
                close_input(ctx, str);
            else if (ret < 0)
                return ret;
        }
        ret = flush_segment(ctx);
        if (ret < 0)
            return ret;
        in_no += ctx->nb_outputs;
    }
}
