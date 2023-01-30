static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    SPPContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int qp_stride = 0;
    const int8_t *qp_table = NULL;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const int depth = desc->comp[0].depth_minus1 + 1;
    



    if (!s->qp) {
        qp_table = av_frame_get_qp_table(in, &qp_stride, &s->qscale_type);
        if (qp_table && !s->use_bframe_qp && in->pict_type != AV_PICTURE_TYPE_B) {
            int w, h;
            

            if (!qp_stride) {
                w = FF_CEIL_RSHIFT(inlink->w, 4);
                h = 1;
            } else {
                w = qp_stride;
                h = FF_CEIL_RSHIFT(inlink->h, 4);
            }
            if (w * h > s->non_b_qp_alloc_size) {
                int ret = av_reallocp_array(&s->non_b_qp_table, w, h);
                if (ret < 0) {
                    s->non_b_qp_alloc_size = 0;
                    return ret;
                }
                s->non_b_qp_alloc_size = w * h;
            }
            av_assert0(w * h <= s->non_b_qp_alloc_size);
            memcpy(s->non_b_qp_table, qp_table, w * h);
        }
    }
    if (s->log2_count && !ctx->is_disabled) {
        if (!s->use_bframe_qp && s->non_b_qp_table)
            qp_table = s->non_b_qp_table;
        if (qp_table || s->qp) {
            const int cw = FF_CEIL_RSHIFT(inlink->w, s->hsub);
            const int ch = FF_CEIL_RSHIFT(inlink->h, s->vsub);
            

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
            filter(s, out->data[0], in->data[0], out->linesize[0], in->linesize[0], inlink->w, inlink->h, qp_table, qp_stride, 1, depth);
            if (out->data[2]) {
                filter(s, out->data[1], in->data[1], out->linesize[1], in->linesize[1], cw,        ch,        qp_table, qp_stride, 0, depth);
                filter(s, out->data[2], in->data[2], out->linesize[2], in->linesize[2], cw,        ch,        qp_table, qp_stride, 0, depth);
            }
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
