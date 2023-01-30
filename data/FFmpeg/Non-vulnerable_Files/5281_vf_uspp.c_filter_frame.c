static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    USPPContext *uspp = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int qp_stride = 0;
    uint8_t *qp_table = NULL;
    



    if (!uspp->qp) {
        qp_table = av_frame_get_qp_table(in, &qp_stride, &uspp->qscale_type);
        if (qp_table && !uspp->use_bframe_qp && in->pict_type != AV_PICTURE_TYPE_B) {
            int w, h;
            

            if (!qp_stride) {
                w = FF_CEIL_RSHIFT(inlink->w, 4);
                h = 1;
            } else {
                w = qp_stride;
                h = FF_CEIL_RSHIFT(inlink->h, 4);
            }
            if (w * h > uspp->non_b_qp_alloc_size) {
                int ret = av_reallocp_array(&uspp->non_b_qp_table, w, h);
                if (ret < 0) {
                    uspp->non_b_qp_alloc_size = 0;
                    return ret;
                }
                uspp->non_b_qp_alloc_size = w * h;
            }
            av_assert0(w * h <= uspp->non_b_qp_alloc_size);
            memcpy(uspp->non_b_qp_table, qp_table, w * h);
        }
    }
    if (uspp->log2_count && !ctx->is_disabled) {
        if (!uspp->use_bframe_qp && uspp->non_b_qp_table)
            qp_table = uspp->non_b_qp_table;
        if (qp_table || uspp->qp) {
            

            if (!av_frame_is_writable(in) || (inlink->w & 7) || (inlink->h & 7)) {
                const int aligned_w = FFALIGN(inlink->w, 8);
                const int aligned_h = FFALIGN(inlink->h, 8);
                out = ff_get_video_buffer(outlink, aligned_w, aligned_h);
                if (!out) {
                    av_frame_free(&in);
                    return AVERROR(ENOMEM);
                }
                av_frame_copy_props(out, in);
                out->width  = in->width;
                out->height = in->height;
            }
            filter(uspp, out->data, in->data, out->linesize, in->linesize,
                   inlink->w, inlink->h, qp_table, qp_stride);
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
