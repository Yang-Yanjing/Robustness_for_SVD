static int filter_frame(AVFilterLink *link, AVFrame *in)
{
    AVFilterContext *ctx = link->dst;
    ColorMatrixContext *color = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    ThreadData td = {0};
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (color->source == COLOR_MODE_NONE) {
        enum AVColorSpace cs = av_frame_get_colorspace(in);
        enum ColorMode source;
        switch(cs) {
        case AVCOL_SPC_BT709     : source = COLOR_MODE_BT709     ; break;
        case AVCOL_SPC_FCC       : source = COLOR_MODE_FCC       ; break;
        case AVCOL_SPC_SMPTE240M : source = COLOR_MODE_SMPTE240M ; break;
        case AVCOL_SPC_BT470BG   : source = COLOR_MODE_BT601     ; break;
        case AVCOL_SPC_SMPTE170M : source = COLOR_MODE_BT601     ; break;
        default :
            av_log(ctx, AV_LOG_ERROR, "Input frame does not specify a supported colorspace, and none has been specified as source either\n");
            av_frame_free(&out);
            return AVERROR(EINVAL);
        }
        color->mode = source * 4 + color->dest;
    } else
        color->mode = color->source * 4 + color->dest;
    switch(color->dest) {
    case COLOR_MODE_BT709    : av_frame_set_colorspace(out, AVCOL_SPC_BT709)    ; break;
    case COLOR_MODE_FCC      : av_frame_set_colorspace(out, AVCOL_SPC_FCC)      ; break;
    case COLOR_MODE_SMPTE240M: av_frame_set_colorspace(out, AVCOL_SPC_SMPTE240M); break;
    case COLOR_MODE_BT601    : av_frame_set_colorspace(out, AVCOL_SPC_BT470BG)  ; break;
    }
    td.src = in;
    td.dst = out;
    td.c2 = color->yuv_convert[color->mode][0][1];
    td.c3 = color->yuv_convert[color->mode][0][2];
    td.c4 = color->yuv_convert[color->mode][1][1];
    td.c5 = color->yuv_convert[color->mode][1][2];
    td.c6 = color->yuv_convert[color->mode][2][1];
    td.c7 = color->yuv_convert[color->mode][2][2];
    if (in->format == AV_PIX_FMT_YUV444P)
        ctx->internal->execute(ctx, process_slice_yuv444p, &td, NULL,
                               FFMIN(in->height, ctx->graph->nb_threads));
    else if (in->format == AV_PIX_FMT_YUV422P)
        ctx->internal->execute(ctx, process_slice_yuv422p, &td, NULL,
                               FFMIN(in->height, ctx->graph->nb_threads));
    else if (in->format == AV_PIX_FMT_YUV420P)
        ctx->internal->execute(ctx, process_slice_yuv420p, &td, NULL,
                               FFMIN(in->height / 2, ctx->graph->nb_threads));
    else
        ctx->internal->execute(ctx, process_slice_uyvy422, &td, NULL,
                               FFMIN(in->height, ctx->graph->nb_threads));
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
