static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    PP7Context *pp7 = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int qp_stride = 0;
    uint8_t *qp_table = NULL;
    if (!pp7->qp)
        qp_table = av_frame_get_qp_table(in, &qp_stride, &pp7->qscale_type);
    if (!ctx->is_disabled) {
        const int cw = FF_CEIL_RSHIFT(inlink->w, pp7->hsub);
        const int ch = FF_CEIL_RSHIFT(inlink->h, pp7->vsub);
        

        if (!av_frame_is_writable(in) || (inlink->w & 7) || (inlink->h & 7)) {
            const int aligned_w = FFALIGN(inlink->w, 8);
            const int aligned_h = FFALIGN(inlink->h, 8);
            out = ff_get_video_buffer(outlink, aligned_w, aligned_h);
            if (!out) {
                av_frame_free(&in);
                return AVERROR(ENOMEM);
            }
            av_frame_copy_props(out, in);
            out->width = in->width;
            out->height = in->height;
        }
        if (qp_table || pp7->qp) {
            filter(pp7, out->data[0], in->data[0], out->linesize[0], in->linesize[0],
                   inlink->w, inlink->h, qp_table, qp_stride, 1);
            filter(pp7, out->data[1], in->data[1], out->linesize[1], in->linesize[1],
                   cw,        ch,        qp_table, qp_stride, 0);
            filter(pp7, out->data[2], in->data[2], out->linesize[2], in->linesize[2],
                   cw,        ch,        qp_table, qp_stride, 0);
            emms_c();
        }
    }
    if (in != out) {
        if (in->data[3])
            av_image_copy_plane(out->data[3], out->linesize[3],
                                in ->data[3], in ->linesize[3],
                                inlink->w, inlink->h);
        av_frame_free(&in);
    }
    return ff_filter_frame(outlink, out);
}
