static int push_frame(AVFilterContext *ctx, unsigned in_no, AVFrame *buf)
{
    ConcatContext *cat = ctx->priv;
    unsigned out_no = in_no % ctx->nb_outputs;
    AVFilterLink * inlink = ctx-> inputs[ in_no];
    AVFilterLink *outlink = ctx->outputs[out_no];
    struct concat_in *in = &cat->in[in_no];
    buf->pts = av_rescale_q(buf->pts, inlink->time_base, outlink->time_base);
    in->pts = buf->pts;
    in->nb_frames++;
    
    if (inlink->sample_rate)
        
        in->pts += av_rescale_q(buf->nb_samples,
                                av_make_q(1, inlink->sample_rate),
                                outlink->time_base);
    else if (in->nb_frames >= 2)
        
        in->pts = av_rescale(in->pts, in->nb_frames, in->nb_frames - 1);
    buf->pts += cat->delta_ts;
    return ff_filter_frame(outlink, buf);
}
