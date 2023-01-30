static int pp_config_props(AVFilterLink *inlink)
{
    int flags = PP_CPU_CAPS_AUTO;
    PPFilterContext *pp = inlink->dst->priv;
    switch (inlink->format) {
    case AV_PIX_FMT_GRAY8:
    case AV_PIX_FMT_YUVJ420P:
    case AV_PIX_FMT_YUV420P: flags |= PP_FORMAT_420; break;
    case AV_PIX_FMT_YUVJ422P:
    case AV_PIX_FMT_YUV422P: flags |= PP_FORMAT_422; break;
    case AV_PIX_FMT_YUV411P: flags |= PP_FORMAT_411; break;
    case AV_PIX_FMT_GBRP:
    case AV_PIX_FMT_YUVJ444P:
    case AV_PIX_FMT_YUV444P: flags |= PP_FORMAT_444; break;
    case AV_PIX_FMT_YUVJ440P:
    case AV_PIX_FMT_YUV440P: flags |= PP_FORMAT_440; break;
    default: av_assert0(0);
    }
    pp->pp_ctx = pp_get_context(inlink->w, inlink->h, flags);
    if (!pp->pp_ctx)
        return AVERROR(ENOMEM);
    return 0;
}
