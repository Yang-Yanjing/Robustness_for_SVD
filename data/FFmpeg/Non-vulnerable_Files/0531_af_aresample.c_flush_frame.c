static int flush_frame(AVFilterLink *outlink, int final, AVFrame **outsamplesref_ret)
{
    AVFilterContext *ctx = outlink->src;
    AResampleContext *aresample = ctx->priv;
    AVFilterLink *const inlink = outlink->src->inputs[0];
    AVFrame *outsamplesref;
    int n_out = 4096;
    int64_t pts;
    outsamplesref = ff_get_audio_buffer(outlink, n_out);
    *outsamplesref_ret = outsamplesref;
    if (!outsamplesref)
        return AVERROR(ENOMEM);
    pts = swr_next_pts(aresample->swr, INT64_MIN);
    pts = ROUNDED_DIV(pts, inlink->sample_rate);
    n_out = swr_convert(aresample->swr, outsamplesref->extended_data, n_out, final ? NULL : (void*)outsamplesref->extended_data, 0);
    if (n_out <= 0) {
        av_frame_free(&outsamplesref);
        return (n_out == 0) ? AVERROR_EOF : n_out;
    }
    outsamplesref->sample_rate = outlink->sample_rate;
    outsamplesref->nb_samples  = n_out;
    outsamplesref->pts = pts;
    return 0;
}
