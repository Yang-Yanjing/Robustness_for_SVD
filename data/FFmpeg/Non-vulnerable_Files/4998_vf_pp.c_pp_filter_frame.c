static int pp_filter_frame(AVFilterLink *inlink, AVFrame *inbuf)
{
    AVFilterContext *ctx = inlink->dst;
    PPFilterContext *pp = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    const int aligned_w = FFALIGN(outlink->w, 8);
    const int aligned_h = FFALIGN(outlink->h, 8);
    AVFrame *outbuf;
    int qstride, qp_type;
    int8_t *qp_table ;
    outbuf = ff_get_video_buffer(outlink, aligned_w, aligned_h);
    if (!outbuf) {
        av_frame_free(&inbuf);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(outbuf, inbuf);
    outbuf->width  = inbuf->width;
    outbuf->height = inbuf->height;
    qp_table = av_frame_get_qp_table(inbuf, &qstride, &qp_type);
    pp_postprocess((const uint8_t **)inbuf->data, inbuf->linesize,
                   outbuf->data,                 outbuf->linesize,
                   aligned_w, outlink->h,
                   qp_table,
                   qstride,
                   pp->modes[pp->mode_id],
                   pp->pp_ctx,
                   outbuf->pict_type | (qp_type ? PP_PICT_TYPE_QP2 : 0));
    av_frame_free(&inbuf);
    return ff_filter_frame(outlink, outbuf);
}
