static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FramepackContext *s  = outlink->src->priv;
    int width            = ctx->inputs[LEFT]->w;
    int height           = ctx->inputs[LEFT]->h;
    AVRational time_base = ctx->inputs[LEFT]->time_base;
    AVRational frame_rate = ctx->inputs[LEFT]->frame_rate;
    
    if (width  != ctx->inputs[RIGHT]->w ||
        height != ctx->inputs[RIGHT]->h) {
        av_log(ctx, AV_LOG_ERROR,
               "Left and right sizes differ (%dx%d vs %dx%d).\n",
               width, height,
               ctx->inputs[RIGHT]->w, ctx->inputs[RIGHT]->h);
        return AVERROR_INVALIDDATA;
    } else if (av_cmp_q(time_base, ctx->inputs[RIGHT]->time_base) != 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Left and right time bases differ (%d/%d vs %d/%d).\n",
               time_base.num, time_base.den,
               ctx->inputs[RIGHT]->time_base.num,
               ctx->inputs[RIGHT]->time_base.den);
        return AVERROR_INVALIDDATA;
    } else if (av_cmp_q(frame_rate, ctx->inputs[RIGHT]->frame_rate) != 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Left and right framerates differ (%d/%d vs %d/%d).\n",
               frame_rate.num, frame_rate.den,
               ctx->inputs[RIGHT]->frame_rate.num,
               ctx->inputs[RIGHT]->frame_rate.den);
        return AVERROR_INVALIDDATA;
    }
    s->pix_desc = av_pix_fmt_desc_get(outlink->format);
    if (!s->pix_desc)
        return AVERROR_BUG;
    
    switch (s->format) {
    case AV_STEREO3D_FRAMESEQUENCE:
        time_base.den *= 2;
        frame_rate.num *= 2;
        s->double_pts = AV_NOPTS_VALUE;
        break;
    case AV_STEREO3D_COLUMNS:
    case AV_STEREO3D_SIDEBYSIDE:
        width *= 2;
        break;
    case AV_STEREO3D_LINES:
    case AV_STEREO3D_TOPBOTTOM:
        height *= 2;
        break;
    default:
        av_log(ctx, AV_LOG_ERROR, "Unknown packing mode.");
        return AVERROR_INVALIDDATA;
    }
    outlink->w         = width;
    outlink->h         = height;
    outlink->time_base = time_base;
    outlink->frame_rate= frame_rate;
    return 0;
}
