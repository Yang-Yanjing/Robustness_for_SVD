static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    FSPPContext *fspp = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int qp_stride = 0;
    uint8_t *qp_table = NULL;
    int i, bias;
    int custom_threshold_m[64];
    bias = (1 << 4) + fspp->strength;
    for (i = 0; i < 64; i++) 
        custom_threshold_m[i] = (int)(custom_threshold[i] * (bias / 71.0) + 0.5);
    for (i = 0; i < 8; i++) {
        fspp->threshold_mtx_noq[2 * i] = (uint64_t)custom_threshold_m[i * 8 + 2]
                                      |(((uint64_t)custom_threshold_m[i * 8 + 6]) << 16)
                                      |(((uint64_t)custom_threshold_m[i * 8 + 0]) << 32)
                                      |(((uint64_t)custom_threshold_m[i * 8 + 4]) << 48);
        fspp->threshold_mtx_noq[2 * i + 1] = (uint64_t)custom_threshold_m[i * 8 + 5]
                                          |(((uint64_t)custom_threshold_m[i * 8 + 3]) << 16)
                                          |(((uint64_t)custom_threshold_m[i * 8 + 1]) << 32)
                                          |(((uint64_t)custom_threshold_m[i * 8 + 7]) << 48);
    }
    if (fspp->qp)
        fspp->prev_q = fspp->qp, fspp->mul_thrmat((int16_t *)(&fspp->threshold_mtx_noq[0]), (int16_t *)(&fspp->threshold_mtx[0]), fspp->qp);
    



    if (!fspp->qp) {
        qp_table = av_frame_get_qp_table(in, &qp_stride, &fspp->qscale_type);
        if (qp_table && !fspp->use_bframe_qp && in->pict_type != AV_PICTURE_TYPE_B) {
            int w, h;
            

           if (!qp_stride) {
                w = FF_CEIL_RSHIFT(inlink->w, 4);
                h = 1;
            } else {
                w = qp_stride;
                h = FF_CEIL_RSHIFT(inlink->h, 4);
            }
            if (w * h > fspp->non_b_qp_alloc_size) {
                int ret = av_reallocp_array(&fspp->non_b_qp_table, w, h);
                if (ret < 0) {
                    fspp->non_b_qp_alloc_size = 0;
                    return ret;
                }
                fspp->non_b_qp_alloc_size = w * h;
            }
            av_assert0(w * h <= fspp->non_b_qp_alloc_size);
            memcpy(fspp->non_b_qp_table, qp_table, w * h);
        }
    }
    if (fspp->log2_count && !ctx->is_disabled) {
        if (!fspp->use_bframe_qp && fspp->non_b_qp_table)
            qp_table = fspp->non_b_qp_table;
        if (qp_table || fspp->qp) {
            const int cw = FF_CEIL_RSHIFT(inlink->w, fspp->hsub);
            const int ch = FF_CEIL_RSHIFT(inlink->h, fspp->vsub);
            

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
            filter(fspp, out->data[0], in->data[0], out->linesize[0], in->linesize[0],
                   inlink->w, inlink->h, qp_table, qp_stride, 1);
            filter(fspp, out->data[1], in->data[1], out->linesize[1], in->linesize[1],
                   cw,        ch,        qp_table, qp_stride, 0);
            filter(fspp, out->data[2], in->data[2], out->linesize[2], in->linesize[2],
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
