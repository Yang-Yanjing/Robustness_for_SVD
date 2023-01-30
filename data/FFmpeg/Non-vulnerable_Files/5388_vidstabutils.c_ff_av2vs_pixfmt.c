VSPixelFormat ff_av2vs_pixfmt(AVFilterContext *ctx, enum AVPixelFormat pf)
{
    switch (pf) {
    case AV_PIX_FMT_YUV420P:  return PF_YUV420P;
    case AV_PIX_FMT_YUV422P:  return PF_YUV422P;
    case AV_PIX_FMT_YUV444P:  return PF_YUV444P;
    case AV_PIX_FMT_YUV410P:  return PF_YUV410P;
    case AV_PIX_FMT_YUV411P:  return PF_YUV411P;
    case AV_PIX_FMT_YUV440P:  return PF_YUV440P;
    case AV_PIX_FMT_YUVA420P: return PF_YUVA420P;
    case AV_PIX_FMT_GRAY8:    return PF_GRAY8;
    case AV_PIX_FMT_RGB24:    return PF_RGB24;
    case AV_PIX_FMT_BGR24:    return PF_BGR24;
    case AV_PIX_FMT_RGBA:     return PF_RGBA;
    default:
        av_log(ctx, AV_LOG_ERROR, "cannot deal with pixel format %i\n", pf);
        return PF_NONE;
    }
}
