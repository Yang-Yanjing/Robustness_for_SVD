static int init_output_bsfs(OutputStream *ost)
{
    AVBSFContext *ctx;
    int i, ret;
    if (!ost->nb_bitstream_filters)
        return 0;
    for (i = 0; i < ost->nb_bitstream_filters; i++) {
        ctx = ost->bsf_ctx[i];
        ret = avcodec_parameters_copy(ctx->par_in,
                                      i ? ost->bsf_ctx[i - 1]->par_out : ost->st->codecpar);
        if (ret < 0)
            return ret;
        ctx->time_base_in = i ? ost->bsf_ctx[i - 1]->time_base_out : ost->st->time_base;
        ret = av_bsf_init(ctx);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error initializing bitstream filter: %s\n",
                   ost->bsf_ctx[i]->filter->name);
            return ret;
        }
    }
    ctx = ost->bsf_ctx[ost->nb_bitstream_filters - 1];
    ret = avcodec_parameters_copy(ost->st->codecpar, ctx->par_out);
    if (ret < 0)
        return ret;
    ost->st->time_base = ctx->time_base_out;
    return 0;
}
